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

#include "PeerConnection.h"
#include "DataChannel.h"
#include "MediaStream.h"

using namespace v8;
using namespace WebRTC;

void PeerConnection::Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, PeerConnection::New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "RTCPeerConnection"));
  
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "createOffer"),
                                FunctionTemplate::New(isolate, PeerConnection::CreateOffer)); 
                                
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "createAnswer"),
                                FunctionTemplate::New(isolate, PeerConnection::CreateAnswer));
                                
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "setLocalDescription"),
                                FunctionTemplate::New(isolate, PeerConnection::SetLocalDescription));
                                
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "setRemoteDescription"),
                                FunctionTemplate::New(isolate, PeerConnection::SetRemoteDescription));
                                
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "addIceCandidate"),
                                FunctionTemplate::New(isolate, PeerConnection::AddIceCandidate));
                                
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "createDataChannel"),
                                FunctionTemplate::New(isolate, PeerConnection::CreateDataChannel));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "addStream"),
                                                    FunctionTemplate::New(isolate, PeerConnection::AddStream));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "removeStream"),
                                                    FunctionTemplate::New(isolate, PeerConnection::RemoveStream));
                                
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "close"),
                                FunctionTemplate::New(isolate, PeerConnection::Close));
                                
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "signalingState"),
                                       PeerConnection::GetSignalingState,
                                       PeerConnection::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "iceConnectionState"),
                                       PeerConnection::GetIceConnectionState,
                                       PeerConnection::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "iceGatheringState"),
                                       PeerConnection::GetIceGatheringState,
                                       PeerConnection::ReadOnly);                                    
  
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onsignalingstatechange"),
                                       PeerConnection::GetOnSignalingStateChange,
                                       PeerConnection::SetOnSignalingStateChange);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onicecandidate"),
                                       PeerConnection::GetOnIceCandidate,
                                       PeerConnection::SetOnIceCandidate);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "ondatachannel"),
                                       PeerConnection::GetOnDataChannel,
                                       PeerConnection::SetOnDataChannel);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onnegotiationneeded"),
                                       PeerConnection::GetOnNegotiationNeeded,
                                       PeerConnection::SetOnNegotiationNeeded);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onaddstream"),
                                       PeerConnection::GetOnAddStream,
                                       PeerConnection::SetOnAddStream);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onremovestream"),
                                       PeerConnection::GetOnRemoveStream,
                                       PeerConnection::SetOnRemoveStream);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "RTCPeerConnection"), tpl->GetFunction());
}

Persistent<Function> PeerConnection::constructor;

PeerConnection::PeerConnection(const Local<Object> &configuration,
                               const Local<Object> &constraints)
{  
  Isolate *isolate = Isolate::GetCurrent();
    
  if (!configuration.IsEmpty()) {
    Local<Value> iceservers_value = configuration->Get(String::NewFromUtf8(isolate, "iceServers"));
    
    if (!iceservers_value.IsEmpty() && iceservers_value->IsArray()) {
      Local<Array> list = Local<Array>::Cast(iceservers_value);

      for (unsigned int index = 0; index < list->Length(); index++) {
        Local<Value> server_value = list->Get(index);

        if (!server_value.IsEmpty() && server_value->IsObject()) {
          Local<Object> server = Local<Object>::Cast(server_value);
          Local<Value> url_value = server->Get(String::NewFromUtf8(isolate, "url"));
          Local<Value> username_value = server->Get(String::NewFromUtf8(isolate, "username"));
          Local<Value> credential_value = server->Get(String::NewFromUtf8(isolate, "credential"));

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

            _servers.push_back(entry);
          }
        }        
      }
    }
  }

  _constraints = MediaConstraints::New(isolate, constraints);

  _offer = new rtc::RefCountedObject<OfferObserver>(this);
  _answer = new rtc::RefCountedObject<AnswerObserver>(this);
  _local = new rtc::RefCountedObject<LocalDescriptionObserver>(this);
  _remote = new rtc::RefCountedObject<RemoteDescriptionObserver>(this);
  _peer = new rtc::RefCountedObject<PeerConnectionObserver>(this);
  
  _factory = webrtc::CreatePeerConnectionFactory();  
}

PeerConnection::~PeerConnection() {
  if (_socket.get()) {
    webrtc::PeerConnectionInterface::SignalingState state(_socket->signaling_state());

    if (state != webrtc::PeerConnectionInterface::kClosed) {
      _socket->Close();
    }
    
    _socket.release();
  }
  
  EventEmitter::End();
}

