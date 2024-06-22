#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct MaterialBuilder
  {
    // PROPERTIES
    Context* context;
    std::vector<Shader*> shader_stages;
    std::vector<VertexDescription*> vertex_descriptions;

    // METHODS
    MaterialBuilder( Context* context ) : context(context) {}
    virtual ~MaterialBuilder();

    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   std::string shader_source, const char* main_function_name="main" );
    virtual void add_shader_stage( VkShaderStageFlagBits stage, std::string shader_filename,
                                   const char* spirv_bytes, size_t spirv_byte_count,
                                   const char* main_function_name="main" );
    virtual void add_shader_stage( Shader* shader );
    virtual void add_vertex_description( VertexDescription* vertex_description );
  };

  struct Material
  {
    Context*             context;
    GraphicsPipeline     gfx;
    std::vector<Shader*> shader_stages;
    Descriptors          descriptors;
    bool                 created = false;

    Material( MaterialBuilder& builder );
    virtual ~Material() { destroy(); }

    virtual void destroy();
  };

};
