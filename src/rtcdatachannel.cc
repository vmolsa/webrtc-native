/*
* The MIT License (MIT)
*
* Copyright (c) 2017 vmolsa <ville.molsa@gmail.com> (http://github.com/vmolsa)
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

#include <iostream>

#include "rtcdatachannel.h"

using namespace WebRTC;
using namespace v8;

Nan::Persistent<Function> RTCDataChannel::constructor;

RTCDataChannel::RTCDataChannel() {

}

RTCDataChannel::~RTCDataChannel() {
  RTCDataChannel::RemoveEvents(_channel, false);
}

void RTCDataChannel::Init(Local<Object> exports) {
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(RTCDataChannel::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("RTCDataChannel").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "close", RTCDataChannel::Close);
  Nan::SetPrototypeMethod(tpl, "send", RTCDataChannel::Send);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("id").ToLocalChecked(), 
                   RTCDataChannel::Id);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("label").ToLocalChecked(), 
                   RTCDataChannel::Label);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("bufferedAmount").ToLocalChecked(), 
                   RTCDataChannel::BufferedAmount);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("maxPacketLifeTime").ToLocalChecked(), 
                   RTCDataChannel::MaxPacketLifeTime);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("MaxRetransmits").ToLocalChecked(), 
                   RTCDataChannel::MaxRetransmits);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("negotiated").ToLocalChecked(), 
                   RTCDataChannel::Negotiated);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("ordered").ToLocalChecked(), 
                   RTCDataChannel::Ordered);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("protocol").ToLocalChecked(), 
                   RTCDataChannel::Protocol);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("readyState").ToLocalChecked(), 
                   RTCDataChannel::ReadyState);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("bufferedAmountLowThreshold").ToLocalChecked(),
                   RTCDataChannel::BufferedAmountLowThreshold,
                   RTCDataChannel::BufferedAmountLowThreshold);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onbufferedamountlow").ToLocalChecked(),
                   RTCDataChannel::onbufferedamountlow,
                   RTCDataChannel::onbufferedamountlow);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onclose").ToLocalChecked(),
                   RTCDataChannel::onclose,
                   RTCDataChannel::onclose);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onerror").ToLocalChecked(),
                   RTCDataChannel::onerror,
                   RTCDataChannel::onerror);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onmessage").ToLocalChecked(),
                   RTCDataChannel::onmessage,
                   RTCDataChannel::onmessage);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onopen").ToLocalChecked(),
                   RTCDataChannel::onopen,
                   RTCDataChannel::onopen);

  constructor.Reset<Function>(tpl->GetFunction());
}

void RTCDataChannel::Init(const crtc::Let<crtc::RTCDataChannel> &dataChannel) {
  dataChannel->onbufferedamountlow = [=]() {
    Nan::HandleScope scope;

    const int argc = 0;
    Local<Value> argv[0];
    Local<Function> callback = Nan::New<Function>(_onbufferedamountlow);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  dataChannel->onclose = [=]() {
    Nan::HandleScope scope;

    const int argc = 0;
    Local<Value> argv[0];
    Local<Function> callback = Nan::New<Function>(_onclose);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }

    RTCDataChannel::RemoveEvents(_channel);
    Nan::ObjectWrap::Unref();
  };

  dataChannel->onerror = [=](const crtc::Let<crtc::Error> &error) {
    Nan::HandleScope scope;

    Local<Object> obj = Nan::New<Object>();

    if (!error.IsEmpty()) {
      obj->Set(Nan::New("message").ToLocalChecked(), Nan::New<String>(error->Message()).ToLocalChecked());
      obj->Set(Nan::New("filename").ToLocalChecked(), Nan::New<String>(error->FileName()).ToLocalChecked());
      obj->Set(Nan::New("lineno").ToLocalChecked(), Nan::New(error->LineNumber()));
    }
    
    const int argc = 1;
    Local<Value> argv[] = {
      obj
    };

    Local<Function> callback = Nan::New<Function>(_onerror);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  dataChannel->onmessage = [=](const crtc::Let<crtc::ArrayBuffer> &buffer, bool binary) {
    Nan::HandleScope scope;

    Local<Object> obj = Nan::New<Object>();

    if (!buffer.IsEmpty()) {
      if (binary) {
        // TODO(): DO NOT COPY MEMORY!

        size_t byteLength = buffer->ByteLength();
        Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(Isolate::GetCurrent(), byteLength);
        ArrayBuffer::Contents content = arrayBuffer->GetContents();

        uint8_t *src = buffer->Data();
        uint8_t *dst = static_cast<uint8_t*>(content.Data());

        for (size_t index = 0; index < byteLength; index++) {
          dst[index] = src[index];
        }

        obj->Set(Nan::New("data").ToLocalChecked(), arrayBuffer);
      } else {
        obj->Set(Nan::New("data").ToLocalChecked(), Nan::New(buffer->ToString().c_str()).ToLocalChecked());
      } 
    }

    const int argc = 1;
    Local<Value> argv[] = {
      obj
    };

    Local<Function> callback = Nan::New<Function>(_onmessage);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  dataChannel->onopen = [=]() {
    Nan::HandleScope scope;

    const int argc = 0;
    Local<Value> argv[0];
    Local<Function> callback = Nan::New<Function>(_onopen);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };
  
  if (dataChannel->ReadyState() != crtc::RTCDataChannel::kClosed) {
    Nan::ObjectWrap::Ref();
  }

  _channel = dataChannel;
}

void RTCDataChannel::New(const Nan::FunctionCallbackInfo<Value> &info) {
  if (info.IsConstructCall()) {
    RTCDataChannel* dataChannel = new RTCDataChannel();
    dataChannel->Wrap(info.This());
    return info.GetReturnValue().Set(info.This());
  } 
}

Local<Value> RTCDataChannel::New(const crtc::Let<crtc::RTCDataChannel> &dataChannel) {
  Nan::EscapableHandleScope scope;
  Local<Function> cons = Nan::New(RTCDataChannel::constructor);
  
  if (cons.IsEmpty() || dataChannel.IsEmpty()) {
    return scope.Escape(Nan::Undefined());
  }
  
  Local<Value> argv[0];
  int argc = 0;
    
  Local<Object> obj = Nan::NewInstance(cons, argc, argv).ToLocalChecked();
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(obj);
  self->Init(dataChannel);

  return scope.Escape(obj);
}

crtc::Let<crtc::RTCDataChannel> RTCDataChannel::New(const Local<Object> &dataChannel) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(dataChannel);
  return self->_channel;
}

crtc::RTCPeerConnection::RTCDataChannelInit RTCDataChannel::RTCDataChannelInitFromValue(Local<Value> value) {
  return crtc::RTCPeerConnection::RTCDataChannelInit();
}

void RTCDataChannel::Close(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  self->_channel->Close();
}

void RTCDataChannel::Send(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());

  if (!info[0].IsEmpty()) {
    if (info[0]->IsString()) {
      v8::String::Utf8Value str(info[0]);
      self->_channel->Send(crtc::ArrayBuffer::New(reinterpret_cast<uint8_t*>(*str), str.length()), false);
    } else if (info[0]->IsArrayBuffer() || info[0]->IsTypedArray()) {
      Local<ArrayBuffer> arrayBuffer;

      if (info[0]->IsArrayBuffer()) {
        arrayBuffer = Local<ArrayBuffer>::Cast(info[0]);
      } else {
        Local<ArrayBufferView> view = Local<ArrayBufferView>::Cast(info[0]);
        arrayBuffer = view->Buffer();
      }

      if (!arrayBuffer.IsEmpty()) {
        ArrayBuffer::Contents content = arrayBuffer->GetContents();
        self->_channel->Send(crtc::ArrayBuffer::New(static_cast<uint8_t*>(content.Data()), content.ByteLength()), true);
      }
    }
  }
}

void RTCDataChannel::Id(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New(self->_channel->Id()));
}

void RTCDataChannel::Label(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<String>(self->_channel->Label()).ToLocalChecked());
}

void RTCDataChannel::BufferedAmount(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New(static_cast<uint32_t>(self->_channel->BufferedAmount())));
}

void RTCDataChannel::MaxPacketLifeTime(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New(self->_channel->MaxPacketLifeTime()));
}

void RTCDataChannel::MaxRetransmits(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New(self->_channel->MaxRetransmits()));
}

void RTCDataChannel::Negotiated(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New(self->_channel->Negotiated()));
}

void RTCDataChannel::Ordered(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New(self->_channel->Ordered()));
}

void RTCDataChannel::Protocol(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<String>(self->_channel->Protocol()).ToLocalChecked());
}

void RTCDataChannel::ReadyState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());

  switch (self->_channel->ReadyState()) {
    case crtc::RTCDataChannel::kConnecting:
      return info.GetReturnValue().Set(Nan::New("connecting").ToLocalChecked());
    case crtc::RTCDataChannel::kOpen:
      return info.GetReturnValue().Set(Nan::New("open").ToLocalChecked());
    case crtc::RTCDataChannel::kClosing:
      return info.GetReturnValue().Set(Nan::New("closing").ToLocalChecked());
    case crtc::RTCDataChannel::kClosed:
      return info.GetReturnValue().Set(Nan::New("closed").ToLocalChecked());
  }
}

void RTCDataChannel::BufferedAmountLowThreshold(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New(static_cast<uint32_t>(self->_channel->BufferedAmountLowThreshold())));
}

void RTCDataChannel::BufferedAmountLowThreshold(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());

  if (!value.IsEmpty() && value->IsUint32()) {
    self->_channel->SetBufferedAmountLowThreshold(static_cast<uint64_t>(Nan::To<uint32_t>(value).FromJust()));
  }
}

void RTCDataChannel::onbufferedamountlow(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onbufferedamountlow));
}

void RTCDataChannel::onbufferedamountlow(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onbufferedamountlow.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onbufferedamountlow.Reset();
  }
}

void RTCDataChannel::onclose(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onclose));
}

void RTCDataChannel::onclose(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onclose.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onclose.Reset();
  }
}

void RTCDataChannel::onerror(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onerror));
}

void RTCDataChannel::onerror(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onerror.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onerror.Reset();
  }
}

void RTCDataChannel::onmessage(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onmessage));
}

void RTCDataChannel::onmessage(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onmessage.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onmessage.Reset();
  }
}

void RTCDataChannel::onopen(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onopen));
}

void RTCDataChannel::onopen(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCDataChannel *self = Nan::ObjectWrap::Unwrap<RTCDataChannel>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onopen.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onopen.Reset();
  }
}

void RTCDataChannel::RemoveEvents(const crtc::Let<crtc::RTCDataChannel> &dataChannel, bool async) {
  crtc::Callback callback = [=]() {
    dataChannel->onbufferedamountlow.Dispose();
    dataChannel->onclose.Dispose();
    dataChannel->onerror.Dispose();
    dataChannel->onmessage.Dispose();
    dataChannel->onopen.Dispose();
  };

  if (async) {
    return crtc::SetImmediate(callback);
  }
  
  callback();
}
