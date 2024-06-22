#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  template <typename ExtendedType>
  struct RefCounted
  {
    int reference_count = 0;

    ExtendedType* retain()  { ++reference_count; return static_cast<ExtendedType*>(this); }
    void          release() { if ( !--reference_count ) delete this; }
  };
};
