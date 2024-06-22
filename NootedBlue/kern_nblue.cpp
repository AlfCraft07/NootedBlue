//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_nblue.hpp"
#include "kern_gen11.hpp"
#include "kern_gen7.hpp"
#include "kern_gen8.hpp"
#include "kern_gen9.hpp"
#include "kern_gen9_5.hpp"
#include "kern_hsw.hpp"
#include "kern_model.hpp"
#include <Headers/kern_api.hpp>
#include <Headers/kern_devinfo.hpp>
#include <Headers/kern_util.hpp>

NBlue *NBlue::callback = nullptr;

static Gen7 gen7;
static Gen8 gen8;
static Gen9 gen9;
static Gen9_5 gen9_5;
static Gen11 gen11;
static HSW hsw;

void NBlue::init() {
    callback = this;
    lilu.onPatcherLoadForce(
        [](void *user, KernelPatcher &patcher) { static_cast<NBlue *>(user)->processPatcher(patcher); }, this);
    lilu.onKextLoadForce(
        nullptr, 0,
        [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
            static_cast<NBlue *>(user)->processKext(patcher, index, address, size);
        },
        this);
	gen7.init();
    gen8.init();
    gen9_5.init();
    gen9.init();
    gen11.init();
    hsw.init();
}

void NBlue::processPatcher(KernelPatcher &patcher) {
    auto *devInfo = DeviceInfo::create();
    if (devInfo) {
        devInfo->processSwitchOff();

        this->iGPU = OSDynamicCast(IOPCIDevice, devInfo->videoBuiltin);
        PANIC_COND(!this->iGPU, "nblue", "videoBuiltin is not IOPCIDevice");

        WIOKit::renameDevice(this->iGPU, "IGPU");
        WIOKit::awaitPublishing(this->iGPU);
        char name[256] = {0};
        for (size_t i = 0, ii = 0; i < devInfo->videoExternal.size(); i++) {
            auto *device = OSDynamicCast(IOPCIDevice, devInfo->videoExternal[i].video);
            if (device) {
                snprintf(name, arrsize(name), "GFX%zu", ii++);
                WIOKit::renameDevice(device, name);
                WIOKit::awaitPublishing(device);
            }
        }

        static uint8_t builtin[] = {0x01};
        this->iGPU->setProperty("built-in", builtin, arrsize(builtin));
        this->deviceId = WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigDeviceID);
        this->pciRevision = WIOKit::readPCIConfigValue(NBlue::callback->iGPU, WIOKit::kIOPCIConfigRevisionID);

        DeviceInfo::deleter(devInfo);
    } else {
        SYSLOG("nblue", "Failed to create DeviceInfo");
    }

    if (getKernelVersion() >= KernelVersion::BigSur) {
        KernelPatcher::RouteRequest requests[] = {
            {"_cs_validate_page", csValidatePage, this->orgCsValidatePage},
        };

        size_t num = arrsize(requests);

        PANIC_COND(!patcher.routeMultipleLong(KernelPatcher::KernelID, requests, num), "nblue",
            "Failed to route kernel symbols");
    }
}

bool NBlue::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (gen7.processKext(patcher, index, address, size)) {
        DBGLOG("nblue", "Processed Ivy Bridge (Generation 7) configuration");
    } else if (hsw.processKext(patcher, index, address, size)) {
        DBGLOG("nblue", "Processed Haswell configuration");
    } else if (gen8.processKext(patcher, index, address, size)) {
        DBGLOG("nblue", "Processed Broadwell (Generation 8) configuration");
    } else if (gen9.processKext(patcher, index, address, size)) {
        DBGLOG("nblue", "Processed Skylake (Generation 9) configuration");
    } else if (gen9_5.processKext(patcher, index, address, size)) {
        DBGLOG("nblue", "Processed KBL-CML (Generation 9.5) configuration");
    } else if (gen11.processKext(patcher, index, address, size)) {
        DBGLOG("nblue", "Processed Ice Lake (Generation 11) configuration");
    }

    return true;
}

void NBlue::csValidatePage(vnode *vp, memory_object_t pager, memory_object_offset_t page_offset, const void *data,
    int *validated_p, int *tainted_p, int *nx_p) {
    FunctionCast(csValidatePage, callback->orgCsValidatePage)(vp, pager, page_offset, data, validated_p, tainted_p,
        nx_p);

    char path[PATH_MAX];
    int pathlen = PATH_MAX;
    if (LIKELY(!vn_getpath(vp, path, &pathlen))) {
        if (UNLIKELY(UserPatcher::matchSharedCachePath(path))) {
            if (callback->igfxGen == iGFXGen::Haswell) {
                if (getKernelVersion() == KernelVersion::BigSur) {
                    DBGLOG("nblue", "Placeholder code for the future, usually this is when we would start patching "
                                    "bundles from the dyld cache");
                }
            }
        }
    }
}
