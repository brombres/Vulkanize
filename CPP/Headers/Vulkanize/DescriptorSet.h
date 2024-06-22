#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct DescriptorSet
  {
    std::vector<Descriptor*> descriptors;

    void add( Descriptor* descriptor ) { descriptors.push_back(descriptor); }
  };
};
