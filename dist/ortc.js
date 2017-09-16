"use strict";
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
const Stream = require("stream");
__export(require("./stun"));
var RTCStatsType;
(function (RTCStatsType) {
    RTCStatsType[RTCStatsType["inboundrtp"] = 0] = "inboundrtp";
    RTCStatsType[RTCStatsType["outboundrtp"] = 1] = "outboundrtp";
    RTCStatsType[RTCStatsType["datachannel"] = 2] = "datachannel";
    RTCStatsType[RTCStatsType["track"] = 3] = "track";
    RTCStatsType[RTCStatsType["transport"] = 4] = "transport";
    RTCStatsType[RTCStatsType["candidatepair"] = 5] = "candidatepair";
    RTCStatsType[RTCStatsType["localcandidate"] = 6] = "localcandidate";
    RTCStatsType[RTCStatsType["remotecandidate"] = 7] = "remotecandidate";
    RTCStatsType[RTCStatsType["certificate"] = 8] = "certificate";
})(RTCStatsType = exports.RTCStatsType || (exports.RTCStatsType = {}));
;
var RTCIceProtocol;
(function (RTCIceProtocol) {
    RTCIceProtocol[RTCIceProtocol["udp"] = 0] = "udp";
    RTCIceProtocol[RTCIceProtocol["tcp"] = 1] = "tcp";
})(RTCIceProtocol = exports.RTCIceProtocol || (exports.RTCIceProtocol = {}));
;
var RTCIceTcpCandidateType;
(function (RTCIceTcpCandidateType) {
    RTCIceTcpCandidateType[RTCIceTcpCandidateType["active"] = 0] = "active";
    RTCIceTcpCandidateType[RTCIceTcpCandidateType["passive"] = 1] = "passive";
    RTCIceTcpCandidateType[RTCIceTcpCandidateType["so"] = 2] = "so";
})(RTCIceTcpCandidateType = exports.RTCIceTcpCandidateType || (exports.RTCIceTcpCandidateType = {}));
;
var RTCIceCandidateType;
(function (RTCIceCandidateType) {
    RTCIceCandidateType[RTCIceCandidateType["host"] = 0] = "host";
    RTCIceCandidateType[RTCIceCandidateType["srflx"] = 1] = "srflx";
    RTCIceCandidateType[RTCIceCandidateType["prflx"] = 2] = "prflx";
    RTCIceCandidateType[RTCIceCandidateType["relay"] = 3] = "relay";
})(RTCIceCandidateType = exports.RTCIceCandidateType || (exports.RTCIceCandidateType = {}));
;
var RTCIceComponent;
(function (RTCIceComponent) {
    RTCIceComponent[RTCIceComponent["RTP"] = 0] = "RTP";
    RTCIceComponent[RTCIceComponent["RTCP"] = 1] = "RTCP";
})(RTCIceComponent = exports.RTCIceComponent || (exports.RTCIceComponent = {}));
;
var RTCIceGathererState;
(function (RTCIceGathererState) {
    RTCIceGathererState[RTCIceGathererState["new"] = 0] = "new";
    RTCIceGathererState[RTCIceGathererState["gathering"] = 1] = "gathering";
    RTCIceGathererState[RTCIceGathererState["complete"] = 2] = "complete";
    RTCIceGathererState[RTCIceGathererState["closed"] = 3] = "closed";
})(RTCIceGathererState = exports.RTCIceGathererState || (exports.RTCIceGathererState = {}));
;
var RTCIceGatherPolicy;
(function (RTCIceGatherPolicy) {
    RTCIceGatherPolicy[RTCIceGatherPolicy["all"] = 0] = "all";
    RTCIceGatherPolicy[RTCIceGatherPolicy["nohost"] = 1] = "nohost";
    RTCIceGatherPolicy[RTCIceGatherPolicy["relay"] = 2] = "relay";
})(RTCIceGatherPolicy = exports.RTCIceGatherPolicy || (exports.RTCIceGatherPolicy = {}));
;
var RTCIceCredentialType;
(function (RTCIceCredentialType) {
    RTCIceCredentialType[RTCIceCredentialType["password"] = 0] = "password";
    RTCIceCredentialType[RTCIceCredentialType["oauth"] = 1] = "oauth";
})(RTCIceCredentialType = exports.RTCIceCredentialType || (exports.RTCIceCredentialType = {}));
;
var RTCIceRole;
(function (RTCIceRole) {
    RTCIceRole[RTCIceRole["controlling"] = 0] = "controlling";
    RTCIceRole[RTCIceRole["controlled"] = 1] = "controlled";
})(RTCIceRole = exports.RTCIceRole || (exports.RTCIceRole = {}));
;
var RTCIceTransportState;
(function (RTCIceTransportState) {
    RTCIceTransportState[RTCIceTransportState["new"] = 0] = "new";
    RTCIceTransportState[RTCIceTransportState["checking"] = 1] = "checking";
    RTCIceTransportState[RTCIceTransportState["connected"] = 2] = "connected";
    RTCIceTransportState[RTCIceTransportState["completed"] = 3] = "completed";
    RTCIceTransportState[RTCIceTransportState["disconnected"] = 4] = "disconnected";
    RTCIceTransportState[RTCIceTransportState["failed"] = 5] = "failed";
    RTCIceTransportState[RTCIceTransportState["closed"] = 6] = "closed";
})(RTCIceTransportState = exports.RTCIceTransportState || (exports.RTCIceTransportState = {}));
;
;
;
;
;
;
;
;
;
;
;
;
var rtcicegatherer_1 = require("./rtcicegatherer");
exports.RTCIceGatherer = rtcicegatherer_1.RTCIceGatherer;
var rtcicetransport_1 = require("./rtcicetransport");
exports.RTCIceTransport = rtcicetransport_1.RTCIceTransport;
;
;
;
class RTCCertificate {
    get expires() {
        return this._expires;
    }
    get fingerprint() {
        return this._fingerprint;
    }
    getAlgorithm() {
        return this._algorithm;
    }
    static generateCertificate(keygenAlgorithm) {
        return undefined;
    }
}
exports.RTCCertificate = RTCCertificate;
;
var RTCDtlsTransportState;
(function (RTCDtlsTransportState) {
    RTCDtlsTransportState[RTCDtlsTransportState["new"] = 0] = "new";
    RTCDtlsTransportState[RTCDtlsTransportState["connecting"] = 1] = "connecting";
    RTCDtlsTransportState[RTCDtlsTransportState["connected"] = 2] = "connected";
    RTCDtlsTransportState[RTCDtlsTransportState["closed"] = 3] = "closed";
    RTCDtlsTransportState[RTCDtlsTransportState["failed"] = 4] = "failed";
})(RTCDtlsTransportState = exports.RTCDtlsTransportState || (exports.RTCDtlsTransportState = {}));
;
var RTCDtlsRole;
(function (RTCDtlsRole) {
    RTCDtlsRole[RTCDtlsRole["auto"] = 0] = "auto";
    RTCDtlsRole[RTCDtlsRole["client"] = 1] = "client";
    RTCDtlsRole[RTCDtlsRole["server"] = 2] = "server";
})(RTCDtlsRole = exports.RTCDtlsRole || (exports.RTCDtlsRole = {}));
;
;
class RTCDtlsTransport extends Stream.Transform {
    constructor(transport, certficates) {
        super();
    }
    get transport() {
        return this._transport;
    }
    get state() {
        return this._state;
    }
    getCertificates() {
        return undefined;
    }
    getLocalParameters() {
        return undefined;
    }
    getRemoteParameters() {
        return undefined;
    }
    getRemoteCertificates() {
        return undefined;
    }
    start(remoteParameters) {
    }
    stop() {
    }
    getStats() {
        return undefined;
    }
}
exports.RTCDtlsTransport = RTCDtlsTransport;
;
;
class RTCRtpListener {
    constructor(transport) {
    }
    get transport() {
        return this._transport;
    }
}
exports.RTCRtpListener = RTCRtpListener;
;
;
;
;
;
;
;
var RTCPriorityType;
(function (RTCPriorityType) {
    RTCPriorityType[RTCPriorityType["very-low"] = 0] = "very-low";
    RTCPriorityType[RTCPriorityType["low"] = 1] = "low";
    RTCPriorityType[RTCPriorityType["medium"] = 2] = "medium";
    RTCPriorityType[RTCPriorityType["high"] = 3] = "high";
})(RTCPriorityType = exports.RTCPriorityType || (exports.RTCPriorityType = {}));
;
;
;
;
var RTCDegradationPreference;
(function (RTCDegradationPreference) {
    RTCDegradationPreference[RTCDegradationPreference["maintain-framerate"] = 0] = "maintain-framerate";
    RTCDegradationPreference[RTCDegradationPreference["maintain-resolution"] = 1] = "maintain-resolution";
    RTCDegradationPreference[RTCDegradationPreference["balanced"] = 2] = "balanced";
})(RTCDegradationPreference = exports.RTCDegradationPreference || (exports.RTCDegradationPreference = {}));
;
;
;
;
class RTCRtpSender extends Stream.Duplex {
    constructor(trackOrKind, transport, rtcpTransport) {
        super();
    }
    get track() {
        return this._track;
    }
    get transport() {
        return this._transport;
    }
    get rtcpTransport() {
        return this._rtcpTransport;
    }
    get kind() {
        return this._kind;
    }
    setTransport(transport, rtcpTransport) {
    }
    setTrack(trackOrKind) {
        return undefined;
    }
    replaceTrack(trackOrKind) {
        return undefined;
    }
    static getCapabilities(kind) {
        return undefined;
    }
    send(parameters) {
        return undefined;
    }
    stop() {
    }
    getStats() {
        return undefined;
    }
}
exports.RTCRtpSender = RTCRtpSender;
;
;
;
class RTCRtpReceiver extends Stream.Duplex {
    constructor(kind, transport, rtcpTransport) {
        super();
    }
    get track() {
        return this._track;
    }
    get transport() {
        return this._transport;
    }
    get rtcpTransport() {
        return this._rtcpTransport;
    }
    setTransport(transport, rtcpTransport) {
    }
    static getCapabilities(kind) {
        return undefined;
    }
    receive(parameters) {
        return undefined;
    }
    getContributingSources() {
        return undefined;
    }
    getSynchronizationSources() {
        return undefined;
    }
    stop() {
    }
    getStats() {
        return undefined;
    }
}
exports.RTCRtpReceiver = RTCRtpReceiver;
;
;
var RTCDataChannelState;
(function (RTCDataChannelState) {
    RTCDataChannelState[RTCDataChannelState["connecting"] = 0] = "connecting";
    RTCDataChannelState[RTCDataChannelState["open"] = 1] = "open";
    RTCDataChannelState[RTCDataChannelState["closing"] = 2] = "closing";
    RTCDataChannelState[RTCDataChannelState["closed"] = 3] = "closed";
})(RTCDataChannelState = exports.RTCDataChannelState || (exports.RTCDataChannelState = {}));
;
;
;
class RTCDataChannel extends Stream.Duplex {
    constructor(transport, parameters) {
        super();
    }
    get readyState() {
        return this._state;
    }
    get transport() {
        return this._transport;
    }
    get bufferedAmount() {
        return this._bufferedAmount;
    }
    getParameters() {
        return undefined;
    }
    close() {
    }
    send(data) {
    }
}
exports.RTCDataChannel = RTCDataChannel;
;
var RTCSctpTransportState;
(function (RTCSctpTransportState) {
    RTCSctpTransportState[RTCSctpTransportState["new"] = 0] = "new";
    RTCSctpTransportState[RTCSctpTransportState["connecting"] = 1] = "connecting";
    RTCSctpTransportState[RTCSctpTransportState["connected"] = 2] = "connected";
    RTCSctpTransportState[RTCSctpTransportState["closed"] = 3] = "closed";
})(RTCSctpTransportState = exports.RTCSctpTransportState || (exports.RTCSctpTransportState = {}));
;
;
class RTCSctpTransport extends Stream.Transform {
    constructor(transport, port) {
        super();
    }
    get transport() {
        return this._transport;
    }
    get state() {
        return this._state;
    }
    get port() {
        return this._port;
    }
    static getCapabilities() {
        return undefined;
    }
    start(remoteCaps, remotePort) {
    }
    stop() {
    }
    getStats() {
        return undefined;
    }
}
exports.RTCSctpTransport = RTCSctpTransport;
;
//# sourceMappingURL=ortc.js.map