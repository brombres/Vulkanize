#pragma once

#include <string>

namespace VKZ
{
  struct Operation : Node<Operation>
  {
    // Internal use. Set by the dispatch_event() framework.
    bool configured = false;
    bool active     = false;

    // Activation progress that can optionally be set by an activate() call.
    // deactivate() will only be called if 'activate' is true OR if 'progress'
    // is non-zero. A deactivate() implementation can examine 'progress' to
    // only clean up certain parts that were successfully activated.
    int  progress = 0;

    //----- Constructor/Destructor ---------------------------------------------
    Operation() {}
    virtual ~Operation();

    //----- Event Callbacks ----------------------------------------------------
    virtual bool activate() { return true; }
    // Return true on success, false on failure.

    virtual void configure() {}
    // Automatically called once before any other event is handled.

    virtual bool execute() { return true; }
    // Return true on success, false on failure.

    virtual void deactivate() {}
    // Called from deactivate() if on_configure() returned true.

    virtual bool on_custom_event( const char* name ) { return true; }

    virtual void on_surface_lost() {}

    //----- Event Handling Framework -------------------------------------------
    virtual bool handle_event( Event event );

    virtual void set_context( Context* context ) = 0;
  };
};
