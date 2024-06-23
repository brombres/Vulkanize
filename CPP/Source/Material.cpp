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
    shader_stages.clear();
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

Ref<CombinedImageSamplerDescriptor> Material::add_combined_image_sampler(
    uint32_t binding, VkShaderStageFlags stage, size_t initial_count )
{
  CombinedImageSamplerDescriptor* descriptor = new CombinedImageSamplerDescriptor(
    context, binding, stage, initial_count
  );
  descriptors.add( descriptor );
  return descriptor;
}

Ref<CombinedImageSamplerDescriptor> Material::add_combined_image_sampler(
    uint32_t binding, VkShaderStageFlags stage, Ref<Image> image, Ref<Sampler> sampler )
{
  if ( !sampler.exists() )
  {
    // Create default sampler
    SamplerInfo sampler_info( context );
    sampler = new Sampler( sampler_info );
  }

  CombinedImageSamplerDescriptor* descriptor = new CombinedImageSamplerDescriptor(
    context, binding, stage, image, sampler
  );
  descriptors.add( descriptor );
  return descriptor;
}

void Material::add_shader_stage( Ref<Shader> shader )
{
  shader_stages.push_back( shader );
}

void Material::add_vertex_description( Ref<VertexDescription> vertex_description )
{
  vertex_descriptions.push_back( vertex_description );
}

