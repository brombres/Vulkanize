#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct StandardVertex
  {
    struct { float x, y, z; } position;
    struct { float u, v; }    uv;
    uint32_t color;
    uint32_t data;

    StandardVertex( float x, float y, float z, uint32_t color ) : color(color)
    {
      position.x = x;
      position.y = y;
      position.z = z;
    }
  };
};

