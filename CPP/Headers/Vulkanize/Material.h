#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Material : RefCounted
  {
    Context*         context;
    Ref<Shader>      shader;
    DescriptorSet    descriptors;
    VkPipelineLayout pipeline_layout;
    VkPipeline       graphics_pipeline;
    bool             created = false;
    std::vector<Ref<VertexDescription>> vertex_descriptions;

    VkPrimitiveTopology                 topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32                            primitive_restart_enabled = VK_FALSE;
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    std::vector<VkDynamicState>         dynamic_states;
    // Constructor default: VK_DYNAMIC_STATE_VIEWPORT, _SCISSOR, and _DEPTH_BIAS
    // clear() and/or push_back() additional dynamic states externally or in create().

    std::vector<VkViewport> viewports;
    std::vector<VkRect2D>   scissor_rects;

    Material( Context* context );
    virtual ~Material() { destroy(); }

    virtual bool create();
    virtual void destroy();

    virtual Ref<CombinedImageSamplerDescriptor>
      add_combined_image_sampler( uint32_t binding, size_t initial_count=0 );
    virtual Ref<CombinedImageSamplerDescriptor>
      add_combined_image_sampler( uint32_t binding, Ref<Image> image, Ref<Sampler> sampler=Ref<Sampler>() );

    virtual void add_vertex_description( Ref<VertexDescription> vertex_description );

    virtual void enable_primitive_restart( VkBool32 setting ) { primitive_restart_enabled = setting; }
    virtual void set_shader( Ref<Shader> shader ) { this->shader = shader; }
    virtual void set_topology( VkPrimitiveTopology topology ) { this->topology = topology; }

    /*
    virtual void cmd_bind( VkCommandBuffer cmd );
    virtual void cmd_set_default_viewports_and_scissor_rects( VkCommandBuffer cmd );
    virtual void cmd_set_viewports_and_scissor_rects( VkCommandBuffer cmd );
    virtual void cmd_bind_descriptor_set( VkCommandBuffer cmd );

    virtual void set_default_scissor_rect( int index );
    virtual void set_default_viewport( int index, float min_depth=0, float max_depth=1.0f );

    virtual void set_scissor_rect( int index, VkExtent2D extent );
    virtual void set_scissor_rect( int index, VkOffset2D offset, VkExtent2D extent );

    virtual void set_viewport( int index, VkExtent2D extent,
                               float min_depth=0, float max_depth=1.0f );
    virtual void set_viewport( int index, VkOffset2D offset, VkExtent2D extent,
                               float min_depth=0, float max_depth=1.0f );
                               */

    // Internal configuration - override as needed
    /*
    virtual void _configure_color_blend_info( VkPipelineColorBlendStateCreateInfo& color_blend_info );
    virtual void _configure_multisampling_info( VkPipelineMultisampleStateCreateInfo& multisampling_info );
    virtual void _configure_pipeline_info( VkGraphicsPipelineCreateInfo& pipeline_info );
    virtual void _configure_pipeline_layout_info( VkPipelineLayoutCreateInfo& pipeline_layout_info );
    virtual void _configure_rasterizer_info( VkPipelineRasterizationStateCreateInfo& rasterizer_info );
    virtual void _configure_viewports_and_scissor_rects();
    */

  };

};
