//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_gen8.hpp"
#include "kern_gen8_patches.hpp"
#include "kern_nblue.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_api.hpp>

static const char *pathG8FB =
    "/System/Library/Extensions/AppleIntelBDWGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelBDWGraphicsFramebuffer";
static const char *pathG8HW =
    "/System/Library/Extensions/AppleIntelBDWGraphics.kext/Contents/MacOS/AppleIntelBDWGraphics";

static KernelPatcher::KextInfo kextG8FB {"com.apple.driver.AppleIntelBDWGraphicsFramebuffer", &pathG8FB, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG8HW {"com.apple.driver.AppleIntelBDWGraphics", &pathG8HW, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};

void Gen8::init() {
    lilu.onKextLoadForce(&kextG8FB);
    lilu.onKextLoadForce(&kextG8HW);
}

bool Gen8::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (kextG8FB.loadIndex == index) {
        NBlue::callback->igfxGen = iGFXGen::Gen8;

        DBGLOG("gen8", "Loaded AppleIntelBDWGraphicsFramebuffer!");
    } else if (kextG8HW.loadIndex == index) {
        DBGLOG("gen8", "Loaded AppleIntelBDWGraphics!");
        switch (getKernelVersion()) {
            case KernelVersion::Mojave: {
                DBGLOG("gen8", "Using Mojave patchset");

                LookupPatchPlus const patches[] = {
                    {&kextG8HW, kGen8HWMojave1Original, kGen8HWMojave1Patched, arrsize(kGen8HWMojave1Original), 1},
                    {&kextG8HW, kGen8HWMojave2Original, kGen8HWMojave2Patched, arrsize(kGen8HWMojave2Original), 1},
                    {&kextG8HW, kGen8HWMojave3Original, kGen8HWMojave3Patched, arrsize(kGen8HWMojave3Original), 1},
                };

                PANIC_COND(!LookupPatchPlus::applyAll(&patcher, patches, address, size), "hsw",
                    "Failed to apply patches: %d", patcher.getError());
            }
            default: {
                PANIC("gen8", "Unsupported OS version!");
            }
        }
        return true;
    }

    return false;
}
