#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

#include "glslang/Include/glslang_c_interface.h"

bool ConfigureStandardVertexBuffer::on_activate()
{
  std::vector<StandardVertex> vertices;
  vertices.push_back( StandardVertex( 0.0f,-0.5f, 0, 0xff0000ff) );
  vertices.push_back( StandardVertex( 0.5f, 0.5f, 0, 0xff00ff00) );
  vertices.push_back( StandardVertex(-0.5f, 0.5f, 0, 0xffff0000) );

  if ( !context->staging_buffer.create_staging_buffer(context,sizeof(StandardVertex),(uint32_t)vertices.size()) ) return false;

  context->staging_buffer.copy_from( vertices.data(), (uint32_t)vertices.size() );

  if ( !context->vertex_buffer.create_vertex_buffer(context,sizeof(StandardVertex),(uint32_t)vertices.size()) ) return false;

  context->vertex_buffer.copy_from( context->staging_buffer );

  return true;
}

void ConfigureStandardVertexBuffer::on_deactivate()
{
  context->vertex_buffer.destroy();
  context->staging_buffer.destroy();
}

