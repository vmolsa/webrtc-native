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

#include "DataChannel.h"

using namespace v8;
using namespace WebRTC;

Persistent<Function> DataChannel::constructor;

void DataChannel::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(DataChannel::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew("RTCDataChannel"));
  
  tpl->PrototypeTemplate()->Set(NanNew("close"), NanNew<FunctionTemplate>(DataChannel::Close)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("send"), NanNew<FunctionTemplate>(DataChannel::Send)->GetFunction());
 
  tpl->InstanceTemplate()->SetAccessor(NanNew("id"),
                                       DataChannel::GetId,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("label"),
                                       DataChannel::GetLabel,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("ordered"),
                                       DataChannel::GetOrdered,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("protocol"),
                                       DataChannel::GetProtocol,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("readyState"),
                                       DataChannel::GetReadyState,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("bufferedAmount"),
                                       DataChannel::GetBufferedAmount,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("binaryType"),
                                       DataChannel::GetBinaryType,
                                       DataChannel::SetBinaryType);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("maxPacketLifeType"),
                                       DataChannel::GetMaxPacketLifeType,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("maxRetransmits"),
                                       DataChannel::GetMaxRetransmits,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("negotiated"),
                                       DataChannel::GetNegotiated,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("reliable"),
                                       DataChannel::GetReliable,
                                       DataChannel::ReadOnly); 
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onopen"),
                                       DataChannel::GetOnOpen,
                                       DataChannel::SetOnOpen); 
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onmessage"),
                                       DataChannel::GetOnMessage,
                                       DataChannel::SetOnMessage);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onclose"),
                                       DataChannel::GetOnClose,
                                       DataChannel::SetOnClose);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onerror"),
                                       DataChannel::GetOnError,
                                       DataChannel::SetOnError); 
  
  NanAssignPersistent(constructor, tpl->GetFunction());
}

DataChannel::DataChannel() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _observer = new rtc::RefCountedObject<DataChannelObserver>(this);
}

DataChannel::~DataChannel() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_socket.get()) {  
    _socket->UnregisterObserver();
    _observer->SetEmitter();
    
    webrtc::DataChannelInterface::DataState state(_socket->state());
    
    if (state != webrtc::DataChannelInterface::kClosing ||
        state != webrtc::DataChannelInterface::kClosed)
    {
      _socket->Close();
    }
  }
}

NAN_METHOD(DataChannel::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  NanScope();

  if (args.IsConstructCall()) {
    DataChannel* dataChannel = new DataChannel();
    dataChannel->Wrap(args.This(), "DataChannel");
    NanReturnValue(args.This());
  }

  NanThrowError("Internal Error");
  NanReturnUndefined();
}

Local<Value> DataChannel::New(rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanEscapableScope();
  Local<Function> instance = NanNew(DataChannel::constructor);
  
  if (instance.IsEmpty() || !dataChannel.get()) {
    return NanEscapeScope(NanNull());
  }
  
  Local<Object> ret = instance->NewInstance();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(ret, "DataChannel");

  self->SetReference(true);
  self->_socket = dataChannel;
  self->_socket->RegisterObserver(self->_observer.get());
  self->Emit(kDataChannelStateChange);

  NanAssignPersistent(self->_binaryType, NanNew("arraybuffer"));

  return NanEscapeScope(ret);
}

webrtc::DataChannelInterface *DataChannel::GetSocket() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _socket.get();
}

NAN_METHOD(DataChannel::Close) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.This(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    webrtc::DataChannelInterface::DataState state(socket->state());
    
    if (state != webrtc::DataChannelInterface::kClosing ||
        state != webrtc::DataChannelInterface::kClosed)
    {
      socket->Close();
    }
  }
  
  NanReturnUndefined();
}

NAN_METHOD(DataChannel::Send) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.This(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  bool retval = false;

  if (socket) {
    if(args[0]->IsString()) {
      std::string data(*NanUtf8String(args[0]));
      webrtc::DataBuffer buffer(data);
      retval = socket->Send(buffer);
    } else {
      node::ArrayBuffer *container = node::ArrayBuffer::New(args[0]);
      rtc::Buffer data(reinterpret_cast<char *>(container->Data()), container->Length());
      webrtc::DataBuffer buffer(data, true);
      retval = socket->Send(buffer);
    }
  }
  
  NanReturnValue(NanNew(retval));
}

NAN_GETTER(DataChannel::GetId) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(socket->id()));
  }

  NanReturnUndefined();
}

NAN_GETTER(DataChannel::GetLabel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(socket->label().c_str()));
  }
  
  NanReturnUndefined();
}

NAN_GETTER(DataChannel::GetOrdered) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(socket->ordered()));
  }
  
  NanReturnUndefined();
}

