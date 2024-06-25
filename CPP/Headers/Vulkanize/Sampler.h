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

  struct Sampler : RefCounted
  {
    static Ref<Sampler> create_default( Context* context );

    Context*   context;
    VkSampler  vk_sampler;
    bool       exists = false;
    bool       error = false;

    Sampler() {}
    Sampler( SamplerInfo& info );

    bool create( SamplerInfo& info );
    void destroy();

    virtual ~Sampler();
  };
};
