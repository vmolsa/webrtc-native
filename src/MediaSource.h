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

#ifndef WEBRTC_MEDIASOURCE_H
#define WEBRTC_MEDIASOURCE_H

#include <nan.h>
#include "Thread.h"
#include "MediaStream.h"
#include "MediaStreamTrack.h"

#include "talk/media/base/videoframe.h"
#include "webrtc/video_frame.h"
#include "webrtc/base/buffer.h"
#include "webrtc/modules/video_capture/include/video_capture.h"
#include "webrtc/modules/video_capture/include/video_capture_factory.h"
#include "webrtc/modules/utility/interface/process_thread.h"
#include "webrtc/modules/video_render/include/video_render.h"
#include "webrtc/system_wrappers/interface/tick_util.h"
#include "talk/media/webrtc/webrtcvideoframe.h"

namespace WebRTC { 
  enum MediaSourceEvent {
    kMediaSourceEnd = 1,
    kMediaSourceFrame,
  };
 
  class MediaSource : public RTCWrap, public EventEmitter { 
    public:
      explicit MediaSource();
      virtual ~MediaSource();
      
      static void Init(v8::Handle<v8::Object> exports);

      bool Connect(MediaSource *source = 0);
      bool Disconnect(MediaSource *source = 0);
      
      virtual void End();
      virtual bool End(v8::Local<v8::Value> data);
      virtual bool Write(v8::Local<v8::Value> data);
      
    private:
      static NAN_METHOD(New);
      static NAN_METHOD(Connect);
      static NAN_METHOD(Disconnect);
      static NAN_METHOD(Write);
      static NAN_METHOD(End);

      static NAN_GETTER(OnData);
      static NAN_GETTER(OnError);
      static NAN_GETTER(OnEnd);
      
      static NAN_SETTER(OnData);
      static NAN_SETTER(OnError);
      static NAN_SETTER(OnEnd);
      
      virtual void On(Event *event);
      
    protected:
      bool _closing;
      bool _callback;
      
      v8::Persistent<v8::Function> _ondata;
      v8::Persistent<v8::Function> _onerror;
      v8::Persistent<v8::Function> _onend;
      
      static v8::Persistent<v8::Object> constructor;
  };
};

#endif