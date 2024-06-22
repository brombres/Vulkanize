#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

bool ConfigureDescriptors::on_activate()
{
  if ( !descriptors->activate(context) ) return false;
  progress = 1;

  for (OldDescriptor* info : descriptors->descriptors)
  {
    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = info->binding;
    layout_binding.descriptorType = info->type;
    layout_binding.descriptorCount = info->count;
    layout_binding.stageFlags = info->stage;
    layout_binding.pImmutableSamplers = info->immutable_samplers;
    bindings.push_back( layout_binding );
  }
  progress = 2;

  // OldDescriptor Set Layout
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

  // OldDescriptor Pool
  uint32_t swapchain_count = context->swapchain_count;
  for (OldDescriptor* info : descriptors->descriptors)
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

  // OldDescriptor Set
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

  if ( !descriptors->configure_descriptor_sets() ) return false;

  progress = 5;

  return true;
}

void ConfigureDescriptors::on_deactivate()
{
  if (progress >= 3) context->device_dispatch.destroyDescriptorPool( descriptors->pool, nullptr );
  if (progress >= 2) context->device_dispatch.destroyDescriptorSetLayout( descriptors->layout, nullptr );
  if (progress >= 1) descriptors->deactivate();

  pool_sizes.clear();
  bindings.clear();
}

