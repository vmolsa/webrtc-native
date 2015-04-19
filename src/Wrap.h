#ifndef WEBRTCWRAP_H
#define WEBRTCWRAP_H

#include <v8.h>

namespace WebRTC {
  class RTCWrap {
   public:
    explicit RTCWrap() { }
    virtual ~RTCWrap() { }
    
    inline void Wrap(v8::Isolate *isolate, v8::Local<v8::Object> obj) {
      if (obj.IsEmpty()) {
        _obj.ClearWeak();
        _obj.Reset();       
      } else {    
        _obj.Reset(isolate, obj);
        _obj.SetWeak(this, RTCWrap::onDispose);
        _obj.MarkIndependent();

        obj->SetHiddenValue(v8::String::NewFromUtf8(isolate, "RTCWrap"), 
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
    
    template<class T> inline static T* Unwrap(v8::Isolate *isolate, v8::Local<v8::Object> obj) {
      if (!obj.IsEmpty()) {
        v8::Local<v8::Value> ptr = obj->GetHiddenValue(v8::String::NewFromUtf8(isolate, "RTCWrap"));
        
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