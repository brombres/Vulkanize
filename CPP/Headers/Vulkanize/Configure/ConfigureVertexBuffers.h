#pragma once

#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct ConfigureVertexBuffers : ContextOperation<Context>
  {
    size_t sizeof_vertex;

    ConfigureVertexBuffers( size_t sizeof_vertex ) : sizeof_vertex(sizeof_vertex) {}

    virtual bool on_activate();
    virtual void on_deactivate();
  };
};
