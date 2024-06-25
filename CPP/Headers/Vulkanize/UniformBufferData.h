#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  template <typename DataType>
  struct UniformBufferData
  {
    DataType            value;
    std::vector<Buffer> buffers;  // 1 per swapchain frame
    Context*            context;
    bool                configured = false;
    bool                is_ready = false;
    int                 update_frames = -1;

    virtual ~UniformBufferData()
    {
      destroy();
    }

    virtual bool configure( Context* context )
    {
      if (configured) return is_ready;
      configured = true;
      is_ready = false;

      this->context = context;

      uint32_t count = context->swapchain_count;
      buffers.resize( count );
      for (uint32_t i=0; i<count; ++i)
      {
        if ( !buffers[i].create_uniform_buffer(context, sizeof(DataType)) ) return false;
        buffers[i].copy_from( &value, sizeof(DataType) );
      }

      is_ready = true;
      return true;
    }

    virtual void destroy()
    {
      if (configured)
      {
        configured = false;
        for (auto& buffer : buffers) buffer.destroy();
        buffers.clear();
        is_ready = false;
      }
    }

    void set( DataType& value )
    {
      this->value = value;
      update_frames = ((1 << context->swapchain_count) - 1);
    }
  };
};