NAN_GETTER(DataChannel::GetProtocol) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(socket->protocol().c_str()));
  }
  
  NanReturnUndefined();
}

NAN_GETTER(DataChannel::GetReadyState) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    webrtc::DataChannelInterface::DataState state(socket->state());
  
    switch (state) {
      case webrtc::DataChannelInterface::kConnecting:
        NanReturnValue(NanNew("connecting"));
        break;
      case webrtc::DataChannelInterface::kOpen:
        NanReturnValue(NanNew("open"));
        break;
      case webrtc::DataChannelInterface::kClosing:
        NanReturnValue(NanNew("closing"));
        break;
      case webrtc::DataChannelInterface::kClosed:
        NanReturnValue(NanNew("closed"));
        break;
    }
  }

  NanReturnUndefined();
}

NAN_GETTER(DataChannel::GetBufferedAmount) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(static_cast<uint32_t>(socket->buffered_amount())));
  }
  
  NanReturnUndefined();
}

NAN_GETTER(DataChannel::GetBinaryType) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  NanReturnValue(NanNew(self->_binaryType));
}

NAN_GETTER(DataChannel::GetMaxPacketLifeType) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(static_cast<uint32_t>(socket->maxRetransmitTime())));
  }
  
  NanReturnUndefined();
}

NAN_GETTER(DataChannel::GetMaxRetransmits) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(static_cast<uint32_t>(socket->maxRetransmits())));
  }
  
  NanReturnUndefined();
}

NAN_GETTER(DataChannel::GetNegotiated) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(socket->negotiated()));
  }
  
  NanReturnValue(NanFalse());
}

NAN_GETTER(DataChannel::GetReliable) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    NanReturnValue(NanNew(socket->reliable()));
  }
  
  NanReturnValue(NanFalse());
}

NAN_GETTER(DataChannel::GetOnOpen) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  NanReturnValue(NanNew(self->_onopen));
}

NAN_GETTER(DataChannel::GetOnMessage) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  NanReturnValue(NanNew(self->_onmessage));
}

NAN_GETTER(DataChannel::GetOnClose) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  NanReturnValue(NanNew(self->_onclose));
}

NAN_GETTER(DataChannel::GetOnError) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");
  NanReturnValue(NanNew(self->_onerror));
}  

NAN_SETTER(DataChannel::ReadOnly) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

NAN_SETTER(DataChannel::SetBinaryType) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsString()) {
    NanAssignPersistent(self->_binaryType, value->ToString());
  } else {
    NanAssignPersistent(self->_binaryType, NanNew("arraybuffer"));
  }
}

NAN_SETTER(DataChannel::SetOnOpen) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onopen, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onopen);
  }
}

NAN_SETTER(DataChannel::SetOnMessage) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onmessage, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onmessage);
  }
}

NAN_SETTER(DataChannel::SetOnClose) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onclose, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onclose);
  }
}

NAN_SETTER(DataChannel::SetOnError) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(args.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onerror, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onclose);
  }
}

void DataChannel::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  DataChannelEvent type = event->Type<DataChannelEvent>();
  node::ArrayBuffer *arrayBuffer = 0;
  Local<Function> callback;
  Local<Value> argv[1];
  bool isError = false;
  int argc = 0;
  
  if (type == kDataChannelStateChange) {
    webrtc::DataChannelInterface *socket = DataChannel::GetSocket();
    
    if (socket) {
      switch (socket->state()) {
        case webrtc::DataChannelInterface::kConnecting:

          break;
        case webrtc::DataChannelInterface::kOpen:
          callback = NanNew(_onopen);

          break;
        case webrtc::DataChannelInterface::kClosing:

          break;
        case webrtc::DataChannelInterface::kClosed:
          EventEmitter::SetReference(false);
          
          callback = NanNew(_onclose);
          NanDisposePersistent(_onclose);

          break;
      }
    }
  } else {
    callback = NanNew(_onmessage);
    rtc::Buffer buffer = event->Unwrap<rtc::Buffer>();
    Local<Object> container = NanNew<Object>();
    argv[0] = container;
    argc = 1;

    if (type == kDataChannelData) {
      container->Set(NanNew("data"), NanNew(reinterpret_cast<char *>(buffer.data()), buffer.size()));
    } else {
      arrayBuffer = node::ArrayBuffer::New(reinterpret_cast<char *>(buffer.data()), buffer.size());                                
      container->Set(NanNew("data"), arrayBuffer->ToArrayBuffer());
    }
  }
  
  if (!callback.IsEmpty() && callback->IsFunction()) {
    callback->Call(RTCWrap::This(), argc, argv);
  } else if (isError) {
    NanThrowError(argv[0]);
  }
}
