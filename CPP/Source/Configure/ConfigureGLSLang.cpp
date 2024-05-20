#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

#include "glslang/Include/glslang_c_interface.h"

bool ConfigureGLSLang::on_activate()
{
  glslang_initialize_process();
  return true;
}

void ConfigureGLSLang::on_deactivate()
{
  glslang_finalize_process();
}

