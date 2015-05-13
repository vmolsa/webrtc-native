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

#include "webrtc/base/gunit.h"
#include "PeerConnectionTest.h"
#include <uv.h>

using namespace WebRTC;

class DataChannelEvents : public EventEmitter, public rtc::RefCountInterface {
 private:
   void On(Event *event) {
     printf("DataChannelEvents::On()\n");
   }
};

int exit_status = 0;
uv_timer_t timeout;
rtc::scoped_refptr<PeerConnection> alice, bob;
rtc::scoped_refptr<webrtc::DataChannelInterface> aliceDataChannel;
rtc::scoped_refptr<DataChannelObserver> aliceDataChannelObserver;
rtc::scoped_refptr<DataChannelEvents> aliceDataChannelEvents;

void OnAliceOffer(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error, const std::string &sdp) {
  printf("OnAliceOffer()\n");
}

void OnAliceAnswer(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error, const std::string &sdp) {
  printf("OnAliceAnswer()\n");
}

void OnAliceLocal(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error) {
  printf("OnAliceLocal()\n");
}

void OnAliceRemote(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error) {
  printf("OnAliceRemote()\n");
}

void OnAliceIceCandidate(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error, const std::string &candidate) {
  printf("OnAliceIceCandidate()\n");
}

void OnAliceNegotiationNeeded(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error) {
  printf("OnAliceNegotiationNeeded()\n");
  peer->CreateOffer(OnAliceOffer);
}

void OnAliceDataChannel(const rtc::scoped_refptr<PeerConnection> &peer, const rtc::scoped_refptr<webrtc::DataChannelInterface> &dataChannel) {
  printf("OnAliceDataChannel()\n");
}

void OnBobOffer(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error, const std::string &sdp) {
  printf("OnBobOffer()\n");
}

void OnBobAnswer(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error, const std::string &sdp) {
  printf("OnBobAnswer()\n");
}

void OnBobLocal(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error) {
  printf("OnBobLocal()\n");
}

void OnBobRemote(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error) {
  printf("OnBobRemote()\n");
}

void OnBobIceCandidate(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error, const std::string &candidate) {
  printf("OnBobIceCandidate()\n");
}

void OnBobNegotiationNeeded(const rtc::scoped_refptr<PeerConnection> &peer, const std::string &error) {
  printf("OnBobNegotiationNeeded()\n");
}

void OnBobDataChannel(const rtc::scoped_refptr<PeerConnection> &peer, const rtc::scoped_refptr<webrtc::DataChannelInterface> &dataChannel) {
  printf("OnBobDataChannel()\n");
}

void onTimeout(uv_timer_t *handle) {
  alice->Close();
  bob->Close();
  
  exit_status = 1;
}

class WebRTCNative : public testing::Test {
  virtual void SetUp() {
    uv_timer_init(uv_default_loop(), &timeout);
    
    alice = PeerConnection::New("Alice");

    alice->OnNegotiationNeeded = OnAliceNegotiationNeeded;
    alice->OnIceCandidate = OnAliceIceCandidate;
    alice->OnDataChannel = OnAliceDataChannel;

    aliceDataChannelEvents = new rtc::RefCountedObject<DataChannelEvents>();
    aliceDataChannelObserver = new rtc::RefCountedObject<DataChannelObserver>(aliceDataChannelEvents.get());
    aliceDataChannelEvents->Start(true);

    bob = PeerConnection::New("Bob");

    bob->OnNegotiationNeeded = OnBobNegotiationNeeded;
    bob->OnIceCandidate = OnBobIceCandidate;
    bob->OnDataChannel = OnBobDataChannel;
  }
};

TEST_F(WebRTCNative, RunTest) {
  uv_timer_start(&timeout, onTimeout, 10000, 0);
  
  aliceDataChannel = alice->CreateDataChannel();
  aliceDataChannel->RegisterObserver(aliceDataChannelObserver.get());
  
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  
  EXPECT_EQ(0, exit_status);
}
