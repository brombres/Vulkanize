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

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = sizeof(StandardVertex) * context->vertices.size();
  buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VKZ_REQUIRE(
    "creating vertex buffer",
    vkCreateBuffer( context->device, &buffer_info, nullptr, &context->vertex_buffer )
  );
  progress = 1;

  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements( context->device,  context->vertex_buffer, &mem_requirements );

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  int mem_type = context->find_memory_type( mem_requirements.memoryTypeBits,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
  if (mem_type == -1)
  {
    VKZ_REPORT_ERROR( "finding memory type for vertex buffer" );
    return false;
  }
  alloc_info.memoryTypeIndex = (uint32_t) mem_type;

  VKZ_ON_ERROR(
    "allocating memory for vertex buffer",
    vkAllocateMemory( context->device, &alloc_info, nullptr, &context->vertex_buffer_memory),
    return false;
  );
  progress = 2;

  vkBindBufferMemory( context->device, context->vertex_buffer, context->vertex_buffer_memory, 0 );

  void* data;
  vkMapMemory( context->device, context->vertex_buffer_memory, 0, buffer_info.size, 0, &data );
  memcpy( data, context->vertices.data(), (size_t) buffer_info.size );
  vkUnmapMemory( context->device, context->vertex_buffer_memory);

  // Create a staging buffer with the VK_BUFFER_USAGE_TRANSFER_SRC_BIT flag.
  // Map the staging buffer memory once during setup.
  // Every frame, update the vertex data in the mapped staging buffer.
  // Use vkCmdCopyBuffer to copy the data from the staging buffer to the final vertex buffer.
  // Unmap the staging buffer memory once at the end of the program.
  //
  // This approach allows you to minimize the number of times you call vkMapMemory and vkUnmapMemory, while also taking advantage of the performance benefits of using a staging buffer for data transfer.
  //
  return true;
}

void ConfigureStandardVertexBuffer::deactivate()
{
  if (progress >= 2) vkDestroyBuffer( context->device, context->vertex_buffer, nullptr );
  if (progress >= 1) vkFreeMemory( context->device, context->vertex_buffer_memory, nullptr );
}

