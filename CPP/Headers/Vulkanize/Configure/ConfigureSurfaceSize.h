#pragma once

#include <string>

namespace VKZ
{
  struct ConfigureSurfaceSize : ContextOperation<Context>
  {
    virtual bool activate();
  };
};
