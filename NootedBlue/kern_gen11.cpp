//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_gen11.hpp"
#include "kern_nblue.hpp"
#include <Headers/kern_api.hpp>

static const char *pathG11FB = "/System/Library/Extensions/AppleIntelICLLPGraphicsFramebuffer.kext/Contents/MacOS/"
                               "AppleIntelICLLPGraphicsFramebuffer";
static const char *pathG11HW =
    "/System/Library/Extensions/AppleIntelICLGraphics.kext/Contents/MacOS/AppleIntelICLGraphics";

static KernelPatcher::KextInfo kextG11FB {"com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", &pathG11FB, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG11HW {"com.apple.driver.AppleIntelICLGraphics", &pathG11HW, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};

void Gen11::init() {
    lilu.onKextLoadForce(&kextG11FB);
    lilu.onKextLoadForce(&kextG11HW);
}

bool Gen11::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (kextG11FB.loadIndex == index) {
        NBlue::callback->igfxGen = iGFXGen::Gen11;

        DBGLOG("gen11", "Loaded AppleIntelICLLPGraphicsFramebuffer!");
        return true;
    } else if (kextG11HW.loadIndex == index) {
        DBGLOG("gen11", "Loaded AppleIntelICLGraphics!");
        // TBF
        return true;
    }

    return false;
}
