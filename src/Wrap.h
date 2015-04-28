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

#include <v8.h>

namespace WebRTC {
  class RTCWrap {
   public:
    explicit RTCWrap();
    virtual ~RTCWrap();
    
    inline void Wrap(v8::Isolate *isolate, v8::Local<v8::Object> obj, const char *className = "RTCWrap") {
      if (obj.IsEmpty()) {
        _obj.ClearWeak();
        _obj.Reset();       
      } else {    
        _obj.Reset(isolate, obj);
        _obj.SetWeak(this, RTCWrap::onDispose);
        _obj.MarkIndependent();

        obj->SetHiddenValue(v8::String::NewFromUtf8(isolate, className),
                            v8::External::New(isolate, this));
      }
    }
    
    inline v8::Local<v8::Object> This(v8::Isolate* isolate) {
      v8::EscapableHandleScope scope(isolate);
      return scope.Escape(v8::Local<v8::Object>::New(isolate, _obj));
    }
    
    template<class T> inline T* Unwrap() {
      return static_cast<T*>(this);
    }
    
    template<class T> inline static T* Unwrap(v8::Isolate *isolate, v8::Local<v8::Object> obj, const char *className = "RTCWrap") {
      if (!obj.IsEmpty()) {
        v8::Local<v8::Value> ptr = obj->GetHiddenValue(v8::String::NewFromUtf8(isolate, className));
        
        if (!ptr.IsEmpty() && ptr->IsExternal()) {
          v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(ptr);
          RTCWrap *wrap = static_cast<RTCWrap*>(ext->Value());
          return wrap->Unwrap<T>();
        }
      }
      
      return 0;
    }

   private:   
    static void onDispose(const v8::WeakCallbackData<v8::Object, RTCWrap> &info) {      
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);
      
      RTCWrap *wrap = info.GetParameter();
      
      v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, wrap->_obj);
      wrap->_obj.Reset();
      
      if (!obj.IsEmpty()) {
        obj->DeleteHiddenValue(v8::String::NewFromUtf8(isolate, "RTCWrap"));
      }
      
      delete wrap;
    }

   protected:
    v8::Persistent<v8::Object> _obj;
  };
};

#endif