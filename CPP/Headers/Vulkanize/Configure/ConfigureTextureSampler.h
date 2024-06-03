#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureTextureSampler : ContextOperation<Context>
  {
    // PROPERTIES
    VkSampler* vk_sampler;

    // METHODS
    ConfigureTextureSampler( VkSampler* vk_sampler );

    // Default VkSamplerCreateInfo is passed in; override and modify as desired.
    virtual void configure_sampler_info( VkSamplerCreateInfo& info ) {}

    virtual bool on_activate();
    virtual void on_deactivate();
  };
};
