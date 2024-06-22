//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_gen9_5.hpp"
#include "kern_gen9_5_patches.hpp"
#include "kern_nblue.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_api.hpp>

static const char *pathG9_5FB =
    "/System/Library/Extensions/AppleIntelKBLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelKBLGraphicsFramebuffer";
static const char *pathG9_5HW =
    "/System/Library/Extensions/AppleIntelKBLGraphics.kext/Contents/MacOS/AppleIntelKBLGraphics";

static const char *pathCFLFB =
    "/System/Library/Extensions/AppleIntelCFLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelCFLGraphicsFramebuffer";

static KernelPatcher::KextInfo kextG9_5FB {"com.apple.driver.AppleIntelKBLGraphicsFramebuffer", &pathG9_5FB, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG9_5HW {"com.apple.driver.AppleIntelKBLGraphics", &pathG9_5HW, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextCFLFB {"com.apple.driver.AppleIntelCFLGraphicsFramebuffer", &pathCFLFB, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};

void Gen9_5::init() {
    lilu.onKextLoadForce(&kextCFLFB);
    lilu.onKextLoadForce(&kextG9_5FB);
    lilu.onKextLoadForce(&kextG9_5HW);
}

bool Gen9_5::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (kextG9_5FB.loadIndex == index) {
        DBGLOG("gen9_5", "Loaded AppleIntelKBLGraphicsFramebuffer!");
    } else if (kextCFLFB.loadIndex == index) {
        NBlue::callback->isCflDerivative = true;
        if ((NBlue::callback->deviceId == 0x3184) || (NBlue::callback->deviceId == 0x3185)) {
            NBlue::callback->isGen9LPDerivative = true;
        }
    } else if (kextG9_5HW.loadIndex == index) {
        DBGLOG("gen9_5", "Loaded AppleIntelKBLGraphics!");
        // TBF, likely needs `-disablegfxfirmware`, thanks Apple.
        switch (getKernelVersion()) {
            case KernelVersion::Mojave: {
                DBGLOG("gen9_5", "Using Mojave patchset");

                LookupPatchPlus const patches[] = {
                    {&kextG9_5HW, kGen9_5HWMojave1Original, kGen9_5HWMojave1Patched, arrsize(kGen9_5HWMojave1Original),
                        1},
                };

                PANIC_COND(!LookupPatchPlus::applyAll(&patcher, patches, address, size), "hsw",
                    "Failed to apply patches: %d", patcher.getError());
            }
            default: {
                PANIC("gen9_5", "Unsupported OS version!");
            }
        }
        return true;
    }

    return false;
}
