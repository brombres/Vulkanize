#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

Ref<ShaderStage> Shader::add_fragment_shader( string shader_filename, string shader_source,
    string main_function_name )
{
  return add_shader( VK_SHADER_STAGE_FRAGMENT_BIT, shader_filename, shader_source, main_function_name );
}

Ref<ShaderStage> Shader::add_fragment_shader( string shader_filename, const char* spirv_bytecode,
    size_t byte_count, string main_function_name )
{
  return add_shader( VK_SHADER_STAGE_FRAGMENT_BIT, shader_filename,
      spirv_bytecode, byte_count, main_function_name );
}

Ref<ShaderStage> Shader::add_shader( VkShaderStageFlagBits stage, std::string shader_filename,
    std::string shader_source, std::string main_function_name )
{
  Ref<ShaderStage> result =
    new ShaderStage( context, stage, shader_filename, shader_source, main_function_name );
  add_stage( result );
  return result;
}

Ref<ShaderStage> Shader::add_shader( VkShaderStageFlagBits stage, std::string shader_filename,
    const char* spirv_bytecode, size_t byte_count, std::string main_function_name )
{
  Ref<ShaderStage> result =
    new ShaderStage( context, stage, shader_filename, spirv_bytecode, byte_count, main_function_name );
  add_stage( result );
  return result;
}

void Shader::add_stage( Ref<ShaderStage> stage )
{
  stages.push_back( stage );
}

Ref<ShaderStage> Shader::add_vertex_shader( string shader_filename, string shader_source,
    string main_function_name )
{
  return add_shader( VK_SHADER_STAGE_VERTEX_BIT, shader_filename, shader_source, main_function_name );
}

Ref<ShaderStage> Shader::add_vertex_shader( string shader_filename, const char* spirv_bytecode,
    size_t byte_count, string main_function_name )
{
  return add_shader( VK_SHADER_STAGE_VERTEX_BIT, shader_filename,
      spirv_bytecode, byte_count, main_function_name );
}

