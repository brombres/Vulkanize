#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureStandardGraphicsPipeline : ConfigureGraphicsPipeline
  {
    ConfigureStandardGraphicsPipeline( GraphicsPipeline* graphics_pipeline )
      : ConfigureGraphicsPipeline(graphics_pipeline) {}
    virtual void configure();
  };
};
