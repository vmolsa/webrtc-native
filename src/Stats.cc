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

Persistent<Function> Stats::resconstructor;
Persistent<Function> Stats::repconstructor;

Stats::Stats() {

}

Stats::~Stats() {

}

void Stats::Init() {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Local<FunctionTemplate> result = FunctionTemplate::New(isolate, Stats::NewResult);
  result->SetClassName(String::NewFromUtf8(isolate, "Stats"));

  result->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "result"),
                                   FunctionTemplate::New(isolate, Stats::Result));

  Local<FunctionTemplate> report = FunctionTemplate::New(isolate, Stats::NewReport);
  report->SetClassName(String::NewFromUtf8(isolate, "Report"));

  report->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "names"),
                                   FunctionTemplate::New(isolate, Stats::Names));

  report->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "stat"),
                                   FunctionTemplate::New(isolate, Stats::Stat));

  report->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "id"),
                                   FunctionTemplate::New(isolate, Stats::Id));

  report->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "type"),
                                   FunctionTemplate::New(isolate, Stats::Type));

  report->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "timestamp"),
                                   FunctionTemplate::New(isolate, Stats::Timestamp));

  resconstructor.Reset(isolate, result->GetFunction());
  repconstructor.Reset(isolate, report->GetFunction());
}

void Stats::NewResult(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    Stats* stats = new Stats();
    stats->Wrap(isolate, args.This(), "Stats");
    return args.GetReturnValue().Set(args.This());
  }

  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

void Stats::NewReport(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    Stats::Reports* reports = new Stats::Reports();
    reports->Wrap(isolate, args.This(), "Reports");
    return args.GetReturnValue().Set(args.This());
  }

  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

Local<Value> Stats::New(Isolate *isolate, const webrtc::StatsReports &reports) {
  EscapableHandleScope scope(isolate);

  Local<Value> argv[1];
  Local<Function> instance = Local<Function>::New(isolate, Stats::resconstructor);

  if (instance.IsEmpty()) {
    Local<Value> empty = Null(isolate);
    return scope.Escape(empty);
  }

  Local<Object> ret = instance->NewInstance(0, argv);
  Stats *stat = RTCWrap::Unwrap<Stats>(isolate, ret, "Stats");

  stat->_reports = reports;

  return scope.Escape(ret);
}

void Stats::Result(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  Stats *stats = RTCWrap::Unwrap<Stats>(isolate, args.This(), "Stats");
  Local<Array> list = Array::New(isolate);

  if (stats) {

  }


  args.GetReturnValue().Set(list);
}

void Stats::Names(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

}

void Stats::Stat(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

}

void Stats::Id(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

}

void Stats::Type(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

}

void Stats::Timestamp(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

}

