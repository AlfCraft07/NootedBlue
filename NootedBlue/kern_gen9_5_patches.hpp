//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_gen9_5_patches_hpp
#define kern_gen9_5_patches_hpp
#include <Headers/kern_user.hpp>
#include <Headers/kern_util.hpp>
/**
 * `Kaby Lake/Coffee Lake Bundle paths`
 */

static const char kGen9_5MTLDriverPath[] = "/System/Library/Extensions/AppleIntelKBLGraphicsMTLDriver.bundle/Contents/"
                                           "MacOS/AppleIntelKBLGraphicsMTLDriver";
static const char kGen9_5VADriverPath[] = "/System/Library/Extensions/AppleIntelKBLGraphicsVADriver.bundle/Contents/"
                                          "MacOS/AppleIntelKBLGraphicsVADriver";

/**
 * `Kaby Lake/Coffee Lake/Gemini Lake Patch Set for Mojave`
 *  In development
 */
// JZ (near) to NO-OP and JMP (near)
// only works for 0x3184 and 0x3185 Device IDs, also this is untested, don't trust it yet
static const uint8_t kGen9_5HWMojave1Original[] = {0x81, 0xFA, 0x86, 0x80, 0x16, 0xD9, 0x0F, 0x84, 0xD1, 0x01, 0x00,
    0x00};
static const uint8_t kGen9_5HWMojave1Patched[] = {0x81, 0xFA, 0x86, 0x80, 0x16, 0xD9, 0x90, 0xE9, 0xD1, 0x01, 0x00,
    0x00};

/** Based off of WhateverGreen code*/

enum : uint32_t {
    Gen9_5MTL = 1,
    Gen9_5VA = 2,
    Gen9_5MTLMojave = 3,
    Gen9_5VAMojave = 4,
    Gen9_5MTLCatalina = 5,
    Gen9_5VACatalina = 6,
};

static uint32_t kGen9_5MTLLength = sizeof("/System/Library/Extensions/AppleIntelKBLGraphicsMTLDriver.bundle/Contents/"
                                          "MacOS/AppleIntelKBLGraphicsMTLDriver") -
                                   1;
static uint32_t kGen9_5VALength =
    sizeof("/System/Library/Extensions/AppleIntelKBLGraphicsVADriver.bundle/Contents/MacOS/"
           "AppleIntelKBLGraphicsVADriver") -
    1;

static UserPatcher::ProcInfo kProcInfoGen9_5[] = {
    {kGen9_5MTLDriverPath, kGen9_5MTLLength, Gen9_5MTL},
    {kGen9_5VADriverPath, kGen9_5VALength, Gen9_5VA},
};

#endif /* kern_gen9_5_patches_hpp */
