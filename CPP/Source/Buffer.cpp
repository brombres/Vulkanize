#include "Vulkanize.h"
using namespace VKZ;

void Buffer::cmd_bind( VkCommandBuffer cmd, VkDeviceSize offset )
{
  VkBuffer vertex_buffers[] = { vk_buffer };
  VkDeviceSize offsets[] = {offset};
  context->device_dispatch.cmdBindVertexBuffers( cmd, 0, 1, vertex_buffers, offsets );
}

bool Buffer::create( Context* context, uint32_t element_size, uint32_t initial_capacity, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags mem_properties )
{
  this->context = context;
  this->element_size = element_size;
  this->capacity = initial_capacity;
  this->count = 0;
  this->usage = usage;
  this->mem_properties = mem_properties;

  if (initial_capacity == 0) return true;

  VkDeviceSize size = element_size * capacity;

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

bool Buffer::create_index_buffer( Context* context, uint32_t element_size, uint32_t initial_capacity )
{
  return create(
    context,
    element_size,
    initial_capacity,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
}

bool Buffer::create_staging_buffer( Context* context, uint32_t element_size, uint32_t initial_capacity )
{
  return create(
    context,
    element_size,
    initial_capacity,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
}

bool Buffer::create_uniform_buffer( Context* context, uint32_t size )
{
  if ( !create(
    context,
    1,
    size,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  ) ) return false;

  count = size;
  return true;
}

bool Buffer::create_vertex_buffer( Context* context, uint32_t element_size, uint32_t initial_capacity )
{
  return create(
    context,
    element_size,
    initial_capacity,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
}

bool Buffer::copy_from( Buffer* src )
{
  return copy_from( src, 0, src->count, 0 );
}

bool Buffer::copy_from( Buffer* src, uint32_t src_index, uint32_t n, uint32_t dest_index )
{
  if ( !ensure_capacity( dest_index + n ) )     return false;
  if ( !(src->is_ready() && this->is_ready()) ) return false;

  VkCommandBuffer cmd = context->begin_cmd();

  VkBufferCopy copy_info = {};
  copy_info.srcOffset = src_index;
  copy_info.dstOffset = dest_index;
  copy_info.size = element_size * n;
  context->device_dispatch.cmdCopyBuffer( cmd, src->vk_buffer, vk_buffer, 1, &copy_info );

  context->end_cmd( cmd );

  if (count < dest_index + n) count = dest_index + n;

  return true;
}

bool Buffer::copy_from( void* src_data, uint32_t n, uint32_t dest_index )
{
  if (n == 0) return true;
  if ( !ensure_capacity( dest_index + n ) ) return false;
  if (allocation_stage < 2) return false;

  if (mapped_memory)
  {
    memcpy( (unsigned char*)mapped_memory + (dest_index*element_size), src_data, (uint32_t)(n*element_size) );
  }
  else
  {
    map();
    memcpy( (unsigned char*)mapped_memory + (dest_index*element_size), src_data, (uint32_t)(n*element_size) );
    unmap();
  }

  if (count < dest_index + n) count = dest_index + n;

  return true;
}

void Buffer::destroy()
{
  unmap();
  if (allocation_stage >= 2) context->device_dispatch.destroyBuffer( vk_buffer, nullptr );
  if (allocation_stage >= 1) context->device_dispatch.freeMemory( memory, nullptr );
  allocation_stage = 0;
}

bool Buffer::ensure_capacity( uint32_t required_capacity )
{
  if (capacity >= required_capacity) return true;

  bool mapped = (mapped_memory != nullptr);
  destroy();
  if ( !create(context, element_size, required_capacity, usage, mem_properties) ) return false;
  if (mapped) return true;
  return true;
}

void* Buffer::map( uint32_t index, uint32_t element_count )
{
  if (mapped_memory) return mapped_memory;
  if (capacity == 0) {
    VKZ_REPORT_ERROR( "mapping memory of zero-capacity buffer" );
    return 0;
  }

  if (element_count == -1) element_count = capacity - index;

  context->device_dispatch.mapMemory( memory, index*element_size, element_count*element_size, 0, &mapped_memory );

  return mapped_memory;
}

void Buffer::unmap()
{
  if (mapped_memory)
  {
    context->device_dispatch.unmapMemory( memory );
    mapped_memory = nullptr;
  }
}

