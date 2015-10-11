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

#include "Common.h"

namespace WebRTC {
  class RTCWrap : public node::ObjectWrap {
    public:
      inline void Wrap(v8::Local<v8::Object> obj, const char *className = "RTCWrap") {
        LOG(LS_INFO) << __PRETTY_FUNCTION__;
        
        _className = className;
        node::ObjectWrap::Wrap(obj);
      }
      
      inline v8::Local<v8::Object> This() {
        LOG(LS_INFO) << __PRETTY_FUNCTION__;
        
#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New<v8::Object>(node::ObjectWrap::handle_));
#else
        return node::ObjectWrap::handle();
#endif
      }
      
      template<class T> inline T* Unwrap() {
        LOG(LS_INFO) << __PRETTY_FUNCTION__;
        
        return static_cast<T*>(this);
      }
      
      template<class T> inline static T* Unwrap(v8::Local<v8::Object> obj, const char *className = "RTCWrap") {
        LOG(LS_INFO) << __PRETTY_FUNCTION__;
        
        RTCWrap *wrap = node::ObjectWrap::Unwrap<RTCWrap>(obj);

        if (wrap) {
          if (!wrap->_className.compare(className)) {
            return wrap->Unwrap<T>();
          }
        }
        
        return 0;
      }
    
    protected:
      std::string _className;   
  };
};

#endif