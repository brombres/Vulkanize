#pragma once

#include <string>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  enum EventType
  {
    ACTIVATE,
    DEACTIVATE,
    EXECUTE,
    SURFACE_LOST,
    CUSTOM
  };

  struct Event
  {
    EventType   type;
    const char* name;
    bool reverse_order;

    Event( EventType type, bool reverse_order=false )
      : type(type), reverse_order(reverse_order) {}

    Event( const char* name, bool reverse_order=false )
      : type(EventType::CUSTOM), name(name), reverse_order(reverse_order) {}
  };
};
