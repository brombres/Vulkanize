#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct DescriptorSet
  {
    Context*                                  context;
    std::vector<Descriptor*>                  descriptors;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    std::vector<VkDescriptorPoolSize>         pool_sizes;
    VkDescriptorSetLayout                     layout;
    VkDescriptorPool                          pool;
    std::vector<VkDescriptorSet>              swapchain_sets;
    bool                                      created = false;
    int                                       construction_stage = 0;

    bool create( Context* context );
    void destroy();

    void add( Descriptor* descriptor ) { descriptors.push_back(descriptor); }
    void cmd_bind( VkCommandBuffer cmd, GraphicsPipeline& pipeline );
    void update_descriptors( VkDescriptorSet& set, size_t swap_index );
  };
};
