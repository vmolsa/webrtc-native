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
#include "PeerConnection.h"
#include "DataChannel.h"
#include "MediaStream.h"
#include "Stats.h"
#include "Core.h"

#if (NODE_MODULE_VERSION <= NODE_0_10_MODULE_VERSION)
namespace v8 {
  class JSON {
   public:
    static v8::Local<v8::Value> Parse(v8::Local<v8::Value> str) {
      NanEscapableScope();
      v8::Local<v8::Object> global = v8::Context::GetCurrent()->Global();
      v8::Local<v8::Object> json = v8::Local<v8::Object>::Cast(global->Get(NanNew("JSON")));
      v8::Local<v8::Function> parse = v8::Local<v8::Function>::Cast(json->Get(NanNew("parse")));
      return NanEscapeScope(parse->Call(global, 1, &str));
    };
  };
};
#endif

using namespace v8;
using namespace WebRTC;

void PeerConnection::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(PeerConnection::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew("RTCPeerConnection"));
  
  tpl->PrototypeTemplate()->Set(NanNew("createOffer"), NanNew<FunctionTemplate>(PeerConnection::CreateOffer)->GetFunction());                      
  tpl->PrototypeTemplate()->Set(NanNew("createAnswer"), NanNew<FunctionTemplate>(PeerConnection::CreateAnswer)->GetFunction());                        
  tpl->PrototypeTemplate()->Set(NanNew("setLocalDescription"), NanNew<FunctionTemplate>(PeerConnection::SetLocalDescription)->GetFunction());                   
  tpl->PrototypeTemplate()->Set(NanNew("setRemoteDescription"), NanNew<FunctionTemplate>(PeerConnection::SetRemoteDescription)->GetFunction());                           
  tpl->PrototypeTemplate()->Set(NanNew("addIceCandidate"), NanNew<FunctionTemplate>(PeerConnection::AddIceCandidate)->GetFunction());                       
  tpl->PrototypeTemplate()->Set(NanNew("createDataChannel"), NanNew<FunctionTemplate>(PeerConnection::CreateDataChannel)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("addStream"), NanNew<FunctionTemplate>(PeerConnection::AddStream)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("removeStream"), NanNew<FunctionTemplate>(PeerConnection::RemoveStream)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("getLocalStreams"), NanNew<FunctionTemplate>(PeerConnection::GetLocalStreams)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("getRemoteStreams"), NanNew<FunctionTemplate>(PeerConnection::GetRemoteStreams)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("getStreamById"), NanNew<FunctionTemplate>(PeerConnection::GetStreamById)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("getStats"), NanNew<FunctionTemplate>(PeerConnection::GetStats)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("close"), NanNew<FunctionTemplate>(PeerConnection::Close)->GetFunction());
                                
  tpl->InstanceTemplate()->SetAccessor(NanNew("signalingState"),
                                       PeerConnection::GetSignalingState,
                                       PeerConnection::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("iceConnectionState"),
                                       PeerConnection::GetIceConnectionState,
                                       PeerConnection::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("iceGatheringState"),
                                       PeerConnection::GetIceGatheringState,
                                       PeerConnection::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("localDescription"),
                                       PeerConnection::GetLocalDescription,
                                       PeerConnection::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("remoteDescription"),
                                       PeerConnection::GetRemoteDescription,
                                       PeerConnection::ReadOnly);
  
  tpl->InstanceTemplate()->SetAccessor(NanNew("onsignalingstatechange"),
                                       PeerConnection::GetOnSignalingStateChange,
                                       PeerConnection::SetOnSignalingStateChange);
  
  tpl->InstanceTemplate()->SetAccessor(NanNew("oniceconnectionstatechange"),
                                       PeerConnection::GetOnIceConnectionStateChange,
                                       PeerConnection::SetOnIceConnectionStateChange);
  
  tpl->InstanceTemplate()->SetAccessor(NanNew("onicecandidate"),
                                       PeerConnection::GetOnIceCandidate,
                                       PeerConnection::SetOnIceCandidate);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("ondatachannel"),
                                       PeerConnection::GetOnDataChannel,
                                       PeerConnection::SetOnDataChannel);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onnegotiationneeded"),
                                       PeerConnection::GetOnNegotiationNeeded,
                                       PeerConnection::SetOnNegotiationNeeded);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onaddstream"),
                                       PeerConnection::GetOnAddStream,
                                       PeerConnection::SetOnAddStream);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onremovestream"),
                                       PeerConnection::GetOnRemoveStream,
                                       PeerConnection::SetOnRemoveStream);

  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew("RTCPeerConnection"), tpl->GetFunction());
}

