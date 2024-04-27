#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

#include "Vulkanize.h"

namespace VKZ
{
  struct Operation;

  struct Process
  {
    // PROPERTIES
    bool configured = false;

    std::vector<std::string>                   phases;
    std::unordered_map<std::string,Operation*> operations;

    // METHODS
    virtual ~Process();

    virtual void add_operation( std::string phase, Operation* operation );
    virtual bool activate( const std::string phase );
    virtual bool configure();

    virtual void configure_operations();

    virtual void deactivate( const std::string phase );
    virtual void destroy();
    virtual bool dispatch_event( std::string phase, std::string event, bool reverse_order=false );
    virtual bool execute( std::string phase );
    virtual void set_operation( std::string phase, Operation* operation );

    static bool _phase_begins_with( const std::string& phase, const std::string& other );
  };
};
