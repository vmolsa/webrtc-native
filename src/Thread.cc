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

#include "Thread.h" 

using namespace WebRTC;

EventLoop::EventLoop() {
  (void) uv_loop_init(&_loop);
}

EventLoop::~EventLoop() {
  (void) uv_loop_close(&_loop);
}

void EventLoop::Run() {
  bool running = false;
  
  do {
    running = (uv_run(&_loop, UV_RUN_NOWAIT) != 0) ? true : false;
  } while (running);
}

uv_loop_t* EventLoop::GetLoop() {
  return &_loop;
}

Thread::Thread(EventEmitter *listener) : EventEmitter(EventLoop::GetLoop()), _listener(listener) {
  EventEmitter::SetReference(true);
  (void) uv_thread_create(&_worker, Thread::onStart, this);
}

Thread::~Thread() {
  Thread::End();
}

void Thread::Run() {
  EventLoop::Run();
}

void Thread::End() {
  EventEmitter::SetReference(false);
  uv_thread_join(&_worker);
}

void Thread::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}

void Thread::Notify(Event *event) {
  if (_listener) {
    _listener->Emit(event);
  }
}

void Thread::Notify(int event) {
  if (_listener) {
    _listener->Emit(event);
  }
}

void Thread::onStart(void *arg) {
  Thread *self = static_cast<Thread*>(arg);
    
  if (self) {
    self->Run();
  }
}
