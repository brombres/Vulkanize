#include <cstdio>

#include "Vulkanize/Vulkanize.h"
using namespace VKZ;
using namespace std;

Process::~Process()
{
  destroy();

  for (auto phase : phases)
  {
    Operation* operation = operations[phase];
    if (operation) delete operation;
  }
  phases.clear();
  operations.clear();
}

void Process::add_operation( string phase, Operation* operation )
{
  Operation* existing = operations[phase];
  if (existing)
  {
    existing->add_sibling( operation );
  }
  else
  {
    // This phase does not yet exist.
    phases.push_back( phase );
    operations[phase] = operation;
  }
}

bool Process::activate( string phase )
{
  if ( !dispatch_event(phase, Event(EventType::ACTIVATE)) ) return false;
  return true;
}

bool Process::configure()
{
  if (configured) return true;
  if ( !operations.size() ) configure_operations();
  if ( !activate("configure") ) return false;
  configured = true;
  return true;
}

void Process::configure_operations()
{
}

void Process::deactivate( string phase )
{
  dispatch_event( phase, Event(EventType::DEACTIVATE,true) );
}

void Process::destroy()
{
  deactivate( "configure" );
  configured = false;
}

bool Process::dispatch_event( string phase, Event event )
{
  if (event.reverse_order)
  {
    for (int i=(int)phases.size(); --i>=0; )
    {
      string cur_phase = phases[i];
      if (_phase_begins_with(cur_phase,phase))
      {
        Operation* operation = operations[ cur_phase ];
        if (operation && !operation->handle_event(event)) return false;
      }
    }
    return true;
  }
  else
  {
    for (auto cur_phase : phases)
    {
      if (_phase_begins_with(cur_phase,phase))
      {
        Operation* operation = operations[ cur_phase ];
        if (operation && !operation->handle_event(event)) return false;
      }
    }
    return true;
  }
}

bool Process::execute( string phase )
{
  return dispatch_event( phase, Event(EventType::EXECUTE) );
}

void Process::set_operation( string phase, Operation* operation )
{
  Operation* existing = operations[phase];
  if (existing)
  {
    delete existing;
  }
  else
  {
    // This phase does not yet exist in 'phases'.
    phases.push_back( phase );
  }
  operations[phase] = operation;
}

bool Process::_phase_begins_with( const string& phase, const string& other )
{
  auto value_n = phase.size();
  auto n = other.size();
  if (value_n < n) return false;

  if (0 != phase.compare(0,n,other)) return false;
  if (value_n == n) return true;
  return (phase[n] == '.');
}
