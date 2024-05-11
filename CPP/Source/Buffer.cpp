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

  VKZ_REQUIRE(
    "creating buffer",
    context->device_dispatch.createBuffer( &buffer_info, nullptr, &buffer )
  );
  allocation_stage = 1;

  VkMemoryRequirements mem_requirements;
  context->device_dispatch.getBufferMemoryRequirements( buffer, &mem_requirements );

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

  VKZ_ON_ERROR(
    "allocating memory for buffer",
    context->device_dispatch.allocateMemory( &alloc_info, nullptr, &memory),
    destroy(); return false;
  );
  allocation_stage = 2;

  context->device_dispatch.bindBufferMemory( buffer, memory, 0 );
  return true;
}

void Buffer::destroy()
{
  if (allocation_stage >= 2) context->device_dispatch.destroyBuffer( buffer, nullptr );
  if (allocation_stage >= 1) context->device_dispatch.freeMemory( memory, nullptr );
  allocation_stage = 0;
}

