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
{ }

MediaConstraints::~MediaConstraints() { }

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

        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, DtlsSrtpKeyAgreement);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, RtpDataChannels);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableDscp, googDscp);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableIPv6, googIPv6);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableVideoSuspendBelowMinBitrate, googSuspendBelowMinBitrate);
        self->SetOptional(webrtc::MediaConstraintsInterface::kNumUnsignalledRecvStreams, googNumUnsignalledRecvStreams);
        self->SetOptional(webrtc::MediaConstraintsInterface::kCombinedAudioVideoBwe, googCombinedAudioVideoBwe);
        self->SetOptional(webrtc::MediaConstraintsInterface::kScreencastMinBitrate, googScreencastMinBitrate);
        self->SetOptional(webrtc::MediaConstraintsInterface::kCpuOveruseDetection, googCpuOveruseDetection);
        self->SetOptional(webrtc::MediaConstraintsInterface::kCpuUnderuseThreshold, googCpuUnderuseThreshold);
        self->SetOptional(webrtc::MediaConstraintsInterface::kCpuOveruseThreshold, googCpuOveruseThreshold);
        self->SetOptional(webrtc::MediaConstraintsInterface::kCpuOveruseEncodeUsage, googCpuOveruseEncodeUsage);
        self->SetOptional(webrtc::MediaConstraintsInterface::kHighStartBitrate, googHighStartBitrate);
        self->SetOptional(webrtc::MediaConstraintsInterface::kHighBitrate, googHighBitrate);
        self->SetOptional(webrtc::MediaConstraintsInterface::kVeryHighBitrate, googVeryHighBitrate);
        self->SetOptional(webrtc::MediaConstraintsInterface::kPayloadPadding, googPayloadPadding);
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

            self->SetOptional(webrtc::MediaConstraintsInterface::kEchoCancellation, googEchoCancellation);
            self->SetOptional(webrtc::MediaConstraintsInterface::kExperimentalEchoCancellation, googEchoCancellation2);
            self->SetOptional(webrtc::MediaConstraintsInterface::kDAEchoCancellation, googDAEchoCancellation);
            self->SetOptional(webrtc::MediaConstraintsInterface::kAutoGainControl, googAutoGainControl);
            self->SetOptional(webrtc::MediaConstraintsInterface::kExperimentalAutoGainControl, googAutoGainControl2);
            self->SetOptional(webrtc::MediaConstraintsInterface::kNoiseSuppression, googNoiseSuppression);
            self->SetOptional(webrtc::MediaConstraintsInterface::kExperimentalNoiseSuppression, googNoiseSuppression2);
            self->SetOptional(webrtc::MediaConstraintsInterface::kHighpassFilter, googHighpassFilter);
            self->SetOptional(webrtc::MediaConstraintsInterface::kTypingNoiseDetection, googTypingNoiseDetection);
            self->SetOptional(webrtc::MediaConstraintsInterface::kAudioMirroring, googAudioMirroring);
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

            self->SetOptional(webrtc::MediaConstraintsInterface::kMinAspectRatio, minAspectRatio);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxAspectRatio, maxAspectRatio);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxWidth, maxWidth);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinWidth, minWidth);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxHeight, maxHeight);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinHeight, minHeight);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxFrameRate, maxFrameRate);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinFrameRate, minFrameRate);
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


void MediaConstraints::SetOptional(std::string key, Local<Value> value) {
  if (!value.IsEmpty() && !value->IsNull() && !value->IsUndefined()) {
    if (value->IsTrue() || value->IsFalse()) {
      MediaConstraints::SetOptional(key, value->IsTrue() ?
                                    std::string(webrtc::MediaConstraintsInterface::kValueTrue) :
                                    std::string(webrtc::MediaConstraintsInterface::kValueFalse));
    } else if (value->IsNumber()) {
      MediaConstraints::SetOptional(key, value->NumberValue());
    } else if (value->IsInt32()) {
      MediaConstraints::SetOptional(key, value->Int32Value());
    } else if (value->IsUint32()) {
      MediaConstraints::SetOptional(key, value->Uint32Value());
    } else {
      Isolate* isolate = Isolate::GetCurrent();
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Unknown MediaConstraints Type")));
    }
  }
}

