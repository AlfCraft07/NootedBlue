//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_gen7.hpp"
#include "kern_nblue.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_api.hpp>

static const char *pathG7FB =
    "/System/Library/Extensions/AppleIntelFramebufferCapri.kext/Contents/MacOS/AppleIntelFramebufferCapri";
static const char *pathG7HW =
    "/System/Library/Extensions/AppleIntelHD4000Graphics.kext/Contents/MacOS/AppleIntelHD4000Graphics";

static KernelPatcher::KextInfo kextG7FB {"com.apple.driver.AppleIntelFramebufferCapri", &pathG7FB, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG7HW {"com.apple.driver.AppleIntelHD4000Graphics", &pathG7HW, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};

void Gen7::init() {
    lilu.onKextLoadForce(&kextG7FB);
    lilu.onKextLoadForce(&kextG7HW);
}

bool Gen7::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (kextG7FB.loadIndex == index) {
        NBlue::callback->igfxGen = iGFXGen::Gen7;

        DBGLOG("gen7", "Loaded AppleIntelFramebufferCapri!");
    } else if (kextG7HW.loadIndex == index) {
        DBGLOG("gen7", "Loaded AppleIntelHD4000Graphics!");
        // TBF
	return true;
    }

    return false;
}
