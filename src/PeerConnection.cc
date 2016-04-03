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

#include <nan.h>
#include "Global.h"
#include "Platform.h"
#include "PeerConnection.h"
#include "DataChannel.h"
#include "MediaStream.h"
#include "Stats.h"

using namespace v8;
using namespace WebRTC;

void PeerConnection::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(PeerConnection::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("RTCPeerConnection").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "createOffer", PeerConnection::CreateOffer);
  Nan::SetPrototypeMethod(tpl, "createAnswer", PeerConnection::CreateAnswer);
  Nan::SetPrototypeMethod(tpl, "setLocalDescription", PeerConnection::SetLocalDescription);
  Nan::SetPrototypeMethod(tpl, "setRemoteDescription", PeerConnection::SetRemoteDescription);
  Nan::SetPrototypeMethod(tpl, "addIceCandidate", PeerConnection::AddIceCandidate);
  Nan::SetPrototypeMethod(tpl, "createDataChannel", PeerConnection::CreateDataChannel);
  Nan::SetPrototypeMethod(tpl, "addStream", PeerConnection::AddStream);
  Nan::SetPrototypeMethod(tpl, "removeStream", PeerConnection::RemoveStream);
  Nan::SetPrototypeMethod(tpl, "getLocalStreams", PeerConnection::GetLocalStreams);
  Nan::SetPrototypeMethod(tpl, "getRemoteStreams", PeerConnection::GetRemoteStreams);
  Nan::SetPrototypeMethod(tpl, "getStreamById", PeerConnection::GetStreamById);
  Nan::SetPrototypeMethod(tpl, "getStats", PeerConnection::GetStats);
  Nan::SetPrototypeMethod(tpl, "close", PeerConnection::Close);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("signalingState").ToLocalChecked(), PeerConnection::GetSignalingState);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("iceConnectionState").ToLocalChecked(), PeerConnection::GetIceConnectionState);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("iceGatheringState").ToLocalChecked(), PeerConnection::GetIceGatheringState);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("localDescription").ToLocalChecked(), PeerConnection::GetLocalDescription);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("remoteDescription").ToLocalChecked(), PeerConnection::GetRemoteDescription);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onsignalingstatechange").ToLocalChecked(), PeerConnection::GetOnSignalingStateChange, PeerConnection::SetOnSignalingStateChange);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("oniceconnectionstatechange").ToLocalChecked(), PeerConnection::GetOnIceConnectionStateChange, PeerConnection::SetOnIceConnectionStateChange);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onicecandidate").ToLocalChecked(), PeerConnection::GetOnIceCandidate, PeerConnection::SetOnIceCandidate);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("ondatachannel").ToLocalChecked(), PeerConnection::GetOnDataChannel, PeerConnection::SetOnDataChannel);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onnegotiationneeded").ToLocalChecked(), PeerConnection::GetOnNegotiationNeeded, PeerConnection::SetOnNegotiationNeeded);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onaddstream").ToLocalChecked(), PeerConnection::GetOnAddStream, PeerConnection::SetOnAddStream);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onremovestream").ToLocalChecked(), PeerConnection::GetOnRemoveStream, PeerConnection::SetOnRemoveStream);

  constructor.Reset<Function>(tpl->GetFunction());
  exports->Set(Nan::New("RTCPeerConnection").ToLocalChecked(), tpl->GetFunction());
}

Nan::Persistent<Function> PeerConnection::constructor;

