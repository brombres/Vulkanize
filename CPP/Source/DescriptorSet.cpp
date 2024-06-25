#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// DescriptorSet
//==============================================================================
bool DescriptorSet::create( Context* context )
{
  if (construction_stage) return created;

  this->context = context;

  bindings.clear();
  for (auto& descriptor : descriptors)
  {
    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = descriptor->binding;
    layout_binding.descriptorType = descriptor->type;
    layout_binding.descriptorCount = descriptor->count;
    layout_binding.stageFlags = descriptor->stage;
    layout_binding.pImmutableSamplers = descriptor->immutable_samplers;
    bindings.push_back( layout_binding );
  }

  // Set Layout
  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = (uint32_t)bindings.size();
  layout_info.pBindings = bindings.data();

  VKZ_ATTEMPT(
    "creating descriptor set layout",
    context->device_dispatch.createDescriptorSetLayout( &layout_info, nullptr, &layout ),
    return false
  );
  construction_stage = 1;

  // Descriptor Pool
  uint32_t swapchain_count = context->swapchain_count;
  for (auto& descriptor : descriptors)
  {
    VkDescriptorPoolSize pool_size = {};
    pool_size.type = descriptor->type;
    pool_size.descriptorCount = descriptor->count * swapchain_count;  // array count * swapchain count
    pool_sizes.push_back( pool_size );
  }

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = (uint32_t)pool_sizes.size();
  pool_info.pPoolSizes = pool_sizes.data();
  pool_info.maxSets = swapchain_count;

  VKZ_ATTEMPT(
    "creating descriptor pool",
    context->device_dispatch.createDescriptorPool( &pool_info, nullptr, &pool ),
    return false;
  );
  construction_stage = 2;

  // Descriptor Sets (one per swapchain frame)
  vector<VkDescriptorSetLayout> layouts( swapchain_count, layout );

  VkDescriptorSetAllocateInfo allocate_info = {};
  allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocate_info.descriptorPool = pool;
  allocate_info.descriptorSetCount = swapchain_count;
  allocate_info.pSetLayouts = layouts.data();

  swapchain_sets.resize( swapchain_count );

  VKZ_ATTEMPT(
    "allocating descriptor sets",
    context->device_dispatch.allocateDescriptorSets( &allocate_info, swapchain_sets.data() ),
    return false;
  );
  construction_stage = 3;

  created = true;
  return true;
}

void DescriptorSet::destroy()
{
  if (construction_stage >= 2) context->device_dispatch.destroyDescriptorPool( pool, nullptr );
  if (construction_stage >= 1) context->device_dispatch.destroyDescriptorSetLayout( layout, nullptr );

  pool_sizes.clear();
  bindings.clear();

  created = false;
  construction_stage = 0;
}

void DescriptorSet::cmd_bind( VkCommandBuffer cmd, VkPipelineLayout pipeline_layout )
{
  size_t swap_index = context->swap_index;
  VkDescriptorSet& set = swapchain_sets[swap_index];
  update_descriptor_set( set, swap_index );

  context->device_dispatch.cmdBindDescriptorSets( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline_layout, 0, 1, &set, 0, nullptr );
}

void DescriptorSet::update_descriptor_set( VkDescriptorSet& set, size_t swap_index )
{
  for (auto& descriptor : descriptors)
  {
    descriptor->update_descriptor_set( set, swap_index );
  }
}
