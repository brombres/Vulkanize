#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureGFXLineListColor : ConfigureGraphicsPipeline
  {
    ConfigureGFXLineListColor( GraphicsPipeline* graphics_pipeline )
      : ConfigureGraphicsPipeline(graphics_pipeline) {}
    virtual void configure();
  };
};