PeerConnection::PeerConnection(const Local<Object> &configuration,
                               const Local<Object> &constraints)
{ 
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
    
  if (!configuration.IsEmpty()) {
    Local<Value> iceservers_value = configuration->Get(Nan::New("iceServers").ToLocalChecked());
    
    if (!iceservers_value.IsEmpty() && iceservers_value->IsArray()) {
      Local<Array> list = Local<Array>::Cast(iceservers_value);

      for (unsigned int index = 0; index < list->Length(); index++) {
        Local<Value> server_value = list->Get(index);

        if (!server_value.IsEmpty() && server_value->IsObject()) {
          Local<Object> server = Local<Object>::Cast(server_value);
          Local<Value> url_value = server->Get(Nan::New("url").ToLocalChecked());
          Local<Value> username_value = server->Get(Nan::New("username").ToLocalChecked());
          Local<Value> credential_value = server->Get(Nan::New("credential").ToLocalChecked());

          if (!url_value.IsEmpty() && url_value->IsString()) {
            v8::String::Utf8Value url(url_value->ToString());
            webrtc::PeerConnectionInterface::IceServer entry;

            entry.uri = *url;

            if (!username_value.IsEmpty() && username_value->IsString()) {
              String::Utf8Value username(username_value->ToString());
              entry.username = *username;
            }

            if (!credential_value.IsEmpty() && credential_value->IsString()) {
              String::Utf8Value credential(credential_value->ToString());
              entry.password = *credential;
            }

            _config.servers.push_back(entry);
          }
        }        
      }
    }
  }

  _constraints = MediaConstraints::New(constraints);

  if (!_constraints->GetOptional("RtpDataChannels")) {
    if (!_constraints->IsOptional("DtlsSrtpKeyAgreement")) {
      _constraints->SetOptional("DtlsSrtpKeyAgreement", "true");
    }
  }

  _stats = new rtc::RefCountedObject<StatsObserver>(this);
  _offer = new rtc::RefCountedObject<OfferObserver>(this);
  _answer = new rtc::RefCountedObject<AnswerObserver>(this);
  _local = new rtc::RefCountedObject<LocalDescriptionObserver>(this);
  _remote = new rtc::RefCountedObject<RemoteDescriptionObserver>(this);
  _peer = new rtc::RefCountedObject<PeerConnectionObserver>(this);
  _factory = webrtc::CreatePeerConnectionFactory(rtc::Thread::Current(), Platform::GetWorker(), 0, 0, 0);
}

PeerConnection::~PeerConnection() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_socket.get()) {
    webrtc::PeerConnectionInterface::SignalingState state(_socket->signaling_state());

    if (state != webrtc::PeerConnectionInterface::kClosed) {
      _socket->Close();
    }
  }
  
  _stats->RemoveListener(this);
  _offer->RemoveListener(this);
  _answer->RemoveListener(this);
  _local->RemoveListener(this);
  _remote->RemoveListener(this);
  _peer->RemoveListener(this);
}

webrtc::PeerConnectionInterface *PeerConnection::GetSocket() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (!_socket.get()) {
    if (_factory.get()) {
      EventEmitter::SetReference(true);
      _socket = _factory->CreatePeerConnection(_config, _constraints->ToConstraints(), NULL, NULL, _peer.get());
      
      if (!_socket.get()) {
        Nan::ThrowError("Internal Socket Error");
      }
    } else {
      Nan::ThrowError("Internal Factory Error");
    }
  }
   
  return _socket.get();
}

void PeerConnection::New(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Local<Object> configuration;
  Local<Object> constraints;
  
  if (info.Length() >= 1 && info[0]->IsObject()) {
    configuration = Local<Object>::Cast(info[0]);
    
    if (info.Length() >= 2 && info[1]->IsObject()) {
      constraints = Local<Object>::Cast(info[1]);
    }
  }

  if (info.IsConstructCall()) {
    PeerConnection* peer = new PeerConnection(configuration, constraints);
    peer->Wrap(info.This(), "PeerConnection");
    return info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 2;
    Local<Value> argv[argc] = {
      configuration,
      constraints
    };
    
    Local<Function> instance = Nan::New(PeerConnection::constructor);
    return info.GetReturnValue().Set(instance->NewInstance(argc, argv));
  }
}

