#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// MaterialBuilder
//==============================================================================
void MaterialBuilder::add_shader_stage( VkShaderStageFlagBits stage,
    string shader_filename, string shader_source, const char* main_function_name )
{
  add_shader_stage(
    new Shader( context, stage, shader_filename, shader_source, main_function_name )
  );
}

MaterialBuilder::~MaterialBuilder()
{
  for (auto shader : shader_stages) shader->release();
  shader_stages.clear();

  for (auto description : vertex_descriptions)
  {
    delete description;
  }
  vertex_descriptions.clear();
}

void MaterialBuilder::add_shader_stage( VkShaderStageFlagBits stage, string shader_filename,
    const char* spirv_bytes, size_t spirv_byte_count, const char* main_function_name )
{
  add_shader_stage(
    new Shader( context, stage, shader_filename, spirv_bytes, spirv_byte_count, main_function_name )
  );
}

void MaterialBuilder::add_shader_stage( Shader* shader )
{
  shader_stages.push_back( shader->retain() );
}

void MaterialBuilder::add_vertex_description( VertexDescription* vertex_description )
{
  vertex_descriptions.push_back( vertex_description );
}

//==============================================================================
// Material
//==============================================================================
Material::Material( MaterialBuilder& builder )
{
  context = builder.context;

  for (Shader* shader : builder.shader_stages)
  {
    shader_stages.push_back( shader->retain() );
  }
}

void Material::destroy()
{
  if (created)
  {
    created = false;
    for (auto shader : shader_stages) shader->release();

    shader_stages.clear();
  }
}

