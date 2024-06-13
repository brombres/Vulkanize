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

  switch (event.type)
  {
    case EventType::ACTIVATE:
      if ( !activate() ) return false;
      break;

    case EventType::DEACTIVATE:
      deactivate();
      break;

    case EventType::EXECUTE:
      if ( !execute() ) return false;
      break;

    case EventType::SURFACE_LOST:
      on_surface_lost();
      break;
      
    default:;
  }

  if ( !event.reverse_order )
  {
    if (first_child && !first_child->handle_event(event)) return false;
    if (next_sibling && !next_sibling->handle_event(event)) return false;
  }

  return true;

}

