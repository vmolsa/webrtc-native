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
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

MediaConstraints::~MediaConstraints() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return new rtc::RefCountedObject<MediaConstraints>();
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New(const Local<Object> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  rtc::scoped_refptr<MediaConstraints> self = MediaConstraints::New();

  if (constraints.IsEmpty()) {
    return self;
  }

  Local<Value> optional_value = constraints->Get(Nan::New("optional").ToLocalChecked());

  if (!optional_value.IsEmpty() && optional_value->IsArray()) {
    Local<Array> options = Local<Array>::Cast(optional_value);

    for (unsigned int index = 0; index < options->Length(); index++) {
      Local<Value> option_value = options->Get(index);

      if (!option_value.IsEmpty() && option_value->IsObject()) {
        Local<Object> option = Local<Object>::Cast(option_value);
        Local<Value> DtlsSrtpKeyAgreement = option->Get(Nan::New("DtlsSrtpKeyAgreement").ToLocalChecked());
        Local<Value> RtpDataChannels = option->Get(Nan::New("RtpDataChannels").ToLocalChecked());
        Local<Value> googDscp = option->Get(Nan::New("googDscp").ToLocalChecked());
        Local<Value> googIPv6 = option->Get(Nan::New("googIPv6").ToLocalChecked());
        Local<Value> googSuspendBelowMinBitrate = option->Get(Nan::New("googSuspendBelowMinBitrate").ToLocalChecked());
        Local<Value> googCombinedAudioVideoBwe = option->Get(Nan::New("googCombinedAudioVideoBwe").ToLocalChecked());
        Local<Value> googScreencastMinBitrate = option->Get(Nan::New("googScreencastMinBitrate").ToLocalChecked());
        Local<Value> googCpuOveruseDetection = option->Get(Nan::New("googCpuOveruseDetection").ToLocalChecked());
        Local<Value> googPayloadPadding = option->Get(Nan::New("googPayloadPadding").ToLocalChecked());

        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, DtlsSrtpKeyAgreement);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, RtpDataChannels);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableDscp, googDscp);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableIPv6, googIPv6);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableVideoSuspendBelowMinBitrate, googSuspendBelowMinBitrate);
        self->SetOptional(webrtc::MediaConstraintsInterface::kCombinedAudioVideoBwe, googCombinedAudioVideoBwe);
        self->SetOptional(webrtc::MediaConstraintsInterface::kScreencastMinBitrate, googScreencastMinBitrate);
        self->SetOptional(webrtc::MediaConstraintsInterface::kCpuOveruseDetection, googCpuOveruseDetection);
        self->SetOptional(webrtc::MediaConstraintsInterface::kPayloadPadding, googPayloadPadding);
      }
    }
  }

  Local<Value> mandatory_value = constraints->Get(Nan::New("mandatory").ToLocalChecked());

  if (!mandatory_value.IsEmpty() && mandatory_value->IsObject()) {
    Local<Object> mandatory = Local<Object>::Cast(mandatory_value);
    Local<Value> OfferToReceiveAudio = mandatory->Get(Nan::New("OfferToReceiveAudio").ToLocalChecked());
    Local<Value> OfferToReceiveVideo = mandatory->Get(Nan::New("OfferToReceiveVideo").ToLocalChecked());
    Local<Value> VoiceActivityDetection = mandatory->Get(Nan::New("VoiceActivityDetection").ToLocalChecked());
    Local<Value> IceRestart = mandatory->Get(Nan::New("IceRestart").ToLocalChecked());
    Local<Value> googUseRtpMUX = mandatory->Get(Nan::New("googUseRtpMUX").ToLocalChecked());

    self->SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveAudio, OfferToReceiveAudio);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveVideo, OfferToReceiveVideo);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kVoiceActivityDetection, VoiceActivityDetection);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kIceRestart, IceRestart);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kUseRtpMux, googUseRtpMUX);
  }

  Local<Value> audio_value = constraints->Get(Nan::New("audio").ToLocalChecked());

  if (!audio_value.IsEmpty()) {
    if (audio_value->IsTrue() || audio_value->IsFalse()) {
      self->_audio = true;
    } else if (audio_value->IsObject()) {
      Local<Object> audio = Local<Object>::Cast(audio_value);
      optional_value = audio->Get(Nan::New("optional").ToLocalChecked());

      if (!optional_value.IsEmpty() && optional_value->IsArray()) {
        Local<Array> options = Local<Array>::Cast(optional_value);

        for (unsigned int index = 0; index < options->Length(); index++) {
          Local<Value> option_value = options->Get(index);

          if (!option_value.IsEmpty() && option_value->IsObject()) {
            Local<Object> option = Local<Object>::Cast(option_value);
            Local<Value> EchoCancellation = option->Get(Nan::New("echoCancellation").ToLocalChecked());
            Local<Value> googEchoCancellation = option->Get(Nan::New("googEchoCancellation").ToLocalChecked());
            Local<Value> googEchoCancellation2 = option->Get(Nan::New("googEchoCancellation2").ToLocalChecked());
            Local<Value> googDAEchoCancellation = option->Get(Nan::New("googDAEchoCancellation").ToLocalChecked());
            Local<Value> googAutoGainControl = option->Get(Nan::New("googAutoGainControl").ToLocalChecked());
            Local<Value> googAutoGainControl2 = option->Get(Nan::New("googAutoGainControl2").ToLocalChecked());
            Local<Value> googNoiseSuppression = option->Get(Nan::New("googNoiseSuppression").ToLocalChecked());
            Local<Value> googNoiseSuppression2 = option->Get(Nan::New("googNoiseSuppression2").ToLocalChecked());
            Local<Value> googHighpassFilter = option->Get(Nan::New("googHighpassFilter").ToLocalChecked());
            Local<Value> googTypingNoiseDetection = option->Get(Nan::New("googTypingNoiseDetection").ToLocalChecked());
            Local<Value> googAudioMirroring = option->Get(Nan::New("googAudioMirroring").ToLocalChecked());
            Local<Value> noiseReduction = option->Get(Nan::New("googNoiseReduction").ToLocalChecked());
            Local<Value> sourceId = option->Get(Nan::New("sourceId").ToLocalChecked());

            self->SetOptional(webrtc::MediaConstraintsInterface::kEchoCancellation, EchoCancellation);
            self->SetOptional(webrtc::MediaConstraintsInterface::kGoogEchoCancellation, googEchoCancellation);
            self->SetOptional(webrtc::MediaConstraintsInterface::kExtendedFilterEchoCancellation, googEchoCancellation2);
            self->SetOptional(webrtc::MediaConstraintsInterface::kDAEchoCancellation, googDAEchoCancellation);
            self->SetOptional(webrtc::MediaConstraintsInterface::kAutoGainControl, googAutoGainControl);
            self->SetOptional(webrtc::MediaConstraintsInterface::kExperimentalAutoGainControl, googAutoGainControl2);
            self->SetOptional(webrtc::MediaConstraintsInterface::kNoiseSuppression, googNoiseSuppression);
            self->SetOptional(webrtc::MediaConstraintsInterface::kExperimentalNoiseSuppression, googNoiseSuppression2);
            self->SetOptional(webrtc::MediaConstraintsInterface::kHighpassFilter, googHighpassFilter);
            self->SetOptional(webrtc::MediaConstraintsInterface::kTypingNoiseDetection, googTypingNoiseDetection);
            self->SetOptional(webrtc::MediaConstraintsInterface::kAudioMirroring, googAudioMirroring);
            self->SetOptional(webrtc::MediaConstraintsInterface::kNoiseReduction, noiseReduction);

            if (!sourceId.IsEmpty() && sourceId->IsString()) {
              String::Utf8Value sourceId_str(sourceId->ToString());
              self->_audioId = *sourceId_str;
            }
          }
        }
      }

      self->_audio = true;
    }
  }

  Local<Value> video_value = constraints->Get(Nan::New("video").ToLocalChecked());

  if (!video_value.IsEmpty()) {
    if (video_value->IsTrue() || video_value->IsFalse()) {
      self->_video = true;
    } else if (video_value->IsObject()) {
      Local<Object> video = Local<Object>::Cast(audio_value);
      optional_value = video->Get(Nan::New("optional").ToLocalChecked());

      if (!optional_value.IsEmpty() && optional_value->IsArray()) {
        Local<Array> options = Local<Array>::Cast(optional_value);

        for (unsigned int index = 0; index < options->Length(); index++) {
          Local<Value> option_value = options->Get(index);

          if (!option_value.IsEmpty() && option_value->IsObject()) {
            Local<Object> option = Local<Object>::Cast(option_value);

            Local<Value> minAspectRatio = option->Get(Nan::New("minAspectRatio").ToLocalChecked());
            Local<Value> maxAspectRatio = option->Get(Nan::New("maxAspectRatio").ToLocalChecked());
            Local<Value> maxWidth = option->Get(Nan::New("maxWidth").ToLocalChecked());
            Local<Value> minWidth = option->Get(Nan::New("minWidth").ToLocalChecked());
            Local<Value> maxHeight = option->Get(Nan::New("maxHeight").ToLocalChecked());
            Local<Value> minHeight = option->Get(Nan::New("minHeight").ToLocalChecked());
            Local<Value> maxFrameRate = option->Get(Nan::New("maxFrameRate").ToLocalChecked());
            Local<Value> minFrameRate = option->Get(Nan::New("minFrameRate").ToLocalChecked());
            Local<Value> sourceId = option->Get(Nan::New("sourceId").ToLocalChecked());

            self->SetOptional(webrtc::MediaConstraintsInterface::kMinAspectRatio, minAspectRatio);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxAspectRatio, maxAspectRatio);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxWidth, maxWidth);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinWidth, minWidth);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxHeight, maxHeight);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinHeight, minHeight);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxFrameRate, maxFrameRate);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinFrameRate, minFrameRate);
            
            if (!sourceId.IsEmpty() && sourceId->IsString()) {
              String::Utf8Value sourceId_str(sourceId->ToString());
              self->_videoId = *sourceId_str;
            }
          }
        }
      }

      self->_video = true;
    }
  }


  return self;
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New(const Local<Value> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  if (!constraints.IsEmpty() && constraints->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(constraints);
    return MediaConstraints::New(obj);
  }

  return MediaConstraints::New();
}


