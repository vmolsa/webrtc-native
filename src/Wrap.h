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

#ifndef WEBRTC_WRAP_H
#define WEBRTC_WRAP_H

#ifdef WIN32
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif
#endif

#include <nan.h>
#include "webrtc/base/logging.h"

namespace WebRTC {
  class RTCWrap {
   public:  
    inline void Wrap(v8::Local<v8::Object> obj, const char *className = "RTCWrap") {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      if (obj.IsEmpty()) {
        NanDisposePersistent(_obj);
      } else {
        NanAssignPersistent(_obj, obj);

#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
        _obj.MakeWeak(this, RTCWrap::onDispose);   
#else
        _obj.SetWeak(this, RTCWrap::onDispose);
#endif
        _obj.MarkIndependent();

        obj->SetHiddenValue(NanNew(className), NanNew<v8::External>(this));
      }
    }

    // TODO(): remove this
    inline void Wrap(v8::Isolate *isolate, v8::Local<v8::Object> obj, const char *className = "RTCWrap") {
      return RTCWrap::Wrap(obj, className);
    }

    inline void Clear() {
      NanDisposePersistent(_obj);
    }
    
    inline v8::Local<v8::Object> This() {     
      NanEscapableScope();
      return NanEscapeScope(NanNew<v8::Object>(_obj));
    }

    // TODO(): remove this
    inline v8::Local<v8::Object> This(v8::Isolate *isolate) {
      return RTCWrap::This();
    }
    
    template<class T> inline T* Unwrap() {     
      return static_cast<T*>(this);
    }
    
    template<class T> inline static T* Unwrap(v8::Local<v8::Object> obj, const char *className = "RTCWrap") {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
      
      if (!obj.IsEmpty()) {
        v8::Local<v8::Value> ptr = obj->GetHiddenValue(NanNew(className));
        
        if (!ptr.IsEmpty() && ptr->IsExternal()) {
          v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(ptr);
          RTCWrap *wrap = static_cast<RTCWrap*>(ext->Value());
          return wrap->Unwrap<T>();
        }
      }
      
      return 0;
    }

    // TODO(): remove this
    template<class T> inline static T* Unwrap(v8::Isolate *isolate, v8::Local<v8::Object> obj, const char *className = "RTCWrap") {
      return RTCWrap::Unwrap<T>(obj, className);
    }

   private:
#if NODE_MINOR_VERSION > 11
    static inline void onDispose(const v8::WeakCallbackData<v8::Object, RTCWrap> &info) {
      RTCWrap *wrap = info.GetParameter();

      if (wrap) {
        wrap->Clear();
        delete wrap;
      }
    }
#else
    virtual ~RTCWrap() {
      _obj.ClearWeak();
      _obj.Dispose();
      _obj.Clear();
    }

    static inline void onDispose(v8::Persistent<v8::Value> value, void *data) {
      RTCWrap *wrap = static_cast<RTCWrap*>(data);

      if (wrap) {
        wrap->Clear();
        delete wrap;
      }
    }
#endif

   protected:
    v8::Persistent<v8::Object> _obj;
  };
};

#endif