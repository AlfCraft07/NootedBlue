//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_gen8_patches_hpp
#define kern_gen8_patches_hpp
#include <Headers/kern_user.hpp>
#include <Headers/kern_util.hpp>
/**
 * `Broadwell Bundle paths`
 */

static const char kGen8MTLDriverPath[] = "/System/Library/Extensions/AppleIntelBDWGraphicsMTLDriver.bundle/Contents/"
                                         "MacOS/AppleIntelBDWGraphicsMTLDriver";
static const char kGen8VADriverPath[] = "/System/Library/Extensions/AppleIntelBDWGraphicsVADriver.bundle/Contents/"
                                        "MacOS/AppleIntelBDWGraphicsVADriver";
/**
 * `Braswell & Broadwell Patch Set for Mojave`
 *  In active development, likely unstable
 */
// JA to NO-OP, avoids _panic call 'Failed to map Device ID to GPU SKU' 1/2
static const uint8_t kGen8HWMojave1Original[] = {0x77, 0x5A, 0xB9, 0x11, 0x1B, 0x00, 0x00};
static const uint8_t kGen8HWMojave1Patched[] = {0x90, 0x90, 0xB9, 0x11, 0x1B, 0x00, 0x00};
// JNC to NO-OP & 02 to 00, avoids _panic call 'Failed to map Device ID to GPU SKU' 2/2
static const uint8_t kGen8HWMojave2Original[] = {0x73, 0x34, 0x41, 0xC7, 0x86, 0x84, 0x0F, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x00, 0xEB, 0x18};
static const uint8_t kGen8HWMojave2Patched[] = {0x90, 0x90, 0x41, 0xC7, 0x86, 0x84, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xEB, 0x18};
// 01 value to 00
static const uint8_t kGen8HWMojave3Original[] = {0x41, 0xC7, 0x86, 0x84, 0x0F, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
static const uint8_t kGen8HWMojave3Patched[] = {0x41, 0xC7, 0x86, 0x84, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/** Based off of WhateverGreen code*/

enum : uint32_t {
    Gen8MTL = 1,
    Gen8VA = 2,
    Gen8MTLMojave = 3,
    Gen8VAMojave = 4,
    Gen8MTLCatalina = 5,
    Gen8VACatalina = 6,
};

static uint32_t kGen8MTLLength = sizeof("/System/Library/Extensions/AppleIntelBDWGraphicsMTLDriver.bundle/Contents/"
                                        "MacOS/AppleIntelBDWGraphicsMTLDriver") -
                                 1;
static uint32_t kGen8VALength = sizeof("/System/Library/Extensions/AppleIntelBDWGraphicsVADriver.bundle/Contents/MacOS/"
                                       "AppleIntelBDWGraphicsVADriver") -
                                1;

static UserPatcher::ProcInfo kProcInfoGen8[] = {
    {kGen8MTLDriverPath, kGen8MTLLength, Gen8MTL},
    {kGen8VADriverPath, kGen8VALength, Gen8VA},
};

#endif /* kern_gen8_patches_hpp */
