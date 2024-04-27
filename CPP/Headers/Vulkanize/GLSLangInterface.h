#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct SPIRVBinary
  {
    int       size; // number of bytes in SPIR-V binary
    uint32_t* code; // SPIR-V words
  };

  VkShaderModule compile_shader( Context* context, VkShaderStageFlagBits stage, const std::string& filename,
                                 const std::string& shader_source );
  SPIRVBinary    compile_shader_to_spirv( int stage, const char* filename, const char* shader_source );
};
