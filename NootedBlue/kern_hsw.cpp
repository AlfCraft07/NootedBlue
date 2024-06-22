//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_hsw.hpp"
#include "kern_hsw_patches.hpp"
#include "kern_nblue.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_api.hpp>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/types.h>

static const char *pathHSWFB =
    "/System/Library/Extensions/AppleIntelFramebufferAzul.kext/Contents/MacOS/AppleIntelFramebufferAzul";
static const char *pathHSWHW =
    "/System/Library/Extensions/AppleIntelHD5000Graphics.kext/Contents/MacOS/AppleIntelHD5000Graphics";

static KernelPatcher::KextInfo kextHSWFB {"com.apple.driver.AppleIntelFramebufferAzul", &pathHSWFB, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextHSWHW {"com.apple.driver.AppleIntelHD5000Graphics", &pathHSWHW, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};

NBlue nblue;

void HSW::init() {
    lilu.onKextLoadForce(&kextHSWFB);
    lilu.onKextLoadForce(&kextHSWHW);
}
bool HSW::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (kextHSWHW.loadIndex == index) {
        NBlue::callback->igfxGen = iGFXGen::Haswell;

        switch (getKernelVersion()) {
            case KernelVersion::Sierra: {
                DBGLOG("hsw", "Applying Sierra patches!");

                LookupPatchPlus const patches[] = {
                    {&kextHSWHW, kHSWHWSierra1Original, kHSWHWSierra1Patched, arrsize(kHSWHWSierra1Original), 1},
                    {&kextHSWHW, kHSWHWSierra2Original, kHSWHWSierra2Patched, arrsize(kHSWHWSierra2Original), 1},
                    {&kextHSWHW, kHSWHWSierra3Original, kHSWHWSierra3Patched, arrsize(kHSWHWSierra3Original), 1},
                    {&kextHSWHW, kHSWHWSierra4Original, kHSWHWSierra4Patched, arrsize(kHSWHWSierra4Original), 1},
                };

                PANIC_COND(!LookupPatchPlus::applyAll(&patcher, patches, address, size), "hsw",
                    "Failed to apply patches: %d", patcher.getError());

                lilu.onProcLoadForce(kHSWProcInfo, 2, nullptr, nullptr, kHSWBinaryModSierra, 2);

                break;
            }
            case KernelVersion::HighSierra: {
                DBGLOG("hsw", "Applying High Sierra patches!");

                LookupPatchPlus const patches[] = {
                    {&kextHSWHW, kHSWHWHS1Original, kHSWHWHS1Patched, arrsize(kHSWHWHS1Original), 1},
                    {&kextHSWHW, kHSWHWHS2Original, kHSWHWHS2Patched, arrsize(kHSWHWHS2Original), 1},
                    {&kextHSWHW, kHSWHWHS3Original, kHSWHWHS3Patched, arrsize(kHSWHWHS3Original), 1},
                    {&kextHSWHW, kHSWHWHS4Original, kHSWHWHS4Patched, arrsize(kHSWHWHS4Original), 1},
                };

                PANIC_COND(!LookupPatchPlus::applyAll(&patcher, patches, address, size), "hsw",
                    "Failed to apply patches: %d", patcher.getError());

                lilu.onProcLoadForce(kHSWProcInfo, 2, nullptr, nullptr, kHSWBinaryModHS, 2);

                break;
            }
            case KernelVersion::Mojave: {
                // Thanks Apple.
                // We don't enable this on any other version since softwareupdate pushes people to download & install
                // 2021-005 it would also take a while to figure out what specific BN has this change
                char osversion[40] = {};
                size_t size = sizeof(osversion);
                sysctlbyname("kern.osversion", osversion, &size, NULL, 0);
                auto needsOtherPatch = (strcmp(osversion, "18G9323") != 0);
                DBGLOG("hsw", "Current Mojave version: %c", osversion);

                LookupPatchPlus const patches[] = {
                    {&kextHSWHW, kHSWHWMojave1Original, kHSWHWMojave1Patched, arrsize(kHSWHWMojave1Original), 1},
                    {&kextHSWHW, kHSWHWMojave2Original, kHSWHWMojave2Patched, arrsize(kHSWHWMojave2Original), 1,
                        !needsOtherPatch},
                    {&kextHSWHW, kHSWHWMojave101462Original, kHSWHWMojave101462Patched,
                        arrsize(kHSWHWMojave101462Original), 1, needsOtherPatch},
                    {&kextHSWHW, kHSWHWMojave3Original, kHSWHWMojave3Patched, arrsize(kHSWHWMojave3Original), 1},
                    {&kextHSWHW, kHSWHWMojave4Original, kHSWHWMojave4Patched, arrsize(kHSWHWMojave4Original), 1},
                };

                PANIC_COND(!LookupPatchPlus::applyAll(&patcher, patches, address, size), "hsw",
                    "Failed to apply patches: %d", patcher.getError());

                break;
            }
            default:
                PANIC("hsw", "Unsupported version!");
                break;
        }

        return true;
    }

    return false;
}
