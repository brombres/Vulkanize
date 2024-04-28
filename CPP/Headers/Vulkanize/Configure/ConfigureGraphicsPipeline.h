#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureGraphicsPipeline : ContextOperation<Context>
  {
    std::vector<ShaderStageInfo*>   shader_stages;
    std::vector<VertexDescription*> vertex_descriptions;

    std::vector<VkDynamicState>     dynamic_states;
    // Constructor default: VK_DYNAMIC_STATE_VIEWPORT, _SCISSOR, and _DEPTH_BIAS
    // clear() and/or push_back() additional dynamic states in configure().

    VkPrimitiveTopology                 topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32                            enable_primitive_restart = VK_FALSE;
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};

    ConfigureGraphicsPipeline();
    virtual ~ConfigureGraphicsPipeline();

    virtual bool activate();
    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   std::string shader_source, const char* main_function_name="main" );
    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   const char* spirv_bytes, size_t spirv_byte_count,
                                   const char* main_function_name="main" );
    virtual void add_vertex_description( VertexDescription* vertex_description );
    virtual void deactivate();

    virtual VkPipelineMultisampleStateCreateInfo   get_multisampling_config();
    virtual VkPipelineRasterizationStateCreateInfo get_rasterizer_config();
    virtual VkPipelineViewportStateCreateInfo      get_viewport_config();

  };
};
