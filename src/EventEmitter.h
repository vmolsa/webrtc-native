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

#ifndef WEBRTC_EVENTEMITTER_H
#define WEBRTC_EVENTEMITTER_H

#include <queue>
#include <string>
#include <uv.h>
#include "webrtc/base/logging.h"

namespace WebRTC { 
  template<class T> class EventWrapper;
  
  class Event {
    template<class T> friend class EventWrapper;
    friend class EventEmitter;
    
   public:
    inline bool HasWrap() const {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      return _wrap;
    }
    
    template <class T> inline T Type() const {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      return static_cast<T>(_event);
    }
    
    template<class T> const T &Unwrap() const {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      static T nowrap;
      
      if (_wrap) {
        const EventWrapper<T> *ptr = static_cast<const EventWrapper<T> *>(this);
        return ptr->_content;
      }
      
      nowrap = T();
      return nowrap;
    }
   
   private: 
    explicit Event(int event = 0) :
      _event(event),
      _wrap(false)
    {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
    }
    
    virtual ~Event() {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
    }
    
   protected:
    int _event;
    bool _wrap;
  };
  
  template<class T> class EventWrapper : public Event {
    friend class Event;
    friend class EventEmitter;

   private:
    explicit EventWrapper(int event, const T &content) :
      Event(event),
      _content(content)
    {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      _wrap = true;
    }

    virtual ~EventWrapper() { }

   protected:
    T _content;
  };  

  class EventEmitter {    
   public:
    explicit EventEmitter();
    virtual ~EventEmitter();
    
    inline void Start(bool unref = false) {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      uv_mutex_lock(&_lock);

      if (!_running) {
        uv_async_init(uv_default_loop(), &_async, reinterpret_cast<uv_async_cb>(EventEmitter::onAsync));
        _running = true;

        if (unref) {
          uv_unref(reinterpret_cast<uv_handle_t*>(&_async));
        }
      }

      uv_mutex_unlock(&_lock);
    }
    
    inline void Stop() {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      uv_mutex_lock(&_lock);

      if (_running) {
        if (_events.empty()) {
          uv_close(reinterpret_cast<uv_handle_t*>(&_async), NULL);
          _running = false;
        } else {
          _closing = true;
          uv_async_send(&_async);
        }
      }

      uv_mutex_unlock(&_lock);
    }
    
    inline void End() {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      uv_mutex_lock(&_lock);

      if (_running) {
        uv_close(reinterpret_cast<uv_handle_t*>(&_async), NULL);
        _running = false;
      }

      uv_mutex_unlock(&_lock);
    }
    
    inline void Emit(Event *event) {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      uv_mutex_lock(&_lock);

      if (_running) {
        if (!_closing) {
          _events.push(event);
        }
        
        uv_async_send(&_async);
      }

      uv_mutex_unlock(&_lock);
    }
    
    inline void Emit(int event = 0) {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      EventEmitter::Emit(new Event(event));
    }
    
    template <class T> inline void Emit(int event, const T &content) {
      LOG(LS_INFO) << __FUNCTION__;
      
      EventWrapper<T> *wrap = new EventWrapper<T>(event, content);
      EventEmitter::Emit(wrap);
    }
    
    virtual void On(Event *event) = 0;
    
   private:
    inline static void onAsync(uv_async_t *handle, int status) {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      EventEmitter *self = static_cast<EventEmitter*>(handle->data);
      bool closing = false;
      uv_mutex_lock(&self->_lock);

      while (!self->_events.empty() && self->_running) {
        Event *event = self->_events.front();
        self->_events.pop();
        
        uv_mutex_unlock(&self->_lock);
        
        self->On(event);
        delete event;
        
        uv_mutex_lock(&self->_lock);
      }

      closing = self->_closing;
      uv_mutex_unlock(&self->_lock);

      if (closing) {
        self->Stop();
      }
    }
    
   protected:
    bool _running;
    bool _closing;
    uv_mutex_t _lock;
    uv_async_t _async;
    std::queue<Event*> _events;
  };
};

#endif