Persistent<Function> PeerConnection::constructor;

PeerConnection::PeerConnection(const Local<Object> &configuration,
                               const Local<Object> &constraints)
{ 
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
    
  if (!configuration.IsEmpty()) {
    Local<Value> iceservers_value = configuration->Get(NanNew("iceServers"));
    
    if (!iceservers_value.IsEmpty() && iceservers_value->IsArray()) {
      Local<Array> list = Local<Array>::Cast(iceservers_value);

      for (unsigned int index = 0; index < list->Length(); index++) {
        Local<Value> server_value = list->Get(index);

        if (!server_value.IsEmpty() && server_value->IsObject()) {
          Local<Object> server = Local<Object>::Cast(server_value);
          Local<Value> url_value = server->Get(NanNew("url"));
          Local<Value> username_value = server->Get(NanNew("username"));
          Local<Value> credential_value = server->Get(NanNew("credential"));

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
}

PeerConnection::~PeerConnection() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_socket.get()) {
    webrtc::PeerConnectionInterface::SignalingState state(_socket->signaling_state());

    if (state != webrtc::PeerConnectionInterface::kClosed) {
      _socket->Close();
    }
  }
  
  _stats->SetEmitter();
  _offer->SetEmitter();
  _answer->SetEmitter();
  _local->SetEmitter();
  _remote->SetEmitter();
  _peer->SetEmitter();
}

webrtc::PeerConnectionInterface *PeerConnection::GetSocket() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (!_socket.get()) {
    webrtc::PeerConnectionFactoryInterface *factory = Core::GetFactory();

    if (factory) {
      EventEmitter::SetReference(true);
      _socket = factory->CreatePeerConnection(_servers, _constraints->ToConstraints(), NULL, NULL, _peer.get());
    }
  }
   
  return _socket.get();
}

NAN_METHOD(PeerConnection::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
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
    peer->Wrap(args.This(), "PeerConnection");
    NanReturnValue(args.This());
  } else {
    const int argc = 2;
    Local<Value> argv[argc] = {
      configuration,
      constraints
    };
    
    Local<Function> instance = NanNew(PeerConnection::constructor);
    NanReturnValue(instance->NewInstance(argc, argv));
  }
}

