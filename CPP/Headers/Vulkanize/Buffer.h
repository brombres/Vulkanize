#pragma once

#include "Vulkanize.h"

namespace VKZ
{
  struct Buffer
  {
    Context*       context;
    VkDeviceSize   size;
    VkBuffer       vk_buffer;
    VkDeviceMemory memory;
    int            allocation_stage = 0;

    virtual ~Buffer() { destroy(); }

    virtual bool create( Context* context, VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags mem_properties );
    virtual void destroy();
    virtual void copy_from( void* src_data, size_t size, uint32_t dest_i=0 );
    virtual void copy_to( uint32_t src_i1, size_t size, Buffer& dest, uint32_t dest_i1=0 );
  };
};
