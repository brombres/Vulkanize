#pragma once

#include "Vulkanize.h"

namespace VKZ
{
  struct Buffer
  {
    Context*              context;
    uint32_t              element_size;
    uint32_t              count;
    uint32_t              capacity;
    VkBufferUsageFlags    usage;
    VkMemoryPropertyFlags mem_properties;
    VkBuffer              vk_buffer;
    VkDeviceMemory        memory;
    int                   allocation_stage = 0;
    void*                 mapped_memory = nullptr;

    virtual ~Buffer() { destroy(); }

    virtual void  clear() { count = 0; }
    virtual void  cmd_bind( VkCommandBuffer cmd, VkDeviceSize offset=0 );
    virtual bool  create( Context* context, uint32_t element_size, uint32_t initial_capacity, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags mem_properties );
    virtual bool  create_index_buffer( Context* context, uint32_t element_size, uint32_t initial_capacity=0 );
    virtual bool  create_staging_buffer( Context* context, uint32_t element_size, uint32_t initial_capacity=0 );
    virtual bool  create_uniform_buffer( Context* context, uint32_t size=0 );
    virtual bool  create_vertex_buffer( Context* context, uint32_t element_size, uint32_t initial_capacity=0 );
    virtual bool  copy_from( Buffer* src );
    virtual bool  copy_from( Buffer* src, uint32_t src_index, uint32_t n, uint32_t dest_index=0 );
    virtual bool  copy_from( void* src_data, uint32_t n, uint32_t dest_index=0 );
    virtual void  destroy();
    virtual bool  ensure_capacity( uint32_t required_capacity );
    virtual bool  is_ready() { return allocation_stage >= 2; }
    virtual void* map( uint32_t index=0, uint32_t element_count=-1 );
    virtual void  unmap();
  };
};
