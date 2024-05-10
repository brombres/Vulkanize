#pragma once

#include "Vulkanize.h"

namespace VKZ
{
  struct RenderPass
  {
    Context*     context;
    VkRenderPass vk_render_pass;
  };
};
