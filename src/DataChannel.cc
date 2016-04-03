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

Nan::Persistent<Function> DataChannel::constructor;

void DataChannel::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(DataChannel::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("RTCDataChannel").ToLocalChecked());
  
  Nan::SetPrototypeMethod(tpl, "close", DataChannel::Close);
  Nan::SetPrototypeMethod(tpl, "send", DataChannel::Send);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("id").ToLocalChecked(), DataChannel::GetId);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("label").ToLocalChecked(), DataChannel::GetLabel);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("ordered").ToLocalChecked(), DataChannel::GetOrdered);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("protocol").ToLocalChecked(), DataChannel::GetProtocol);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("readyState").ToLocalChecked(), DataChannel::GetReadyState);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("bufferedAmount").ToLocalChecked(), DataChannel::GetBufferedAmount);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("binaryType").ToLocalChecked(), DataChannel::GetBinaryType, DataChannel::SetBinaryType);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("maxPacketLifeType").ToLocalChecked(), DataChannel::GetMaxPacketLifeType);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("maxRetransmits").ToLocalChecked(), DataChannel::GetMaxRetransmits);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("negotiated").ToLocalChecked(), DataChannel::GetNegotiated);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("reliable").ToLocalChecked(), DataChannel::GetReliable);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onopen").ToLocalChecked(), DataChannel::GetOnOpen, DataChannel::SetOnOpen);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onmessage").ToLocalChecked(), DataChannel::GetOnMessage, DataChannel::SetOnMessage);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onclose").ToLocalChecked(), DataChannel::GetOnClose, DataChannel::SetOnClose);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onerror").ToLocalChecked(), DataChannel::GetOnError, DataChannel::SetOnError);
  
  constructor.Reset<Function>(tpl->GetFunction());
}

DataChannel::DataChannel() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _observer = new rtc::RefCountedObject<DataChannelObserver>(this);
}

DataChannel::~DataChannel() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_socket.get()) {  
    _socket->UnregisterObserver();
    _observer->RemoveListener(this);
    
    webrtc::DataChannelInterface::DataState state(_socket->state());
    
    if (state != webrtc::DataChannelInterface::kClosing ||
        state != webrtc::DataChannelInterface::kClosed)
    {
      _socket->Close();
    }
  }
}

void DataChannel::New(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  if (info.IsConstructCall()) {
    DataChannel* dataChannel = new DataChannel();
    dataChannel->Wrap(info.This(), "DataChannel");
    return info.GetReturnValue().Set(info.This());
  }

  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

Local<Value> DataChannel::New(rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::EscapableHandleScope scope;
  Local<Function> instance = Nan::New(DataChannel::constructor);
  
  if (instance.IsEmpty() || !dataChannel.get()) {
    return scope.Escape(Nan::Null());
  }
  
  Local<Object> ret = instance->NewInstance();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(ret, "DataChannel");

  self->SetReference(true);
  self->_socket = dataChannel;
  self->_socket->RegisterObserver(self->_observer.get());
  self->Emit(kDataChannelStateChange);

  self->_binaryType.Reset(Nan::New("arraybuffer").ToLocalChecked());

  return scope.Escape(ret);
}

webrtc::DataChannelInterface *DataChannel::GetSocket() const {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _socket.get();
}

void DataChannel::Close(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.This(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    webrtc::DataChannelInterface::DataState state(socket->state());
    
    if (state != webrtc::DataChannelInterface::kClosing ||
        state != webrtc::DataChannelInterface::kClosed)
    {
      socket->Close();
    }
  }
  
  info.GetReturnValue().SetUndefined();
}

void DataChannel::Send(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.This(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  bool retval = false;

  if (socket) {
    if(info[0]->IsString()) {
      std::string data(*Nan::Utf8String(info[0]));
      webrtc::DataBuffer buffer(data);
      retval = socket->Send(buffer);
    } else {
      node::ArrayBuffer *container = node::ArrayBuffer::New(info[0]);
      rtc::Buffer data(reinterpret_cast<uint8_t*>(container->Data()), container->Length());
      //rtc::CopyOnWriteBuffer data(reinterpret_cast<uint8_t*>(container->Data()), container->Length());
      webrtc::DataBuffer buffer(data, true);
      retval = socket->Send(buffer);
    }
  }
  
  return info.GetReturnValue().Set(Nan::New(retval));
}

void DataChannel::GetId(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(socket->id()));
  }

  info.GetReturnValue().SetUndefined();
}

