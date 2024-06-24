#include <string>
using namespace std;

#include "glslang/Include/glslang_c_interface.h"

// Required for use of glslang_default_resource
#include <glslang/Public/resource_limits_c.h>

#include "Vulkanize/GLSLangInterface.h"

using namespace VKZ;

bool VKZ::compile_shader( VkShaderStageFlagBits stage, const std::string& filename,
                          const std::string& shader_source, const char** bytecode, size_t* size )
{
  glslang_stage_t glsl_stage;
  switch (stage)
  {
    case VK_SHADER_STAGE_VERTEX_BIT:                  glsl_stage = GLSLANG_STAGE_VERTEX; break;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:    glsl_stage = GLSLANG_STAGE_TESSCONTROL; break;
    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: glsl_stage = GLSLANG_STAGE_TESSEVALUATION; break;
    case VK_SHADER_STAGE_GEOMETRY_BIT:                glsl_stage = GLSLANG_STAGE_GEOMETRY; break;
    case VK_SHADER_STAGE_FRAGMENT_BIT:                glsl_stage = GLSLANG_STAGE_FRAGMENT; break;
    case VK_SHADER_STAGE_COMPUTE_BIT:                 glsl_stage = GLSLANG_STAGE_COMPUTE; break;
    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:              glsl_stage = GLSLANG_STAGE_RAYGEN; break;
    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:             glsl_stage = GLSLANG_STAGE_ANYHIT; break;
    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:         glsl_stage = GLSLANG_STAGE_CLOSESTHIT; break;
    case VK_SHADER_STAGE_MISS_BIT_KHR:                glsl_stage = GLSLANG_STAGE_MISS; break;
    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:        glsl_stage = GLSLANG_STAGE_INTERSECT; break;
    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:            glsl_stage = GLSLANG_STAGE_CALLABLE; break;
    case VK_SHADER_STAGE_TASK_BIT_EXT:                glsl_stage = GLSLANG_STAGE_TASK; break;
    case VK_SHADER_STAGE_MESH_BIT_EXT:                glsl_stage = GLSLANG_STAGE_MESH; break;
    default:
      VKZ_LOG_ERROR( "[Vulkanize] Internal error: unhandled VkShaderStageFlagBits stage in Context::compile_shader().\n" );
      return false;
  }

  const glslang_input_t input = {
    .language = GLSLANG_SOURCE_GLSL,
    .stage = (glslang_stage_t)glsl_stage,
    .client = GLSLANG_CLIENT_VULKAN,
    .client_version = GLSLANG_TARGET_VULKAN_1_2,
    .target_language = GLSLANG_TARGET_SPV,
    .target_language_version = GLSLANG_TARGET_SPV_1_5,
    .code = shader_source.c_str(),
    .default_version = 100,
    .default_profile = GLSLANG_NO_PROFILE,
    .force_default_version_and_profile = false,
    .forward_compatible = false,
    .messages = GLSLANG_MSG_DEFAULT_BIT,
    .resource = glslang_default_resource(),
  };

  glslang_shader_t* shader = glslang_shader_create( &input );

  if (!glslang_shader_preprocess(shader, &input))	{
    VKZ_LOG_ERROR( "[Vulkanize] Preprocessing error compiling shader '%s'.\n", filename.c_str() );
    VKZ_LOG_ERROR( "%s\n", glslang_shader_get_info_log(shader) );
    VKZ_LOG_ERROR( "%s\n", glslang_shader_get_info_debug_log(shader) );
    VKZ_LOG_ERROR( "%s\n", input.code );
    glslang_shader_delete( shader );
    return false;
  }

  if (!glslang_shader_parse(shader, &input))
  {
    VKZ_LOG_ERROR( "[Vulkanize] Parse error compiling shader '%s'.\n", filename.c_str() );
    VKZ_LOG_ERROR( "%s\n", glslang_shader_get_info_log(shader) );
    VKZ_LOG_ERROR( "%s\n", glslang_shader_get_info_debug_log(shader) );
    VKZ_LOG_ERROR( "%s\n", glslang_shader_get_preprocessed_code(shader) );
    glslang_shader_delete( shader );
    return false;
  }

  glslang_program_t* program = glslang_program_create();
  glslang_program_add_shader(program, shader);

  if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
  {
    VKZ_LOG_ERROR( "[Vulkanize] Link error compiling shader '%s'.\n", filename.c_str() );
    VKZ_LOG_ERROR( "%s\n", glslang_program_get_info_log(program) );
    VKZ_LOG_ERROR( "%s\n", glslang_program_get_info_debug_log(program) );
    glslang_program_delete( program );
    glslang_shader_delete( shader );
    return false;
  }

  glslang_program_SPIRV_generate( program, (glslang_stage_t)glsl_stage );

  *size = glslang_program_SPIRV_get_size(program) * sizeof(uint32_t);
  *bytecode = (const char*) malloc( *size );
  glslang_program_SPIRV_get( program, (uint32_t*)*bytecode );

  const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
  if (spirv_messages) printf("[Vulkanize] ShaderStage compilation: (%s) %s\b", filename.c_str(), spirv_messages);

  glslang_program_delete( program );
  glslang_shader_delete( shader );

  return true;
}

