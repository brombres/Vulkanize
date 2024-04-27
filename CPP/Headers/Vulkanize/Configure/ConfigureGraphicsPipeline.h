#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureGraphicsPipeline : ContextOperation<Context>
  {
    std::vector<ShaderStageInfo*>   shader_stages;
    std::vector<VertexDescription*> vertex_descriptions;

    virtual ~ConfigureGraphicsPipeline();

    virtual bool activate();
    virtual void add_shader_stage( VkShaderStageFlagBits stage, VkShaderModule module,
                                   const char* main_function_name="main" );
    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   std::string shader_source, const char* main_function_name="main" );
    virtual void add_shader_stage( VkShaderStageFlagBits stage, const char* spirv_bytes, size_t spirv_byte_count,
                                   const char* main_function_name="main" );
    virtual void add_vertex_description( VertexDescription* vertex_description );
    virtual void deactivate();

    VkShaderModule _create_shader_module( const Byte* code, int count );
  };
};
