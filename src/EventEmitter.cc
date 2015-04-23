#include "EventEmitter.h"

using namespace WebRTC;

EventEmitter::EventEmitter() :
  _running(false),
  _closing(false)
{
  uv_mutex_init(&_lock);
  _async.data = this;
}

EventEmitter::~EventEmitter() {
  EventEmitter::End();

  while (!_events.empty()) {
    Event *event = _events.front();
    _events.pop();
    delete event;
  }

  uv_mutex_destroy(&_lock); 
}