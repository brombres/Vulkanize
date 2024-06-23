#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct RefCounted
  {
    int reference_count = 0;

    virtual ~RefCounted() {}

    virtual void retain()  { ++reference_count; }
    virtual void release() { if ( !--reference_count ) delete this; }
  };

  template <typename DataType>
  struct Ref
  {
    DataType* object = nullptr;

    Ref<DataType>() {}

    Ref<DataType>( DataType* object )
    {
      this->object = object;
      if (object) object->retain();
    }

    Ref<DataType>( Ref<DataType> const &existing )
    {
      if ((object=existing.object)) object->retain();
    }

    ~Ref()
    {
      if (object)
      {
        object->release();
        object = nullptr;
      }
    }

    Ref<DataType>& operator=( DataType* new_object )
    {
      if (new_object) new_object->retain();
      if (object) object->release();
      object = new_object;
      return *this;
    }

    Ref<DataType>& operator=( Ref<DataType> &other )
    {
      if (other.object) other.object->retain();
      if (object) object->release();
      object = other.object;
      return *this;
    }

    operator DataType*() { return object; }

    DataType* operator->() { return object; }

    DataType* operator*() { return object; }

    bool exists() { return object != nullptr; }
  };

}; // namespace VKZ

