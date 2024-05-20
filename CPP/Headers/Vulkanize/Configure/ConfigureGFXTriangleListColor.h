#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureGFXTriangleListColor : ConfigureGraphicsPipeline
  {
    ConfigureGFXTriangleListColor( GraphicsPipeline* graphics_pipeline )
      : ConfigureGraphicsPipeline(graphics_pipeline) {}
    virtual void on_configure();
  };
};
