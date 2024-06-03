#include "VkBootstrap.h"
#include "Vulkanize/Vulkanize.h"
using namespace std;
using namespace VKZ;

ConfigureTextureSampler::ConfigureTextureSampler( VkSampler* vk_sampler )
  : vk_sampler(vk_sampler)
{
}

bool ConfigureTextureSampler::on_activate()
{
  VkSamplerCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.magFilter = VK_FILTER_LINEAR;
  info.minFilter = VK_FILTER_LINEAR;
  info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  VkPhysicalDeviceProperties properties = {};
  vkGetPhysicalDeviceProperties( context->physical_device, &properties );
  info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

  info.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
  info.unnormalizedCoordinates = VK_FALSE;

  info.compareEnable = VK_FALSE;
  info.compareOp = VK_COMPARE_OP_ALWAYS;

  info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  info.mipLodBias = 0.0f;
  info.minLod = 0.0f;
  info.maxLod = 0.0f;

  configure_sampler_info( info );

  VKZ_ATTEMPT(
    "creating sampler",
    context->device_dispatch.createSampler( &info, nullptr, vk_sampler ),
    return false
  );

  return true;
}

void ConfigureTextureSampler::on_deactivate()
{
  context->device_dispatch.destroySampler( *vk_sampler, nullptr );
}

