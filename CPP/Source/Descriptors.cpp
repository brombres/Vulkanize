#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
//  Descriptor
//==============================================================================
bool Descriptor::activate( Context* context )
{
  if (activated) return true;
  this->context = context;
  if ( !on_activate() ) return false;
  activated = true;
  return true;
}

void Descriptor::deactivate()
{
  if ( !activated ) return;
  on_deactivate();
  activated = false;
}

//==============================================================================
//  Descriptors
//==============================================================================
Descriptors::~Descriptors()
{
  for (Descriptor* descriptor : descriptors)
  {
    delete descriptor;
  }
  descriptors.clear();
}

bool Descriptors::activate( Context* context )
{
  if (activated) return true;
  this->context = context;

  for (auto descriptor : descriptors)
  {
    if ( !descriptor->activate(context) ) return false;
  }

  activated = true;

  return true;
}

void Descriptors::deactivate()
{
  if ( !activated ) return;
  for (auto descriptor : descriptors) descriptor->deactivate();
}

//Descriptor* Descriptors::add_combined_image_sampler( uint32_t binding, VkShaderStageFlags stage,
//    VkSampler* samplers, uint32_t count )
//{
//  return add_descriptor(
//    binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage, samplers, count
//  );
//}
//
//Descriptor* Descriptors::add_descriptor( uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage,
//    VkSampler* samplers, uint32_t count )
//{
//  Descriptor* descriptor = new Descriptor();
//  descriptor->binding  = binding;
//  descriptorcount    = count;
//  descriptor->type     = type;
//  descriptor->stage    = stage;
//  descriptor->samplers = samplers;
//  descriptors.push_back( descriptor );
//  return descriptor;
//}

bool Descriptors::configure_descriptor_sets( uint32_t swap_index )
{
  for (Descriptor* descriptor : descriptors)
  {
    if ( !descriptor->configure_descriptor_set(swap_index, sets[swap_index]) ) return false;
  }
  return true;
}

