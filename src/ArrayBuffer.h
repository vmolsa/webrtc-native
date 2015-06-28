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

#include <nan.h>
#include <string>

namespace node {
  class ArrayBuffer {
  public:
    inline static ArrayBuffer* New(const char *str = 0) {
#if (NODE_MODULE_VERSION >= NODE_0_12_MODULE_VERSION)
      return ArrayBuffer::New(v8::Isolate::GetCurrent(), std::string(str));
#else 
      return ArrayBuffer::New(std::string(str));
#endif
    }

    inline static ArrayBuffer* New(const std::string &data) {
#if (NODE_MODULE_VERSION >= NODE_0_12_MODULE_VERSION)
      return ArrayBuffer::New(v8::Isolate::GetCurrent(), data.data(), data.size());
#else
      return ArrayBuffer::New(data.data(), data.size());
#endif
    }

#if (NODE_MODULE_VERSION >= NODE_0_12_MODULE_VERSION)
    inline static ArrayBuffer* New(const char *str, size_t length) {
      return ArrayBuffer::New(v8::Isolate::GetCurrent(), str, length);
    }
    
    inline static ArrayBuffer* New(const v8::Local<v8::ArrayBuffer> &arrayBuffer) {
      return ArrayBuffer::New(v8::Isolate::GetCurrent(), arrayBuffer);
    }

    inline static ArrayBuffer* New(const v8::Local<v8::Value> &arg) {
      return ArrayBuffer::New(v8::Isolate::GetCurrent(), arg);
    }

    inline static ArrayBuffer* New(v8::Isolate *isolate, const std::string &data) {
      return ArrayBuffer::New(isolate, data.data(), data.size());
    }

    inline static ArrayBuffer* New(v8::Isolate *isolate, const char *str = 0) {
      return ArrayBuffer::New(isolate, std::string(str));
    }

    inline static ArrayBuffer* New(v8::Isolate *isolate, const char *str, size_t length) {
      if (!isolate) {
        isolate = v8::Isolate::GetCurrent();
      }

      ArrayBuffer *buffer = new ArrayBuffer();
      v8::Local<v8::ArrayBuffer> arrayBuffer;

      buffer->_data = 0;
      buffer->_len = length;

      if (length) {
        buffer->_data = new char[length + 1];
        buffer->_data[length] = '\0';

        for (size_t index = 0; index < length; index++) {
          buffer->_data[index] = str[index];
        }

        arrayBuffer = v8::ArrayBuffer::New(isolate, buffer->_data, length);
      }
      else {
        arrayBuffer = v8::ArrayBuffer::New(isolate, length);
      }

      buffer->_arrayBuffer.Reset(isolate, arrayBuffer);
      buffer->_arrayBuffer.SetWeak(buffer, ArrayBuffer::onDispose);
      buffer->_arrayBuffer.MarkIndependent();

      arrayBuffer->SetHiddenValue(v8::String::NewFromUtf8(isolate, "node::ArrayBuffer"), v8::External::New(isolate, buffer));
      return buffer;
    }

    inline static ArrayBuffer* New(v8::Isolate *isolate, const v8::Local<v8::ArrayBuffer> &arrayBuffer) {
      if (!isolate) {
        isolate = v8::Isolate::GetCurrent();
      }

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
        ArrayBuffer *buffer = new ArrayBuffer();
        v8::ArrayBuffer::Contents content = arrayBuffer->Externalize();

        buffer->_data = static_cast<char*>(content.Data());
        buffer->_len = content.ByteLength();
        buffer->_arrayBuffer.Reset(isolate, arrayBuffer);
        buffer->_arrayBuffer.SetWeak(buffer, ArrayBuffer::onDispose);
        buffer->_arrayBuffer.MarkIndependent();

        arrayBuffer->SetHiddenValue(v8::String::NewFromUtf8(isolate, "node::ArrayBuffer"), v8::External::New(isolate, buffer));

        return buffer;
      }

      return 0;
    }

    inline static ArrayBuffer* New(v8::Isolate *isolate, const v8::Local<v8::Value> &arg) {
      if (!arg.IsEmpty()) {
        if (arg->IsArrayBuffer() || arg->IsTypedArray()) {
          v8::Local<v8::ArrayBuffer> arrayBuffer;

          if (arg->IsArrayBuffer()) {
            arrayBuffer = v8::Local<v8::ArrayBuffer>::Cast(arg);
          }
          else {
            v8::Local<v8::ArrayBufferView> view = v8::Local<v8::ArrayBufferView>::Cast(arg);
            arrayBuffer = view->Buffer();
          }

          return ArrayBuffer::New(isolate, arrayBuffer);
        }

        if (arg->IsString()) {
          v8::String::Utf8Value str(arg->ToString());
          return ArrayBuffer::New(isolate, *str, str.length());
        }
      }

      return ArrayBuffer::New(isolate);
    }

    inline v8::Local<v8::ArrayBuffer> ToArrayBuffer(v8::Isolate *isolate = 0) const {
      if (!isolate) {
        isolate = v8::Isolate::GetCurrent();
      }

      v8::EscapableHandleScope scope(isolate);
      return scope.Escape(v8::Local<v8::ArrayBuffer>::New(isolate, _arrayBuffer));
    }

    inline v8::Local<v8::String> ToString(v8::Isolate *isolate = 0) const {
      if (!isolate) {
        isolate = v8::Isolate::GetCurrent();
      }

      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::String> retval = v8::String::NewFromUtf8(isolate, ArrayBuffer::ToUtf8(), v8::String::kNormalString, ArrayBuffer::Length());
      return scope.Escape(retval);
    }

#else

