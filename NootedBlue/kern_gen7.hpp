//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_gen7_hpp
#define kern_gen7_hpp
#include <Headers/kern_patcher.hpp>
#include <Headers/kern_util.hpp>

class Gen7 {
    public:
    void init();
    bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
};

#endif /* kern_gen7_hpp */
