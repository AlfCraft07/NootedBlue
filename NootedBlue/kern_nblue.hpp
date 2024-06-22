//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_nblue_hpp
#define kern_nblue_hpp
#include "kern_gen7.hpp"
#include "kern_gen8.hpp"
#include "kern_gen9.hpp"
#include "kern_hsw.hpp"
#include <Headers/kern_patcher.hpp>
#include <Headers/kern_user.hpp>
#include <Headers/kern_util.hpp>
#include <IOKit/pci/IOPCIDevice.h>

class EXPORT PRODUCT_NAME : public IOService {
    OSDeclareDefaultStructors(PRODUCT_NAME);

    public:
    IOService *probe(IOService *provider, SInt32 *score) override;
    bool start(IOService *provider) override;
};

enum struct iGFXGen {
    Gen7,       // Ivy Bridge
    Haswell,    // Haswell had no lower end rebrands, so it gets it's name plastered here
    Gen8,       // Broadwell & Braswell/Cherry Trail/Cherry View
    Gen9,       // Skylake & Apollo Lake/Broxton
    Gen9_5,     // Coffee Lake, Comet Lake, Gemini Lake, Kaby Lake
    Gen11,      // Ice Lake kexts support G1 with device-id spoofs, this, however, is inconvenient, also Elkhart/Jasper
                // Lake exist, pain.
    Gen12_1,    // Tiger Lake, these placeholders, don't get your hopes up
    Gen12_2,    // Alder Lake & Raptor Lake, placeholder
    Unknown,
};

class NBlue {
    friend class Gen7;
    friend class Gen8;
    friend class Gen9;
    friend class Gen9_5;
    friend class Gen11;
    friend class HSW;

    public:
    static NBlue *callback;
    void init();
    void processPatcher(KernelPatcher &patcher);
    bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);

    private:
    bool isCflDerivative = false;
    bool isJslDerivative = false;
    bool isGen9LPDerivative = false;
    bool isGen8LPDerivative = false;
    iGFXGen igfxGen = iGFXGen::Unknown;
    uint32_t deviceId {0};
    uint16_t revision {0};
    uint32_t pciRevision {0};
    IOPCIDevice *iGPU {nullptr};
    mach_vm_address_t orgCsValidatePage {0};

    static OSMetaClassBase *wrapSafeMetaCast(const OSMetaClassBase *anObject, const OSMetaClass *toMeta);
    static void csValidatePage(vnode *vp, memory_object_t pager, memory_object_offset_t page_offset, const void *data,
        int *validated_p, int *tainted_p, int *nx_p);
};

#endif /* kern_nblue_hpp */
