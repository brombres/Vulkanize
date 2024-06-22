#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// Material
//==============================================================================
Material::Material( Context* context ) : context(context)
{
}

bool Material::create()
{
  return true;
}

void Material::destroy()
{
  if (created)
  {
    created = false;
    for (auto shader : shader_stages) shader->release();

    shader_stages.clear();
  }

  for (auto description : vertex_descriptions)
  {
    delete description;
  }
  vertex_descriptions.clear();
}

void Material::add_shader_stage( VkShaderStageFlagBits stage,
    string shader_filename, string shader_source, const char* main_function_name )
{
  add_shader_stage(
    new Shader( context, stage, shader_filename, shader_source, main_function_name )
  );
}

void Material::add_shader_stage( VkShaderStageFlagBits stage, string shader_filename,
    const char* spirv_bytes, size_t spirv_byte_count, const char* main_function_name )
{
  add_shader_stage(
    new Shader( context, stage, shader_filename, spirv_bytes, spirv_byte_count, main_function_name )
  );
}

CombinedImageSamplerDescriptor* Material::add_combined_image_sampler(
    uint32_t binding, VkShaderStageFlags stage, Image* image, Sampler* sampler )
{
  CombinedImageSamplerDescriptor* descriptor = new CombinedImageSamplerDescriptor(
    context, binding, stage, image, sampler
  );
  descriptors.add( descriptor );
  return descriptor;
}

void Material::add_shader_stage( Shader* shader )
{
  shader_stages.push_back( shader->retain() );
}

void Material::add_vertex_description( VertexDescription* vertex_description )
{
  vertex_descriptions.push_back( vertex_description );
}

