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

#include "Global.h"

using namespace v8;
using namespace WebRTC;

Nan::Persistent<Function> _require;

#if (NODE_MODULE_VERSION <= NODE_0_10_MODULE_VERSION)
Nan::Persistent<Function> _parse;
#endif

void WebRTC::Global::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  Local<Object> global = Nan::GetCurrentContext()->Global();
  _require.Reset(Local<Function>::Cast(global->Get(Nan::New("require").ToLocalChecked())));
  
#if (NODE_MODULE_VERSION <= NODE_0_10_MODULE_VERSION)
  Local<Object> json = Local<Object>::Cast(global->Get(Nan::New("JSON").ToLocalChecked()));
  _parse.Reset(Local<Function>::Cast(json->Get(Nan::New("parse").ToLocalChecked())));   
#endif
}

Local<Function> WebRTC::Global::Require(Local<String> library) {
  Nan::EscapableHandleScope scope;
  Local<Value> argv[] = { library };
  Local<Value> retval = Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(_require), 1, argv);
  return scope.Escape(Local<Function>::Cast(retval));
}

#if (NODE_MODULE_VERSION <= NODE_0_10_MODULE_VERSION)
Local<Value> JSON::Parse(Local<Value> str) {
  Nan::EscapableHandleScope scope;
  Local<Value> argv[] = { str };
  return scope.Escape(Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(_parse), 1, argv));
};
#endif