webrtc::PeerConnectionInterface *PeerConnection::GetSocket() {  
  if (!_socket.get()) {
    EventEmitter::Start();    
    webrtc::FakeConstraints constraints = _constraints->ToConstraints();
    _socket = _factory->CreatePeerConnection(_servers, &constraints, NULL, NULL, _peer.get());
  }
   
  return _socket.get();
}

void PeerConnection::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  Local<Object> configuration;
  Local<Object> constraints;
  
  if (args.Length() >= 1 && args[0]->IsObject()) {
    configuration = Local<Object>::Cast(args[0]);
    
    if (args.Length() >= 2 && args[1]->IsObject()) {
      constraints = Local<Object>::Cast(args[1]);
    }
  }

  if (args.IsConstructCall()) {
    PeerConnection* peer = new PeerConnection(configuration, constraints);
    peer->Wrap(isolate, args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    const int argc = 2;
    Local<Value> argv[argc] = {
      configuration,
      constraints
    };
    
    Local<Function> instance = Local<Function>::New(isolate, PeerConnection::constructor);
    args.GetReturnValue().Set(instance->NewInstance(argc, argv));
  }
}

void PeerConnection::CreateOffer(const FunctionCallbackInfo<Value> &args) {  
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (!args[0].IsEmpty() && args[0]->IsFunction()) {
    self->_offerCallback.Reset(isolate, Local<Function>::Cast(args[0]));
  } else {
    self->_offerCallback.Reset();
  }
  
  if (!args[1].IsEmpty() && args[1]->IsFunction()) {
    self->_offerErrorCallback.Reset(isolate, Local<Function>::Cast(args[1]));
  } else {
    self->_offerErrorCallback.Reset();
  }
  
  if (socket) {
    socket->CreateOffer(self->_offer.get(), NULL);
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void PeerConnection::CreateAnswer(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (!args[0].IsEmpty() && args[0]->IsFunction()) {
    self->_answerCallback.Reset(isolate, Local<Function>::Cast(args[0]));
  } else {
    self->_answerCallback.Reset();
  }
  
  if (!args[1].IsEmpty() && args[1]->IsFunction()) {
    self->_answerErrorCallback.Reset(isolate, Local<Function>::Cast(args[1]));
  } else {
    self->_answerErrorCallback.Reset();
  }
  
  if (socket) {
    socket->CreateAnswer(self->_answer.get(), NULL);
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void PeerConnection::SetLocalDescription(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  const char *error = "Invalid SessionDescription";

  if (!args[0].IsEmpty() && args[0]->IsObject()) {
    Local<Object> desc = Local<Object>::Cast(args[0]);
    Local<Value> type_value = desc->Get(String::NewFromUtf8(isolate, "type"));
    Local<Value> sdp_value = desc->Get(String::NewFromUtf8(isolate, "sdp"));
    
    if (!type_value.IsEmpty() && type_value->IsString()) {
      if (!sdp_value.IsEmpty() && sdp_value->IsString()) {
        if (!args[1].IsEmpty() && args[1]->IsFunction()) {
          self->_localCallback.Reset(isolate, Local<Function>::Cast(args[1]));
        } else {
          self->_localCallback.Reset();
        }

        if (!args[2].IsEmpty() && args[2]->IsFunction()) {
          self->_localErrorCallback.Reset(isolate, Local<Function>::Cast(args[2]));
        } else {
          self->_localErrorCallback.Reset();
        }
    
        String::Utf8Value type(type_value->ToString());
        String::Utf8Value sdp(sdp_value->ToString());

        webrtc::SessionDescriptionInterface *desc(webrtc::CreateSessionDescription(*type, *sdp));
        
        if (desc) {
          if (socket) {
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
    if (!args[2].IsEmpty() && args[2]->IsFunction()) {
      Handle<Value> argv[1] = {
        Exception::Error(String::NewFromUtf8(isolate, error))
      };

      Local<Function> onerror = Local<Function>::Cast(args[2]);
      onerror->Call(args.This(), 1, argv);
    } else {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, error)));
    }
  }
}

void PeerConnection::SetRemoteDescription(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  const char *error = "Invalid SessionDescription";
  
  if (!args[0].IsEmpty() && args[0]->IsObject()) {
    Local<Object> desc = Local<Object>::Cast(args[0]);
    Local<Value> type_value = desc->Get(String::NewFromUtf8(isolate, "type"));
    Local<Value> sdp_value = desc->Get(String::NewFromUtf8(isolate, "sdp"));
    
    if (!type_value.IsEmpty() && type_value->IsString()) {
      if (!sdp_value.IsEmpty() && sdp_value->IsString()) {
        if (!args[1].IsEmpty() && args[1]->IsFunction()) {
          self->_remoteCallback.Reset(isolate, Local<Function>::Cast(args[1]));
        } else {
          self->_remoteCallback.Reset();
        }

        if (!args[2].IsEmpty() && args[2]->IsFunction()) {
          self->_remoteErrorCallback.Reset(isolate, Local<Function>::Cast(args[2]));
        } else {
          self->_remoteErrorCallback.Reset();
        }
    
        String::Utf8Value type(type_value->ToString());
        String::Utf8Value sdp(sdp_value->ToString());

        webrtc::SessionDescriptionInterface *desc(webrtc::CreateSessionDescription(*type, *sdp));
        
        if (desc) {
          if (socket) {
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
    if (!args[2].IsEmpty() && args[2]->IsFunction()) {
      Handle<Value> argv[1] = {
        Exception::Error(String::NewFromUtf8(isolate, error))
      };

      Local<Function> onerror = Local<Function>::Cast(args[2]);
      onerror->Call(args.This(), 1, argv);
    } else {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, error)));
    }
  }
}

void PeerConnection::AddIceCandidate(const FunctionCallbackInfo<Value> &args) {  
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  const char *error = 0;
  Handle<Value> argv[1];

  if (!args[0].IsEmpty() && args[0]->IsObject()) {
    Local<Object> desc = Local<Object>::Cast(args[0]);
    Local<Value> sdpMid_value = desc->Get(String::NewFromUtf8(isolate, "sdpMid"));
    Local<Value> sdpMLineIndex_value = desc->Get(String::NewFromUtf8(isolate, "sdpMLineIndex"));
    Local<Value> sdp_value = desc->Get(String::NewFromUtf8(isolate, "candidate"));
    
    if (!sdpMid_value.IsEmpty() && sdpMid_value->IsString()) {
      if (!sdpMLineIndex_value.IsEmpty() && sdpMLineIndex_value->IsInt32()) {
        if (!sdp_value.IsEmpty() && sdp_value->IsString()) {
          Local<Int32> sdpMLineIndex(sdpMLineIndex_value->ToInt32());
          String::Utf8Value sdpMid(sdpMid_value->ToString());
          String::Utf8Value sdp(sdp_value->ToString());
          
          rtc::scoped_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(*sdpMid, sdpMLineIndex->Value(), *sdp));
  
          if (candidate.get()) {
            if (socket) {
              if (socket->AddIceCandidate(candidate.get())) {
                if (!args[1].IsEmpty() && args[1]->IsFunction()) {
                  Local<Function> success = Local<Function>::Cast(args[1]);
                  success->Call(args.This(), 0, argv);
                }
              }
            } else {            
              error = "Failed to add ICECandidate";
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
    if (!args[2].IsEmpty() && args[2]->IsFunction()) {
      argv[1] = Exception::Error(String::NewFromUtf8(isolate, error));
      
      Local<Function> onerror = Local<Function>::Cast(args[2]);
      onerror->Call(args.This(), 1, argv);
    } else {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, error)));
    }
  }
}

void PeerConnection::CreateDataChannel(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  std::string label;
  webrtc::DataChannelInit config;
  
  if (!args[0].IsEmpty() && args[0]->IsString()) {
    String::Utf8Value label_utf8(args[0]->ToString());
    label = *label_utf8;
  }
  
  if (!args[1].IsEmpty() && args[1]->IsObject()) {
    Local<Object> config_obj = Local<Object>::Cast(args[0]);
    
    Local<Value> reliable_value = config_obj->Get(String::NewFromUtf8(isolate, "reliable"));
    Local<Value> ordered_value = config_obj->Get(String::NewFromUtf8(isolate, "ordered"));
    Local<Value> maxRetransmitTime_value = config_obj->Get(String::NewFromUtf8(isolate, "maxRetransmitTime"));
    Local<Value> maxRetransmits_value = config_obj->Get(String::NewFromUtf8(isolate, "maxRetransmits"));
    Local<Value> protocol_value = config_obj->Get(String::NewFromUtf8(isolate, "protocol"));
    Local<Value> id_value = config_obj->Get(String::NewFromUtf8(isolate, "id"));

    if (!reliable_value.IsEmpty() && reliable_value->IsTrue()) {
      config.reliable = true;
    }
    
    if (!ordered_value.IsEmpty() && ordered_value->IsFalse()) {
      config.ordered = false;
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
      return args.GetReturnValue().Set(DataChannel::New(isolate, dataChannel));
    }
  }
  
  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

void PeerConnection::AddStream(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This());
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = MediaStream::Unwrap(isolate, args[0]);

  if (mediaStream.get()) {
    webrtc::PeerConnectionInterface *socket = self->GetSocket();

    if (socket) {
      if (!socket->AddStream(mediaStream)) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "AddStream Failed")));
      }
    } else {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
    }
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid MediaStream Object")));
  }
}

void PeerConnection::RemoveStream(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This());
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = MediaStream::Unwrap(isolate, args[0]);

  if (mediaStream.get()) {
    webrtc::PeerConnectionInterface *socket = self->GetSocket();

    if (socket) {
      socket->RemoveStream(mediaStream);
    } else {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
    }
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid MediaStream Object")));
  }
}

void PeerConnection::Close(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, args.This()); 
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (socket) {
    socket->Close();
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void PeerConnection::GetSignalingState(Local<String> property, 
                                       const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::SignalingState state(socket->signaling_state());
    
    switch (state) {
      case webrtc::PeerConnectionInterface::kStable:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "stable"));
        break;
      case webrtc::PeerConnectionInterface::kHaveLocalOffer:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "have-local-offer"));
        break;
      case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "have-local-pranswer"));
        break;
      case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "have-remote-offer"));
        break;
      case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "have-remote-pranswer"));
        break;
      default: 
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "closed"));
        break;
    }
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void PeerConnection::GetIceConnectionState(Local<String> property, 
                                           const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::IceConnectionState state(socket->ice_connection_state());

    switch (state) {
      case webrtc::PeerConnectionInterface::kIceConnectionNew:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "new"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionChecking:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "checking"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionConnected:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "connected"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionCompleted:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "completed"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionFailed:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "failed"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "disconnected"));
        break;
      default:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "closed"));
        break;
    }
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void PeerConnection::GetIceGatheringState(Local<String> property, 
                                          const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::IceGatheringState state(socket->ice_gathering_state());

    switch (state) {
      case webrtc::PeerConnectionInterface::kIceGatheringNew:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "new"));
        break;
      case webrtc::PeerConnectionInterface::kIceGatheringGathering:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "gathering"));
        break;
      default:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "complete"));
        break;
    }   
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void PeerConnection::GetOnSignalingStateChange(Local<String> property, 
                                               const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onsignalingstatechange));
}

