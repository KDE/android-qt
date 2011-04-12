/*
 * Copyright (C) 2010 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* ChangeLog for this library:
 *
 * NDK r5: Handle buggy kernels which report a CPU Architecture number of 7
 *         for an ARMv6 CPU (see below).
 *
 *         Handle kernels that only report 'neon', and not 'vfpv3'
 *         (VFPv3 is mandated by the ARM architecture is Neon is implemented)
 *
 *         Handle kernels that only report 'vfpv3d16', and not 'vfpv3'
 *
 *         Fix x86 compilation. Report ANDROID_CPU_FAMILY_X86 in
 *         android_getCpuFamily().
 *
 * NDK r4: Initial release
 */
#include <sys/system_properties.h>
#ifdef __arm__
#include <machine/cpu-features.h>
#endif
#include <pthread.h>
#include "cpu-features.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

static  pthread_once_t     g_once;
static  AndroidCpuFamily   g_cpuFamily;
static  uint64_t           g_cpuFeatures;

static const int  android_cpufeatures_debug = 0;

#define  D(...) \
    do { \
        if (android_cpufeatures_debug) { \
            printf(__VA_ARGS__); fflush(stdout); \
        } \
    } while (0)

/* Read the content of /proc/cpuinfo into a user-provided buffer.
 * Return the length of the data, or -1 on error. Does *not*
 * zero-terminate the content. Will not read more
 * than 'buffsize' bytes.
 */
static int
read_file(const char*  pathname, char*  buffer, size_t  buffsize)
{
    int  fd, len;

    fd = open(pathname, O_RDONLY);
    if (fd < 0)
        return -1;

    do {
        len = read(fd, buffer, buffsize);
    } while (len < 0 && errno == EINTR);

    close(fd);

    return len;
}

/* Extract the content of a the first occurence of a given field in
 * the content of /proc/cpuinfo and return it as a heap-allocated
 * string that must be freed by the caller.
 *
 * Return NULL if not found
 */
static char*
extract_cpuinfo_field(char* buffer, int buflen, const char* field)
{
    int  fieldlen = strlen(field);
    char* bufend = buffer + buflen;
    char* result = NULL;
    int len, ignore;
    const char *p, *q;

    /* Look for first field occurence, and ensures it starts the line.
     */
    p = buffer;
    bufend = buffer + buflen;
    for (;;) {
        p = memmem(p, bufend-p, field, fieldlen);
        if (p == NULL)
            goto EXIT;

        if (p == buffer || p[-1] == '\n')
            break;

        p += fieldlen;
    }

    /* Skip to the first column followed by a space */
    p += fieldlen;
    p  = memchr(p, ':', bufend-p);
    if (p == NULL || p[1] != ' ')
        goto EXIT;

    /* Find the end of the line */
    p += 2;
    q = memchr(p, '\n', bufend-p);
    if (q == NULL)
        q = bufend;

    /* Copy the line into a heap-allocated buffer */
    len = q-p;
    result = malloc(len+1);
    if (result == NULL)
        goto EXIT;

    memcpy(result, p, len);
    result[len] = '\0';

EXIT:
    return result;
}


/* Checks that a space-separated list of items contains one given 'item'.
 * Returns 1 if found, 0 otherwise.
 */
static int
has_list_item(const char* list, const char* item)
{
    const char*  p = list;
    int itemlen = strlen(item);

    if (list == NULL)
        return 0;

    while (*p) {
        const char*  q;

        /* skip spaces */
        while (*p == ' ' || *p == '\t')
            p++;

        /* find end of current list item */
        q = p;
        while (*q && *q != ' ' && *q != '\t')
            q++;

        if (itemlen == q-p && !memcmp(p, item, itemlen))
            return 1;

        /* skip to next item */
        p = q;
    }
    return 0;
}


