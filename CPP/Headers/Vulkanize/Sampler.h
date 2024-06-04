#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct SamplerInfo
  {
    VkSamplerCreateInfo sampler_info = {};
    Context*   context;

    SamplerInfo( Context* context );
  };

  struct Sampler
  {
    Context*   context;
    VkSampler  vk_sampler;
    bool       exists = false;

    Sampler( SamplerInfo& info );
    virtual ~Sampler();
  };
};