NAN_METHOD(PeerConnection::CreateOffer) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (!args[0].IsEmpty() && args[0]->IsFunction()) {
    NanAssignPersistent(self->_offerCallback, Local<Function>::Cast(args[0]));
  } else {
    NanDisposePersistent(self->_offerCallback);
  }
  
  if (!args[1].IsEmpty() && args[1]->IsFunction()) {
    NanAssignPersistent(self->_offerErrorCallback, Local<Function>::Cast(args[1]));
  } else {
    NanDisposePersistent(self->_offerErrorCallback);
  }
  
  if (socket) {
    socket->CreateOffer(self->_offer.get(), self->_constraints->ToConstraints());
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::CreateAnswer) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (!args[0].IsEmpty() && args[0]->IsFunction()) {
    NanAssignPersistent(self->_answerCallback, Local<Function>::Cast(args[0]));
  } else {
    NanDisposePersistent(self->_answerCallback);
  }
  
  if (!args[1].IsEmpty() && args[1]->IsFunction()) {
    NanAssignPersistent(self->_answerErrorCallback, Local<Function>::Cast(args[1]));
  } else {
    NanDisposePersistent(self->_answerErrorCallback);
  }
  
  if (socket) {
    socket->CreateAnswer(self->_answer.get(), self->_constraints->ToConstraints());
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::SetLocalDescription) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  const char *error = "Invalid SessionDescription";

  if (!args[0].IsEmpty() && args[0]->IsObject()) {
    Local<Object> desc_obj = Local<Object>::Cast(args[0]);
    Local<Value> type_value = desc_obj->Get(NanNew("type"));
    Local<Value> sdp_value = desc_obj->Get(NanNew("sdp"));
    
    if (!type_value.IsEmpty() && type_value->IsString()) {
      if (!sdp_value.IsEmpty() && sdp_value->IsString()) {
        if (!args[1].IsEmpty() && args[1]->IsFunction()) {
          NanAssignPersistent(self->_localCallback, Local<Function>::Cast(args[1]));
        } else {
          NanDisposePersistent(self->_localCallback);
        }

        if (!args[2].IsEmpty() && args[2]->IsFunction()) {
          NanAssignPersistent(self->_localErrorCallback, Local<Function>::Cast(args[2]));
        } else {
          NanDisposePersistent(self->_localErrorCallback);
        }
    
        String::Utf8Value type(type_value->ToString());
        String::Utf8Value sdp(sdp_value->ToString());

        webrtc::SessionDescriptionInterface *desc(webrtc::CreateSessionDescription(*type, *sdp));
        
        if (desc) {
          if (socket) {
            NanAssignPersistent(self->_localsdp, desc_obj);
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
        NanError(error)
      };

      Local<Function> onerror = Local<Function>::Cast(args[2]);
      onerror->Call(args.This(), 1, argv);
    } else {
      NanThrowError(error);
    }
  }
  
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::SetRemoteDescription) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  const char *error = "Invalid SessionDescription";
  
  if (!args[0].IsEmpty() && args[0]->IsObject()) {
    Local<Object> desc_obj = Local<Object>::Cast(args[0]);
    Local<Value> type_value = desc_obj->Get(NanNew("type"));
    Local<Value> sdp_value = desc_obj->Get(NanNew("sdp"));
    
    if (!type_value.IsEmpty() && type_value->IsString()) {
      if (!sdp_value.IsEmpty() && sdp_value->IsString()) {
        if (!args[1].IsEmpty() && args[1]->IsFunction()) {
          NanAssignPersistent(self->_remoteCallback, Local<Function>::Cast(args[1]));
        } else {
          NanDisposePersistent(self->_remoteCallback);
        }

        if (!args[2].IsEmpty() && args[2]->IsFunction()) {
          NanAssignPersistent(self->_remoteErrorCallback, Local<Function>::Cast(args[2]));
        } else {
          NanDisposePersistent(self->_remoteErrorCallback);
        }
    
        String::Utf8Value type(type_value->ToString());
        String::Utf8Value sdp(sdp_value->ToString());

        webrtc::SessionDescriptionInterface *desc(webrtc::CreateSessionDescription(*type, *sdp));
        
        if (desc) {
          if (socket) {
            NanAssignPersistent(self->_remotesdp, desc_obj);
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
        NanError(error)
      };

      Local<Function> onerror = Local<Function>::Cast(args[2]);
      onerror->Call(args.This(), 1, argv);
    } else {
      NanThrowError(error);
    }
  }
  
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::AddIceCandidate) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  const char *error = 0;
  Handle<Value> argv[1];

  if (!args[0].IsEmpty() && args[0]->IsObject()) {
    Local<Object> desc = Local<Object>::Cast(args[0]);
    Local<Value> sdpMid_value = desc->Get(NanNew("sdpMid"));
    Local<Value> sdpMLineIndex_value = desc->Get(NanNew("sdpMLineIndex"));
    Local<Value> sdp_value = desc->Get(NanNew("candidate"));
    
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
      argv[1] = NanError(error);
      
      Local<Function> onerror = Local<Function>::Cast(args[2]);
      onerror->Call(args.This(), 1, argv);
    } else {
      NanThrowError(error);
    }
  }
  
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::CreateDataChannel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  std::string label;
  webrtc::DataChannelInit config;
  
  if (!args[0].IsEmpty() && args[0]->IsString()) {
    String::Utf8Value label_utf8(args[0]->ToString());
    label = *label_utf8;
  }
  
  if (!args[1].IsEmpty() && args[1]->IsObject()) {
    Local<Object> config_obj = Local<Object>::Cast(args[0]);
    
    Local<Value> reliable_value = config_obj->Get(NanNew("reliable"));
    Local<Value> ordered_value = config_obj->Get(NanNew("ordered"));
    Local<Value> maxRetransmitTime_value = config_obj->Get(NanNew("maxRetransmitTime"));
    Local<Value> maxRetransmits_value = config_obj->Get(NanNew("maxRetransmits"));
    Local<Value> protocol_value = config_obj->Get(NanNew("protocol"));
    Local<Value> id_value = config_obj->Get(NanNew("id"));

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
      NanReturnValue(DataChannel::New(dataChannel));
    }
  }
  
  NanThrowError("Internal Error");
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::AddStream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = MediaStream::Unwrap(args[0]);

  if (mediaStream.get()) {
    webrtc::PeerConnectionInterface *socket = self->GetSocket();

    if (socket) {
      if (!socket->AddStream(mediaStream)) {
        NanThrowError("AddStream Failed");
      }
    } else {
      NanThrowError("Internal Error");
    }
  } else {
    NanThrowError("Invalid MediaStream Object");
  }
  
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::RemoveStream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = MediaStream::Unwrap(args[0]);

  if (mediaStream.get()) {
    webrtc::PeerConnectionInterface *socket = self->GetSocket();

    if (socket) {
      socket->RemoveStream(mediaStream);
    } else {
      NanThrowError("Internal Error");
    }
  } else {
    NanThrowError("Invalid MediaStream Object");
  }
  
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::GetLocalStreams) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    rtc::scoped_refptr<webrtc::StreamCollectionInterface> local = socket->local_streams();

    if (local.get()) {
      Local<Array> list = NanNew<Array>();
      uint32_t index = 0;
      size_t count;

      for (count = 0; count < local->count(); count++) {
        list->Set(index, MediaStream::New(local->at(count)));
      }

      NanReturnValue(list);
    }
  }
    
  NanThrowError("Internal Error");
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::GetRemoteStreams) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    rtc::scoped_refptr<webrtc::StreamCollectionInterface> remote = socket->remote_streams();

    if (remote.get()) {
      Local<Array> list = NanNew<Array>();
      uint32_t index = 0;
      size_t count;

      for (count = 0; count < remote->count(); count++) {
        list->Set(index, MediaStream::New(remote->at(count)));
      }

      NanReturnValue(list);
    }
  }

  NanThrowError("Internal Error");
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::GetStreamById) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    if (args.Length() >= 1 && args[0]->IsString()) {
      v8::String::Utf8Value idValue(args[0]->ToString());
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
        NanReturnValue(MediaStream::New(stream));
      } else {
        NanReturnValue(NanNull());
      }
    } else {
      NanThrowError("Invalid Argument");
    }
  }

  NanThrowError("Internal Error");
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::GetStats) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (!args[0].IsEmpty() && args[0]->IsFunction()) {
    NanAssignPersistent(self->_onstats, Local<Function>::Cast(args[0]));

    if (socket) {
      if (!socket->GetStats(self->_stats.get(), 0, webrtc::PeerConnectionInterface::kStatsOutputLevelStandard)) {
        Local<Function> callback = NanNew(self->_onstats);
        Local<Value> argv[1] = { NanNull() };

        callback->Call(args.This(), 1, argv);
        NanDisposePersistent(self->_onstats);
      }
    } else {
      NanThrowError("Internal Error");
    }
  } else {
    NanThrowError("Missing Callback");
  }
  
  NanReturnUndefined();
}

