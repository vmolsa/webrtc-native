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

#ifndef WEBRTC_MEDIASTREAM_H
#define WEBRTC_MEDIASTREAM_H

#include "Common.h"
#include "Observers.h" 
#include "EventEmitter.h"
#include "Wrap.h"

namespace WebRTC {
  enum MediaStreamEvent {
    kMediaStreamChanged
  };
  
  class MediaStream : public RTCWrap, public EventEmitter {
   public:
    static void Init();    
    static v8::Local<v8::Value> New(rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream);

    static rtc::scoped_refptr<webrtc::MediaStreamInterface> Unwrap(v8::Local<v8::Object> value);
    static rtc::scoped_refptr<webrtc::MediaStreamInterface> Unwrap(v8::Local<v8::Value> value);

   private:
    MediaStream();
    ~MediaStream() final;
    
    static NAN_METHOD(New);
    static NAN_METHOD(AddTrack);
    static NAN_METHOD(Clone);
    static NAN_METHOD(GetTrackById);
    static NAN_METHOD(GetAudioTracks);
    static NAN_METHOD(GetVideoTracks);
    static NAN_METHOD(RemoveTrack);
    
    static NAN_GETTER(GetEnded);
    static NAN_GETTER(GetId);
    static NAN_GETTER(GetOnAddTrack);
    static NAN_GETTER(GetOnRemoveTrack);
    static NAN_GETTER(GetOnEnded);

    static NAN_SETTER(ReadOnly);
    static NAN_SETTER(SetOnAddTrack);
    static NAN_SETTER(SetOnRemoveTrack);
    static NAN_SETTER(SetOnEnded);

    void On(Event *event) final;

   protected:
     bool _ended;

    v8::Persistent<v8::Function> _onaddtrack;
    v8::Persistent<v8::Function> _onremovetrack;
    v8::Persistent<v8::Function> _onended;

    rtc::scoped_refptr<MediaStreamObserver> _observer;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> _stream;

    webrtc::AudioTrackVector _audio_tracks;
    webrtc::VideoTrackVector _video_tracks;

    static v8::Persistent<v8::Function> constructor;
  };
};

#endif
