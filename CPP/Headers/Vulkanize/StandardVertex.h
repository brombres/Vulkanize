#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct StandardVertex
  {
    struct { float x, y, z; } position;
    uint32_t color;
    uint32_t data;
    struct { float u, v; }    uv;

    StandardVertex( float x, float y, float z, uint32_t color ) : color(color)
    {
      position = { x, y, z };
    }
  };
};

