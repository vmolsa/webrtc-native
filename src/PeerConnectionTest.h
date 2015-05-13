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

#ifndef WEBRTC_PEERCONNECTION_TEST_H
#define WEBRTC_PEERCONNECTION_TEST_H

#include "Observers.h" 
#include "EventEmitter.h"

namespace WebRTC {
  class PeerConnection : public EventEmitter, public rtc::RefCountInterface {
    friend class rtc::RefCountedObject<PeerConnection>;

   public:
    typedef void (*Callback) (const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error);
    typedef void (*SdpCallback) (const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error, const std::string &sdp);
    typedef void (*IceCallback) (const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error, const std::string &candidate);
    typedef void (*DataChannelCallback) (const rtc::scoped_refptr<PeerConnection> &peer, const rtc::scoped_refptr<webrtc::DataChannelInterface> &dataChannel);

    static rtc::scoped_refptr<PeerConnection> New(const char *id = "Peer");

    IceCallback OnIceCandidate;
    Callback OnNegotiationNeeded;
    DataChannelCallback OnDataChannel;

    void CreateOffer(SdpCallback callback = 0);
    void CreateAnswer(SdpCallback callback = 0);
    void SetLocalDescription(const std::string &sdp, Callback callback = 0);
    void SetRemoteDescription(const std::string &sdp, Callback callback = 0);
    void AddIceCandidate(const std::string &candidate);

    rtc::scoped_refptr<webrtc::DataChannelInterface> CreateDataChannel();

    void AddStream();
    void RemoveStream();

    void Close();

   private:
     PeerConnection(const char *id = "Peer");
     ~PeerConnection() override;

     void On(Event *event) final;

     webrtc::PeerConnectionInterface *GetSocket();

   protected:
    const char* _id;
    SdpCallback _onoffer;
    SdpCallback _onanswer;
    Callback _onlocal;
    Callback _onremote;

    rtc::scoped_refptr<OfferObserver> _offer;
    rtc::scoped_refptr<AnswerObserver> _answer;
    rtc::scoped_refptr<LocalDescriptionObserver> _local;
    rtc::scoped_refptr<RemoteDescriptionObserver> _remote;
    rtc::scoped_refptr<PeerConnectionObserver> _peer;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> _socket;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _factory;
    rtc::scoped_refptr<webrtc::FakePortAllocatorFactory> _port;
    webrtc::FakeConstraints _constraints;

    webrtc::PeerConnectionInterface::IceServers _servers;
  };
};

#endif