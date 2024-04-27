#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureStandardVertexBuffer : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
  };
};
