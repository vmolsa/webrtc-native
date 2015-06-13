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

Persistent<Function> RTCStatsReport::constructor;

void RTCStatsReport::Init() {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, RTCStatsReport::New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "RTCStatsReport"));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "names"),
                                FunctionTemplate::New(isolate, RTCStatsReport::Names));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "stat"),
                                FunctionTemplate::New(isolate, RTCStatsReport::Stat));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "id"),
                                FunctionTemplate::New(isolate, RTCStatsReport::Id));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "type"),
                                FunctionTemplate::New(isolate, RTCStatsReport::Type));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "timestamp"),
                                FunctionTemplate::New(isolate, RTCStatsReport::Timestamp));

  constructor.Reset(isolate, tpl->GetFunction());
};

Local<Value> RTCStatsReport::New(Isolate *isolate, webrtc::StatsReport *report) {  
  EscapableHandleScope scope(isolate);
  Local<Function> instance = Local<Function>::New(isolate, RTCStatsReport::constructor);

  if (instance.IsEmpty()) {
    Local<Value> empty = Null(isolate);
    return scope.Escape(empty);
  }

  Local<Object> ret = instance->NewInstance();
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(isolate, ret, "RTCStatsReport");

  if (stats) {
    stats->_report = report;
  }

  return scope.Escape(ret);
}

void RTCStatsReport::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    RTCStatsReport* report = new RTCStatsReport();
    report->Wrap(isolate, args.This(), "RTCStatsReport");
    return args.GetReturnValue().Set(args.This());
  }

  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

void RTCStatsReport::Names(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(isolate, args.This(), "RTCStatsReport");
  webrtc::StatsReport::Values values = stats->_report->values();
  Local<Array> list = Array::New(isolate);
  unsigned int index = 0;
  
  for (webrtc::StatsReport::Values::iterator it = values.begin(); it != values.end(); it++) {
    webrtc::StatsReport::ValuePtr value = values[it->first];
    list->Set(index, String::NewFromUtf8(isolate, value->display_name()));
    index++;
  }
  
  args.GetReturnValue().Set(list);
}

void RTCStatsReport::Stat(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(isolate, args.This(), "RTCStatsReport");
  webrtc::StatsReport::Values values = stats->_report->values();

  if (args.Length() >= 1 && args[0]->IsString()) {
    String::Utf8Value entry_value(args[0]->ToString());
    std::string entry(*entry_value);
    
    for (webrtc::StatsReport::Values::iterator it = values.begin(); it != values.end(); it++) {
      webrtc::StatsReport::ValuePtr value = values[it->first];

      if (!entry.compare(value->display_name())) {
        switch (value->type()) {
          case webrtc::StatsReport::Value::kInt:
            args.GetReturnValue().Set(Integer::New(isolate, value->int_val()));
            
            break;
          case webrtc::StatsReport::Value::kInt64:
            args.GetReturnValue().Set(Integer::New(isolate, static_cast<int32_t>(value->int64_val())));
            
            break;
          case webrtc::StatsReport::Value::kFloat:
            args.GetReturnValue().Set(Number::New(isolate, value->float_val()));
          
            break;
          case webrtc::StatsReport::Value::kString:
            args.GetReturnValue().Set(String::NewFromUtf8(isolate, value->string_val().c_str()));
            
            break;
          case webrtc::StatsReport::Value::kStaticString:
            args.GetReturnValue().Set(String::NewFromUtf8(isolate, value->static_string_val()));
            
            break;
          case webrtc::StatsReport::Value::kBool:
            args.GetReturnValue().Set(v8::Boolean::New(isolate, value->bool_val()));
            
            break;
          case webrtc::StatsReport::Value::kId:
            args.GetReturnValue().Set(String::NewFromUtf8(isolate, value->ToString().c_str()));
          
            break;
        }
      }
    }
  }
}

void RTCStatsReport::Id(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(isolate, args.This(), "RTCStatsReport");
  std::string id(stats->_report->id()->ToString());
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, id.c_str())); 
}

void RTCStatsReport::Type(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(isolate, args.This(), "RTCStatsReport");
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, stats->_report->TypeToString()));
}

void RTCStatsReport::Timestamp(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(isolate, args.This(), "RTCStatsReport");
  args.GetReturnValue().Set(Number::New(isolate, stats->_report->timestamp()));
}

Persistent<Function> RTCStatsResponse::constructor;

void RTCStatsResponse::Init() {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, RTCStatsResponse::New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "RTCStatsResponse"));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "result"),
                                FunctionTemplate::New(isolate, RTCStatsResponse::Result));
                                
  constructor.Reset(isolate, tpl->GetFunction());
}

void RTCStatsResponse::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    RTCStatsResponse *response = new RTCStatsResponse();
    response->Wrap(isolate, args.This(), "RTCStatsResponse");
    return args.GetReturnValue().Set(args.This());
  }

  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

Local<Value> RTCStatsResponse::New(Isolate *isolate, const webrtc::StatsReports &reports) {  
  EscapableHandleScope scope(isolate);
  Local<Function> instance = Local<Function>::New(isolate, RTCStatsResponse::constructor);

  if (instance.IsEmpty()) {
    Local<Value> empty = Null(isolate);
    return scope.Escape(empty);
  }

  Local<Object> ret = instance->NewInstance();
  RTCStatsResponse *response = RTCWrap::Unwrap<RTCStatsResponse>(isolate, ret, "RTCStatsResponse");

  response->_reports = reports;

  return scope.Escape(ret);
}

void RTCStatsResponse::Result(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  RTCStatsResponse *response = RTCWrap::Unwrap<RTCStatsResponse>(isolate, args.This(), "RTCStatsResponse");
  Local<Array> list = Array::New(isolate, response->_reports.size());
 
  for(unsigned int index = 0; index < response->_reports.size(); index++) {
    list->Set(index, RTCStatsReport::New(isolate, const_cast<webrtc::StatsReport*>(response->_reports.at(index))));
  }

  args.GetReturnValue().Set(list);
}
