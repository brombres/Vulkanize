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

  if ( !context->vertex_buffer.create(
    context,
    size,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  ) ) return false;

  void* data;
  context->device_dispatch.mapMemory( context->vertex_buffer.memory, 0, size, 0, &data );
  memcpy( data, context->vertices.data(), (size_t) size );
  context->device_dispatch.unmapMemory( context->vertex_buffer.memory);
  return true;
}

void ConfigureStandardVertexBuffer::deactivate()
{
  context->vertex_buffer.destroy();
}

