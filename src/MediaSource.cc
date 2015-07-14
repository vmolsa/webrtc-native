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

#include "talk/media/base/videoframe.h"

using namespace v8;
using namespace WebRTC;

Persistent<Function> MediaSource::constructor;

enum MediaSourceEvent {
  kMediaSourceData = 1,
  kMediaSourceEncode,
  kMediaSourceDecode,
};

enum MediaSourceType {
  kMediaSourceNone = 1,
  kMediaSourceBuffer,
  kMediaSourceImage,
  kMediaSourceAudio,
};

class MediaSourceContext {
  public:
    explicit MediaSourceContext(MediaSourceType contentType = kMediaSourceNone) : type(contentType) { }
    
    MediaSourceType type;
};

class MediaSourceBuffer : public MediaSourceContext {
  public:
    explicit MediaSourceBuffer(size_t length = 0) : MediaSourceContext(kMediaSourceBuffer), _length(length), _data(0) {
      if (_length) {
        _data = new uint8_t[_length];
      }
    }

    MediaSourceBuffer(uint8_t *data, size_t length) : MediaSourceContext(kMediaSourceBuffer), _length(0), _data(0) {
      if (length) {
        _data = new uint8_t[_length];
      }
      
      for (size_t index = 0; index < _length; index++) {
        _data[index] = data[index];
      }
    }
    
    MediaSourceBuffer(const char *data, size_t length) : MediaSourceContext(kMediaSourceBuffer), _length(0), _data(0) {
      if (length) {
        _data = new uint8_t[_length];
      }
      
      for (size_t index = 0; index < _length; index++) {
        _data[index] = data[index];
      }
    }    
    
    virtual ~MediaSourceBuffer() {
      if (_length) {
        delete [] _data;
      }
    }

    uint8_t *Data() const {
      return _data;
    }
    
    size_t Length() const {
      return _length;
    }

  protected:
    size_t _length;
    uint8_t* _data;
};

class MediaSourceImage : public MediaSourceContext {
  public:
    MediaSourceImage() : MediaSourceContext(kMediaSourceImage) { }
  
    size_t width;
    size_t height;
    
    std::string mime;
    MediaSourceBuffer buffer;
};

class MediaSourceAudio : public MediaSourceContext {
  public:
    MediaSourceAudio() : MediaSourceContext(kMediaSourceAudio) { }
  
    int bits;
    int rate;
    int channels;
    int frames;
    
    std::string mime;
    MediaSourceBuffer buffer;
};

class yuv_transformer : public Thread {
  public:
    yuv_transformer(EventEmitter *listener) : Thread(listener) { }

    void On(Event *event) final {
      MediaSourceEvent type = event->Type<MediaSourceEvent>();

      switch (type) {
        case kMediaSourceData:
        case kMediaSourceDecode:
          break;
        case kMediaSourceEncode:
          Notify(kMediaSourceData, event->Unwrap<MediaSourceImage>());
          break;
      }
    }
};

rtc::scoped_refptr<VideoRenderer> VideoRenderer::New(webrtc::MediaStreamTrackInterface *track, Thread *worker) {
  return new rtc::RefCountedObject<VideoRenderer>(track, worker);
}

VideoRenderer::VideoRenderer(webrtc::MediaStreamTrackInterface *track, Thread *worker) : 
  _track(static_cast<webrtc::VideoTrackInterface*>(track)), 
  _worker(worker)
{
  if (_track.get()) {
    _track->AddRenderer(this);
  }
}
      
VideoRenderer::~VideoRenderer() {
  VideoRenderer::End();
}

void VideoRenderer::RenderFrame(const cricket::VideoFrame* frame) {
  size_t total = frame->CopyToBuffer(0, 0); // Get Buffer Size
  MediaSourceImage image;
  
  image.mime = std::string("image/i420");
  image.width = frame->GetWidth();
  image.height = frame->GetHeight();
  image.buffer = MediaSourceBuffer(total);
  
  frame->CopyToBuffer(image.buffer.Data(), image.buffer.Length());
  
  if (_worker) {
    _worker->Emit(kMediaSourceEncode, image);
  }
}

void VideoRenderer::End() {
  if (_worker) {
    Thread *worker = _worker;
    _worker = 0;
    
    worker->SetEmitter();
    worker->End();
    
    delete worker;
  }
  
  if (_track.get()) {
    _track->RemoveRenderer(this);
    (void) _track.release();
  }
}

Thread *MediaSource::Format2Worker(const std::string &fmt) {
  if (!fmt.compare("image/i420")) {
    return Thread::New<yuv_transformer>(this);
  }
  
  return 0;
}

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

MediaSource::MediaSource(const std::string &fmt, v8::Local<v8::Function> callback) { // Encoder
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanAssignPersistent(_callback, callback);
  
}

MediaSource::MediaSource(const std::string &fmt, rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream) { // Decoder
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  
}

MediaSource::MediaSource(const std::string &fmt, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> mediaStreamTrack) { // Decoder
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (mediaStreamTrack->kind().compare("video")) {
    _renderer = VideoRenderer::New(mediaStreamTrack.get(), MediaSource::Format2Worker(fmt));
  } else if (mediaStreamTrack->kind().compare("audio")) {
    
  }
}

MediaSource::~MediaSource() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
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
    MediaSource* source = 0;
    
    if (!stream.IsEmpty()) {
      rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = MediaStream::Unwrap(stream);
      
      if (mediaStream.get()) {
        source = new MediaSource(std::string(*format_str), mediaStream);
      } else {
        rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = MediaStreamTrack::Unwrap(stream);
        
        if (track.get()) {
          source = new MediaSource(std::string(*format_str), track);
        } else {
          NanThrowError("Invalid Arguments");
        }
      }
    } else if (!callback.IsEmpty()) {
      source = new MediaSource(std::string(*format_str), callback);
    } else {
      NanThrowError("Invalid Arguments");
    }

    if (source) {
      source->Wrap(args.This(), "MediaSource");
      NanReturnValue(args.This());
    }
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
  //MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.This(), "MediaSource");
  
  if (args.Length() >= 1) {
    //node::ArrayBuffer *arrayBuffer = node::ArrayBuffer::New(args[0]);
    //worker->Emit(kMediaSourceDecode, MediaSourceBuffer(arrayBuffer->ToUtf8(), arrayBuffer->Length()));
  }
  
  NanReturnUndefined();
}

NAN_METHOD(MediaSource::End) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.This(), "MediaSource");
  
  if (self->_renderer.get()) {
    self->_renderer->End();
    (void) self->_renderer.release();
  }

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
  MediaSourceEvent type = event->Type<MediaSourceEvent>();
  
  if (type == kMediaSourceData) {
    MediaSourceContext ctx = event->Unwrap<MediaSourceContext>();
    MediaSourceImage image = event->Unwrap<MediaSourceImage>();
    MediaSourceAudio audio = event->Unwrap<MediaSourceAudio>();
    
    switch (ctx.type) {
      case kMediaSourceNone:
        break;
      case kMediaSourceImage:
        printf("MediaSource::On(kMediaSourceImage)\n");
        
        break;
      case kMediaSourceAudio:
        
        
        break;
    }
  }
}