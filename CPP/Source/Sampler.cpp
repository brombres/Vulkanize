#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

//==============================================================================
// SamplerInfo
//==============================================================================
SamplerInfo::SamplerInfo( Context* context ) : context(context)
{
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  VkPhysicalDeviceProperties properties = {};
  vkGetPhysicalDeviceProperties( context->physical_device, &properties );
  sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

  sampler_info.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;

  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;
}

//==============================================================================
// Sampler
//==============================================================================
Sampler::Sampler( SamplerInfo& info ) : context(context)
{
  this->context = info.context;

  exists = true;
  VKZ_ATTEMPT(
    "creating sampler",
    context->device_dispatch.createSampler( &info.sampler_info, nullptr, &vk_sampler ),
    exists = false;
  );
}

Sampler::~Sampler()
{
  if (exists)
  {
    exists = false;
    context->device_dispatch.destroySampler( vk_sampler, nullptr );
  }
}
