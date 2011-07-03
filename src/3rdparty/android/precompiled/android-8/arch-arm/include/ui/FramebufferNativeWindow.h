/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_FRAMEBUFFER_NATIVE_WINDOW_H
#define ANDROID_FRAMEBUFFER_NATIVE_WINDOW_H

#include <stdint.h>
#include <sys/types.h>

#include <EGL/egl.h>

#include <utils/threads.h>
#include <ui/Rect.h>

#include <pixelflinger/pixelflinger.h>

#include <ui/egl/android_natives.h>


extern "C" EGLNativeWindowType android_createDisplaySurface(void);

// ---------------------------------------------------------------------------
namespace android {
// ---------------------------------------------------------------------------

class Surface;
class NativeBuffer;

// ---------------------------------------------------------------------------

class FramebufferNativeWindow 
    : public EGLNativeBase<
        android_native_window_t, 
        FramebufferNativeWindow, 
        LightRefBase<FramebufferNativeWindow> >
{
public:
    FramebufferNativeWindow(); 

    framebuffer_device_t const * getDevice() const { return fbDev; } 

    bool isUpdateOnDemand() const { return mUpdateOnDemand; }
    status_t setUpdateRectangle(const Rect& updateRect);
    status_t compositionComplete();
    
private:
    friend class LightRefBase<FramebufferNativeWindow>;    
    ~FramebufferNativeWindow(); // this class cannot be overloaded
    static int setSwapInterval(android_native_window_t* window, int interval);
    static int dequeueBuffer(android_native_window_t* window, android_native_buffer_t** buffer);
    static int lockBuffer(android_native_window_t* window, android_native_buffer_t* buffer);
    static int queueBuffer(android_native_window_t* window, android_native_buffer_t* buffer);
    static int query(android_native_window_t* window, int what, int* value);
    static int perform(android_native_window_t* window, int operation, ...);
    
    framebuffer_device_t* fbDev;
    alloc_device_t* grDev;

    sp<NativeBuffer> buffers[2];
    sp<NativeBuffer> front;
    
    mutable Mutex mutex;
    Condition mCondition;
    int32_t mNumBuffers;
    int32_t mNumFreeBuffers;
    int32_t mBufferHead;
    bool mUpdateOnDemand;
};
    
// ---------------------------------------------------------------------------
}; // namespace android
// ---------------------------------------------------------------------------

#endif // ANDROID_FRAMEBUFFER_NATIVE_WINDOW_H

