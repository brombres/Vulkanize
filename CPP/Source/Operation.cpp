#include "Vulkanize/Vulkanize.h"
using namespace VKZ;
using namespace std;

bool Operation::activate()
{
  configure();

  if ( !active )
  {
    if ( !on_activate() ) return false;
    active = true;
  }

  return true;
}

void Operation::configure()
{
  if (configured) return;
  configured = true;
  on_configure();
}

bool Operation::execute()
{
  configure();
  return on_execute();
}

void Operation::deactivate()
{
  configure();

  if (active || progress)
  {
    on_deactivate();
    active = false;
    progress = 0;
  }
}

bool Operation::custom_event( const char* name )
{
  configure();
  return on_custom_event( name );
}

void Operation::surface_lost()
{
  configure();
  on_surface_lost();
}

bool Operation::handle_event( Event event )
{
  if (event.reverse_order)
  {
    if (next_sibling && !next_sibling->handle_event(event)) return false;
    if (first_child && !first_child->handle_event(event)) return false;
  }

  if ( !configured ) {
    configured = true;
    on_configure();
  }

  if (event.type == EventType::DEACTIVATE)
  {
    if (active || progress)
    {
      on_deactivate();
      active = false;
      progress = 0;
    }
  }
  else if (event.type == EventType::ACTIVATE)
  {
    if ( !active )
    {
      if ( !on_activate() ) return false;
      active = true;
    }
  }
  else if (event.type == EventType::EXECUTE)
  {
    if ( !on_execute() ) return false;
  }
  else if (event.type == EventType::SURFACE_LOST)
  {
    on_surface_lost();
  }
  else
  {
    if ( !on_custom_event(event.name) ) return false;
  }

  if ( !event.reverse_order )
  {
    if (first_child && !first_child->handle_event(event)) return false;
    if (next_sibling && !next_sibling->handle_event(event)) return false;
  }

  return true;

}

