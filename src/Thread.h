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

#ifndef WEBRTC_THREAD_H
#define WEBRTC_THREAD_H

#include "EventEmitter.h"

namespace WebRTC {
  class EventLoop {
    public:
      explicit EventLoop();
      virtual ~EventLoop();
      
      virtual void Run();
      virtual bool Once();
    
      uv_loop_t* GetLoop();
      
    protected:
#if (UV_VERSION_MAJOR > 0)
      uv_loop_t _loop;
#else
      uv_loop_t* _loop;
#endif
  };
  
  class Thread : public EventLoop, public EventEmitter {
    public:
      template <class T>
      inline static Thread *New(EventEmitter *listener = 0) {
        return static_cast<Thread*>(new T(listener));
      }
    
      explicit Thread(EventEmitter *listener = 0);
      virtual ~Thread();
      
      virtual void Run();
      void End();
    
    private:
      static void onStart(void *arg);
    
    protected:
      uv_thread_t _worker;
  };
};

#endif