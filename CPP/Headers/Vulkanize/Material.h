#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Material : RefCounted
  {
    Context*                 context;
    GraphicsPipeline         gfx;
    std::vector<Ref<Shader>> shader_stages;
    std::vector<Ref<VertexDescription>> vertex_descriptions;
    DescriptorSet            descriptors;
    bool                     created = false;

    VkPrimitiveTopology      topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32                 primitive_restart_enabled = VK_FALSE;

    Material( Context* context );
    virtual ~Material() { destroy(); }

    virtual bool create();
    virtual void destroy();

    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   std::string shader_source, const char* main_function_name="main" );
    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   const char* spirv_bytes, size_t spirv_byte_count,
                                   const char* main_function_name="main" );
    virtual void add_shader_stage( Ref<Shader> shader );

    virtual Ref<CombinedImageSamplerDescriptor> add_combined_image_sampler(
        uint32_t binding, VkShaderStageFlags stage, size_t initial_count=0 );
    virtual Ref<CombinedImageSamplerDescriptor> add_combined_image_sampler(
        uint32_t binding, VkShaderStageFlags stage, Ref<Image> image, Ref<Sampler> sampler=Ref<Sampler>() );

    virtual void add_vertex_description( Ref<VertexDescription> vertex_description );

    virtual void enable_primitive_restart( VkBool32 setting ) { primitive_restart_enabled = setting; }
    virtual void set_topology( VkPrimitiveTopology topology ) { this->topology = topology; }
  };

};
