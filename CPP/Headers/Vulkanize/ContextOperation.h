#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  template <typename ContextType>
  struct ContextOperation : Operation
  {
    ContextType* context = nullptr;

    template <typename ExtendedContextPointerType>
    ExtendedContextPointerType context_as()
    {
      return reinterpret_cast<ExtendedContextPointerType>( context );
    }

    virtual void set_context( Context* context )
    {
      this->context = (ContextType*) context;
    }
  };
};