NAN_METHOD(PeerConnection::Close) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.This(), "PeerConnection"); 
  webrtc::PeerConnectionInterface *socket = self->GetSocket();
  
  if (socket) {
    socket->Close();
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_GETTER(PeerConnection::GetSignalingState) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::SignalingState state(socket->signaling_state());
    
    switch (state) {
      case webrtc::PeerConnectionInterface::kStable:
        NanReturnValue(NanNew("stable"));
        break;
      case webrtc::PeerConnectionInterface::kHaveLocalOffer:
        NanReturnValue(NanNew("have-local-offer"));
        break;
      case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
        NanReturnValue(NanNew("have-local-pranswer"));
        break;
      case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
        NanReturnValue(NanNew("have-remote-offer"));
        break;
      case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
        NanReturnValue(NanNew("have-remote-pranswer"));
        break;
      default: 
        NanReturnValue(NanNew("closed"));
        break;
    }
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_GETTER(PeerConnection::GetIceConnectionState) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::IceConnectionState state(socket->ice_connection_state());

    switch (state) {
      case webrtc::PeerConnectionInterface::kIceConnectionNew:
        NanReturnValue(NanNew("new"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionChecking:
        NanReturnValue(NanNew("checking"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionConnected:
        NanReturnValue(NanNew("connected"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionCompleted:
        NanReturnValue(NanNew("completed"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionFailed:
        NanReturnValue(NanNew("failed"));
        break;
      case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:
        NanReturnValue(NanNew("disconnected"));
        break;
      default:
        NanReturnValue(NanNew("closed"));
        break;
    }
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_GETTER(PeerConnection::GetIceGatheringState) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  webrtc::PeerConnectionInterface *socket = self->GetSocket();

  if (socket) {
    webrtc::PeerConnectionInterface::IceGatheringState state(socket->ice_gathering_state());

    switch (state) {
      case webrtc::PeerConnectionInterface::kIceGatheringNew:
        NanReturnValue(NanNew("new"));
        break;
      case webrtc::PeerConnectionInterface::kIceGatheringGathering:
        NanReturnValue(NanNew("gathering"));
        break;
      default:
        NanReturnValue(NanNew("complete"));
        break;
    }   
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_GETTER(PeerConnection::GetOnSignalingStateChange) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew(self->_onsignalingstatechange));
}

NAN_GETTER(PeerConnection::GetOnIceConnectionStateChange) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew(self->_oniceconnectionstatechange));
}

NAN_GETTER(PeerConnection::GetOnIceCandidate) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew(self->_onicecandidate));
}

NAN_GETTER(PeerConnection::GetLocalDescription) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew<Object>(self->_localsdp));
}

NAN_GETTER(PeerConnection::GetRemoteDescription) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew<Object>(self->_remotesdp));
}

