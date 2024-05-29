#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

bool ConfigureVertexBuffers::on_activate()
{
  context->staging_buffers.resize( context->swapchain_count );
  context->vertex_buffers.resize( context->swapchain_count );

  for (int i=0; i<context->swapchain_count; ++i)
  {
    if ( !context->staging_buffers[i].create_staging_buffer(context,(uint32_t)sizeof_vertex) )
    {
      return false;
    }

    if ( !context->vertex_buffers[i].create_vertex_buffer(context,(uint32_t)sizeof_vertex) )
    {
      return false;
    }
  }

  return true;
}

void ConfigureVertexBuffers::on_deactivate()
{
  context->staging_buffers.clear();
  context->staging_buffer = nullptr;

  context->vertex_buffers.clear();
  context->vertex_buffer = nullptr;
}

