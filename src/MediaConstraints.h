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


#ifndef WEBRTC_MEDIACONSTRAINTS_H
#define WEBRTC_MEDIACONSTRAINTS_H

#include "Common.h"

namespace WebRTC {
  class MediaConstraints : public webrtc::MediaConstraintsInterface, public rtc::RefCountInterface {
    friend class rtc::RefCountedObject<MediaConstraints>;

   public:
    static rtc::scoped_refptr<MediaConstraints> New();
    static rtc::scoped_refptr<MediaConstraints> New(const v8::Local<v8::Object> &constraints);
    static rtc::scoped_refptr<MediaConstraints> New(const v8::Local<v8::Value> &constraints);

    bool UseAudio() const;
    bool UseVideo() const;

    std::string AudioId() const;
    std::string VideoId() const;

    bool IsMandatory(const std::string& key);
    bool GetMandatory(const std::string& key);
    void RemoveMandatory(const std::string& key);
    void AddMandatory(const std::string &key, const std::string &value);
    void SetMandatory(const std::string &key, const std::string &value);

    template <class T> void SetMandatory(const std::string& key, const T& value) {
      SetMandatory(key, rtc::ToString<T>(value));
    }

    bool IsOptional(const std::string& key);
    bool GetOptional(const std::string& key);
    void RemoveOptional(const std::string& key);
    void AddOptional(const std::string &key, const std::string &value);
    void SetOptional(const std::string &key, const std::string &value);

    template <class T> void SetOptional(const std::string& key, const T& value) {
      SetOptional(key, rtc::ToString<T>(value));
    }

    const webrtc::MediaConstraintsInterface *ToConstraints() const;
    const webrtc::MediaConstraintsInterface::Constraints &GetMandatory() const final;
    const webrtc::MediaConstraintsInterface::Constraints &GetOptional() const final;

   private:
    explicit MediaConstraints();
    ~MediaConstraints() override;

    void SetOptional(std::string key, v8::Local<v8::Value> value);
    void SetMandatory(std::string key, v8::Local<v8::Value> value);

   protected:
    bool _audio;
    bool _video;

    std::string _audioId;
    std::string _videoId;

    webrtc::MediaConstraintsInterface::Constraints _mandatory;
    webrtc::MediaConstraintsInterface::Constraints _optional;
  };
};

#endif
