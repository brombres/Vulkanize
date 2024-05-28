#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

void ConfigureDescriptorPool::add_pool_size( VkDescriptorType type, uint32_t count )
{
  if (count == 0) count = context->swapchain_count;

  VkDescriptorPoolSize pool_size = {};
  pool_size.type = type;
  pool_size.descriptorCount = count;
  pool_sizes.push_back( pool_size );
}

void ConfigureDescriptorPool::add_combined_image_sampler()
{
  add_pool_size( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, context->swapchain_count );
}

void ConfigureDescriptorPool::add_uniform_buffer()
{
  add_pool_size( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, context->swapchain_count );
}

bool ConfigureDescriptorPool::on_activate()
{
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = (uint32_t)pool_sizes.size();
  pool_info.pPoolSizes = pool_sizes.data();
  pool_info.maxSets = max_sets;

  VKZ_ATTEMPT(
    "creating descriptor pool",
    context->device_dispatch.createDescriptorPool( &pool_info, nullptr, pool ),
    return false;
  );

  return true;
}

void ConfigureDescriptorPool::on_deactivate()
{
  context->device_dispatch.destroyDescriptorPool( *pool, nullptr );
}

