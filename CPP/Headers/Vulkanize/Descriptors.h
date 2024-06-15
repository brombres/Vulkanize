// Thanks to Vasif Abdullayev for this helpful article on configuring descriptors:
// https://mr-vasifabdullayev.medium.com/multiple-object-rendering-in-vulkan-3d07aa583cec
#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Descriptor
  {
    Descriptors*       descriptors = nullptr;
    Context*           context = nullptr;
    bool               activated = false;
    uint32_t           binding;
    VkDescriptorType   type;
    uint32_t           count = 1;
    VkSampler*         immutable_samplers = nullptr;
    VkShaderStageFlags stage;
    int                update_frames = 0;

    Descriptor( Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, VkDescriptorType type );
    virtual ~Descriptor(){}

    virtual bool activate( Context* context );
    virtual void deactivate();

    virtual VkDescriptorSet get_descriptor_set( int swap_index );

    virtual bool update_descriptor_set( uint32_t swap_index, VkDescriptorSet& set ) = 0;
    virtual bool update_descriptor_set_if_modified();

    virtual bool on_activate() { return true; }
    virtual void on_deactivate() {}
  };

  template <typename DataType>
  struct UniformBufferDescriptor : Descriptor
  {
    // PROPERTIES
    DataType value;
    std::vector<Buffer> buffers;

    // METHODS
    UniformBufferDescriptor( Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage )
      : Descriptor( descriptors, binding, stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {}

    bool on_activate() override
    {
      uint32_t count = context->swapchain_count;
      buffers.resize( count );
      for (uint32_t i=0; i<count; ++i)
      {
        if ( !buffers[i].create_uniform_buffer(context, sizeof(DataType)) ) return false;
        buffers[i].copy_from( &value, sizeof(DataType) );
      }

      return true;
    }

    void on_deactivate() override
    {
      uint32_t count = context->swapchain_count;
      for (uint32_t i=0; i<count; ++i)
      {
        buffers[i].destroy();
      }
    }

    bool update_descriptor_set( uint32_t swap_index, VkDescriptorSet& set ) override
    {
      VkDescriptorBufferInfo buffer_info = {};
      buffer_info.buffer = buffers[swap_index].vk_buffer;
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

      return true;
    }

    void set( DataType value )
    {
      this->value = value;

      if (activated)
      {
        for (auto& buffer : buffers)
        {
          buffer.copy_from( &value, sizeof(DataType) );
        }

        uint32_t swap_index = context->swap_index;
        int frame_bit = (1 << swap_index);
        update_frames = ((1 << context->swapchain_count) - 1) & ~frame_bit;
        update_descriptor_set( swap_index, get_descriptor_set(swap_index) );
      }
    }
  };

  struct SamplerDescriptor : Descriptor
  {
    // PROPERTIES
    VkDescriptorImageInfo image_info = {};
    Sampler*              sampler;

    // METHODS
    SamplerDescriptor( Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage, Sampler* sampler );

    void set( Sampler* new_sampler );
    bool update_descriptor_set( uint32_t swap_index, VkDescriptorSet& set ) override;
  };

  struct CombinedImageSamplerDescriptor : Descriptor
  {
    // PROPERTIES
    Image*    image;
    Sampler*  sampler;
    VkDescriptorImageInfo image_info = {};

    // METHODS
    CombinedImageSamplerDescriptor( Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage,
                                    Image* image, Sampler* sampler );

    void set( Image* new_image );
    void set( Image* new_image, Sampler* new_sampler );
    void set( Sampler* new_sampler );
    bool update_descriptor_set( uint32_t swap_index, VkDescriptorSet& set ) override;
  };

  struct SampledImageDescriptor : Descriptor
  {
    // PROPERTIES
    Image*    image;
    VkDescriptorImageInfo image_info = {};

    // METHODS
    SampledImageDescriptor( Descriptors* descriptors, uint32_t binding, VkShaderStageFlags stage,
                            Image* image );

    void set( Image* new_image );
    bool update_descriptor_set( uint32_t swap_index, VkDescriptorSet& set ) override;
  };

  struct Descriptors
  {
    // PROPERTIES
    Context*                 context = nullptr;
    std::vector<Descriptor*> descriptors;

    VkDescriptorSetLayout        layout;
    VkDescriptorPool             pool;
    std::vector<VkDescriptorSet> sets;

    bool activated = false;

    // METHODS
    ~Descriptors();

    virtual bool activate( Context* context );
    virtual void deactivate();

    virtual CombinedImageSamplerDescriptor* add_combined_image_sampler(
        uint32_t binding, VkShaderStageFlags stage, Image* image, Sampler* sampler
    );

    virtual SampledImageDescriptor* add_sampled_image(
        uint32_t binding, VkShaderStageFlags stage, Image* image
    );

    virtual SamplerDescriptor* add_sampler(
        uint32_t binding, VkShaderStageFlags stage, Sampler* sampler
    );

    template <typename DataType>
    UniformBufferDescriptor<DataType>* add_uniform_buffer( uint32_t binding, VkShaderStageFlags stage )
    {
      UniformBufferDescriptor<DataType>* descriptor = new UniformBufferDescriptor<DataType>(
        this, binding, stage );
      descriptors.push_back( descriptor );
      return descriptor;
    }

    virtual bool configure_descriptor_sets();
    virtual bool update_modified_descriptor_sets();

    virtual Descriptor*& operator[]( size_t index )
    {
      return descriptors[index];
    }
  };
};
