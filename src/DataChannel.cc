#include "DataChannel.h"

using namespace v8;
using namespace WebRTC;

Persistent<Function> DataChannel::constructor;

void DataChannel::Init() {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, DataChannel::New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "RTCDataChannel"));
  
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "close"),
                                FunctionTemplate::New(isolate, DataChannel::Close));
                                
  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "send"),
                                FunctionTemplate::New(isolate, DataChannel::Send));
  
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "id"),
                                       DataChannel::GetId,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "label"),
                                       DataChannel::GetLabel,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "ordered"),
                                       DataChannel::GetOrdered,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "protocol"),
                                       DataChannel::GetProtocol,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "readyState"),
                                       DataChannel::GetReadyState,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "bufferedAmount"),
                                       DataChannel::GetBufferedAmount,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "binaryType"),
                                       DataChannel::GetBinaryType,
                                       DataChannel::SetBinaryType);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "maxPacketLifeType"),
                                       DataChannel::GetMaxPacketLifeType,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "maxRetransmits"),
                                       DataChannel::GetMaxRetransmits,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "negotiated"),
                                       DataChannel::GetNegotiated,
                                       DataChannel::ReadOnly);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "reliable"),
                                       DataChannel::GetReliable,
                                       DataChannel::ReadOnly); 
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onopen"),
                                       DataChannel::GetOnOpen,
                                       DataChannel::SetOnOpen); 
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onmessage"),
                                       DataChannel::GetOnMessage,
                                       DataChannel::SetOnMessage);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onclose"),
                                       DataChannel::GetOnClose,
                                       DataChannel::SetOnClose);
                                       
  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onerror"),
                                       DataChannel::GetOnError,
                                       DataChannel::SetOnError); 
                                       
  constructor.Reset(isolate, tpl->GetFunction());
}

DataChannel::DataChannel() {
  _observer = new rtc::RefCountedObject<DataChannelObserver>(this);
}

DataChannel::~DataChannel() {
  if (_socket.get()) {  
    _socket->UnregisterObserver();
    
    webrtc::DataChannelInterface::DataState state(_socket->state());
    
    if (state != webrtc::DataChannelInterface::kClosing ||
        state != webrtc::DataChannelInterface::kClosed)
    {
      _socket->Close();
    }
  }

  EventEmitter::End();
}

void DataChannel::New(const v8::FunctionCallbackInfo<v8::Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  if (args.IsConstructCall()) {
    DataChannel* dataChannel = new DataChannel();
    dataChannel->Wrap(isolate, args.This());
    return args.GetReturnValue().Set(args.This());
  }

  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

Local<Value> DataChannel::New(Isolate *isolate, rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel) {
  EscapableHandleScope scope(isolate);
  
  Local<Value> argv[1];
  Local<Function> instance = Local<Function>::New(isolate, DataChannel::constructor);
  
  if (instance.IsEmpty() || !dataChannel.get()) {
    Local<Value> empty = Null(isolate);
    return scope.Escape(empty);
  }
  
  Local<Object> ret = instance->NewInstance(0, argv);
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, ret);

  self->Start();
  self->_socket = dataChannel;
  self->_socket->RegisterObserver(self->_observer.get());
  self->_binaryType.Reset(isolate, String::NewFromUtf8(isolate, "arraybuffer"));
  self->Emit(kDataChannelStateChange);

  return scope.Escape(ret);
}

webrtc::DataChannelInterface *DataChannel::GetSocket() const {
  return _socket.get();
}

void DataChannel::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate *isolate = args.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, args.This());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  
  if (socket) {
    webrtc::DataChannelInterface::DataState state(socket->state());
    
    if (state != webrtc::DataChannelInterface::kClosing ||
        state != webrtc::DataChannelInterface::kClosed)
    {
      socket->Close();
    }
  }
}

void DataChannel::Send(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate *isolate = args.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, args.This());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  bool retval = false;

  if (socket) {
    webrtc::DataChannelInterface::DataState state(socket->state());
    
    if (state != webrtc::DataChannelInterface::kOpen) {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Not Connected")));
    } else {  
      if(args[0]->IsString()) {
        Local<String> str = Local<String>::Cast(args[0]);
        std::string data(*String::Utf8Value(str));

        webrtc::DataBuffer buffer(data);
        retval = socket->Send(buffer);
      } else {
        if (args[0]->IsArrayBuffer() || args[0]->IsTypedArray()) {
          node::ArrayBuffer *container = node::ArrayBuffer::New(isolate, args[0]);
          rtc::Buffer data(container->Data(), container->Length());
          webrtc::DataBuffer buffer(data, true);
          retval = socket->Send(buffer);
        } else {
          isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid argument")));
        }
      }
    }
  }
  
  return args.GetReturnValue().Set(v8::Boolean::New(isolate, retval));
}