void DataChannel::GetLabel(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(socket->label().c_str()).ToLocalChecked());
  }
  
  info.GetReturnValue().SetUndefined();
}

void DataChannel::GetOrdered(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(socket->ordered()));
  }
  
  info.GetReturnValue().SetUndefined();
}

void DataChannel::GetProtocol(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(socket->protocol().c_str()).ToLocalChecked());
  }
  
  info.GetReturnValue().SetUndefined();
}

void DataChannel::GetReadyState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    webrtc::DataChannelInterface::DataState state(socket->state());
  
    switch (state) {
      case webrtc::DataChannelInterface::kConnecting:
        return info.GetReturnValue().Set(Nan::New("connecting").ToLocalChecked());
        break;
      case webrtc::DataChannelInterface::kOpen:
        return info.GetReturnValue().Set(Nan::New("open").ToLocalChecked());
        break;
      case webrtc::DataChannelInterface::kClosing:
        return info.GetReturnValue().Set(Nan::New("closing").ToLocalChecked());
        break;
      case webrtc::DataChannelInterface::kClosed:
        return info.GetReturnValue().Set(Nan::New("closed").ToLocalChecked());
        break;
    }
  }

  info.GetReturnValue().SetUndefined();
}

void DataChannel::GetBufferedAmount(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(static_cast<uint32_t>(socket->buffered_amount())));
  }
  
  info.GetReturnValue().SetUndefined();
}

void DataChannel::GetBinaryType(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  return info.GetReturnValue().Set(Nan::New(self->_binaryType));
}

void DataChannel::GetMaxPacketLifeType(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(static_cast<uint32_t>(socket->maxRetransmitTime())));
  }
  
  info.GetReturnValue().SetUndefined();
}

void DataChannel::GetMaxRetransmits(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(static_cast<uint32_t>(socket->maxRetransmits())));
  }
  
  info.GetReturnValue().SetUndefined();
}

void DataChannel::GetNegotiated(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(socket->negotiated()));
  }
  
  return info.GetReturnValue().Set(Nan::False());
}

void DataChannel::GetReliable(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    return info.GetReturnValue().Set(Nan::New(socket->reliable()));
  }
  
  return info.GetReturnValue().Set(Nan::False());
}

void DataChannel::GetOnOpen(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onopen));
}

void DataChannel::GetOnMessage(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onmessage));
}

void DataChannel::GetOnClose(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onclose));
}

void DataChannel::GetOnError(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onerror));
}  

void DataChannel::ReadOnly(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

void DataChannel::SetBinaryType(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsString()) {
    self->_binaryType.Reset(value->ToString());
  } else {
    self->_binaryType.Reset(Nan::New("arraybuffer").ToLocalChecked());
  }
}

void DataChannel::SetOnOpen(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onopen.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onopen.Reset();
  }
}

void DataChannel::SetOnMessage(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onmessage.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onmessage.Reset();
  }
}

void DataChannel::SetOnClose(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onclose.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onclose.Reset();
  }
}

void DataChannel::SetOnError(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(info.Holder(), "DataChannel");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onerror.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onclose.Reset();
  }
}

void DataChannel::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
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
          callback = Nan::New<Function>(_onopen);

          break;
        case webrtc::DataChannelInterface::kClosing:

          break;
        case webrtc::DataChannelInterface::kClosed:
          EventEmitter::SetReference(false);
          
          callback = Nan::New<Function>(_onclose);
          _onclose.Reset();

          break;
      }
    }
  } else {
    callback = Nan::New<Function>(_onmessage);
    rtc::Buffer buffer = event->Unwrap<rtc::Buffer>();
    Local<Object> container = Nan::New<Object>();
    argv[0] = container;
    argc = 1;

    if (type == kDataChannelData) {
      container->Set(Nan::New("data").ToLocalChecked(), Nan::New(reinterpret_cast<char *>(buffer.data()), buffer.size()).ToLocalChecked());
    } else {
      arrayBuffer = node::ArrayBuffer::New(buffer.data(), buffer.size());                                
      container->Set(Nan::New("data").ToLocalChecked(), arrayBuffer->ToArrayBuffer());
    }
  }
  
  if (!callback.IsEmpty() && callback->IsFunction()) {
    callback->Call(RTCWrap::This(), argc, argv);
  } else if (isError) {
    Nan::ThrowError(argv[0]);
  }
}
