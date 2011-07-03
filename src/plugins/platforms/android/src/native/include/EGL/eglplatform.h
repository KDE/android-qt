#ifndef __eglplatform_h_wrapper_
#define __eglplatform_h_wrapper_

#include <android/api-level.h>

#if __ANDROID_API__ < 9
# include <EGL/eglplatform_a5.h>
#else
# include <EGL/eglplatform_a9.h>
#endif

#endif
