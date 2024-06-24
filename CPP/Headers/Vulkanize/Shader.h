#pragma once

#include <string>
#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Shader : RefCounted
  {
    Context*                      context;
    std::vector<Ref<ShaderStage>> stages;

    Shader( Context* context ) : context(context) {}
    virtual ~Shader() {}

    Ref<ShaderStage> add_fragment_shader( std::string shader_filename, std::string shader_source,
        std::string main_function_name="main" );
    Ref<ShaderStage> add_fragment_shader( std::string shader_filename, const char* spirv_bytecode,
        size_t byte_count, std::string main_function_name="main" );

    Ref<ShaderStage> add_shader( VkShaderStageFlagBits stage, std::string shader_filename,
        std::string shader_source, std::string main_function_name="main" );
    Ref<ShaderStage> add_shader( VkShaderStageFlagBits stage, std::string shader_filename,
        const char* spirv_bytecode, size_t byte_count, std::string main_function_name="main" );

    void add_stage( Ref<ShaderStage> stage );

    Ref<ShaderStage> add_vertex_shader( std::string shader_filename, std::string shader_source,
        std::string main_function_name="main" );
    Ref<ShaderStage> add_vertex_shader( std::string shader_filename, const char* spirv_bytecode,
        size_t byte_count, std::string main_function_name="main" );
  };
};
