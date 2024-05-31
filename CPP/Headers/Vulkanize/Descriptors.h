// Thanks to Vasif Abdullayev for this helpful article on configuring descriptors:
// https://mr-vasifabdullayev.medium.com/multiple-object-rendering-in-vulkan-3d07aa583cec
#pragma once

#include <vector>
#include "Vulkanize/Vulkanize.h"

namespace VKZ
{
  struct Descriptor
  {
    Context*               context;
    bool                   activated = false;
    uint32_t               binding;
    uint32_t               count;
    size_t                 size;
    VkDescriptorType       type;
    VkShaderStageFlags     stage;
    VkSampler*             samplers = nullptr;
    VkDescriptorBufferInfo buffer_info;
    VkDescriptorImageInfo  image_info;

    virtual ~Descriptor(){}

    virtual bool activate( Context* context );
    virtual void deactivate();

    virtual bool collect_descriptor_write( uint32_t swap_index, VkDescriptorSet& set,
                                           std::vector<VkWriteDescriptorSet>& writes ) = 0;
    virtual vkb::DispatchTable device_dispatch();

    virtual bool on_activate() { return true; }
    virtual void on_deactivate() {}

    virtual uint32_t swapchain_count();
  };

  template <typename DataType>
  struct UniformBufferDescriptor : Descriptor
  {
    // PROPERTIES
    DataType value;
    std::vector<Buffer> buffers;

    // METHODS
    virtual ~UniformBufferDescriptor(){}

    bool on_activate() override
    {
      uint32_t count = swapchain_count();
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
      uint32_t count = swapchain_count();
      for (uint32_t i=0; i<count; ++i)
      {
        buffers[i].destroy();
      }
    }

    bool collect_descriptor_write( uint32_t swap_index, VkDescriptorSet& set,
        std::vector<VkWriteDescriptorSet>& writes ) override
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
      write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      write.pBufferInfo = &buffer_info;

      device_dispatch().updateDescriptorSets( 1, &write, 0, nullptr );

      writes.push_back( write );

      return true;
    }

    void set( DataType value )
    {
      this->value = value;

      for (auto& buffer : buffers)
      {
        buffer.copy_from( &value, sizeof(DataType) );
      }
    }
  };

  struct Descriptors
  {
    // PROPERTIES
    Context*                 context = nullptr;
    std::vector<Descriptor*> descriptor_info;

    VkDescriptorSetLayout        layout;
    VkDescriptorPool             pool;
    std::vector<VkDescriptorSet> sets;

    bool activated = false;

    // METHODS
    ~Descriptors();

    virtual bool activate( Context* context );
    virtual void deactivate();

    //virtual Descriptor* add_combined_image_sampler( uint32_t binding, VkShaderStageFlags stage,
    //                                                VkSampler* samplers, uint32_t count=1 );
    //virtual Descriptor* add_descriptor( uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage,
    //                                    VkSampler* samplers=nullptr, uint32_t count=1 );

    template <typename DataType>
    UniformBufferDescriptor<DataType>* add_uniform_buffer( uint32_t binding, VkShaderStageFlags stage, uint32_t count=1 )
    {
      UniformBufferDescriptor<DataType>* info = new UniformBufferDescriptor<DataType>();
      info->binding  = binding;
      info->count    = count;
      info->type     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      info->stage    = stage;
      descriptor_info.push_back( info );
      return info;
    }

    virtual bool collect_descriptor_writes( uint32_t swap_index,
        std::vector<VkWriteDescriptorSet>& writes );

    virtual Descriptor*& operator[]( size_t index )
    {
      return descriptor_info[index];
    }
  };
};
