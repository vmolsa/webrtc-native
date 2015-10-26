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

#include "Stats.h"

using namespace v8;
using namespace WebRTC;

Nan::Persistent<Function> RTCStatsReport::constructor;

RTCStatsReport::~RTCStatsReport() {
  
}

void RTCStatsReport::Init() {
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(RTCStatsReport::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("RTCStatsReport").ToLocalChecked());

  tpl->PrototypeTemplate()->Set(Nan::New("names").ToLocalChecked(), Nan::New<FunctionTemplate>(RTCStatsReport::Names)->GetFunction());
  tpl->PrototypeTemplate()->Set(Nan::New("stat").ToLocalChecked(), Nan::New<FunctionTemplate>(RTCStatsReport::Stat)->GetFunction());
  
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("id").ToLocalChecked(), RTCStatsReport::Id);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("type").ToLocalChecked(), RTCStatsReport::Type);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("timestamp").ToLocalChecked(), RTCStatsReport::Timestamp);
    
  constructor.Reset(tpl->GetFunction());
};

Local<Value> RTCStatsReport::New(webrtc::StatsReport *report) {  
  Nan::EscapableHandleScope scope;
  Local<Function> instance = Nan::New(RTCStatsReport::constructor);

  if (instance.IsEmpty()) {
    return scope.Escape(Nan::Null());
  }

  Local<Object> ret = instance->NewInstance();
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(ret, "RTCStatsReport");

  if (stats) {
    stats->_report = report;
  }

  return scope.Escape(ret);
}

void RTCStatsReport::New(const Nan::FunctionCallbackInfo<Value> &info) {
  if (info.IsConstructCall()) {
    RTCStatsReport* report = new RTCStatsReport();
    report->Wrap(info.This(), "RTCStatsReport");
    return info.GetReturnValue().Set(info.This());
  }

  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

void RTCStatsReport::Names(const Nan::FunctionCallbackInfo<Value> &info) { 
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(info.This(), "RTCStatsReport");
  webrtc::StatsReport::Values values = stats->_report->values();
  Local<Array> list = Nan::New<Array>();
  unsigned int index = 0;
  
  for (webrtc::StatsReport::Values::iterator it = values.begin(); it != values.end(); it++) {
    webrtc::StatsReport::ValuePtr value = values[it->first];
    list->Set(index, Nan::New(value->display_name()).ToLocalChecked());
    index++;
  }
  
  return info.GetReturnValue().Set(list);
}

void RTCStatsReport::Stat(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(info.This(), "RTCStatsReport");
  webrtc::StatsReport::Values values = stats->_report->values();

  if (info.Length() >= 1 && info[0]->IsString()) {
    String::Utf8Value entry_value(info[0]->ToString());
    std::string entry(*entry_value);
    
    for (webrtc::StatsReport::Values::iterator it = values.begin(); it != values.end(); it++) {
      webrtc::StatsReport::ValuePtr value = values[it->first];

      if (!entry.compare(value->display_name())) {
        switch (value->type()) {
          case webrtc::StatsReport::Value::kInt:
            return info.GetReturnValue().Set(Nan::New(value->int_val()));
            
            break;
          case webrtc::StatsReport::Value::kInt64:
            return info.GetReturnValue().Set(Nan::New(static_cast<int32_t>(value->int64_val())));
            
            break;
          case webrtc::StatsReport::Value::kFloat:
            return info.GetReturnValue().Set(Nan::New(value->float_val()));
          
            break;
          case webrtc::StatsReport::Value::kString:
            return info.GetReturnValue().Set(Nan::New(value->string_val().c_str()).ToLocalChecked());
            
            break;
          case webrtc::StatsReport::Value::kStaticString:
            return info.GetReturnValue().Set(Nan::New(value->static_string_val()).ToLocalChecked());
            
            break;
          case webrtc::StatsReport::Value::kBool:
            return info.GetReturnValue().Set(Nan::New(value->bool_val()));
            
            break;
          case webrtc::StatsReport::Value::kId:
            return info.GetReturnValue().Set(Nan::New(value->ToString().c_str()).ToLocalChecked());
          
            break;
        }
      }
    }
  }
  
  info.GetReturnValue().SetUndefined();
}

void RTCStatsReport::Id(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(info.This(), "RTCStatsReport");
  std::string id(stats->_report->id()->ToString());
  return info.GetReturnValue().Set(Nan::New(id.c_str()).ToLocalChecked()); 
}

void RTCStatsReport::Type(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(info.This(), "RTCStatsReport");
  return info.GetReturnValue().Set(Nan::New(stats->_report->TypeToString()).ToLocalChecked());
}

void RTCStatsReport::Timestamp(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(info.This(), "RTCStatsReport");
  return info.GetReturnValue().Set(Nan::New(stats->_report->timestamp()));
}

Nan::Persistent<Function> RTCStatsResponse::constructor;

RTCStatsResponse::~RTCStatsResponse() {
  
}

void RTCStatsResponse::Init() {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(RTCStatsResponse::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("RTCStatsResponse").ToLocalChecked());

  tpl->PrototypeTemplate()->Set(Nan::New("result").ToLocalChecked(), Nan::New<FunctionTemplate>(RTCStatsResponse::Result)->GetFunction());
                                
  constructor.Reset(tpl->GetFunction());
}

void RTCStatsResponse::New(const Nan::FunctionCallbackInfo<Value> &info) {
  if (info.IsConstructCall()) {
    RTCStatsResponse *response = new RTCStatsResponse();
    response->Wrap(info.This(), "RTCStatsResponse");
    return info.GetReturnValue().Set(info.This());
  }

  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

Local<Value> RTCStatsResponse::New(const webrtc::StatsReports &reports) {  
  Nan::EscapableHandleScope scope;
  Local<Function> instance = Nan::New(RTCStatsResponse::constructor);

  if (instance.IsEmpty()) {
    return scope.Escape(Nan::Null());
  }

  Local<Object> ret = instance->NewInstance();
  RTCStatsResponse *response = RTCWrap::Unwrap<RTCStatsResponse>(ret, "RTCStatsResponse");

  response->_reports = reports;

  return scope.Escape(ret);
}

void RTCStatsResponse::Result(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCStatsResponse *response = RTCWrap::Unwrap<RTCStatsResponse>(info.This(), "RTCStatsResponse");
  Local<Array> list = Nan::New<Array>(response->_reports.size());
 
  for(unsigned int index = 0; index < response->_reports.size(); index++) {
    list->Set(index, RTCStatsReport::New(const_cast<webrtc::StatsReport*>(response->_reports.at(index))));
  }

  return info.GetReturnValue().Set(list);
}
