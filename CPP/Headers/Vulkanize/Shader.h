#pragma once

#include <string>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Shader
  {
    const int SOURCE = 1;
    const int SPIRV  = 2;

    Context*              context = nullptr;
    VkShaderStageFlagBits stage;
    VkShaderModule        module = VK_NULL_HANDLE;
    std::string           main_function_name;
    std::string           shader_filename;
    std::string           shader_source;
    std::string           spirv_bytecode;
    int                   type;
    int                   refcount = 0;

    Shader( Context* context, VkShaderStageFlagBits stage, std::string shader_filename,
        std::string shader_source, std::string main_function_name="main" )
      : context(context), stage(stage), shader_filename(shader_filename), shader_source(shader_source),
        main_function_name(main_function_name)
    {
      type = SOURCE;
    }

    Shader( Context* context, VkShaderStageFlagBits stage, std::string shader_filename,
        const char* spirv_bytecode, size_t byte_count,
        std::string main_function_name="main" )
      : context(context), stage(stage), shader_filename(shader_filename), main_function_name(main_function_name)
    {
      type = SPIRV;
      this->spirv_bytecode.assign( spirv_bytecode, byte_count );
    }

    virtual ~Shader();

    virtual void           destroy();
    virtual bool           get_create_info( VkPipelineShaderStageCreateInfo& info );
    virtual VkShaderModule get_module();
    virtual void           release() { if ( !--refcount ) delete this; }
    virtual Shader*        retain() { ++refcount; return this; }
  };
};