void PeerConnection::CreateOffer(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (!info[0].IsEmpty() && info[0]->IsFunction()) {
    self->_offerCallback.Reset<Function>(Local<Function>::Cast(info[0]));
  } else {
    self->_offerCallback.Reset();
  }
  
  if (!info[1].IsEmpty() && info[1]->IsFunction()) {
    self->_offerErrorCallback.Reset<Function>(Local<Function>::Cast(info[1]));
  } else {
    self->_offerErrorCallback.Reset();
  }
  
  if (socket) {
    socket->CreateOffer(self->_offer.get(), self->_constraints->ToConstraints());
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::CreateAnswer(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (!info[0].IsEmpty() && info[0]->IsFunction()) {
    self->_answerCallback.Reset<Function>(Local<Function>::Cast(info[0]));
  } else {
    self->_answerCallback.Reset();
  }
  
  if (!info[1].IsEmpty() && info[1]->IsFunction()) {
    self->_answerErrorCallback.Reset<Function>(Local<Function>::Cast(info[1]));
  } else {
    self->_answerErrorCallback.Reset();
  }
  
  if (socket) {
    socket->CreateAnswer(self->_answer.get(), self->_constraints->ToConstraints());
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::SetLocalDescription(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  const char *error = "Invalid SessionDescription";

  if (!info[0].IsEmpty() && info[0]->IsObject()) {
    Local<Object> desc_obj = Local<Object>::Cast(info[0]);
    Local<Value> type_value = desc_obj->Get(Nan::New("type").ToLocalChecked());
    Local<Value> sdp_value = desc_obj->Get(Nan::New("sdp").ToLocalChecked());
    
    if (!type_value.IsEmpty() && type_value->IsString()) {
      if (!sdp_value.IsEmpty() && sdp_value->IsString()) {
        if (!info[1].IsEmpty() && info[1]->IsFunction()) {
          self->_localCallback.Reset<Function>(Local<Function>::Cast(info[1]));
        } else {
          self->_localCallback.Reset();
        }

        if (!info[2].IsEmpty() && info[2]->IsFunction()) {
          self->_localErrorCallback.Reset<Function>(Local<Function>::Cast(info[2]));
        } else {
          self->_localErrorCallback.Reset();
        }
    
        String::Utf8Value type(type_value->ToString());
        String::Utf8Value sdp(sdp_value->ToString());

        webrtc::SessionDescriptionInterface *desc(webrtc::CreateSessionDescription(*type, *sdp, 0));
        
        if (desc) {
          if (socket) {
            self->_localsdp.Reset<Object>(desc_obj);
            socket->SetLocalDescription(self->_local.get(), desc);
            error = 0;
          } else {
            error = "Internal Error";
          }
        }
      }
    }
  }
  
  if (error) {
    if (!info[2].IsEmpty() && info[2]->IsFunction()) {
      Local<Value> argv[1] = {
        Nan::Error(error)
      };

      Local<Function> onerror = Local<Function>::Cast(info[2]);
      onerror->Call(info.This(), 1, argv);
    } else {
      Nan::ThrowError(error);
    }
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::SetRemoteDescription(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  const char *error = "Invalid SessionDescription";
  
  if (!info[0].IsEmpty() && info[0]->IsObject()) {
    Local<Object> desc_obj = Local<Object>::Cast(info[0]);
    Local<Value> type_value = desc_obj->Get(Nan::New("type").ToLocalChecked());
    Local<Value> sdp_value = desc_obj->Get(Nan::New("sdp").ToLocalChecked());
    
    if (!type_value.IsEmpty() && type_value->IsString()) {
      if (!sdp_value.IsEmpty() && sdp_value->IsString()) {
        if (!info[1].IsEmpty() && info[1]->IsFunction()) {
          self->_remoteCallback.Reset<Function>(Local<Function>::Cast(info[1]));
        } else {
          self->_remoteCallback.Reset();
        }

        if (!info[2].IsEmpty() && info[2]->IsFunction()) {
          self->_remoteErrorCallback.Reset<Function>(Local<Function>::Cast(info[2]));
        } else {
          self->_remoteErrorCallback.Reset();
        }
    
        String::Utf8Value type(type_value->ToString());
        String::Utf8Value sdp(sdp_value->ToString());

        webrtc::SessionDescriptionInterface *desc(webrtc::CreateSessionDescription(*type, *sdp, 0));
        
        if (desc) {
          if (socket) {
            self->_remotesdp.Reset<Object>(desc_obj);
            socket->SetRemoteDescription(self->_remote.get(), desc);
            error = 0;
          } else {
            error = "Internal Error";
          }
        }
      }
    }
  }

  if (error) {
    if (!info[2].IsEmpty() && info[2]->IsFunction()) {
      Local<Value> argv[1] = {
        Nan::Error(error)
      };

      Local<Function> onerror = Local<Function>::Cast(info[2]);
      onerror->Call(info.This(), 1, argv);
    } else {
      Nan::ThrowError(error);
    }
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::AddIceCandidate(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  const char *error = 0;
  Local<Value> argv[1];

  if (!info[0].IsEmpty() && info[0]->IsObject()) {
    Local<Object> desc = Local<Object>::Cast(info[0]);
    Local<Value> sdpMid_value = desc->Get(Nan::New("sdpMid").ToLocalChecked());
    Local<Value> sdpMLineIndex_value = desc->Get(Nan::New("sdpMLineIndex").ToLocalChecked());
    Local<Value> sdp_value = desc->Get(Nan::New("candidate").ToLocalChecked());
    
    if (!sdpMid_value.IsEmpty() && sdpMid_value->IsString()) {
      if (!sdpMLineIndex_value.IsEmpty() && sdpMLineIndex_value->IsInt32()) {
        if (!sdp_value.IsEmpty() && sdp_value->IsString()) {
          Local<Int32> sdpMLineIndex(sdpMLineIndex_value->ToInt32());
          String::Utf8Value sdpMid(sdpMid_value->ToString());
          String::Utf8Value sdp(sdp_value->ToString());
          
          rtc::scoped_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(*sdpMid, sdpMLineIndex->Value(), *sdp, 0));
  
          if (candidate.get()) {
            if (socket) {
              if (socket->AddIceCandidate(candidate.get())) {
                if (!info[1].IsEmpty() && info[1]->IsFunction()) {
                  Local<Function> success = Local<Function>::Cast(info[1]);
                  success->Call(info.This(), 0, argv);
                }
              } else {            
                error = "Failed to add ICECandidate";
              }
            } else {
              error = "Internal Error";
            }
          } else {
            error = "Invalid ICECandidate";
          }
        } else {
          error = "Invalid candidate";
        }
      } else {
        error = "Invalid sdpMLineIndex";
      }
    } else {
      error = "Invalid sdpMid";
    }
  }
  
  if (error) {
    if (!info[2].IsEmpty() && info[2]->IsFunction()) {
      argv[0] = Nan::Error(error);
      
      Local<Function> onerror = Local<Function>::Cast(info[2]);
      onerror->Call(info.This(), 1, argv);
    } else {
      Nan::ThrowError(error);
    }
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::CreateDataChannel(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  std::string label;
  webrtc::DataChannelInit config;
  
  if (!info[0].IsEmpty() && info[0]->IsString()) {
    String::Utf8Value label_utf8(info[0]->ToString());
    label = *label_utf8;
  }
  
  if (!info[1].IsEmpty() && info[1]->IsObject()) {
    Local<Object> config_obj = Local<Object>::Cast(info[0]);
    
    Local<Value> reliable_value = config_obj->Get(Nan::New("reliable").ToLocalChecked());
    Local<Value> ordered_value = config_obj->Get(Nan::New("ordered").ToLocalChecked());
    Local<Value> maxRetransmitTime_value = config_obj->Get(Nan::New("maxRetransmitTime").ToLocalChecked());
    Local<Value> maxRetransmits_value = config_obj->Get(Nan::New("maxRetransmits").ToLocalChecked());
    Local<Value> protocol_value = config_obj->Get(Nan::New("protocol").ToLocalChecked());
    Local<Value> id_value = config_obj->Get(Nan::New("id").ToLocalChecked());

    if (!reliable_value.IsEmpty()) {
      if (reliable_value->IsTrue()) {
        config.reliable = true;
      } else {
        config.reliable = false;
      }
    }
    
    if (!ordered_value.IsEmpty()) {
      if (ordered_value->IsTrue()) {
        config.ordered = true;
      } else {
        config.ordered = false;
      }
    }
    
    if (!maxRetransmitTime_value.IsEmpty() && maxRetransmitTime_value->IsInt32()) {
      Local<Int32> maxRetransmitTime(maxRetransmitTime_value->ToInt32());
      config.maxRetransmitTime = maxRetransmitTime->Value();
    }
    
    if (!maxRetransmits_value.IsEmpty() && maxRetransmits_value->IsInt32()) {
      Local<Int32> maxRetransmits(maxRetransmits_value->ToInt32());
      config.maxRetransmits = maxRetransmits->Value();
    }
    
    if (!protocol_value.IsEmpty() && protocol_value->IsString()) {
      String::Utf8Value protocol(protocol_value->ToString());
      config.protocol = *protocol;
    }
    
    if (!id_value.IsEmpty() && id_value->IsInt32()) {
      Local<Int32> id(id_value->ToInt32());
      config.id = id->Value();
    }
  }
  
  if (socket) {
    rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel = socket->CreateDataChannel(label, &config);
    
    if (dataChannel.get()) {
      return info.GetReturnValue().Set(DataChannel::New(dataChannel));
    }
  }
  
  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::AddStream(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = MediaStream::Unwrap(info[0]);

  if (mediaStream.get()) {
    webrtc::PeerConnectionInterface *socket = self->GetSocket();

    if (socket) {
      if (!socket->AddStream(mediaStream)) {
        Nan::ThrowError("AddStream Failed");
      }
    } else {
      Nan::ThrowError("Internal Error");
    }
  } else {
    Nan::ThrowError("Invalid MediaStream Object");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::RemoveStream(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = MediaStream::Unwrap(info[0]);

  if (mediaStream.get()) {
    webrtc::PeerConnectionInterface *socket = self->GetSocket();

    if (socket) {
      socket->RemoveStream(mediaStream);
    } else {
      Nan::ThrowError("Internal Error");
    }
  } else {
    Nan::ThrowError("Invalid MediaStream Object");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::GetLocalStreams(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    rtc::scoped_refptr<webrtc::StreamCollectionInterface> local = socket->local_streams();

    if (local.get()) {
      Local<Array> list = Nan::New<Array>();
      uint32_t index = 0;
      size_t count;

      for (count = 0; count < local->count(); count++) {
        list->Set(index, MediaStream::New(local->at(count)));
      }

      return info.GetReturnValue().Set(list);
    }
  }
    
  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::GetRemoteStreams(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    rtc::scoped_refptr<webrtc::StreamCollectionInterface> remote = socket->remote_streams();

    if (remote.get()) {
      Local<Array> list = Nan::New<Array>();
      uint32_t index = 0;
      size_t count;

      for (count = 0; count < remote->count(); count++) {
        list->Set(index, MediaStream::New(remote->at(count)));
      }

      return info.GetReturnValue().Set(list);
    }
  }

  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::GetStreamById(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    if (info.Length() >= 1 && info[0]->IsString()) {
      v8::String::Utf8Value idValue(info[0]->ToString());
      std::string id(*idValue);

      rtc::scoped_refptr<webrtc::StreamCollectionInterface> local = socket->local_streams();
      rtc::scoped_refptr<webrtc::StreamCollectionInterface> remote = socket->remote_streams();
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream;

      if (local.get()) {
        stream = local->find(id);
      }

      if (remote.get() && !stream.get()) {
        stream = remote->find(id);
      }

      if (stream.get()) {
        return info.GetReturnValue().Set(MediaStream::New(stream));
      } else {
        return info.GetReturnValue().Set(Nan::Null());
      }
    } else {
      Nan::ThrowError("Invalid Argument");
    }
  }

  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::GetStats(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (!info[0].IsEmpty() && info[0]->IsFunction()) {
    self->_onstats.Reset<Function>(Local<Function>::Cast(info[0]));

    if (socket) {
      if (!socket->GetStats(self->_stats.get(), 0, webrtc::PeerConnectionInterface::kStatsOutputLevelStandard)) {
        Local<Function> callback = Nan::New<Function>(self->_onstats);
        Local<Value> argv[1] = { Nan::Null() };

        callback->Call(info.This(), 1, argv);
        self->_onstats.Reset();
      }
    } else {
      Nan::ThrowError("Internal Error");
    }
  } else {
    Nan::ThrowError("Missing Callback");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::Close(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.This(), "PeerConnection"); 
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (socket) {
    socket->Close();
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::GetSignalingState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::SignalingState state(socket->signaling_state());
    
    switch (state) {
      case webrtc::PeerConnectionInterface::kStable:
        return info.GetReturnValue().Set(Nan::New("stable").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kHaveLocalOffer:
        return info.GetReturnValue().Set(Nan::New("have-local-offer").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
        return info.GetReturnValue().Set(Nan::New("have-local-pranswer").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
        return info.GetReturnValue().Set(Nan::New("have-remote-offer").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
        return info.GetReturnValue().Set(Nan::New("have-remote-pranswer").ToLocalChecked());
        break;
      default: 
        return info.GetReturnValue().Set(Nan::New("closed").ToLocalChecked());
        break;
    }
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::GetIceConnectionState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::IceConnectionState state(socket->ice_connection_state());

    switch (state) {
      case webrtc::PeerConnectionInterface::kIceConnectionNew:
        return info.GetReturnValue().Set(Nan::New("new").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionChecking:
        return info.GetReturnValue().Set(Nan::New("checking").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionConnected:
        return info.GetReturnValue().Set(Nan::New("connected").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionCompleted:
        return info.GetReturnValue().Set(Nan::New("completed").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionFailed:
        return info.GetReturnValue().Set(Nan::New("failed").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:
        return info.GetReturnValue().Set(Nan::New("disconnected").ToLocalChecked());
        break;
      default:
        return info.GetReturnValue().Set(Nan::New("closed").ToLocalChecked());
        break;
    }
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::GetIceGatheringState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::IceGatheringState state(socket->ice_gathering_state());

    switch (state) {
      case webrtc::PeerConnectionInterface::kIceGatheringNew:
        return info.GetReturnValue().Set(Nan::New("new").ToLocalChecked());
        break;
      case webrtc::PeerConnectionInterface::kIceGatheringGathering:
        return info.GetReturnValue().Set(Nan::New("gathering").ToLocalChecked());
        break;
      default:
        return info.GetReturnValue().Set(Nan::New("complete").ToLocalChecked());
        break;
    }   
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void PeerConnection::GetOnSignalingStateChange(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onsignalingstatechange));
}

void PeerConnection::GetOnIceConnectionStateChange(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_oniceconnectionstatechange));
}

void PeerConnection::GetOnIceCandidate(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onicecandidate));
}

void PeerConnection::GetLocalDescription(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Object>(self->_localsdp));
}

void PeerConnection::GetRemoteDescription(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Object>(self->_remotesdp));
}

void PeerConnection::GetOnDataChannel(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_ondatachannel));
}

void PeerConnection::GetOnNegotiationNeeded(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onnegotiationneeded));
}

void PeerConnection::GetOnAddStream(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onaddstream));
}

void PeerConnection::GetOnRemoveStream(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onremovestream));
}

void PeerConnection::ReadOnly(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

void PeerConnection::SetOnSignalingStateChange(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onsignalingstatechange.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onsignalingstatechange.Reset();
  }
}

void PeerConnection::SetOnIceConnectionStateChange(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_oniceconnectionstatechange.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_oniceconnectionstatechange.Reset();
  }
}

void PeerConnection::SetOnIceCandidate(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onicecandidate.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onicecandidate.Reset();
  }
}

void PeerConnection::SetOnDataChannel(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_ondatachannel.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_ondatachannel.Reset();
  }
}

void PeerConnection::SetOnNegotiationNeeded(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onnegotiationneeded.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onnegotiationneeded.Reset();
  }
}

void PeerConnection::SetOnAddStream(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onaddstream.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onaddstream.Reset();
  }
}

void PeerConnection::SetOnRemoveStream(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(info.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onremovestream.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onremovestream.Reset();
  }
}

void PeerConnection::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  PeerConnectionEvent type = event->Type<PeerConnectionEvent>();
  Local<Function> callback;
  Local<Object> container;
  Local<Value> argv[1];
  bool isError = false;
  std::string data;
  int argc = 0;
  
  switch (type) {
    case kPeerConnectionCreateClosed:
      EventEmitter::SetReference(false);
      
      break;
    case kPeerConnectionCreateOffer:
      callback = Nan::New<Function>(_offerCallback);
      
      _offerCallback.Reset();
      _offerErrorCallback.Reset();

      data = event->Unwrap<std::string>();
      argv[0] = JSON::Parse(Nan::New(data.c_str()).ToLocalChecked());
      argc = 1;
      
      break;
    case kPeerConnectionCreateOfferError:
      callback = Nan::New<Function>(_offerErrorCallback);
      
      _offerCallback.Reset();
      _offerErrorCallback.Reset();
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      
      break;
    case kPeerConnectionCreateAnswer:
      callback = Nan::New<Function>(_answerCallback);
      
      _answerCallback.Reset();
      _answerErrorCallback.Reset();
      
      data = event->Unwrap<std::string>();
      argv[0] = JSON::Parse(Nan::New(data.c_str()).ToLocalChecked());
      argc = 1;
      
      break;
    case kPeerConnectionCreateAnswerError:
      callback = Nan::New<Function>(_answerErrorCallback);
      
      _answerCallback.Reset();
      _answerErrorCallback.Reset();
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      
      break;
    case kPeerConnectionSetLocalDescription:
      callback = Nan::New<Function>(_localCallback);
      
      _localCallback.Reset();
      _localErrorCallback.Reset();
      
      break;
    case kPeerConnectionSetLocalDescriptionError:
      callback = Nan::New<Function>(_localErrorCallback);
      
      _localCallback.Reset();
      _localErrorCallback.Reset();
      _localsdp.Reset();
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      
      break;
    case kPeerConnectionSetRemoteDescription:
      callback = Nan::New<Function>(_remoteCallback);
      
      _remoteCallback.Reset();
      _remoteErrorCallback.Reset();
      
      break;
    case kPeerConnectionSetRemoteDescriptionError:
      callback = Nan::New<Function>(_remoteErrorCallback);
      
      _remoteCallback.Reset();
      _remoteErrorCallback.Reset();
      _remotesdp.Reset();
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      
      break;
    case kPeerConnectionIceCandidate:
      callback = Nan::New<Function>(_onicecandidate);
      container = Nan::New<Object>();
      
      data = event->Unwrap<std::string>();
      
      if (data.empty()) {
        container->Set(Nan::New("candidate").ToLocalChecked(), Nan::Null());
      } else {
        container->Set(Nan::New("candidate").ToLocalChecked(), JSON::Parse(Nan::New(data.c_str()).ToLocalChecked()));
      }
      
      argv[0] = container;
      argc = 1;
      
      break;
    case kPeerConnectionSignalChange:
      callback = Nan::New<Function>(_onsignalingstatechange);
      
      break;
    case kPeerConnectionIceChange:
      callback = Nan::New<Function>(_oniceconnectionstatechange);
      
      break;
    case kPeerConnectionIceGathering:
      
      break;
    case kPeerConnectionDataChannel:
      callback = Nan::New<Function>(_ondatachannel);
      
      container = Nan::New<Object>();
      container->Set(Nan::New("channel").ToLocalChecked(), DataChannel::New(event->Unwrap<rtc::scoped_refptr<webrtc::DataChannelInterface> >()));

      argv[0] = container;
      argc = 1;
      
      break;
    case kPeerConnectionAddStream:
      callback = Nan::New<Function>(_onaddstream);

      container = Nan::New<Object>();
      container->Set(Nan::New("stream").ToLocalChecked(), MediaStream::New(event->Unwrap<rtc::scoped_refptr<webrtc::MediaStreamInterface> >()));
      
      argv[0] = container;
      argc = 1;

      break;
    case kPeerConnectionRemoveStream:
      callback = Nan::New<Function>(_onremovestream);
      
      container = Nan::New<Object>();
      container->Set(Nan::New("stream").ToLocalChecked(), MediaStream::New(event->Unwrap<rtc::scoped_refptr<webrtc::MediaStreamInterface> >()));
      
      argv[0] = container;
      argc = 1;

      break;
    case kPeerConnectionRenegotiation:
      callback = Nan::New<Function>(_onnegotiationneeded);
      
      break;
    case kPeerConnectionStats:
      callback = Nan::New<Function>(_onstats);

      argv[0] = RTCStatsResponse::New(event->Unwrap<webrtc::StatsReports>());
      argc = 1;

      break;
  }
  
  if (!callback.IsEmpty() && callback->IsFunction()) {
    callback->Call(RTCWrap::This(), argc, argv);
  } else if (isError) {
    Nan::ThrowError(argv[0]);
  }
}

bool PeerConnection::IsStable() {
  webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::SignalingState state(socket->signaling_state());
    
    if (state == webrtc::PeerConnectionInterface::kStable) {
      return true;
    }
  }
  
  return false;
}

