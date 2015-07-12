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

Persistent<Function> _require;

#if (NODE_MODULE_VERSION <= NODE_0_10_MODULE_VERSION)
Persistent<Function> _parse;
#endif

void Global::Init(Handle<Object> exports) {
  NanScope();
  
  Local<Object> global = NanGetCurrentContext()->Global();
  NanAssignPersistent(_require, Local<Function>::Cast(global->Get(NanNew("require"))));
  
#if (NODE_MODULE_VERSION <= NODE_0_10_MODULE_VERSION)
  Local<Object> json = Local<Object>::Cast(global->Get(NanNew("JSON")));
  NanAssignPersistent(_parse, Local<Function>::Cast(json->Get(NanNew("parse"))));   
#endif
}

Local<Function> Global::Require(Local<String> library) {
  NanEscapableScope();
  Local<Value> argv[] = { library };
  Local<Value> retval = NanMakeCallback(NanGetCurrentContext()->Global(), NanNew(_require), 1, argv);
  return NanEscapeScope(Local<Function>::Cast(retval));
}

#if (NODE_MODULE_VERSION <= NODE_0_10_MODULE_VERSION)
Local<Value> JSON::Parse(Local<Value> str) {
  NanEscapableScope();
  Local<Value> argv[] = { str };
  return NanEscapeScope(NanMakeCallback(NanGetCurrentContext()->Global(), NanNew(_parse), 1, argv));
};
#endif