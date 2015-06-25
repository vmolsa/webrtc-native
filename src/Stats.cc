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
  NanScope();
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(RTCStatsReport::New);
  tpl->SetClassName(NanNew("RTCStatsReport"));

  tpl->PrototypeTemplate()->Set(NanNew("names"), NanNew<FunctionTemplate>(RTCStatsReport::Names)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("stat"), NanNew<FunctionTemplate>(RTCStatsReport::Stat)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("id"), NanNew<FunctionTemplate>(RTCStatsReport::Id)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("type"), NanNew<FunctionTemplate>(RTCStatsReport::Type)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("timestamp"), NanNew<FunctionTemplate>(RTCStatsReport::Timestamp)->GetFunction());
  
  NanAssignPersistent(constructor, tpl->GetFunction());
};

Local<Value> RTCStatsReport::New(webrtc::StatsReport *report) {  
  NanEscapableScope();
  Local<Function> instance = NanNew(RTCStatsReport::constructor);

  if (instance.IsEmpty()) {
    return NanEscapeScope(NanNull());
  }

  Local<Object> ret = instance->NewInstance();
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(ret, "RTCStatsReport");

  if (stats) {
    stats->_report = report;
  }

  return NanEscapeScope(ret);
}

NAN_METHOD(RTCStatsReport::New) {
  NanScope();

  if (args.IsConstructCall()) {
    RTCStatsReport* report = new RTCStatsReport();
    report->Wrap(args.This(), "RTCStatsReport");
    NanReturnValue(args.This());
  }

  NanThrowError("Internal Error");
}

NAN_METHOD(RTCStatsReport::Names) {
  NanScope();
  
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(args.This(), "RTCStatsReport");
  webrtc::StatsReport::Values values = stats->_report->values();
  Local<Array> list = NanNew<Array>();
  unsigned int index = 0;
  
  for (webrtc::StatsReport::Values::iterator it = values.begin(); it != values.end(); it++) {
    webrtc::StatsReport::ValuePtr value = values[it->first];
    list->Set(index, NanNew(value->display_name()));
    index++;
  }
  
  NanReturnValue(list);
}

NAN_METHOD(RTCStatsReport::Stat) {
  NanScope();
  
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(args.This(), "RTCStatsReport");
  webrtc::StatsReport::Values values = stats->_report->values();

  if (args.Length() >= 1 && args[0]->IsString()) {
    String::Utf8Value entry_value(args[0]->ToString());
    std::string entry(*entry_value);
    
    for (webrtc::StatsReport::Values::iterator it = values.begin(); it != values.end(); it++) {
      webrtc::StatsReport::ValuePtr value = values[it->first];

      if (!entry.compare(value->display_name())) {
        switch (value->type()) {
          case webrtc::StatsReport::Value::kInt:
            NanReturnValue(NanNew(value->int_val()));
            
            break;
          case webrtc::StatsReport::Value::kInt64:
            NanReturnValue(NanNew(static_cast<int32_t>(value->int64_val())));
            
            break;
          case webrtc::StatsReport::Value::kFloat:
            NanReturnValue(NanNew(value->float_val()));
          
            break;
          case webrtc::StatsReport::Value::kString:
            NanReturnValue(NanNew(value->string_val().c_str()));
            
            break;
          case webrtc::StatsReport::Value::kStaticString:
            NanReturnValue(NanNew(value->static_string_val()));
            
            break;
          case webrtc::StatsReport::Value::kBool:
            NanReturnValue(NanNew(value->bool_val()));
            
            break;
          case webrtc::StatsReport::Value::kId:
            NanReturnValue(NanNew(value->ToString().c_str()));
          
            break;
        }
      }
    }
  }
}

NAN_METHOD(RTCStatsReport::Id) {
  NanScope();

  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(args.This(), "RTCStatsReport");
  std::string id(stats->_report->id()->ToString());
  NanReturnValue(NanNew(id.c_str())); 
}

NAN_METHOD(RTCStatsReport::Type) {
  NanScope();
  
  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(args.This(), "RTCStatsReport");
  NanReturnValue(NanNew(stats->_report->TypeToString()));
}

NAN_METHOD(RTCStatsReport::Timestamp) {
  NanScope();

  RTCStatsReport *stats = RTCWrap::Unwrap<RTCStatsReport>(args.This(), "RTCStatsReport");
  NanReturnValue(NanNew(stats->_report->timestamp()));
}

Persistent<Function> RTCStatsResponse::constructor;

void RTCStatsResponse::Init() {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(RTCStatsResponse::New);
  tpl->SetClassName(NanNew("RTCStatsResponse"));

  tpl->PrototypeTemplate()->Set(NanNew("result"), NanNew<FunctionTemplate>(RTCStatsResponse::Result)->GetFunction());
                                
  NanAssignPersistent(constructor, tpl->GetFunction());
}

NAN_METHOD(RTCStatsResponse::New) {
  NanScope();

  if (args.IsConstructCall()) {
    RTCStatsResponse *response = new RTCStatsResponse();
    response->Wrap(args.This(), "RTCStatsResponse");
    NanReturnValue(args.This());
  }

  NanThrowError("Internal Error");
}

Local<Value> RTCStatsResponse::New(const webrtc::StatsReports &reports) {  
  NanEscapableScope();
  Local<Function> instance = NanNew(RTCStatsResponse::constructor);

  if (instance.IsEmpty()) {
    return NanEscapeScope(NanNull());
  }

  Local<Object> ret = instance->NewInstance();
  RTCStatsResponse *response = RTCWrap::Unwrap<RTCStatsResponse>(ret, "RTCStatsResponse");

  response->_reports = reports;

  return NanEscapeScope(ret);
}

NAN_METHOD(RTCStatsResponse::Result) {
  NanScope();

  RTCStatsResponse *response = RTCWrap::Unwrap<RTCStatsResponse>(args.This(), "RTCStatsResponse");
  Local<Array> list = NanNew<Array>(response->_reports.size());
 
  for(unsigned int index = 0; index < response->_reports.size(); index++) {
    list->Set(index, RTCStatsReport::New(const_cast<webrtc::StatsReport*>(response->_reports.at(index))));
  }

  NanReturnValue(list);
}