void PeerConnection::GetOnIceCandidate(Local<String> property, 
                                       const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onicecandidate));
}

void PeerConnection::GetOnDataChannel(Local<String> property, 
                                      const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_ondatachannel));
}

void PeerConnection::GetOnNegotiationNeeded(Local<String> property, 
                                            const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onnegotiationneeded));
}

void PeerConnection::GetOnAddStream(Local<String> property, 
                                    const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onaddstream));
}

void PeerConnection::GetOnRemoveStream(Local<String> property,
                                       const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onremovestream));
}

void PeerConnection::ReadOnly(Local<String> property, 
                              Local<Value> value, 
                              const PropertyCallbackInfo<void> &info) 
{
  //Isolate *isolate = info.GetIsolate();
  //PeerConnection *self = node::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
}

void PeerConnection::SetOnSignalingStateChange(Local<String> property, 
                                               Local<Value> value, 
                                               const PropertyCallbackInfo<void> &info) 
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onsignalingstatechange.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onsignalingstatechange.Reset();
  }
}

void PeerConnection::SetOnIceCandidate(Local<String> property, 
                                       Local<Value> value, 
                                       const PropertyCallbackInfo<void> &info) 
{  
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onicecandidate.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onicecandidate.Reset();
  }
}

void PeerConnection::SetOnDataChannel(Local<String> property, 
                                      Local<Value> value, 
                                      const PropertyCallbackInfo<void> &info) 
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_ondatachannel.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_ondatachannel.Reset();
  }
}