NAN_GETTER(PeerConnection::GetOnDataChannel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew(self->_ondatachannel));
}

NAN_GETTER(PeerConnection::GetOnNegotiationNeeded) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew(self->_onnegotiationneeded));
}

NAN_GETTER(PeerConnection::GetOnAddStream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew(self->_onaddstream));
}

NAN_GETTER(PeerConnection::GetOnRemoveStream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");
  NanReturnValue(NanNew(self->_onremovestream));
}

NAN_SETTER(PeerConnection::ReadOnly) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

NAN_SETTER(PeerConnection::SetOnSignalingStateChange) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onsignalingstatechange, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onsignalingstatechange);
  }
}

NAN_SETTER(PeerConnection::SetOnIceConnectionStateChange) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_oniceconnectionstatechange, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_oniceconnectionstatechange);
  }
}

NAN_SETTER(PeerConnection::SetOnIceCandidate) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onicecandidate, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onicecandidate);
  }
}

NAN_SETTER(PeerConnection::SetOnDataChannel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_ondatachannel, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_ondatachannel);
  }
}

NAN_SETTER(PeerConnection::SetOnNegotiationNeeded) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onnegotiationneeded, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onnegotiationneeded);
  }
}

NAN_SETTER(PeerConnection::SetOnAddStream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onaddstream, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onaddstream);
  }
}

NAN_SETTER(PeerConnection::SetOnRemoveStream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnection *self = RTCWrap::Unwrap<PeerConnection>(args.Holder(), "PeerConnection");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onremovestream, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onremovestream);
  }
}