static void
android_cpuInit(void)
{
    g_cpuFamily   = ANDROID_CPU_FAMILY_UNKNOWN;
    g_cpuFeatures = 0;

#ifdef __ARM_ARCH__
    {
        char   cpuinfo[4096];
        int    cpuinfo_len = read_file("/proc/cpuinfo", cpuinfo, sizeof cpuinfo);
        char*  features = NULL;
        char*  architecture = NULL;

        g_cpuFamily = ANDROID_CPU_FAMILY_ARM;

        D("cpuinfo_len is (%d):\n%.*s\n", cpuinfo_len, cpuinfo_len, cpuinfo);

        if (cpuinfo_len < 0)  /* should not happen */ {
            return;
        }

        /* Extract architecture from the "CPU Architecture" field.
         * The list is well-known, unlike the the output of
         * the 'Processor' field which can vary greatly.
         *
         * See the definition of the 'proc_arch' array in
         * $KERNEL/arch/arm/kernel/setup.c and the 'c_show' function in
         * same file.
         */
        {
            char* cpuArch = extract_cpuinfo_field(cpuinfo, cpuinfo_len, "CPU architecture");

            if (cpuArch != NULL) {
                char*  end;
                long   archNumber;
                int    hasARMv7 = 0;

                D("found cpuArch = '%s'\n", cpuArch);

                /* read the initial decimal number, ignore the rest */
                archNumber = strtol(cpuArch, &end, 10);

                /* Here we assume that ARMv8 will be upwards compatible with v7
                 * in the future. Unfortunately, there is no 'Features' field to
                 * indicate that Thumb-2 is supported.
                 */
                if (end > cpuArch && archNumber >= 7) {
                    hasARMv7 = 1;
                }

                /* Unfortunately, it seems that certain ARMv6-based CPUs
                 * report an incorrect architecture number of 7!
                 *
                 * See http://code.google.com/p/android/issues/detail?id=10812
                 *
                 * We try to correct this by looking at the 'elf_format'
                 * field reported by the 'Processor' field, which is of the
                 * form of "(v7l)" for an ARMv7-based CPU, and "(v6l)" for
                 * an ARMv6-one.
                 */
                if (hasARMv7) {
                    char* cpuProc = extract_cpuinfo_field(cpuinfo, cpuinfo_len,
                                                          "Processor");
                    if (cpuProc != NULL) {
                        D("found cpuProc = '%s'\n", cpuProc);
                        if (has_list_item(cpuProc, "(v6l)")) {
                            D("CPU processor and architecture mismatch!!\n");
                            hasARMv7 = 0;
                        }
                        free(cpuProc);
                    }
                }

                if (hasARMv7) {
                    g_cpuFeatures |= ANDROID_CPU_ARM_FEATURE_ARMv7;
                }

                free(cpuArch);
            }
        }

        /* Extract the list of CPU features from 'Features' field */
        {
            char* cpuFeatures = extract_cpuinfo_field(cpuinfo, cpuinfo_len, "Features");

            if (cpuFeatures != NULL) {

                D("found cpuFeatures = '%s'\n", cpuFeatures);

                if (has_list_item(cpuFeatures, "vfpv3"))
                    g_cpuFeatures |= ANDROID_CPU_ARM_FEATURE_VFPv3;

                else if (has_list_item(cpuFeatures, "vfpv3d16"))
                    g_cpuFeatures |= ANDROID_CPU_ARM_FEATURE_VFPv3;

                if (has_list_item(cpuFeatures, "neon")) {
                    /* Note: Certain kernels only report neon but not vfpv3
                     *       in their features list. However, ARM mandates
                     *       that if Neon is implemented, so must be VFPv3
                     *       so always set the flag.
                     */
                    g_cpuFeatures |= ANDROID_CPU_ARM_FEATURE_NEON |
                                     ANDROID_CPU_ARM_FEATURE_VFPv3;
                }
                free(cpuFeatures);
            }
        }
    }
#endif /* __ARM_ARCH__ */

#ifdef __i386__
    g_cpuFamily = ANDROID_CPU_FAMILY_X86;
#endif
}


AndroidCpuFamily
android_getCpuFamily(void)
{
    pthread_once(&g_once, android_cpuInit);
    return g_cpuFamily;
}


uint64_t
android_getCpuFeatures(void)
{
    pthread_once(&g_once, android_cpuInit);
    return g_cpuFeatures;
}
