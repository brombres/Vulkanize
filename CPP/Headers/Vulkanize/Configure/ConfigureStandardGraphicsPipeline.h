#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureStandardGraphicsPipeline : ConfigureGraphicsPipeline
  {
    virtual void configure();
  };
};
