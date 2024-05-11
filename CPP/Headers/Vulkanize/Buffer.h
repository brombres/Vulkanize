#pragma once

#include "Vulkanize.h"

namespace VKZ
{
  struct Buffer
  {
    Context*       context;
    VkDeviceSize   size;
    VkBuffer       buffer;
    VkDeviceMemory memory;
    int            allocation_stage = 0;

    virtual ~Buffer() { destroy(); }

    virtual bool create( Context* context, VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags mem_properties );
    virtual void destroy();
  };
};
