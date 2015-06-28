/*
* The MIT License (MIT)
*
* Copyright (c) 2015 vmolsa <ville.molsa@gmail.com> (http://github.com/vmolsa)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*/

#include "EventEmitter.h"

using namespace WebRTC;

EventEmitter::EventEmitter() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  uv_mutex_init(&_lock);
  
  _async = new uv_async_t();
  _async->data = this;
  
  uv_async_init(uv_default_loop(), _async, reinterpret_cast<uv_async_cb>(EventEmitter::onAsync));
  
  EventEmitter::SetReference(false);
}

EventEmitter::~EventEmitter() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  _async->data = 0;

  while (!_events.empty()) {
    Event *event = _events.front();
    _events.pop();
    delete event;
  }

  uv_close(reinterpret_cast<uv_handle_t*>(_async), EventEmitter::onEnded);
  uv_mutex_destroy(&_lock);
}

void EventEmitter::SetReference(bool alive) {
  uv_mutex_lock(&_lock);
  
  if (alive) {
    uv_ref(reinterpret_cast<uv_handle_t*>(_async));
  } else {
    uv_unref(reinterpret_cast<uv_handle_t*>(_async));
  }
  
  uv_mutex_unlock(&_lock);
}

void EventEmitter::Emit(int event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  EventEmitter::Emit(new Event(event));
}

void EventEmitter::Emit(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  uv_mutex_lock(&_lock);
  
  _events.push(event);  
  uv_async_send(_async);
  
  uv_mutex_unlock(&_lock);
}

void EventEmitter::onAsync(uv_async_t *handle, int status) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  EventEmitter *self = static_cast<EventEmitter*>(handle->data);
    
  if (self) {
    self->DispatchEvents();
  }
}
 
void EventEmitter::onEnded(uv_handle_t *handle) {
  uv_async_t* async = reinterpret_cast<uv_async_t*>(handle);
   
  if (async) {
    delete async;
  }
}
 
void EventEmitter::DispatchEvents() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  uv_mutex_lock(&_lock);

  while (!_events.empty()) {
    Event *event = _events.front();
    _events.pop();
    
    uv_mutex_unlock(&_lock);
    
    On(event);
    delete event;
    
    uv_mutex_lock(&_lock);
  }
  
  uv_mutex_unlock(&_lock);
}