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

#ifndef NODE_ARRAYBUFFER_H
#define NODE_ARRAYBUFFER_H

#ifdef WIN32
  #pragma warning( disable : 4267 )
#endif

#include <v8.h>
#include "webrtc/base/logging.h"

namespace node {
  template<class T> class ArrayBufferWrapper;
  
  class ArrayBuffer {
    template<class T> friend class ArrayBufferWrapper;
    
   public:
    inline static ArrayBuffer* New(v8::Isolate *isolate = 0, void *ptr = 0, size_t length = 0, bool release = false) {
      LOG(LS_INFO) << __FUNCTION__;
      
      ArrayBuffer *data = new ArrayBuffer(isolate);
      v8::Local<v8::ArrayBuffer> arrayBuffer = v8::ArrayBuffer::New(data->_isolate, ptr, length);

      data->_rel = release;
      data->_len = length;
      data->_data = ptr;
      data->_arrayBuffer.Reset(isolate, arrayBuffer);
      data->_arrayBuffer.SetWeak(data, ArrayBuffer::onDispose);
      data->_arrayBuffer.MarkIndependent();

      arrayBuffer->SetHiddenValue(v8::String::NewFromUtf8(data->_isolate, "node::ArrayBuffer"), 
                                  v8::External::New(data->_isolate, data));

      return data;
    }
    
    inline static ArrayBuffer* New(v8::Isolate *isolate, const char *ptr, int length = -1, bool release = false) {
      LOG(LS_INFO) << __FUNCTION__;
      
      const void *data = ptr;
      
      if (length < 0) {        
        for (length = 0; ptr && (ptr[length] || ptr[length] != '\0'); length++) { }
      }
      
      return ArrayBuffer::New(isolate, const_cast<void*>(data), length, release);
    }
    
    inline static ArrayBuffer* New(v8::Isolate *isolate, const v8::Local<v8::ArrayBuffer> &arrayBuffer) {
      LOG(LS_INFO) << __FUNCTION__;
      
      if (arrayBuffer.IsEmpty()) {
        return ArrayBuffer::New(isolate);
      }

      if (arrayBuffer->IsExternal()) {
        v8::Local<v8::Value> ptr = arrayBuffer->GetHiddenValue(v8::String::NewFromUtf8(isolate, "node::ArrayBuffer"));

        if (ptr.IsEmpty()) {
          v8::Local<v8::Value> uintArray = v8::Uint8Array::New(arrayBuffer, 0, arrayBuffer->ByteLength());
          return ArrayBuffer::New(isolate, uintArray);
        } else {          
          v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(ptr);
          return static_cast<ArrayBuffer*>(ext->Value());
        }
      } else {        
        ArrayBuffer *data = new ArrayBuffer(isolate);
        v8::ArrayBuffer::Contents content = arrayBuffer->Externalize();
        
        data->_rel = true;
        data->_len = content.ByteLength();
        data->_data = content.Data();
        data->_arrayBuffer.Reset(data->_isolate, arrayBuffer);
        data->_arrayBuffer.SetWeak(data, ArrayBuffer::onDispose);
        data->_arrayBuffer.MarkIndependent();

        arrayBuffer->SetHiddenValue(v8::String::NewFromUtf8(data->_isolate, "node::ArrayBuffer"), 
                                    v8::External::New(data->_isolate, data));

        return data;
      }

      return 0;
    }
    
    template<class T>
    inline static ArrayBuffer* New(v8::Isolate *isolate, const T &content) {
      LOG(LS_INFO) << __FUNCTION__;
      
      return ArrayBuffer::New(isolate, content, content.data(), content.size());
    }
    
    template<class T>
    inline static ArrayBuffer* New(v8::Isolate *isolate, const T &content, void *ptr, size_t length = 0) {
      LOG(LS_INFO) << __FUNCTION__;
      
      ArrayBufferWrapper<T> *ret = new ArrayBufferWrapper<T>(isolate, content, ptr, length);
      return static_cast<ArrayBuffer*>(ret);
    }   

    template<class T> 
    inline static ArrayBuffer* New(v8::Isolate *isolate, const T &content, const char *ptr, int length = -1) { 
      LOG(LS_INFO) << __FUNCTION__;
      
      const void *data = ptr;

      if (length < 0) {        
        for (length = 0; ptr && (ptr[length] || ptr[length] != '\0'); length++) { }
      }

      ArrayBufferWrapper<T> *ret = new ArrayBufferWrapper<T>(isolate, content, const_cast<void*>(data), length);
      return static_cast<ArrayBuffer*>(ret);
    }
    
