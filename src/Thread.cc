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

#include "Common.h" 
#include "Thread.h" 

using namespace WebRTC;

EventLoop::EventLoop() {
#if (UV_VERSION_MAJOR > 0)
  (void) uv_loop_init(&_loop);
#else
  _loop = uv_loop_new();
#endif
}

EventLoop::~EventLoop() {
#if (UV_VERSION_MAJOR > 0)
  (void) uv_loop_close(&_loop);
#else
  uv_loop_delete(_loop);
  _loop = 0;
#endif
}

void EventLoop::Run() {
  bool running = false;
  
  do {
    running = EventLoop::Once();
  } while (running);
}

bool EventLoop::Once() {
  return (uv_run(EventLoop::GetLoop(), UV_RUN_NOWAIT) != 0) ? true : false;
}

uv_loop_t* EventLoop::GetLoop() {
#if (UV_VERSION_MAJOR > 0)
  return &_loop;
#else
  return _loop;
#endif
}

Thread::Thread(EventEmitter *listener) : EventEmitter(EventLoop::GetLoop()) {
  EventEmitter::SetReference(true);
  EventEmitter::AddListener(listener);
  
  (void) uv_thread_create(&_worker, Thread::onStart, this);
}

Thread::~Thread() {
  Thread::End();
}

void Thread::Run() {
  EventLoop::Run();
}

void Thread::End() {
  EventEmitter::RemoveAllListeners();
  EventEmitter::SetReference(false);
  uv_thread_join(&_worker);
}

void Thread::onStart(void *arg) {
  Thread *self = static_cast<Thread*>(arg);
    
  if (self) {
    self->Run();
  }
}
