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

#ifndef WEBRTC_STATS_H
#define WEBRTC_STATS_H

#include "Common.h"
#include "Observers.h" 
#include "EventEmitter.h"
#include "Wrap.h"

namespace WebRTC {
  class RTCStatsReport : public RTCWrap {
   public:
    static void Init();
    static v8::Local<v8::Value> New(webrtc::StatsReport *report);
    
   private:
    ~RTCStatsReport() final;
    
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Names(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Stat(const Nan::FunctionCallbackInfo<v8::Value> &info);
    
    static void Id(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void Type(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void Timestamp(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    
   protected:
    static Nan::Persistent<v8::Function> constructor;
    webrtc::StatsReport* _report;
  };
  
  class RTCStatsResponse : public RTCWrap {
   public:
    static void Init();
    static v8::Local<v8::Value> New(const webrtc::StatsReports &reports);
    
   private:
    ~RTCStatsResponse() final;
   
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Result(const Nan::FunctionCallbackInfo<v8::Value> &info);
    
   protected:
    static Nan::Persistent<v8::Function> constructor;
    webrtc::StatsReports _reports;
  };
};

#endif
