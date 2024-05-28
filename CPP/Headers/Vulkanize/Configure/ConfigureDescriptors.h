#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureDescriptors : ContextOperation<Context>
  {
    Descriptors* descriptors;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    std::vector<VkDescriptorPoolSize>         pool_sizes;

    ConfigureDescriptors( Descriptors* descriptors ) : descriptors(descriptors) {}

    // Override on_configure() and call descriptors->add_uniform_buffer() etc.
    // void on_configure() override;

    bool on_activate() override;
    void on_deactivate() override;
  };
};