    inline static ArrayBuffer* New(const char *str, size_t length) {
      ArrayBuffer *buffer = new ArrayBuffer();

      v8::Local<v8::Object> global = v8::Context::GetCurrent()->Global();
      v8::Local<v8::Value> instance = global->Get(v8::String::New("ArrayBuffer"));
      v8::Local<v8::Function> constructor = v8::Local<v8::Function>::Cast(instance);
      v8::Local<v8::Object> arrayBuffer = constructor->NewInstance();

      buffer->_data = 0;
      buffer->_len = length;

      if (length) {
        buffer->_data = new char[length + 1];
        buffer->_data[length] = '\0';

        for (size_t index = 0; index < length; index++) {
          buffer->_data[index] = str[index];
        }

        arrayBuffer->SetIndexedPropertiesToExternalArrayData(buffer->_data, v8::kExternalByteArray, buffer->_len);
      }

      buffer->_arrayBuffer = v8::Persistent<v8::Object>::New(arrayBuffer);
      buffer->_arrayBuffer.MakeWeak(buffer, ArrayBuffer::onDispose);
      buffer->_arrayBuffer.MarkIndependent();

      arrayBuffer->SetHiddenValue(v8::String::New("node::ArrayBuffer"), v8::External::New(buffer));
      return buffer;
    }

    inline static ArrayBuffer* New(const v8::Local<v8::Object> &arrayBuffer) {
      if (!arrayBuffer.IsEmpty()) {
        v8::Local<v8::Value> ptr = arrayBuffer->GetHiddenValue(v8::String::New("node::ArrayBuffer"));

        if (!ptr.IsEmpty()) {
          v8::Local<v8::External> ext = v8::Local<v8::External>::Cast(ptr);
          return static_cast<ArrayBuffer*>(ext->Value());
        } else {
          if (arrayBuffer->HasIndexedPropertiesInExternalArrayData()) {
            int length = arrayBuffer->GetIndexedPropertiesExternalArrayDataLength();

            if (length > 0) {
              char *data = static_cast<char*>(arrayBuffer->GetIndexedPropertiesExternalArrayData());
              return ArrayBuffer::New(data, static_cast<size_t>(length));
            }
          }
        }
      }

      return ArrayBuffer::New();
    }

    inline static ArrayBuffer* New(const v8::Local<v8::Value> &arg) {
      if (!arg.IsEmpty()) {
        if (arg->IsObject()) {
          v8::Local<v8::Object> arrayBuffer = v8::Local<v8::Object>::Cast(arg);
          return ArrayBuffer::New(arrayBuffer);
        }

        if (arg->IsString()) {
          v8::String::Utf8Value str(arg->ToString());
          return ArrayBuffer::New(*str, str.length());
        }
      }

      return ArrayBuffer::New();
    }

    inline v8::Local<v8::Object> ToArrayBuffer() const {
      v8::HandleScope scope;
      v8::Local<v8::Object> arrayBuffer = v8::Local<v8::Object>::New(_arrayBuffer);
      return scope.Close(arrayBuffer);
    }

    inline v8::Local<v8::String> ToString() const {
      v8::HandleScope scope;
      v8::Local<v8::String> str = v8::String::New(ArrayBuffer::ToUtf8(), ArrayBuffer::Length());
      return scope.Close(str);
    }

#endif

    inline const char *ToUtf8() const {
      return _data;
    }

    inline void *Data() const {
      return _data;
    }

    inline size_t Length() const {
      return _len;
    }

    inline size_t ByteLength() const {
      return _len;
    }

#if (NODE_MODULE_VERSION >= NODE_0_12_MODULE_VERSION)
    static inline void onDispose(const v8::WeakCallbackData<v8::ArrayBuffer, ArrayBuffer> &info) {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      ArrayBuffer *wrap = info.GetParameter();

      if (wrap) {
        v8::Local<v8::ArrayBuffer> arrayBuffer = v8::Local<v8::ArrayBuffer>::New(isolate, wrap->_arrayBuffer);
        wrap->_arrayBuffer.Reset();

        if (!arrayBuffer.IsEmpty()) {
          arrayBuffer->DeleteHiddenValue(v8::String::NewFromUtf8(isolate, "node::ArrayBuffer"));
        }

        delete wrap;
      }
    }
#else
    static inline void onDispose(v8::Persistent<v8::Value> value, void *data) {
      v8::HandleScope scope;
      ArrayBuffer *wrap = static_cast<ArrayBuffer*>(data);

      if (wrap) {
        v8::Local<v8::Object> arrayBuffer = v8::Local<v8::Object>::New(wrap->_arrayBuffer);

        if (!arrayBuffer.IsEmpty()) {
          arrayBuffer->DeleteHiddenValue(v8::String::New("node::ArrayBuffer"));
        }

        delete wrap;
      }
    }
#endif

  private:
    virtual ~ArrayBuffer() {
#if (NODE_MODULE_VERSION >= NODE_0_12_MODULE_VERSION)
      if (_len) {
        delete [] _data;
      }
#else
      _arrayBuffer.ClearWeak();
      _arrayBuffer.Dispose();
      _arrayBuffer.Clear();
#endif
    }

  protected:
    char* _data;
    size_t _len;

#if (NODE_MODULE_VERSION >= NODE_0_12_MODULE_VERSION)
    v8::Persistent<v8::ArrayBuffer> _arrayBuffer;
#else
    v8::Persistent<v8::Object> _arrayBuffer;
#endif

  };
};

#endif
