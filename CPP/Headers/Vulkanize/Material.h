#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Material
  {
    Context*                        context;
    GraphicsPipeline                gfx;
    std::vector<Shader*>            shader_stages;
    std::vector<VertexDescription*> vertex_descriptions;
    DescriptorSet                   descriptors;
    bool                            created = false;

    Material( Context* context );
    virtual ~Material() { destroy(); }

    virtual bool create();
    virtual void destroy();

    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   std::string shader_source, const char* main_function_name="main" );
    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   const char* spirv_bytes, size_t spirv_byte_count,
                                   const char* main_function_name="main" );
    virtual void add_shader_stage( Shader* shader );

    virtual CombinedImageSamplerDescriptor* add_combined_image_sampler(
        uint32_t binding, VkShaderStageFlags stage, Image* image, Sampler* sampler );

    virtual void add_vertex_description( VertexDescription* vertex_description );
  };

};
