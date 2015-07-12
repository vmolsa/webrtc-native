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

#include "ArrayBuffer.h"
#include "MediaSource.h"

using namespace v8;
using namespace WebRTC;

Persistent<Function> MediaSource::constructor;

enum MediaSourceEvent {
  kMediaSourceEncode = 1,
  kMediaSourceDecode,
};

enum MediaSourceType {
  kMediaSourceNone = 1,
  kMediaSourceImage,
  kMediaSourceAudio,
};

class MediaSourceContext {
  public:
    explicit MediaSourceContext(MediaSourceType contentType = kMediaSourceNone) : type(contentType) { }
    
    MediaSourceType type;
};

class MediaSourceImage : public MediaSourceContext {
  public:
    MediaSourceImage() : MediaSourceContext(kMediaSourceImage) { }
  
    int width;
    int height;
    
    std::string mime;
    std::string data; // <- void *?
};

class MediaSourceAudio : public MediaSourceContext {
  public:
    MediaSourceAudio() : MediaSourceContext(kMediaSourceAudio) { }
  
    int bits;
    int rate;
    int channels;
    int frames;
    
    std::string mime;
    std::string data; // <- void *?
};

class yuv_handler : public Thread {
  public:
    yuv_handler(EventEmitter *listener) : Thread(listener) { }
  
    void Decode(const std::string &data) {
      
    }
  
    void On(Event *event) final {
      MediaSourceEvent type = event->Type<MediaSourceEvent>();

      switch (type) {
        case kMediaSourceEncode:
          break;
        case kMediaSourceDecode:
          Decode(event->Unwrap<std::string>());
          break;
      }
    }
};

void MediaSource::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(MediaSource::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew("MediaSource"));
  
  tpl->PrototypeTemplate()->Set(NanNew("write"), NanNew<FunctionTemplate>(MediaSource::Write)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("end"), NanNew<FunctionTemplate>(MediaSource::End)->GetFunction());

  tpl->InstanceTemplate()->SetAccessor(NanNew("ondata"),
                                       MediaSource::OnData,
                                       MediaSource::OnData);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onerror"),
                                       MediaSource::OnError,
                                       MediaSource::OnError);
                                       
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew("MediaSource"), tpl->GetFunction());                                   
}

MediaSource::MediaSource(const std::string &format, Local<Object> stream, Local<Function> callback) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (!stream.IsEmpty()) { // Encoder
    if (!format.compare("image/i420")) {
      _worker = Thread::New<yuv_handler>(this);
    }
  } else if (!callback.IsEmpty()) { // Decoder
    NanAssignPersistent(_callback, callback);
  }
  
  if (!_worker) {
    _worker = Thread::New<yuv_handler>(this);
  }
}

MediaSource::~MediaSource() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_worker) {
    _worker->SetEmitter();
    _worker->End();
    
    delete _worker;
  }
}

NAN_METHOD(MediaSource::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  Local<String> format = NanNew("image/i420");
  Local<Object> stream;
  Local<Function> callback;
  
  switch (args.Length()) {
    case 1:
      NanThrowError("Invalid Argument");
              
      break;
    case 2:
      if (args[0]->IsObject() && args[1]->IsString()) {
        stream = Local<Object>::Cast(args[0]);
        format = args[1]->ToString();
      } else if (args[0]->IsString() && args[1]->IsFunction()) {
        format = args[0]->ToString();
        callback = Local<Function>::Cast(args[1]);
      } else {
        NanThrowError("Invalid Arguments");
      }
      
      break;
    default:
      NanThrowError("Invalid Arguments");
      break;
  }

  if (args.IsConstructCall()) {
    String::Utf8Value format_str(format);
    MediaSource* source = new MediaSource(std::string(*format_str), stream, callback);
    source->Wrap(args.This(), "MediaSource");
    NanReturnValue(args.This());
  } else {
    const int argc = 3;
    Local<Value> argv[3] = {
      format,
      stream,
      callback
    };
    
    Local<Function> instance = NanNew(MediaSource::constructor);
    NanReturnValue(instance->NewInstance(argc, argv));
  }
  
  NanReturnUndefined();
}

NAN_METHOD(MediaSource::Write) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.This(), "MediaSource");
  Thread *worker = self->_worker;
  
  if (args.Length() >= 1) {
    node::ArrayBuffer *arrayBuffer = node::ArrayBuffer::New(args[0]);
    worker->Emit(kMediaSourceDecode, arrayBuffer->ToCString());
  }
  
  NanReturnUndefined();
}

NAN_METHOD(MediaSource::End) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.This(), "MediaSource");

  self->_worker->End();

  NanReturnUndefined();
}

NAN_GETTER(MediaSource::OnData) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  NanReturnValue(NanNew(self->_ondata));
}

NAN_GETTER(MediaSource::OnError) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  NanReturnValue(NanNew(self->_onerror));
}

NAN_SETTER(MediaSource::OnData) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  
  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_ondata, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_ondata);
  }
}

NAN_SETTER(MediaSource::OnError) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  
  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onerror, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onerror);
  }  
}

void MediaSource::On(Event *event) {

}