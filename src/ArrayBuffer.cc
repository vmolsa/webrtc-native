#include "ArrayBuffer.h"

node::ArrayBuffer::ArrayBuffer(v8::Isolate *isolate) : 
  _rel(false),
  _wrap(false),
  _len(0),
  _data(0),
  _isolate(isolate)
{
  if (!_isolate) {
    _isolate = v8::Isolate::GetCurrent();
  }
}

node::ArrayBuffer::~ArrayBuffer() {
  if (_rel && _data) {
    delete [] reinterpret_cast<char*>(_data);
  }
}