void PeerConnection::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  PeerConnectionEvent type = event->Type<PeerConnectionEvent>();
  Local<Function> callback;
  Local<Object> container;
  Local<Value> argv[1];
  bool isError = false;
  std::string data;
  int argc = 0;
  
  switch(type) {
    case kPeerConnectionCreateOffer:
      callback = NanNew(_offerCallback);
      
      NanDisposePersistent(_offerCallback);
      NanDisposePersistent(_offerErrorCallback);

      data = event->Unwrap<std::string>();
      argv[0] = JSON::Parse(NanNew(data.c_str()));
      argc = 1;
      
      break;
    case kPeerConnectionCreateOfferError:
      callback = NanNew(_offerErrorCallback);
      
      NanDisposePersistent(_offerCallback);
      NanDisposePersistent(_offerErrorCallback);
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = NanError(data.c_str());
      argc = 1;
      
      break;
    case kPeerConnectionCreateAnswer:
      callback = NanNew(_answerCallback);
      
      NanDisposePersistent(_answerCallback);
      NanDisposePersistent(_answerErrorCallback);
      
      data = event->Unwrap<std::string>();
      argv[0] = JSON::Parse(NanNew(data.c_str()));
      argc = 1;
      
      break;
    case kPeerConnectionCreateAnswerError:
      callback = NanNew(_answerErrorCallback);
      
      NanDisposePersistent(_answerCallback);
      NanDisposePersistent(_answerErrorCallback);
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = NanError(data.c_str());
      argc = 1;
      
      break;
    case kPeerConnectionSetLocalDescription:
      callback = NanNew(_localCallback);
      
      NanDisposePersistent(_localCallback);
      NanDisposePersistent(_localErrorCallback);
      
      break;
    case kPeerConnectionSetLocalDescriptionError:
      callback = NanNew(_localErrorCallback);
      
      NanDisposePersistent(_localCallback);
      NanDisposePersistent(_localErrorCallback);
      NanDisposePersistent(_localsdp);
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = NanError(data.c_str());
      argc = 1;
      
      break;
    case kPeerConnectionSetRemoteDescription:
      callback = NanNew(_remoteCallback);
      
      NanDisposePersistent(_remoteCallback);
      NanDisposePersistent(_remoteErrorCallback);
      
      break;
    case kPeerConnectionSetRemoteDescriptionError:
      callback = NanNew(_remoteErrorCallback);
      
      NanDisposePersistent(_remoteCallback);
      NanDisposePersistent(_remoteErrorCallback);
      NanDisposePersistent(_remotesdp);
      
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = NanError(data.c_str());
      argc = 1;
      
      break;
    case kPeerConnectionIceCandidate:
      callback = NanNew(_onicecandidate);
      
      data = event->Unwrap<std::string>();
      
      container = NanNew<Object>();
      container->Set(NanNew("candidate"), JSON::Parse(NanNew(data.c_str())));

      argv[0] = container;
      argc = 1;
      
      break;
    case kPeerConnectionSignalChange:
      callback = NanNew(_onsignalingstatechange);
      
      break;
    case kPeerConnectionIceChange:
      callback = NanNew(_oniceconnectionstatechange);
      
      break;
    case kPeerConnectionIceGathering:
      
      break;
    case kPeerConnectionDataChannel:
      callback = NanNew(_ondatachannel);
      
      container = NanNew<Object>();
      container->Set(NanNew("channel"), DataChannel::New(event->Unwrap<rtc::scoped_refptr<webrtc::DataChannelInterface> >()));

      argv[0] = container;
      argc = 1;
      
      break;
    case kPeerConnectionAddStream:
      callback = NanNew(_onaddstream);

      argv[0] = MediaStream::New(event->Unwrap<rtc::scoped_refptr<webrtc::MediaStreamInterface> >());
      argc = 1;

      break;
    case kPeerConnectionRemoveStream:
      callback = NanNew(_onremovestream);
      
      argv[0] = MediaStream::New(event->Unwrap<rtc::scoped_refptr<webrtc::MediaStreamInterface> >());
      argc = 1;

      break;
    case kPeerConnectionRenegotiation:
      callback = NanNew(_onnegotiationneeded);
      
      break;
    case kPeerConnectionStats:
      callback = NanNew(_onstats);

      argv[0] = RTCStatsResponse::New(event->Unwrap<webrtc::StatsReports>());
      argc = 1;

      break;
  }
  
  if (!callback.IsEmpty() && callback->IsFunction()) {
    callback->Call(RTCWrap::This(), argc, argv);
  } else if (isError) {
    NanThrowError(argv[0]);
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

