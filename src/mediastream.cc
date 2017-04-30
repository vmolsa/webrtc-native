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

#include "mediastream.h"

using namespace WebRTC;
using namespace v8;

Nan::Persistent<Function> MediaStream::constructor;

MediaStream::MediaStream() {

}

MediaStream::~MediaStream() {

}

void MediaStream::Init(Local<Object> exports) {

}

v8::Local<v8::Object> MediaStream::New(const crtc::Let<crtc::MediaStream> &mediaStream) {
   Nan::EscapableHandleScope scope;
   Local<Object> obj = Nan::New<Object>();
   return scope.Escape(obj);
}

crtc::Let<crtc::MediaStream> MediaStream::New(const v8::Local<v8::Object> &mediaStream) {
  return crtc::Let<crtc::MediaStream>();
}
