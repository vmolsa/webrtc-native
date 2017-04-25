#include <nan.h>
#include <crtc.h>

using namespace v8;

uv_idle_t idle;

void WebRTCModuleDispose(void *arg) {
  crtc::Module::Dispose();
  uv_idle_stop(&idle);
}

void WebRTCDispatchEvents(uv_idle_t *handle) {
  if (crtc::Module::DispatchEvents(false)) {
    uv_ref((uv_handle_t*) &idle);
  } else {
    uv_unref((uv_handle_t*) &idle);
  }
}

void RunTest(const Nan::FunctionCallbackInfo<Value> &info) {
  if (!info[0].IsEmpty() && info[0]->IsFunction()) {
    Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

    crtc::SetTimeout([=]() {
      Nan::HandleScope scope;
      
      Local<Value> argv[] = {
        Nan::Null()
      };

      callback->Call(1, argv);
      delete callback;
    }, 5000);
  }
}

void WebRTCModuleInit(v8::Handle<v8::Object> exports) {
  uv_idle_init(uv_default_loop(), &idle);
  uv_idle_start(&idle, WebRTCDispatchEvents);

  crtc::Module::Init();

  Nan::HandleScope scope;

  exports->Set(Nan::New("runTest").ToLocalChecked(), Nan::New<FunctionTemplate>(RunTest)->GetFunction());

  node::AtExit(WebRTCModuleDispose);
}

NODE_MODULE(webrtc, WebRTCModuleInit)
