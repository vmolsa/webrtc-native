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

#include "nan.h"
#include "Core.h"

#ifdef WIN32
#include "webrtc/base/win32socketinit.h"
#endif

using namespace v8;
using namespace WebRTC;

rtc::Thread _signal, _worker;
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _factory;
rtc::scoped_ptr<cricket::DeviceManagerInterface> _manager;

class ProcessMessages : public rtc::Runnable {
 public:
  virtual void Run(rtc::Thread* thread) {
    LOG(LS_INFO) << __PRETTY_FUNCTION__;
    
    thread->ProcessMessages(rtc::ThreadManager::kForever);
  }
};

void Core::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  ProcessMessages task;
  
#ifdef WIN32
  rtc::EnsureWinsockInit();
#endif
  rtc::InitializeSSL();
  
  _signal.Start(&task);
  _worker.Start(&task);
  
  rtc::ThreadManager::Instance()->SetCurrentThread(&_signal);
  
  if (ThreadManager::Instance()->CurrentThread() != &_signal) {
    LOG(LS_ERROR) << "Internal Thread Error!";
    abort();
  }
  
  _factory = webrtc::CreatePeerConnectionFactory(&_signal, &_worker, NULL, NULL, NULL);
  _manager.reset(cricket::DeviceManagerFactory::Create());

  if (!_manager->Init()) {
    _manager.release();
  }
}

void Core::Dispose() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
  V8::LowMemoryNotification();
#else
  Isolate* isolate = Isolate::GetCurrent();
  isolate->LowMemoryNotification();
#endif
  
  _factory.release();

  if (_manager.get()) {
    _manager->Terminate();
  }

  _manager.release();
  
  _signal.Stop();
  _worker.Stop();
}

webrtc::PeerConnectionFactoryInterface* Core::GetFactory() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _factory.get();
}

cricket::DeviceManagerInterface* Core::GetManager() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return _manager.get();
}

rtc::Thread *Core::GetWorker() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return &_worker;
}

rtc::Thread *Core::GetSignal() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return &_signal;
}
