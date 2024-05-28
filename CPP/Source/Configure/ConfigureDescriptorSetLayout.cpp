#include "Vulkanize/Vulkanize.h"
using namespace VKZ;

void ConfigureDescriptorSetLayout::add_binding( uint32_t index, VkDescriptorType type,
    VkShaderStageFlags stage, uint32_t count, VkSampler* samplers )
{
  VkDescriptorSetLayoutBinding binding = {};
  binding.binding = index;
  binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  binding.descriptorCount = count;
  binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  binding.pImmutableSamplers = samplers;
  bindings.push_back( binding );
}

bool ConfigureDescriptorSetLayout::on_activate()
{
  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = (uint32_t)bindings.size();
  layout_info.pBindings = bindings.data();

  VKZ_ATTEMPT(
    "creating descriptor set layout",
    context->device_dispatch.createDescriptorSetLayout( &layout_info, nullptr, layout ),
    return false
  );

  return true;
}

void ConfigureDescriptorSetLayout::on_deactivate()
{
  context->device_dispatch.destroyDescriptorSetLayout( *layout, nullptr );
}

