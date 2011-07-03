#ifndef __eglext_h_wrapper_
#define __eglext_h_wrapper_

#include <android/api-level.h>

#if __ANDROID_API__ < 9
# include <eglext_v3.h>
#else
# include <eglext_v5.h>
#endif

#endif
