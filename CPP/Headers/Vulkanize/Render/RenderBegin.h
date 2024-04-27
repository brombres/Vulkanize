#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct RenderBegin : ContextOperation<Context>
  {
    virtual bool execute();
  };
};
