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

#include <tchar.h>
#include <windows.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <ddraw.h>

#include "WindowRenderer.h"
 
using namespace v8;
using namespace WebRTC;

WNDCLASSEX wcx;
HINSTANCE hinst;
int WindowRenderer::StreamId = 0;

LRESULT CALLBACK WebRtcWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_DESTROY:
      break;
    case WM_COMMAND:
      break;
  }
  
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WindowRenderer::WindowRenderer(v8::Local<v8::Object> properties) : 
  _id(WindowRenderer::StreamId++), 
  _width(600), 
  _height(480),
  _window(0),
  _fullScreen(false),
  _module(0),
  _type(webrtc::kRenderDefault),
  _renderer(0)
{
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  EventEmitter::SetReference(true);
  const char *error = 0;

  HWND window = CreateWindowEx(0, TEXT("WebRTC"), TEXT("WebRTC @ NodeJS"), WS_OVERLAPPED | WS_THICKFRAME,
                               0, 0, _width, _height, (HWND) NULL, (HMENU) NULL, hinst, NULL);
    
  if (window) {
    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
    
    _window = window;
    _type = webrtc::kRenderWindows;
    _module = webrtc::VideoRender::CreateVideoRender(1337, _window, _fullScreen, _type);    
  }

  if (_module) {
    _renderer = _module->AddIncomingRenderStream(_id, 0, 0.0f, 0.0f, 1.0f, 1.0f);
    
    if (_renderer) {
      if (_module->StartRender(_id)) {
        error = "Unable to start renderer";
      }
    } else {
      error = "Unable to create renderer";
    }
  } else {
    error = "Unable to create window";
  }

  if (error) {
    NanThrowError(error);
  }
}

WindowRenderer::~WindowRenderer() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  WindowRenderer::End();
}

void WindowRenderer::Init() {
  hinst = GetModuleHandle(0);
  
  wcx.hInstance = hinst;
  wcx.lpszClassName = TEXT("WebRTC");
  wcx.lpfnWndProc = (WNDPROC) WebRtcWinProc;
  wcx.style = CS_DBLCLKS;
  wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wcx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcx.lpszMenuName = NULL;
  wcx.cbSize = sizeof (WNDCLASSEX);
  wcx.cbClsExtra = 0;
  wcx.cbWndExtra = 0;
  wcx.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
  
  if (!RegisterClassEx(&wcx)) {
    NanThrowError("WindowRenderer: Failed to register window class!");
  }
}

void WindowRenderer::Init(v8::Local<v8::Object> constructor) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(WindowRenderer::New);
  constructor->Set(NanNew("window"), tpl->GetFunction());
}

NAN_METHOD(WindowRenderer::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  NanScope();

  WindowRenderer* renderer = new WindowRenderer(Local<Object>::Cast(args[0]));
  renderer->Wrap(args.This(), "MediaSource");
  NanReturnValue(args.This());
}

void WindowRenderer::End() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_module && _renderer) {
    _module->StopRender(_id);
    _module->DeleteIncomingRenderStream(_id);
    
    webrtc::VideoRender::DestroyVideoRender(_module);
    
    _renderer = 0;
    _module = 0;
  }
  
  MediaSource::End();
}

void WindowRenderer::On(Event *event) {
  MediaSourceEvent type = event->Type<MediaSourceEvent>();
  webrtc::VideoFrame frame;
  
  if (type == kMediaSourceFrame) {
    frame.set_video_frame_buffer(event->Unwrap<rtc::scoped_refptr<webrtc::VideoFrameBuffer> >());
    
    if (_renderer) {
      _renderer->RenderFrame(_id, frame);
    }
  } else if (type == kMediaSourceEnd) {
    WindowRenderer::End();
  }
}