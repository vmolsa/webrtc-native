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

#include "MediaConstraints.h"

using namespace v8;
using namespace WebRTC;

MediaConstraints::MediaConstraints() :
  _audio(false),
  _video(false)
{
  _conf.SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveAudio, "false");
  _conf.SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveVideo, "false");
  _conf.SetMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "true");
}

MediaConstraints::~MediaConstraints() {

}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New() {
  return new rtc::RefCountedObject<MediaConstraints>();
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New(const v8::Local<v8::Object> &constraints) {
  rtc::scoped_refptr<MediaConstraints> self = MediaConstraints::New();

  if (constraints.IsEmpty()) {
    return self;
  }



  return self;
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New(const v8::Local<v8::Value> &constraints) {
  if (!constraints.IsEmpty() && constraints->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(constraints);
    return MediaConstraints::New(obj);
  }

  return MediaConstraints::New();
}

void MediaConstraints::UseAudio(bool enable) {
  _audio = enable;
  _conf.SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveAudio, _audio ? "true" : "false");
}

void MediaConstraints::UseVideo(bool enable) {
  _video = enable;
  _conf.SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveVideo, _audio ? "true" : "false");
}

bool MediaConstraints::UseAudio() const {
  return _audio;
}

bool MediaConstraints::UseVideo() const {
  return _video;
}

std::string MediaConstraints::AudioId() const {
  return _audioId;
}

std::string MediaConstraints::VideoId() const {
  return _videoId;
}

const webrtc::FakeConstraints MediaConstraints::ToConstraints() const {
  return _conf;
}