#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureGLSLang : ContextOperation<Context>
  {
    virtual bool on_activate();
    virtual void on_deactivate();
  };
};
