#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureGraphicsPipeline : ContextOperation<Context>
  {
    GraphicsPipeline* graphics_pipeline;

    std::vector<ShaderStageInfo*>   shader_stages;
    std::vector<VertexDescription*> vertex_descriptions;

    std::vector<VkDynamicState>     dynamic_states;
    // Constructor default: VK_DYNAMIC_STATE_VIEWPORT, _SCISSOR, and _DEPTH_BIAS
    // clear() and/or push_back() additional dynamic states in on_configure().

    VkPrimitiveTopology     topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32                enable_primitive_restart = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};

    ConfigureGraphicsPipeline( GraphicsPipeline* graphics_pipeline );
    virtual ~ConfigureGraphicsPipeline();

    virtual bool on_activate();
    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   std::string shader_source, const char* main_function_name="main" );
    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   const char* spirv_bytes, size_t spirv_byte_count,
                                   const char* main_function_name="main" );
    virtual void add_vertex_description( VertexDescription* vertex_description );
    virtual void on_deactivate();

    virtual void configure_color_blend_info( VkPipelineColorBlendStateCreateInfo& color_blend_info );
    virtual void configure_multisampling_info( VkPipelineMultisampleStateCreateInfo& multisampling_info );
    virtual void configure_pipeline_info( VkGraphicsPipelineCreateInfo& pipeline_info );
    virtual void configure_pipeline_layout_info( VkPipelineLayoutCreateInfo& pipeline_layout_info );
    virtual void configure_rasterizer_info( VkPipelineRasterizationStateCreateInfo& rasterizer_info );
    virtual void configure_viewports_and_scissor_rects();

    virtual void set_descriptors( Descriptors* descriptors ) { graphics_pipeline->descriptors = descriptors; }
  };
};
