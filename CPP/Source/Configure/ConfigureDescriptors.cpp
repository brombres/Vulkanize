#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

bool ConfigureDescriptors::on_activate()
{
  for (auto descriptor : descriptors->descriptor_info)
  {
    if ( !descriptor->activate(context) ) return false;
  }
  progress = 1;

  for (Descriptor* info : descriptors->descriptor_info)
  {
    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = info->binding;
    layout_binding.descriptorType = info->type;
    layout_binding.descriptorCount = info->count;
    layout_binding.stageFlags = info->stage;
    layout_binding.pImmutableSamplers = info->samplers;
    bindings.push_back( layout_binding );
  }
  progress = 2;

  // Descriptor Set Layout
  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = (uint32_t)bindings.size();
  layout_info.pBindings = bindings.data();

  VKZ_ATTEMPT(
    "creating descriptor set layout",
    context->device_dispatch.createDescriptorSetLayout( &layout_info, nullptr, &descriptors->layout ),
    return false
  );
  progress = 3;

  // Descriptor Pool
  uint32_t swapchain_count = context->swapchain_count;
  for (Descriptor* info : descriptors->descriptor_info)
  {
    VkDescriptorPoolSize pool_size = {};
    pool_size.type = info->type;
    pool_size.descriptorCount = swapchain_count;
    pool_sizes.push_back( pool_size );
  }

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = (uint32_t)pool_sizes.size();
  pool_info.pPoolSizes = pool_sizes.data();
  pool_info.maxSets = swapchain_count;

  VKZ_ATTEMPT(
    "creating descriptor pool",
    context->device_dispatch.createDescriptorPool( &pool_info, nullptr, &descriptors->pool ),
    return false;
  );
  progress = 4;

  // Descriptor Set
  vector<VkDescriptorSetLayout> layouts( swapchain_count, descriptors->layout );

  VkDescriptorSetAllocateInfo allocate_info = {};
  allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocate_info.descriptorPool = descriptors->pool;
  allocate_info.descriptorSetCount = swapchain_count;
  allocate_info.pSetLayouts = layouts.data();

  vector<VkDescriptorSet>& sets = descriptors->sets;
  sets.resize( swapchain_count );

  VKZ_ATTEMPT(
    "allocating descriptor sets",
    context->device_dispatch.allocateDescriptorSets( &allocate_info, sets.data() ),
    return false;
  );

  for (uint32_t i=0; i<swapchain_count; ++i)
  {
    vector<VkWriteDescriptorSet> descriptor_writes;
    if ( !descriptors->collect_descriptor_writes(i, descriptor_writes) ) return false;
  }

  progress = 5;

  return true;
}

void ConfigureDescriptors::on_deactivate()
{
  if (progress >= 3) context->device_dispatch.destroyDescriptorPool( descriptors->pool, nullptr );
  if (progress >= 2) context->device_dispatch.destroyDescriptorSetLayout( descriptors->layout, nullptr );

  if (progress >= 1)
  {
    for (auto descriptor : descriptors->descriptor_info) descriptor->deactivate();
  }

  pool_sizes.clear();
  bindings.clear();
}

