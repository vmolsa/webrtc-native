"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Stream = require("stream");
const ortc_1 = require("./ortc");
class RTCIceTransport extends Stream.Duplex {
    constructor(gatherer) {
        super();
    }
    get iceGatherer() {
        return this._iceGatherer;
    }
    get role() {
        return this._role;
    }
    get component() {
        return this._component;
    }
    get state() {
        return this._state;
    }
    getRemoteCandidates() {
        return undefined;
    }
    getSelectedCandidatePair() {
        return undefined;
    }
    start(gatherer, remoteParameters, role = ortc_1.RTCIceRole.controlled) {
    }
    stop() {
    }
    getRemoteParameters() {
        return undefined;
    }
    createAssociatedTransport() {
        return undefined;
    }
    addRemoteCandidate(remoteCandidate) {
    }
    setRemoteCandidates(remoteCandidates) {
    }
    getStats() {
        return undefined;
    }
}
exports.RTCIceTransport = RTCIceTransport;
;
//# sourceMappingURL=rtcicetransport.js.map