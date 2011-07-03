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

#ifndef ANDROID_SF_SURFACE_COMPOSER_CLIENT_H
#define ANDROID_SF_SURFACE_COMPOSER_CLIENT_H

#include <stdint.h>
#include <sys/types.h>

#include <binder/IBinder.h>

#include <utils/SortedVector.h>
#include <utils/RefBase.h>
#include <utils/threads.h>

#include <ui/PixelFormat.h>
#include <ui/Region.h>

#include <surfaceflinger/Surface.h>

namespace android {

// ---------------------------------------------------------------------------

class Region;
class SharedClient;
class ISurfaceComposer;
class DisplayInfo;

class SurfaceComposerClient : virtual public RefBase
{
public:    
                SurfaceComposerClient();
    virtual     ~SurfaceComposerClient();

    // Always make sure we could initialize
    status_t    initCheck() const;

    // Return the connection of this client
    sp<IBinder> connection() const;
    
    // Retrieve a client for an existing connection.
    static sp<SurfaceComposerClient>
                clientForConnection(const sp<IBinder>& conn);

    // Forcibly remove connection before all references have gone away.
    void        dispose();

    // ------------------------------------------------------------------------
    // surface creation / destruction

    //! Create a surface
    sp<SurfaceControl> createSurface(
            int pid,            // pid of the process the surface is for
            const String8& name,// name of the surface
            DisplayID display,  // Display to create this surface on
            uint32_t w,         // width in pixel
            uint32_t h,         // height in pixel
            PixelFormat format, // pixel-format desired
            uint32_t flags = 0  // usage flags
    );

    sp<SurfaceControl> createSurface(
            int pid,            // pid of the process the surface is for
            DisplayID display,  // Display to create this surface on
            uint32_t w,         // width in pixel
            uint32_t h,         // height in pixel
            PixelFormat format, // pixel-format desired
            uint32_t flags = 0  // usage flags
    );


    // ------------------------------------------------------------------------
    // Composer parameters
    // All composer parameters must be changed within a transaction
    // several surfaces can be updated in one transaction, all changes are
    // committed at once when the transaction is closed.
    // CloseTransaction() usually requires an IPC with the server.
    
    //! Open a composer transaction
    status_t    openTransaction();

    //! commit the transaction
    status_t    closeTransaction();

    //! Open a composer transaction on all active SurfaceComposerClients.
    static void openGlobalTransaction();
        
    //! Close a composer transaction on all active SurfaceComposerClients.
    static void closeGlobalTransaction();
    
    //! Freeze the specified display but not transactions.
    static status_t freezeDisplay(DisplayID dpy, uint32_t flags = 0);
        
    //! Resume updates on the specified display.
    static status_t unfreezeDisplay(DisplayID dpy, uint32_t flags = 0);

    //! Set the orientation of the given display
    static int setOrientation(DisplayID dpy, int orientation, uint32_t flags);

    // Query the number of displays
    static ssize_t getNumberOfDisplays();

    // Get information about a display
    static status_t getDisplayInfo(DisplayID dpy, DisplayInfo* info);
    static ssize_t getDisplayWidth(DisplayID dpy);
    static ssize_t getDisplayHeight(DisplayID dpy);
    static ssize_t getDisplayOrientation(DisplayID dpy);

    status_t linkToComposerDeath(const sp<IBinder::DeathRecipient>& recipient,
            void* cookie = NULL, uint32_t flags = 0);

private:
    friend class Surface;
    friend class SurfaceControl;
    
    SurfaceComposerClient(const sp<ISurfaceComposer>& sm, 
            const sp<IBinder>& conn);

    status_t    hide(SurfaceID id);
    status_t    show(SurfaceID id, int32_t layer = -1);
    status_t    freeze(SurfaceID id);
    status_t    unfreeze(SurfaceID id);
    status_t    setFlags(SurfaceID id, uint32_t flags, uint32_t mask);
    status_t    setTransparentRegionHint(SurfaceID id, const Region& transparent);
    status_t    setLayer(SurfaceID id, int32_t layer);
    status_t    setAlpha(SurfaceID id, float alpha=1.0f);
    status_t    setFreezeTint(SurfaceID id, uint32_t tint);
    status_t    setMatrix(SurfaceID id, float dsdx, float dtdx, float dsdy, float dtdy);
    status_t    setPosition(SurfaceID id, int32_t x, int32_t y);
    status_t    setSize(SurfaceID id, uint32_t w, uint32_t h);
    
    void        signalServer();

    status_t    destroySurface(SurfaceID sid);

    void        _init(const sp<ISurfaceComposer>& sm,
                    const sp<ISurfaceFlingerClient>& conn);

    inline layer_state_t*   _get_state_l(SurfaceID id);
    layer_state_t*          _lockLayerState(SurfaceID id);
    inline void             _unlockLayerState();

    mutable     Mutex                               mLock;
                layer_state_t*                      mPrebuiltLayerState;
                SortedVector<layer_state_t>         mStates;
                int32_t                             mTransactionOpen;

                // these don't need to be protected because they never change
                // after assignment
                status_t                    mStatus;
                SharedClient*               mControl;
                sp<IMemoryHeap>             mControlMemory;
                sp<ISurfaceFlingerClient>   mClient;
                sp<ISurfaceComposer>        mSignalServer;
};

}; // namespace android

#endif // ANDROID_SF_SURFACE_COMPOSER_CLIENT_H

