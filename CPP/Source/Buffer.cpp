#include "Vulkanize.h"
using namespace VKZ;

bool Buffer::create( Context* context, VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags mem_properties )
{
  this->context = context;
  this->size = size;

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size  = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VKZ_ATTEMPT(
    "creating buffer",
    context->device_dispatch.createBuffer( &buffer_info, nullptr, &vk_buffer ),
    return false
  );
  allocation_stage = 1;

  VkMemoryRequirements mem_requirements;
  context->device_dispatch.getBufferMemoryRequirements( vk_buffer, &mem_requirements );

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  int mem_type = context->find_memory_type( mem_requirements.memoryTypeBits, mem_properties );
  if (mem_type == -1)
  {
    VKZ_REPORT_ERROR( "finding memory type for buffer" );
    return false;
  }
  alloc_info.memoryTypeIndex = (uint32_t) mem_type;

  VKZ_ATTEMPT(
    "allocating memory for buffer",
    context->device_dispatch.allocateMemory( &alloc_info, nullptr, &memory),
    destroy(); return false;
  );
  allocation_stage = 2;

  context->device_dispatch.bindBufferMemory( vk_buffer, memory, 0 );
  return true;
}

void Buffer::destroy()
{
  if (allocation_stage >= 2) context->device_dispatch.destroyBuffer( vk_buffer, nullptr );
  if (allocation_stage >= 1) context->device_dispatch.freeMemory( memory, nullptr );
  allocation_stage = 0;
}

void Buffer::copy_from( void* src_data, size_t size, uint32_t dest_i )
{
  void* dest_data;
  context->device_dispatch.mapMemory( memory, dest_i, size, 0, &dest_data );
  memcpy( dest_data, src_data, (size_t) size );
  context->device_dispatch.unmapMemory( memory);
}

void Buffer::copy_to( uint32_t src_i1, size_t size, Buffer& dest, uint32_t dest_i1 )
{
  // We may want to use a separate command pool with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = context->command_pool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer cmd;
  context->device_dispatch.allocateCommandBuffers( &alloc_info, &cmd );

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  context->device_dispatch.beginCommandBuffer( cmd, &begin_info );

  VkBufferCopy copy_info = {};
  copy_info.srcOffset = src_i1;
  copy_info.dstOffset = dest_i1;
  copy_info.size = size;
  context->device_dispatch.cmdCopyBuffer( cmd, vk_buffer, dest.vk_buffer, 1, &copy_info );

  context->device_dispatch.endCommandBuffer( cmd );

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &cmd;

  context->device_dispatch.queueSubmit( context->graphics_queue, 1, &submit_info, VK_NULL_HANDLE );
  context->device_dispatch.queueWaitIdle( context->graphics_queue );

  context->device_dispatch.freeCommandBuffers( context->command_pool, 1, &cmd );
}
