#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct RenderEnd : ContextOperation<Context>
  {
    virtual bool on_execute();
  };
};
