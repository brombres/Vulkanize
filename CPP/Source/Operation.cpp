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

bool Operation::handle_event( string event, bool reverse_order )
{
  if (reverse_order)
  {
    if (next_sibling && !next_sibling->handle_event(event)) return false;
    if (first_child && !first_child->handle_event(event)) return false;
  }

  if ( !configured ) {
    configured = true;
    configure();
  }

  if (event == "deactivate")
  {
    if (active || progress)
    {
      deactivate();
      active = false;
      progress = 0;
    }
  }
  else
  {
    if (event == "activate")
    {
      if ( !active )
      {
        if ( !activate() ) return false;
        active = true;
      }
    }
    else if (event == "execute")
    {
      if ( !execute() ) return false;
    }
    else
    {
      if ( !on_event(event) ) return false;
    }
  }

  if ( !reverse_order )
  {
    if (first_child && !first_child->handle_event(event)) return false;
    if (next_sibling && !next_sibling->handle_event(event)) return false;
  }

  return true;

}

