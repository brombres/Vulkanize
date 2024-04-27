#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

ShaderStageInfo::~ShaderStageInfo()
{
  if (state == MODULE)
  {
    context->device_dispatch.destroyShaderModule( module, nullptr );
  }
  state = DESTROYED;
}
