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

  /*
  for (uint32_t i=0; i<swapchain_count; ++i)
  {
    vector<VkWriteDescriptorSet> descriptor_writes;

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = textureSampler;

    VkDescriptorImageInfo imageInfo2 = {};
    imageInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo2.imageView = textureImageView2;
    imageInfo2.sampler = textureSampler2;

    std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].pImageInfo = &imageInfo;

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = descriptorSets[i];
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].dstArrayElement = 0;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[2].pImageInfo = &imageInfo2;

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
  }
  */

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

