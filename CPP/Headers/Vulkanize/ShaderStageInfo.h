#pragma once

#include <string>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ShaderStageInfo
  {
    const int MODULE    = 1;
    const int SOURCE    = 2;
    const int SPIRV     = 3;
    const int DESTROYED = 3;

    Context*              context = nullptr;
    VkShaderStageFlagBits stage;
    VkShaderModule        module;
    std::string           main_function_name;
    std::string           shader_filename;
    std::string           shader_source;
    std::string           spirv_bytecode;
    int                   state;

    ShaderStageInfo( VkShaderStageFlagBits stage, VkShaderModule module, const char* main_function_name="main" )
      : stage(stage), module(module), main_function_name(main_function_name)
    {
      state = MODULE;
    }

    ShaderStageInfo( VkShaderStageFlagBits stage, std::string shader_filename,
        std::string shader_source, const char* main_function_name="main" )
      : stage(stage), shader_filename(shader_filename), shader_source(shader_source),
        main_function_name(main_function_name)
    {
      state = SOURCE;
    }

    ShaderStageInfo( VkShaderStageFlagBits stage, const char* spirv_bytecode, size_t byte_count,
        const char* main_function_name="main" )
      : stage(stage), main_function_name(main_function_name)
    {
      state = SPIRV;
      this->spirv_bytecode.assign( spirv_bytecode, byte_count );
    }

    ~ShaderStageInfo();
  };
};
