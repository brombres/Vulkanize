#include "Vulkanize/Vulkanize.h"
using namespace VKZ;
using namespace std;

Operation::~Operation()
{
  if (next_sibling) delete next_sibling;
  if (first_child)  delete first_child;

  if (parent)
  {
    parent->remove_child( this );
  }
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
    configure();
  }

  if (event.type == EventType::DEACTIVATE)
  {
    if (active || progress)
    {
      deactivate();
      active = false;
      progress = 0;
    }
  }
  else if (event.type == EventType::ACTIVATE)
  {
    if ( !active )
    {
      if ( !activate() ) return false;
      active = true;
    }
  }
  else if (event.type == EventType::EXECUTE)
  {
    if ( !execute() ) return false;
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