void PeerConnection::SetOnNegotiationNeeded(Local<String> property, 
                                            Local<Value> value, 
                                            const PropertyCallbackInfo<void> &info) 
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onnegotiationneeded.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onnegotiationneeded.Reset();
  }
}

void PeerConnection::SetOnAddStream(Local<String> property, 
                                    Local<Value> value, 
                                    const PropertyCallbackInfo<void> &info) 
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onaddstream.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onaddstream.Reset();
  }
}

void PeerConnection::SetOnRemoveStream(Local<String> property,
                                       Local<Value> value,
                                       const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onremovestream.Reset(isolate, Local<Function>::Cast(value));
  }
  else {
    self->_onremovestream.Reset();
  }
}

void PeerConnection::On(Event *event) {
  Isolate *isolate = Isolate::GetCurrent();
  PeerConnectionEvent type = event->Type<PeerConnectionEvent>();
  Local<Function> callback;
  Local<Object> container;
  Local<Value> argv[1];
  bool isError = false;
  std::string data;
  int argc = 0;
  
  switch(type) {
    case kPeerConnectionCreateOffer:
      callback = Local<Function>::New(isolate, _offerCallback);
      
      _offerCallback.Reset();
      _offerErrorCallback.Reset();
      
      data = event->Unwrap<std::string>();
      argv[0] = JSON::Parse(String::NewFromUtf8(isolate, data.c_str()));
      argc = 1;
      
      break;
    case kPeerConnectionCreateOfferError:
      callback = Local<Function>::New(isolate, _offerErrorCallback);
      
      _offerCallback.Reset();
      _offerErrorCallback.Reset();
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Exception::Error(String::NewFromUtf8(isolate, data.c_str()));
      argc = 1;
      
      break;
    case kPeerConnectionCreateAnswer:
      callback = Local<Function>::New(isolate, _answerCallback);
      
      _answerCallback.Reset();
      _answerErrorCallback.Reset();
      
      data = event->Unwrap<std::string>();
      argv[0] = JSON::Parse(String::NewFromUtf8(isolate, data.c_str()));
      argc = 1;
      
      break;
    case kPeerConnectionCreateAnswerError:
      callback = Local<Function>::New(isolate, _answerErrorCallback);
      
      _answerCallback.Reset();
      _answerErrorCallback.Reset();
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Exception::Error(String::NewFromUtf8(isolate, data.c_str()));
      argc = 1;
      
      break;
    case kPeerConnectionSetLocalDescription:
      callback = Local<Function>::New(isolate, _localCallback);
      
      _localCallback.Reset();
      _localErrorCallback.Reset();
      
      break;
    case kPeerConnectionSetLocalDescriptionError:
      callback = Local<Function>::New(isolate, _localErrorCallback);
      
      _localCallback.Reset();
      _localErrorCallback.Reset();
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Exception::Error(String::NewFromUtf8(isolate, data.c_str()));
      argc = 1;
      
      break;
    case kPeerConnectionSetRemoteDescription:
      callback = Local<Function>::New(isolate, _remoteCallback);
      
      _remoteCallback.Reset();
      _remoteErrorCallback.Reset();
      
      break;
    case kPeerConnectionSetRemoteDescriptionError:
      callback = Local<Function>::New(isolate, _remoteErrorCallback);
      
      _remoteCallback.Reset();
      _remoteErrorCallback.Reset();
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Exception::Error(String::NewFromUtf8(isolate, data.c_str()));
      argc = 1;
      
      break;
    case kPeerConnectionIceCandidate:
      callback = Local<Function>::New(isolate, _onicecandidate);
      
      data = event->Unwrap<std::string>();
      
      container = Object::New(isolate);
      container->Set(String::NewFromUtf8(isolate, "candidate"),
                     JSON::Parse(String::NewFromUtf8(isolate, data.c_str())));

      argv[0] = container;
      argc = 1;
      
      break;
    case kPeerConnectionSignalChange:
      callback = Local<Function>::New(isolate, _onsignalingstatechange);
      
      break;
    case kPeerConnectionIceChange:
      
      break;
    case kPeerConnectionIceGathering:
    
      break;
    case kPeerConnectionDataChannel:
      callback = Local<Function>::New(isolate, _ondatachannel);
      
      container = Object::New(isolate);
      container->Set(String::NewFromUtf8(isolate, "channel"),
                     DataChannel::New(isolate, event->Unwrap<rtc::scoped_refptr<webrtc::DataChannelInterface> >()));

      argv[0] = container;
      argc = 1;
      
      break;
    case kPeerConnectionAddStream:
      callback = Local<Function>::New(isolate, _onaddstream);

      argv[0] = MediaStream::New(isolate, event->Unwrap<rtc::scoped_refptr<webrtc::MediaStreamInterface> >());
      argc = 1;

      break;
    case kPeerConnectionRemoveStream:
      callback = Local<Function>::New(isolate, _onremovestream);
      
      argv[0] = MediaStream::New(isolate, event->Unwrap<rtc::scoped_refptr<webrtc::MediaStreamInterface> >());
      argc = 1;

      break;
    case kPeerConnectionRenegotiation:
      callback = Local<Function>::New(isolate, _onnegotiationneeded);
      
      break;
  }
  
  if (!callback.IsEmpty() && callback->IsFunction()) {
    callback->Call(RTCWrap::This(isolate), argc, argv);
  } else if (isError) {
    isolate->ThrowException(argv[0]);
  }
}
