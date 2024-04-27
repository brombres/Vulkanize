#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureGLSLang : ContextOperation<Context>
  {
    virtual bool activate();
    virtual void deactivate();
  };
};
