const ORTC = require('../');
const RTCIceGatherer = ORTC.RTCIceGatherer;
const RTCIceGathererState = ORTC.RTCIceGathererState;
const RTCIceGatherPolicy = ORTC.RTCIceGatherPolicy;

var gatherOptions = {
  gatherPolicy: "all",
  iceServers: [
    { urls: "stuns:stun1.example.net" },
    { urls: [ 
        "stun:stun1.example.net", 
        "stun:stun2.example.net",
        "stuns:stun3.example.net",
        "stun:stun4.example.net",
      ]
    },
    {
      urls: "stun:stun.l.google.com:19302"
    },
    { 
      urls: "turn:turn.example.org", 
      username: "user", 
      credential: "myPassword", 
      credentialType: "password" 
    },
    { 
      urls: "turn:turn1.example.org", 
      username: "user", 
      credential: "myPassword", 
      credentialType: "password" 
    }
  ],
};

var iceGatherer = new RTCIceGatherer(gatherOptions);

iceGatherer.on('statechange', (event) => {
  switch (event.state) {
    case RTCIceGathererState.new:
      console.log('statechange: new');
      break;
    case RTCIceGathererState.complete:
      console.log('statechange: complete');
      break;
    case RTCIceGathererState.gathering:
      console.log('statechange: gathering');
      break;
    case RTCIceGathererState.closed:
      console.log('statechange: closed');
      break;
  }
});

iceGatherer.on('localcandidate', (event) => {
  if (!event.complete) {
    console.log('localcandidate:', event.candidate, event.url);
  } else {
    console.log('localcandidate: completed');
    iceGatherer.close();
  }
});

console.log('LocalParameters:', iceGatherer.getLocalParameters());

iceGatherer.gather();