void MediaConstraints::SetOptional(std::string key, Local<Value> value) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
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
      Nan::ThrowError("Unknown MediaConstraints Type");
    }
  }
}

void MediaConstraints::SetMandatory(std::string key, Local<Value> value) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
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
      Nan::ThrowError("Unknown MediaConstraints Type");
    }
  }
}

bool MediaConstraints::IsMandatory(const std::string& key) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  std::string value;

  if (_mandatory.FindFirst(key, &value)) {
    return true;
  }

  return false;
}

bool MediaConstraints::GetMandatory(const std::string& key) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  std::string value;

  if (_mandatory.FindFirst(key, &value)) {
    if (!value.compare("true")) {
      return true;
    }
  }

  return false;
}

void MediaConstraints::RemoveMandatory(const std::string& key) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
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
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _mandatory.push_back(webrtc::MediaConstraintsInterface::Constraint(key, value));
}

void MediaConstraints::SetMandatory(const std::string &key, const std::string &value) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaConstraints::RemoveMandatory(key);
  MediaConstraints::AddMandatory(key, value);
}

bool MediaConstraints::IsOptional(const std::string& key) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  std::string value;

  if (_mandatory.FindFirst(key, &value)) {
    return true;
  }

  return false;
}

bool MediaConstraints::GetOptional(const std::string& key) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  std::string value;

  if (_optional.FindFirst(key, &value)) {
    if (!value.compare("true")) {
      return true;
    }
  }

  return false;
}

void MediaConstraints::RemoveOptional(const std::string& key) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
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
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _optional.push_back(webrtc::MediaConstraintsInterface::Constraint(key, value));
}

void MediaConstraints::SetOptional(const std::string &key, const std::string &value) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaConstraints::RemoveOptional(key);
  MediaConstraints::AddOptional(key, value);
}

bool MediaConstraints::UseAudio() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _audio;
}

bool MediaConstraints::UseVideo() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _video;
}

std::string MediaConstraints::AudioId() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _audioId;
}

std::string MediaConstraints::VideoId() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _videoId;
}

const webrtc::MediaConstraintsInterface *MediaConstraints::ToConstraints() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return this;
}

const webrtc::MediaConstraintsInterface::Constraints &MediaConstraints::GetMandatory() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _mandatory;
}

const webrtc::MediaConstraintsInterface::Constraints &MediaConstraints::GetOptional() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _optional;
}
