#pragma once

#include "Vulkanize.h"

namespace VKZ
{
  struct Buffer
  {
    Context*              context;
    size_t                element_size;
    size_t                count;
    VkBufferUsageFlags    usage;
    VkMemoryPropertyFlags mem_properties;
    VkBuffer              vk_buffer;
    VkDeviceMemory        memory;
    int                   allocation_stage = 0;

    virtual ~Buffer() { destroy(); }

    virtual void cmd_bind( VkCommandBuffer cmd );
    virtual bool create( Context* context, size_t element_size, size_t count, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags mem_properties );
    virtual bool create_staging_buffer( Context* context, size_t element_size, size_t count );
    virtual bool create_vertex_buffer( Context* context, size_t element_size, size_t count );
    virtual bool copy_from( void* src_data, size_t n, size_t dest_index=0 );
    virtual bool copy_to( size_t src_index, size_t n, Buffer& dest, size_t dest_index=0 );
    virtual void destroy();
    virtual bool ensure_count( size_t required_count );
  };
};
