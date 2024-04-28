#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

ShaderStageInfo::~ShaderStageInfo()
{
  if (module != VK_NULL_HANDLE)
  {
    context->device_dispatch.destroyShaderModule( module, nullptr );
    module = VK_NULL_HANDLE;
  }
}

/*
VkShaderModule ShaderStageInfo::get_module()
{
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = count;
  create_info.pCode = (const uint32_t*)code;

  VkShaderModule shader_module;
  VKZ_ON_ERROR(
    "creating shader module",
    context->device_dispatch.createShaderModule( &create_info, nullptr, &shader_module ),
    return VK_NULL_HANDLE;
  );

  return shader_module;
}
*/
