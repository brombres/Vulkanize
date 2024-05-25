#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

#include "glslang/Include/glslang_c_interface.h"

bool ConfigureStandardVertexBuffer::on_activate()
{
  context->staging_buffers.resize( context->swapchain_count );
  context->vertex_buffers.resize( context->swapchain_count );

  for (int i=0; i<context->swapchain_count; ++i)
  {
    if ( !context->staging_buffers[i].create_staging_buffer(context,sizeof(StandardVertex)) )
    {
      return false;
    }

    if ( !context->vertex_buffers[i].create_vertex_buffer(context,sizeof(StandardVertex)) )
    {
      return false;
    }
  }

  return true;
}

void ConfigureStandardVertexBuffer::on_deactivate()
{
  context->staging_buffers.clear();
  context->staging_buffer = nullptr;

  context->vertex_buffers.clear();
  context->vertex_buffer = nullptr;
}

