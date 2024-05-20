#pragma once

#include <string>

namespace VKZ
{
  struct Operation : Node<Operation>
  {
    // Internal use. Set by the dispatch_event() framework.
    bool configured = false;
    bool active     = false;

    // Activation progress that can optionally be set by an on_activate() call.
    // on_deactivate() will only be called if 'active' is true OR if 'progress'
    // is non-zero. An on_deactivate() implementation can examine 'progress' to
    // only clean up certain parts that were successfully activated.
    int  progress = 0;

    //----- Constructor/Destructor ---------------------------------------------
    Operation() {}
    virtual ~Operation() {}

    //----- Event Callbacks ----------------------------------------------------
    virtual bool on_activate() { return true; }
    // Return true on success, false on failure.

    virtual void on_configure() {}
    // Automatically called once before any other event is handled.

    virtual bool on_execute() { return true; }
    // Return true on success, false on failure.

    virtual void on_deactivate() {}
    // Called during deactivation if on_configure() returned true.

    virtual bool on_custom_event( const char* name ) { return true; }

    virtual void on_surface_lost() {}

    //----- Event Handlers -----------------------------------------------------
    // In most cases these direct events do not need to be invoked. Instead
    // call e.g. Context::activate(), which will call Operation::handle_event(),
    // which will call e.g. Operation::on_activate() in the correct order within
    // the process and within the operation subtree.
    //
    // In some cases - e.g. a new GraphicsPipeline is being dynamically
    // configured after the initial activation process - it is appropriate to
    // call e.g. Operation::activate(), which will handle calling
    // Operation::on_activate().
    virtual bool activate();
    // Return true on success, false on failure.

    virtual void configure();
    // Automatically called once before any other event is handled.

    virtual bool execute();
    // Return true on success, false on failure.

    virtual void deactivate();
    // Called during deactivation if on_configure() returned true or 'progress' > 0.

    virtual bool custom_event( const char* name );

    virtual void surface_lost();

    //----- Event Handling Framework -------------------------------------------
    virtual bool handle_event( Event event );

    virtual void set_context( Context* context ) = 0;
  };
};
