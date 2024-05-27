#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureDescriptorPool : ContextOperation<Context>
  {
    // PROPERTIES
    VkDescriptorPool* pool;
    uint32_t uniform_buffer_count; // will be multiplied by context->swapchain_count
    uint32_t max_sets; // will be multiplied by context->swapchain_count
    std::vector<VkDescriptorPoolSize> pool_sizes;

    // METHODS
    ConfigureDescriptorPool( VkDescriptorPool* pool, uint32_t uniform_buffer_count, uint32_t max_sets=1 )
      : pool(pool), uniform_buffer_count(uniform_buffer_count), max_sets(max_sets) {}

    virtual void add_pool_size( VkDescriptorType type, uint32_t count );
    virtual void add_uniform_buffers( uint32_t count );  // count will be multiplied by context->swapchain_count


    void on_configure() override;
    bool on_activate() override;
    void on_deactivate() override;
  };
};
