#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Material : RefCounted
  {
    // PROPERTIES
    Context*         context;
    Ref<Shader>      shader;
    DescriptorSet    descriptors;
    VkPipelineLayout pipeline_layout;
    VkPipeline       pipeline;
    std::vector<Ref<VertexDescription>> vertex_descriptions;

    bool created = false;
    int  pipeline_construction_stage = 0;

    VkPrimitiveTopology     topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32                primitive_restart_enabled = VK_FALSE;

    std::vector<VkViewport> viewports;
    std::vector<VkRect2D>   scissor_rects;

    // CONSTRUCTION
    Material( Context* context );
    virtual ~Material() { destroy(); }

    virtual bool create();
    virtual void destroy();

    // GENERAL INTERFACE
    virtual void cmd_bind( VkCommandBuffer cmd );
    virtual void cmd_set_default_viewports_and_scissor_rects( VkCommandBuffer cmd );
    virtual void cmd_set_viewports_and_scissor_rects( VkCommandBuffer cmd );

    virtual void set_default_scissor_rect( int index );
    virtual void set_default_viewport( int index, float min_depth=0, float max_depth=1.0f );

    virtual void set_scissor_rect( int index, VkExtent2D extent );
    virtual void set_scissor_rect( int index, VkOffset2D offset, VkExtent2D extent );

    virtual void set_viewport( int index, VkExtent2D extent,
                               float min_depth=0, float max_depth=1.0f );
    virtual void set_viewport( int index, VkOffset2D offset, VkExtent2D extent,
                               float min_depth=0, float max_depth=1.0f );

    // CONFIGURATION INTERFACE
    virtual Ref<CombinedImageSamplerDescriptor>
      add_combined_image_sampler( uint32_t binding, size_t initial_count=0 );
    virtual Ref<CombinedImageSamplerDescriptor>
      add_combined_image_sampler( uint32_t binding, Ref<Image> image, Ref<Sampler> sampler=Ref<Sampler>() );

    virtual Ref<SampledImageDescriptor>
      add_sampled_image( uint32_t binding, size_t initial_count=0 );
    virtual Ref<SampledImageDescriptor>
      add_sampled_image( uint32_t binding, Ref<Image> image );

    virtual Ref<SamplerDescriptor>
      add_sampler( uint32_t binding, size_t initial_count=0 );
    virtual Ref<SamplerDescriptor>
      add_sampler( uint32_t binding, Ref<Sampler> sampler );

    virtual void add_vertex_description( Ref<VertexDescription> vertex_description );
    virtual void enable_primitive_restart( VkBool32 setting ) { primitive_restart_enabled = setting; }
    virtual void set_shader( Ref<Shader> shader ) { this->shader = shader; }
    virtual void set_topology( VkPrimitiveTopology topology ) { this->topology = topology; }

    // Internal configuration - override as needed
    virtual bool _create_graphics_pipeline();
    virtual void _configure_multisampling_info( VkPipelineMultisampleStateCreateInfo& multisampling_info );
    virtual void _configure_color_blend_attachments(
        std::vector<VkPipelineColorBlendAttachmentState>& color_blend_attachments );
    virtual void _configure_color_blend_info( VkPipelineColorBlendStateCreateInfo& color_blend_info,
        std::vector<VkPipelineColorBlendAttachmentState>& color_blend_attachments );
    virtual void _configure_dynamic_states( std::vector<VkDynamicState>& dynamic_states );
    virtual void _configure_pipeline_info( VkGraphicsPipelineCreateInfo& pipeline_info );
    virtual void _configure_pipeline_layout_info( VkPipelineLayoutCreateInfo& pipeline_layout_info );
    virtual void _configure_rasterizer_info( VkPipelineRasterizationStateCreateInfo& rasterizer_info );
    virtual void _configure_viewports_and_scissor_rects();

  };

};