    inline static ArrayBuffer* New(v8::Isolate *isolate, const v8::Local<v8::Value> &arg) {
      LOG(LS_INFO) << __FUNCTION__;
      
      if (!arg.IsEmpty()) {            
        if (arg->IsArrayBuffer() || arg->IsTypedArray()) {
          v8::Local<v8::ArrayBuffer> arrayBuffer;

          if (arg->IsArrayBuffer()) {
            arrayBuffer = v8::Local<v8::ArrayBuffer>::Cast(arg);
          } else {
            v8::Local<v8::ArrayBufferView> view = v8::Local<v8::ArrayBufferView>::Cast(arg);
            arrayBuffer = view->Buffer();
          }

          return ArrayBuffer::New(isolate, arrayBuffer);
        }

        if (arg->IsString()) {
          v8::String::Utf8Value str(arg->ToString());
          int length = str.length();
          const char *ptr = *str;
          char *data = 0;
          
          if (length > 0) {
            data = new char[length];
            data[length] = '\0';
            
            for (int index = 0; index < length; index++) {
              data[index] = ptr[index];
            }
          }
          
          return ArrayBuffer::New(isolate, data, length, (length > 0));
        }
      }
      
      return ArrayBuffer::New(isolate);
    }
     
    inline v8::Local<v8::ArrayBuffer> ToArrayBuffer() const {
      LOG(LS_INFO) << __FUNCTION__;
      
      v8::EscapableHandleScope scope(_isolate);
      return scope.Escape(v8::Local<v8::ArrayBuffer>::New(_isolate, _arrayBuffer));
    }
    
    v8::Local<v8::String> ToString() const {
      LOG(LS_INFO) << __FUNCTION__;
      
      v8::EscapableHandleScope scope(_isolate);
      v8::Local<v8::String> retval = v8::String::NewFromUtf8(_isolate, 
                                                             ArrayBuffer::ToUtf8(),
                                                             v8::String::kNormalString,
                                                             ArrayBuffer::Length());
      return scope.Escape(retval);
    }
    
    const char *ToUtf8() const {
      LOG(LS_INFO) << __FUNCTION__;
      
      return reinterpret_cast<const char*>(_data);
    }
    
    void *Data() const {
      LOG(LS_INFO) << __FUNCTION__;
      
      return _data;
    }
    
    size_t Length() const {
      LOG(LS_INFO) << __FUNCTION__;
      
      return _len;
    }
    
    size_t ByteLength() const {
      LOG(LS_INFO) << __FUNCTION__;
      
      return _len;
    }
    
    template<class T> const T &Unwrap() const {
      LOG(LS_INFO) << __FUNCTION__;
      
      static T nowrap;
      
      if (_wrap) {
        const ArrayBufferWrapper<T> *ptr = static_cast<const ArrayBufferWrapper<T>*>(this);
        return ptr->_content;
      }
      
      nowrap = T();
      return nowrap;
    }
    
   private:
    explicit ArrayBuffer(v8::Isolate *isolate = 0);    
    virtual ~ArrayBuffer();
    
    static void onDispose(const v8::WeakCallbackData<v8::ArrayBuffer, ArrayBuffer> &info) {
      LOG(LS_INFO) << __FUNCTION__;
      
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);
      
      ArrayBuffer *wrap = info.GetParameter();
      
      v8::Local<v8::ArrayBuffer> arrayBuffer = v8::Local<v8::ArrayBuffer>::New(isolate, wrap->_arrayBuffer);
      wrap->_arrayBuffer.Reset();
      
      if (!arrayBuffer.IsEmpty()) {
        arrayBuffer->DeleteHiddenValue(v8::String::NewFromUtf8(isolate, "node::ArrayBuffer"));
      }
      
      delete wrap;
    }
    
   protected:
    bool _rel;
    bool _wrap;
    size_t _len;
    void* _data;
    v8::Isolate* _isolate;
    v8::Persistent<v8::ArrayBuffer> _arrayBuffer;
  };
  
  template<class T> class ArrayBufferWrapper : public ArrayBuffer {
    friend class ArrayBuffer;

   private:
    explicit ArrayBufferWrapper(v8::Isolate *isolate, const T &content, void *ptr = 0, size_t length = 0) :
      ArrayBuffer(isolate),
      _content(content)
    {
      LOG(LS_INFO) << __FUNCTION__;
      
      v8::Local<v8::ArrayBuffer> arrayBuffer = v8::ArrayBuffer::New(_isolate, ptr, length);

      _wrap = true;
      _len = length;
      _data = ptr;
      _arrayBuffer.Reset(_isolate, arrayBuffer);
      _arrayBuffer.SetWeak(static_cast<ArrayBuffer*>(this), ArrayBuffer::onDispose);
      _arrayBuffer.MarkIndependent();

      arrayBuffer->SetHiddenValue(v8::String::NewFromUtf8(_isolate, "node::ArrayBuffer"), 
                                  v8::External::New(_isolate, this));
    }

    virtual ~ArrayBufferWrapper() {
      LOG(LS_INFO) << __FUNCTION__;
    }

   protected:
    T _content;
  };  
};

#endif
