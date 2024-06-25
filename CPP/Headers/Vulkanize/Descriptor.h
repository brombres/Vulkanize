#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Descriptor : RefCounted
  {
    Context*           context;
    VkShaderStageFlags stage;
    VkDescriptorType   type;
    uint32_t           count = 1;
    uint32_t           binding;
    int                update_frames = -1;
    VkSampler*         immutable_samplers = nullptr;

    Descriptor( Context* context, uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type )
      : context(context), binding(binding), stage(stage), type(type) {}

    virtual ~Descriptor() {}

    virtual bool configure() { return true; }
    virtual void destroy() {}

    virtual void update_descriptor_set( VkDescriptorSet& set, size_t swap_index ) = 0;
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

    void update_descriptor_set( VkDescriptorSet& set, size_t swap_index ) override
    {
      int frame_bit = (1 << swap_index);
      if ( !(data->update_frames & frame_bit) ) return;
      data->update_frames &= ~frame_bit;

      auto& buffer = data->buffers[swap_index];
      if (data->is_ready)
      {
        buffer.copy_from( &data->value, sizeof(DataType) );
      }

      VkDescriptorBufferInfo buffer_info = {};
      buffer_info.buffer = buffer.vk_buffer;
      buffer_info.offset = 0;
      buffer_info.range = sizeof(DataType);

      VkWriteDescriptorSet write;
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = set;
      write.dstBinding = binding;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = type;
      write.pBufferInfo = &buffer_info;

      context->device_dispatch.updateDescriptorSets( 1, &write, 0, nullptr );
    }
  };

  struct ImageInfoDescriptor : Descriptor
  {
    std::vector<VkDescriptorImageInfo> image_infos;

    ImageInfoDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type )
      : Descriptor( context, binding, stage, type ) {}

    void update_descriptor_set( VkDescriptorSet& set, size_t swap_index ) override;
  };

  struct CombinedImageSamplerDescriptor : ImageInfoDescriptor
  {
    std::vector<Ref<Image>>    images;
    std::vector<Ref<Sampler>>  samplers;

    CombinedImageSamplerDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage,
        size_t initial_count );

    CombinedImageSamplerDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage,
        Ref<Image> image, Ref<Sampler> sampler );

    virtual void add( Ref<Image> image, Ref<Sampler> sampler=Ref<Sampler>() );

    virtual void set( size_t index, Ref<Image> image, Ref<Sampler> sampler=Ref<Sampler>() );
    virtual void set_image( size_t index, Ref<Image> image );
    virtual void set_sampler( size_t index, Ref<Sampler> sampler );

    virtual bool _validate_index( size_t index );
  };

  struct SampledImageDescriptor : ImageInfoDescriptor
  {
    std::vector<Ref<Image>>    images;

    SampledImageDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage,
        size_t initial_count );

    SampledImageDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage,
        Ref<Image> image );

    virtual void add( Ref<Image> image );
    virtual void set( size_t index, Ref<Image> image );
  };

  struct SamplerDescriptor : ImageInfoDescriptor
  {
    std::vector<Ref<Sampler>>  samplers;

    SamplerDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage,
        size_t initial_count );

    SamplerDescriptor( Context* context, uint32_t binding, VkShaderStageFlags stage,
        Ref<Sampler> sampler );

    virtual void add( Ref<Sampler> sampler );
    virtual void set( size_t index, Ref<Sampler> sampler );
  };
};
