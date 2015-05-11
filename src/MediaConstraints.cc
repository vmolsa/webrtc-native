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
  _conf = new webrtc::FakeConstraints();
}

MediaConstraints::~MediaConstraints() {
  delete _conf;
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New() {
  return new rtc::RefCountedObject<MediaConstraints>();
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New(Isolate *isolate, const Local<Object> &constraints) {
  rtc::scoped_refptr<MediaConstraints> self = MediaConstraints::New();

  if (constraints.IsEmpty()) {
    return self;
  }

  Local<Value> optional_value = constraints->Get(String::NewFromUtf8(isolate, "optional"));

  if (!optional_value.IsEmpty() && optional_value->IsArray()) {
    Local<Array> options = Local<Array>::Cast(optional_value);

    for (unsigned int index = 0; index < options->Length(); index++) {
      Local<Value> option_value = options->Get(index);

      if (!option_value.IsEmpty() && option_value->IsObject()) {
        Local<Object> option = Local<Object>::Cast(option_value);
        Local<Value> DtlsSrtpKeyAgreement = option->Get(String::NewFromUtf8(isolate, "DtlsSrtpKeyAgreement"));
        Local<Value> RtpDataChannels = option->Get(String::NewFromUtf8(isolate, "RtpDataChannels"));
        Local<Value> googDscp = option->Get(String::NewFromUtf8(isolate, "googDscp"));
        Local<Value> googIPv6 = option->Get(String::NewFromUtf8(isolate, "googIPv6"));
        Local<Value> googSuspendBelowMinBitrate = option->Get(String::NewFromUtf8(isolate, "googSuspendBelowMinBitrate"));
        Local<Value> googNumUnsignalledRecvStreams = option->Get(String::NewFromUtf8(isolate, "googNumUnsignalledRecvStreams"));
        Local<Value> googCombinedAudioVideoBwe = option->Get(String::NewFromUtf8(isolate, "googCombinedAudioVideoBwe"));
        Local<Value> googScreencastMinBitrate = option->Get(String::NewFromUtf8(isolate, "googScreencastMinBitrate"));
        Local<Value> googCpuOveruseDetection = option->Get(String::NewFromUtf8(isolate, "googCpuOveruseDetection"));
        Local<Value> googCpuUnderuseThreshold = option->Get(String::NewFromUtf8(isolate, "googCpuUnderuseThreshold"));
        Local<Value> googCpuOveruseThreshold = option->Get(String::NewFromUtf8(isolate, "googCpuOveruseThreshold"));
        Local<Value> googCpuOveruseEncodeUsage = option->Get(String::NewFromUtf8(isolate, "googCpuOveruseEncodeUsage"));
        Local<Value> googHighStartBitrate = option->Get(String::NewFromUtf8(isolate, "googHighStartBitrate"));
        Local<Value> googHighBitrate = option->Get(String::NewFromUtf8(isolate, "googHighBitrate"));
        Local<Value> googVeryHighBitrate = option->Get(String::NewFromUtf8(isolate, "googVeryHighBitrate"));
        Local<Value> googPayloadPadding = option->Get(String::NewFromUtf8(isolate, "googPayloadPadding"));

        self->AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, DtlsSrtpKeyAgreement);
        self->AddOptional(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, RtpDataChannels);
        self->AddOptional(webrtc::MediaConstraintsInterface::kEnableDscp, googDscp);
        self->AddOptional(webrtc::MediaConstraintsInterface::kEnableIPv6, googIPv6);
        self->AddOptional(webrtc::MediaConstraintsInterface::kEnableVideoSuspendBelowMinBitrate, googSuspendBelowMinBitrate);
        self->AddOptional(webrtc::MediaConstraintsInterface::kNumUnsignalledRecvStreams, googNumUnsignalledRecvStreams);
        self->AddOptional(webrtc::MediaConstraintsInterface::kCombinedAudioVideoBwe, googCombinedAudioVideoBwe);
        self->AddOptional(webrtc::MediaConstraintsInterface::kScreencastMinBitrate, googScreencastMinBitrate);
        self->AddOptional(webrtc::MediaConstraintsInterface::kCpuOveruseDetection, googCpuOveruseDetection);
        self->AddOptional(webrtc::MediaConstraintsInterface::kCpuUnderuseThreshold, googCpuUnderuseThreshold);
        self->AddOptional(webrtc::MediaConstraintsInterface::kCpuOveruseThreshold, googCpuOveruseThreshold);
        self->AddOptional(webrtc::MediaConstraintsInterface::kCpuOveruseEncodeUsage, googCpuOveruseEncodeUsage);
        self->AddOptional(webrtc::MediaConstraintsInterface::kHighStartBitrate, googHighStartBitrate);
        self->AddOptional(webrtc::MediaConstraintsInterface::kHighBitrate, googHighBitrate);
        self->AddOptional(webrtc::MediaConstraintsInterface::kVeryHighBitrate, googVeryHighBitrate);
        self->AddOptional(webrtc::MediaConstraintsInterface::kPayloadPadding, googPayloadPadding);
      }
    }
  }

  Local<Value> mandatory_value = constraints->Get(String::NewFromUtf8(isolate, "mandatory"));

  if (!mandatory_value.IsEmpty() && mandatory_value->IsObject()) {
    Local<Object> mandatory = Local<Object>::Cast(mandatory_value);
    Local<Value> OfferToReceiveAudio = mandatory->Get(String::NewFromUtf8(isolate, "OfferToReceiveAudio"));
    Local<Value> OfferToReceiveVideo = mandatory->Get(String::NewFromUtf8(isolate, "OfferToReceiveVideo"));
    Local<Value> VoiceActivityDetection = mandatory->Get(String::NewFromUtf8(isolate, "VoiceActivityDetection"));
    Local<Value> IceRestart = mandatory->Get(String::NewFromUtf8(isolate, "IceRestart"));
    Local<Value> googUseRtpMUX = mandatory->Get(String::NewFromUtf8(isolate, "googUseRtpMUX"));

    self->SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveAudio, OfferToReceiveAudio);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveVideo, OfferToReceiveVideo);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kVoiceActivityDetection, VoiceActivityDetection);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kIceRestart, IceRestart);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kUseRtpMux, googUseRtpMUX);
  }

  Local<Value> audio_value = constraints->Get(String::NewFromUtf8(isolate, "audio"));

  if (!audio_value.IsEmpty()) {
    if (audio_value->IsTrue() || audio_value->IsFalse()) {
      self->_audio = true;
    } else if (audio_value->IsObject()) {
      Local<Object> audio = Local<Object>::Cast(audio_value);
      optional_value = audio->Get(String::NewFromUtf8(isolate, "optional"));

      if (!optional_value.IsEmpty() && optional_value->IsArray()) {
        Local<Array> options = Local<Array>::Cast(optional_value);

        for (unsigned int index = 0; index < options->Length(); index++) {
          Local<Value> option_value = options->Get(index);

          if (!option_value.IsEmpty() && option_value->IsObject()) {
            Local<Object> option = Local<Object>::Cast(option_value);
            Local<Value> googEchoCancellation = option->Get(String::NewFromUtf8(isolate, "googEchoCancellation"));
            Local<Value> googEchoCancellation2 = option->Get(String::NewFromUtf8(isolate, "googEchoCancellation2"));
            Local<Value> googDAEchoCancellation = option->Get(String::NewFromUtf8(isolate, "googDAEchoCancellation"));
            Local<Value> googAutoGainControl = option->Get(String::NewFromUtf8(isolate, "googAutoGainControl"));
            Local<Value> googAutoGainControl2 = option->Get(String::NewFromUtf8(isolate, "googAutoGainControl2"));
            Local<Value> googNoiseSuppression = option->Get(String::NewFromUtf8(isolate, "googNoiseSuppression"));
            Local<Value> googNoiseSuppression2 = option->Get(String::NewFromUtf8(isolate, "googNoiseSuppression2"));
            Local<Value> googHighpassFilter = option->Get(String::NewFromUtf8(isolate, "googHighpassFilter"));
            Local<Value> googTypingNoiseDetection = option->Get(String::NewFromUtf8(isolate, "googTypingNoiseDetection"));
            Local<Value> googAudioMirroring = option->Get(String::NewFromUtf8(isolate, "googAudioMirroring"));

            self->AddOptional(webrtc::MediaConstraintsInterface::kEchoCancellation, googEchoCancellation);
            self->AddOptional(webrtc::MediaConstraintsInterface::kExperimentalEchoCancellation, googEchoCancellation2);
            self->AddOptional(webrtc::MediaConstraintsInterface::kDAEchoCancellation, googDAEchoCancellation);
            self->AddOptional(webrtc::MediaConstraintsInterface::kAutoGainControl, googAutoGainControl);
            self->AddOptional(webrtc::MediaConstraintsInterface::kExperimentalAutoGainControl, googAutoGainControl2);
            self->AddOptional(webrtc::MediaConstraintsInterface::kNoiseSuppression, googNoiseSuppression);
            self->AddOptional(webrtc::MediaConstraintsInterface::kExperimentalNoiseSuppression, googNoiseSuppression2);
            self->AddOptional(webrtc::MediaConstraintsInterface::kHighpassFilter, googHighpassFilter);
            self->AddOptional(webrtc::MediaConstraintsInterface::kTypingNoiseDetection, googTypingNoiseDetection);
            self->AddOptional(webrtc::MediaConstraintsInterface::kAudioMirroring, googAudioMirroring);
          }
        }
      }

      self->_audio = true;
    }
  }

  Local<Value> video_value = constraints->Get(String::NewFromUtf8(isolate, "video"));

  if (!video_value.IsEmpty()) {
    if (video_value->IsTrue() || video_value->IsFalse()) {
      self->_video = true;
    } else if (video_value->IsObject()) {
      Local<Object> video = Local<Object>::Cast(audio_value);
      optional_value = video->Get(String::NewFromUtf8(isolate, "optional"));

      if (!optional_value.IsEmpty() && optional_value->IsArray()) {
        Local<Array> options = Local<Array>::Cast(optional_value);

        for (unsigned int index = 0; index < options->Length(); index++) {
          Local<Value> option_value = options->Get(index);

          if (!option_value.IsEmpty() && option_value->IsObject()) {
            Local<Object> option = Local<Object>::Cast(option_value);

            Local<Value> minAspectRatio = option->Get(String::NewFromUtf8(isolate, "minAspectRatio"));
            Local<Value> maxAspectRatio = option->Get(String::NewFromUtf8(isolate, "maxAspectRatio"));
            Local<Value> maxWidth = option->Get(String::NewFromUtf8(isolate, "maxWidth"));
            Local<Value> minWidth = option->Get(String::NewFromUtf8(isolate, "minWidth"));
            Local<Value> maxHeight = option->Get(String::NewFromUtf8(isolate, "maxHeight"));
            Local<Value> minHeight = option->Get(String::NewFromUtf8(isolate, "minHeight"));
            Local<Value> maxFrameRate = option->Get(String::NewFromUtf8(isolate, "maxFrameRate"));
            Local<Value> minFrameRate = option->Get(String::NewFromUtf8(isolate, "minFrameRate"));

            self->AddOptional(webrtc::MediaConstraintsInterface::kMinAspectRatio, minAspectRatio);
            self->AddOptional(webrtc::MediaConstraintsInterface::kMaxAspectRatio, maxAspectRatio);
            self->AddOptional(webrtc::MediaConstraintsInterface::kMaxWidth, maxWidth);
            self->AddOptional(webrtc::MediaConstraintsInterface::kMinWidth, minWidth);
            self->AddOptional(webrtc::MediaConstraintsInterface::kMaxHeight, maxHeight);
            self->AddOptional(webrtc::MediaConstraintsInterface::kMinHeight, minHeight);
            self->AddOptional(webrtc::MediaConstraintsInterface::kMaxFrameRate, maxFrameRate);
            self->AddOptional(webrtc::MediaConstraintsInterface::kMinFrameRate, minFrameRate);
          }
        }
      }

      self->_video = true;
    }
  }


  return self;
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New(Isolate *isolate, const Local<Value> &constraints) {
  if (!constraints.IsEmpty() && constraints->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(constraints);
    return MediaConstraints::New(isolate, obj);
  }

  return MediaConstraints::New();
}


void MediaConstraints::AddOptional(std::string key, Local<Value> value) {
  if (!value.IsEmpty()) {
    if (value->IsTrue() || value->IsFalse()) {
      MediaConstraints::AddOptional(key, value->IsTrue() ? 
                                    webrtc::MediaConstraintsInterface::kValueTrue : 
                                    webrtc::MediaConstraintsInterface::kValueFalse);
    }

    // TODO(): IsNumber()
  }
}

void MediaConstraints::AddOptional(std::string key, const char *value) {
  _conf->AddOptional(key, value);
}

void MediaConstraints::SetMandatory(std::string key, Local<Value> value) {
  if (!value.IsEmpty()) {
    if (value->IsTrue() || value->IsFalse()) {
      MediaConstraints::SetMandatory(key, value->IsTrue() ? 
                                     webrtc::MediaConstraintsInterface::kValueTrue : 
                                     webrtc::MediaConstraintsInterface::kValueFalse);
    }
  }
}

void MediaConstraints::SetMandatory(std::string key, const char *value) {
  _conf->SetMandatory(key, value);
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

const webrtc::FakeConstraints *MediaConstraints::ToConstraints() const {
  return _conf;
}
