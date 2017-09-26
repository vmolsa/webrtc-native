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

#include "v8-promise.h"

using namespace v8;

Local<Promise> WebRTC::CreatePromise(Isolate *isolate, 
  const V8ExecutorCallback &callback, 
  Local<Object> This) 
{
  Nan::EscapableHandleScope scope;
  
  Local<Promise::Resolver> resolver = Promise::Resolver::New(isolate);
  Local<Promise> promise = resolver->GetPromise();
  Nan::Persistent<Promise::Resolver> *persistent = new Nan::Persistent<Promise::Resolver>(resolver);
  Nan::Persistent<Object> *reflock = nullptr;
  
  if (!This.IsEmpty()) {
    reflock = new Nan::Persistent<Object>(This);
  }

  callback([persistent, reflock](const Local<Value> &result) {
    Nan::HandleScope scope;
    Local<Promise::Resolver> resolver = Nan::New(*persistent);

    (void)resolver->Resolve(Nan::GetCurrentContext(), result);
    Isolate::GetCurrent()->RunMicrotasks();
    
    if (reflock) {
      reflock->Reset();
      delete reflock;
    }

    persistent->Reset();
    delete persistent;
  }, [persistent, reflock](const crtc::Let<crtc::Error> &error) {
    Nan::HandleScope scope;
    Local<Promise::Resolver> resolver = Nan::New(*persistent);

    if (!error.IsEmpty()) {
      (void)resolver->Reject(Nan::GetCurrentContext(), Nan::Error(error->Message().c_str()));
    } else {
      (void)resolver->Reject(Nan::GetCurrentContext(), Nan::Undefined());
    }

    Isolate::GetCurrent()->RunMicrotasks();

    if (reflock) {
      reflock->Reset();
      delete reflock;
    }

    persistent->Reset();
    delete persistent;
  });

  return scope.Escape(promise);
}
