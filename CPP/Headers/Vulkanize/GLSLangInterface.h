#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  bool compile_shader( VkShaderStageFlagBits stage, const std::string& filename,
                       const std::string& shader_source, const char** bytecode, size_t* size );
};