void DataChannel::GetId(v8::Local<v8::String> property, 
                        const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    retval = Integer::New(isolate, socket->id());
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetLabel(v8::Local<v8::String> property, 
                           const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    std::string data = socket->label();
    retval = String::NewFromUtf8(isolate, data.c_str());
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetOrdered(v8::Local<v8::String> property, 
                             const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    retval = v8::Boolean::New(isolate, socket->ordered());
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetProtocol(v8::Local<v8::String> property, 
                              const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    std::string data = socket->protocol();
    retval = String::NewFromUtf8(isolate, data.c_str());
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetReadyState(v8::Local<v8::String> property, 
                                const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    webrtc::DataChannelInterface::DataState state(socket->state());
  
    switch (state) {
      case webrtc::DataChannelInterface::kConnecting:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "connecting"));
        break;
      case webrtc::DataChannelInterface::kOpen:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "open"));
        break;
      case webrtc::DataChannelInterface::kClosing:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "closing"));
        break;
      case webrtc::DataChannelInterface::kClosed:
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "closed"));
        break;
    }
  }
}

void DataChannel::GetBufferedAmount(v8::Local<v8::String> property, 
                                    const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    uint64_t value = socket->buffered_amount();
    retval = Integer::NewFromUnsigned(isolate, static_cast<uint32_t>(value));
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetBinaryType(v8::Local<v8::String> property, 
                                const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<String>::New(isolate, self->_binaryType));
}

void DataChannel::GetMaxPacketLifeType(v8::Local<v8::String> property, 
                                       const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    uint16_t value = socket->maxRetransmitTime();
    retval = Integer::NewFromUnsigned(isolate, static_cast<uint32_t>(value));
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetMaxRetransmits(v8::Local<v8::String> property, 
                                    const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    uint16_t value = socket->maxRetransmits();
    retval = Integer::NewFromUnsigned(isolate, static_cast<uint32_t>(value));
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetNegotiated(v8::Local<v8::String> property, 
                                   const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    retval = v8::Boolean::New(isolate, socket->negotiated());
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetReliable(v8::Local<v8::String> property, 
                              const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  webrtc::DataChannelInterface *socket = self->GetSocket();
  Local<Value> retval;
  
  if (socket) {
    retval = v8::Boolean::New(isolate, socket->reliable());
  }
  
  info.GetReturnValue().Set(retval);
}

void DataChannel::GetOnOpen(v8::Local<v8::String> property, 
                            const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onopen));
}

void DataChannel::GetOnMessage(v8::Local<v8::String> property, 
                               const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onmessage));
}

void DataChannel::GetOnClose(v8::Local<v8::String> property, 
                             const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onclose));
}

void DataChannel::GetOnError(v8::Local<v8::String> property, 
                             const v8::PropertyCallbackInfo<v8::Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onerror));
}  

void DataChannel::ReadOnly(v8::Local<v8::String> property, 
                              v8::Local<v8::Value> value, 
                              const v8::PropertyCallbackInfo<void> &info)
{

}

void DataChannel::SetBinaryType(v8::Local<v8::String> property, 
                                v8::Local<v8::Value> value, 
                                const v8::PropertyCallbackInfo<void> &info)
{  
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsString()) {
    
    self->_binaryType.Reset(isolate, value->ToString());
  } else {
    self->_binaryType.Reset(isolate, String::NewFromUtf8(isolate, "arraybuffer"));
  }
}

void DataChannel::SetOnOpen(v8::Local<v8::String> property, 
                            v8::Local<v8::Value> value, 
                            const v8::PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onopen.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onopen.Reset();
  }
}

void DataChannel::SetOnMessage(v8::Local<v8::String> property, 
                               v8::Local<v8::Value> value, 
                               const v8::PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onmessage.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onmessage.Reset();
  }
}

void DataChannel::SetOnClose(v8::Local<v8::String> property, 
                             v8::Local<v8::Value> value, 
                             const v8::PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onclose.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onclose.Reset();
  }
}

void DataChannel::SetOnError(v8::Local<v8::String> property, 
                             v8::Local<v8::Value> value, 
                             const v8::PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  DataChannel *self = RTCWrap::Unwrap<DataChannel>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onerror.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onerror.Reset();
  }
}

void DataChannel::On(Event *event) {
  Isolate *isolate = Isolate::GetCurrent();
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
          callback = Local<Function>::New(isolate, _onopen);

          break;
        case webrtc::DataChannelInterface::kClosing:

          break;
        case webrtc::DataChannelInterface::kClosed:
          EventEmitter::Stop();
          socket->UnregisterObserver();
          
          callback = Local<Function>::New(isolate, _onclose);          
          _onclose.Reset();

          break;
      }
    }
  } else {
    callback = Local<Function>::New(isolate, _onmessage);
    rtc::Buffer buffer = event->Unwrap<rtc::Buffer>();
    
    if (type == kDataChannelData) {
      argv[0] = String::NewFromUtf8(isolate, buffer.data(), String::kNormalString, buffer.size());
      argc = 1;
    } else {
      // TODO(): Wrapping rtc::Buffer to ArrayBuffer is not working properly?
      std::string data(buffer.data(), buffer.size());

      arrayBuffer = node::ArrayBuffer::New(isolate, data);                                
      argv[0] = arrayBuffer->ToArrayBuffer();
      argc = 1;
    }
  }
  
  if (!callback.IsEmpty() && callback->IsFunction()) {
    callback->Call(RTCWrap::This(isolate), argc, argv);
  } else if (isError) {
    isolate->ThrowException(argv[0]);
  }
}
