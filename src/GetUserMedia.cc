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

#include "GetUserMedia.h"
#include "Observers.h"

using namespace v8;
using namespace WebRTC;

void GetUserMedia::Init(v8::Handle<v8::Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  exports->Set(String::NewFromUtf8(isolate, "getUserMedia"), FunctionTemplate::New(isolate, GetUserMedia::OpenMedia)->GetFunction());
}

void GetUserMedia::OpenMedia(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  Local<Object> constraints;  // TODO(): Required? -> Throw Error
  Local<Function> onsuccess;  // TODO(): Required? -> Throw Error 
  Local<Function> onerror;    // TODO(): Required? -> Throw Error

  bool echo = true;

  uint32_t min_width = 320;
  uint32_t max_width = 1280;

  uint32_t min_height = 180;
  uint32_t max_height = 720;

  double min_fps = 24.0;
  double max_fps = 60.0;

  double min_aspectRatio = 1.33333333333; //  4/3 
  double max_aspectRatio = 1.7777777778;  //  16/9

  std::string audioId;
  std::string videoId;
  
  if (args.Length() >= 1 && args[0]->IsObject()) {
    constraints = Local<Object>::Cast(args[0]);

    if (args.Length() >= 2 && args[1]->IsFunction()) {
      onsuccess = Local<Function>::Cast(args[1]);
    }

    if (args.Length() == 3 && args[2]->IsFunction()) {
      onerror = Local<Function>::Cast(args[2]);
    }
  }

  if (!constraints.IsEmpty()) {
    Local<Value> audio_value = constraints->Get(String::NewFromUtf8(isolate, "audio"));
    Local<Value> video_value = constraints->Get(String::NewFromUtf8(isolate, "video"));

    if (!audio_value.IsEmpty() && audio_value->IsObject()) {
      Local<Object> audio_obj = Local<Object>::Cast(audio_value);

    }

    if (!video_value.IsEmpty() && video_value->IsObject()) {
      Local<Object> video_obj = Local<Object>::Cast(video_value);

    }
  }

  // TODO(): Implement This
}