void MediaConstraints::SetMandatory(std::string key, Local<Value> value) {
  if (!value.IsEmpty() && !value->IsNull() && !value->IsUndefined()) {
    if (value->IsTrue() || value->IsFalse()) {
      MediaConstraints::SetMandatory(key, value->IsTrue() ?
                                     std::string(webrtc::MediaConstraintsInterface::kValueTrue) :
                                     std::string(webrtc::MediaConstraintsInterface::kValueFalse));
    } else if (value->IsNumber()) {
      MediaConstraints::SetMandatory(key, value->NumberValue());
    } else if (value->IsInt32()) {
      MediaConstraints::SetMandatory(key, value->Int32Value());
    } else if (value->IsUint32()) {
      MediaConstraints::SetMandatory(key, value->Uint32Value());
    } else {
      Isolate* isolate = Isolate::GetCurrent();
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Unknown MediaConstraints Type")));
    }
  }
}

bool MediaConstraints::IsMandatory(const std::string& key) {
  std::string value;

  if (_mandatory.FindFirst(key, &value)) {
    return true;
  }

  return false;
}

bool MediaConstraints::GetMandatory(const std::string& key) {
  std::string value;

  if (_mandatory.FindFirst(key, &value)) {
    if (!value.compare("true")) {
      return true;
    }
  }

  return false;
}

void MediaConstraints::RemoveMandatory(const std::string& key) {
  std::string value;

  if (_mandatory.FindFirst(key, &value)) {
    for (webrtc::MediaConstraintsInterface::Constraints::iterator iter = _mandatory.begin(); iter != _mandatory.end(); ++iter) {
      if (iter->key == key) {
        _mandatory.erase(iter);
        break;
      }
    }
  }
}

void MediaConstraints::AddMandatory(const std::string &key, const std::string &value) {
  _mandatory.push_back(webrtc::MediaConstraintsInterface::Constraint(key, value));
}

void MediaConstraints::SetMandatory(const std::string &key, const std::string &value) {
  MediaConstraints::RemoveMandatory(key);
  MediaConstraints::AddMandatory(key, value);
}

bool MediaConstraints::IsOptional(const std::string& key) {
  std::string value;

  if (_mandatory.FindFirst(key, &value)) {
    return true;
  }

  return false;
}

bool MediaConstraints::GetOptional(const std::string& key) {
  std::string value;

  if (_optional.FindFirst(key, &value)) {
    if (!value.compare("true")) {
      return true;
    }
  }

  return false;
}

void MediaConstraints::RemoveOptional(const std::string& key) {
  std::string value;

  if (_optional.FindFirst(key, &value)) {
    for (webrtc::MediaConstraintsInterface::Constraints::iterator iter = _optional.begin(); iter != _optional.end(); ++iter) {
      if (iter->key == key) {
        _optional.erase(iter);
        break;
      }
    }
  }
}

void MediaConstraints::AddOptional(const std::string &key, const std::string &value) {
  _optional.push_back(webrtc::MediaConstraintsInterface::Constraint(key, value));
}

void MediaConstraints::SetOptional(const std::string &key, const std::string &value) {
  MediaConstraints::RemoveOptional(key);
  MediaConstraints::AddOptional(key, value);
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

const webrtc::MediaConstraintsInterface *MediaConstraints::ToConstraints() const {
  return this;
}

const webrtc::MediaConstraintsInterface::Constraints &MediaConstraints::GetMandatory() const {
  return _mandatory;
}

const webrtc::MediaConstraintsInterface::Constraints &MediaConstraints::GetOptional() const {
  return _optional;
}
