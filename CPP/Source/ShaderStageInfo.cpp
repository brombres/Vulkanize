#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

ShaderStageInfo::~ShaderStageInfo()
{
  destroy_module();
}

void ShaderStageInfo::destroy_module()
{
  if (module != VK_NULL_HANDLE)
  {
    context->device_dispatch.destroyShaderModule( module, nullptr );
    module = VK_NULL_HANDLE;
  }
}

bool ShaderStageInfo::get_create_info( VkPipelineShaderStageCreateInfo& info )
{
  memset( &info, 0, sizeof(VkPipelineShaderStageCreateInfo) );
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = stage;

  info.module = get_module();
  if (info.module == VK_NULL_HANDLE) return false;

  info.pName = main_function_name.c_str();
  return true;
}

VkShaderModule ShaderStageInfo::get_module()
{
  if (module != VK_NULL_HANDLE) return module;

  if (type == SOURCE)
  {
    const char* shader_bytes;
    size_t shader_size;
    if (compile_shader(stage, shader_filename, shader_source, &shader_bytes, &shader_size))
    {
      type = SPIRV;
      this->spirv_bytecode.assign( shader_bytes, shader_size );
      delete shader_bytes;
    }
    else
    {
      return VK_NULL_HANDLE;
    }
  }

  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = spirv_bytecode.size();
  create_info.pCode = (const uint32_t*)spirv_bytecode.data();

  string action = "creating shader module '";
  action.append( shader_filename );
  action.append( "'" );
  VKZ_ATTEMPT(
    action.c_str(),
    context->device_dispatch.createShaderModule( &create_info, nullptr, &module ),
    return VK_NULL_HANDLE;
  );

  return module;
}
