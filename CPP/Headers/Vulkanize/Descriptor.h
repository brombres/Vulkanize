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

      if (is_ready)
      {
        for (auto& buffer : buffers)
        {
          buffer.copy_from( &value, sizeof(DataType) );
        }
        //update_frames = ((1 << context->swapchain_count) - 1);
      }
    }
  };

  struct Descriptor : RefCounted<Descriptor>
  {
    Context*           context;
    VkShaderStageFlags stage;
    VkDescriptorType   type;
    uint32_t           count = 1;
    uint32_t           binding;
    VkSampler*         immutable_samplers = nullptr;

    Descriptor( Context* context, uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type )
      : context(context), binding(binding), stage(stage), type(type) {}

    virtual ~Descriptor() {}

    virtual bool configure() { return true; }
    virtual void destroy() {}
  };

  template <typename DataType>
  struct UniformBufferDescriptor : Descriptor
  {
    UniformBufferData<DataType>* data;

    UniformBufferDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage,
                             UniformBufferData<DataType>* data )
      : Descriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ), data(data)
    {
    }

    bool configure() override
    {
      return data->configure( context );
    }

    void destroy() override
    {
      data->destroy();
    }

    void set( DataType& value )
    {
      data->set( value );
    }
  };

  struct ImageInfoDescriptor : Descriptor
  {
    VkDescriptorImageInfo image_info = {};

    ImageInfoDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type )
      : Descriptor( context, binding, stage, type )
    {
    }
  };

  struct CombinedImageSamplerDescriptor : ImageInfoDescriptor
  {
    Image*    image;
    Sampler*  sampler;

    CombinedImageSamplerDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage,
        Image* image, Sampler* sampler )
      : ImageInfoDescriptor( context, binding, stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ),
        image(image), sampler(sampler)
    {
    }
  };
};
