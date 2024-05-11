#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

#include "glslang/Include/glslang_c_interface.h"

bool ConfigureStandardVertexBuffer::activate()
{
  context->vertices.clear();
  context->vertices.push_back( StandardVertex( 0.0f,-0.5f, 0, 0xff0000ff) );
  context->vertices.push_back( StandardVertex( 0.5f, 0.5f, 0, 0xff00ff00) );
  context->vertices.push_back( StandardVertex(-0.5f, 0.5f, 0, 0xffff0000) );

  VkDeviceSize size = sizeof(StandardVertex) * context->vertices.size();

  Buffer staging_buffer;
  if ( !staging_buffer.create(
    context,
    size,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  ) ) return false;

  staging_buffer.copy_from( context->vertices.data(), (size_t) size );

  if ( !context->vertex_buffer.create(
    context,
    size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  ) ) return false;

  staging_buffer.copy_to( 0, size, context->vertex_buffer );
  staging_buffer.destroy();

  return true;
}

void ConfigureStandardVertexBuffer::deactivate()
{
  context->vertex_buffer.destroy();
}

