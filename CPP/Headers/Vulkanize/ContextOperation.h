#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  template <typename ContextType>
  struct ContextOperation : Operation
  {
    ContextType* context = nullptr;

    virtual void set_context( Context* context )
    {
      this->context = (ContextType*) context;
    }
  };
};
