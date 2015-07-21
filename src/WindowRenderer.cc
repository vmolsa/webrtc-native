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

#include "WindowRenderer.h"
#include "talk/media/devices/videorendererfactory.h"
 
using namespace v8;
using namespace WebRTC;

#ifdef WIN32

#include <tchar.h>
#include <windows.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <ddraw.h>

LRESULT CALLBACK WebRtcWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_DESTROY:
      break;
    case WM_COMMAND:
      break;
  }
  
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WebRtcCreateWindow(HWND &hwndMain, int winNum, int width, int height) {
  HINSTANCE hinst = GetModuleHandle(0);
  WNDCLASSEX wcx;
  
  wcx.hInstance = hinst;
  wcx.lpszClassName = TEXT("WebRTCWindow");
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
    MessageBox(0, TEXT("Failed to register window class!"), TEXT("Error!"), MB_OK | MB_ICONERROR);
    return 0;
  }
  
  hwndMain = CreateWindowEx(0, TEXT("WebRTCWindow"), TEXT("WebRTC @ NodeJS"), WS_OVERLAPPED | WS_THICKFRAME,
                            0, 0, width, height, (HWND) NULL, (HMENU) NULL, hinst, NULL);
    
  if (!hwndMain) {
    return -1;
  }

  ShowWindow(hwndMain, SW_SHOWDEFAULT);
  UpdateWindow(hwndMain);
  
  return 0;
}

#endif

WindowRenderer::WindowRenderer(v8::Local<v8::Object> properties) : 
  _id(0), 
  _width(600), 
  _height(480), 
  _fullScreen(false), 
  _renderer(0)
{
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  EventEmitter::SetReference(true);

  _renderer = cricket::VideoRendererFactory::CreateGuiVideoRenderer(_width, _height);
  
  if (!_renderer) {
    NanThrowError("Internal Error");
  }
  
  DrawBlackFrame();
}

WindowRenderer::~WindowRenderer() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

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

bool WindowRenderer::End(Local<Value> data) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  EventEmitter::SetReference(false);
  
  return true;
}

bool WindowRenderer::Write(Local<Value> data) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return false;
}

void WindowRenderer::DrawBlackFrame() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  cricket::WebRtcVideoFrame frame;
  frame.InitToBlack(_width, _height, 1, 1, 0, 0);
  
  _renderer->RenderFrame(&frame);
}

void WindowRenderer::DrawFrame(const MediaSourceImage &image) {
  
}