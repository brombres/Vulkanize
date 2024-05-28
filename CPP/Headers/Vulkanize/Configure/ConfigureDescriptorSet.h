#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureDescriptorSet : ContextOperation<Context>
  {
    // PROPERTIES
    VkDescriptorSet*       set;
    VkDescriptorPool*      pool;
    VkDescriptorSetLayout* layout;

    // METHODS
    ConfigureDescriptorSet(
      VkDescriptorSet*       set,
      VkDescriptorPool*      pool,
      VkDescriptorSetLayout* layout
    ) : set(set), pool(pool), layout(layout) {}

    // Override on_activate() in an extended class.
    // bool on_activate() override;
  };
};
