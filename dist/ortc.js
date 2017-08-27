"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Stream = require("stream");
var Events = require("events");
/**
 * References:
 *
 * http://draft.ortc.org/
 * https://tools.ietf.org/html/rfc5245 - Interactive Connectivity Establishment (ICE)
 * https://tools.ietf.org/html/rfc3489 - STUN - Simple Traversal of User Datagram Protocol (UDP)
 * https://tools.ietf.org/html/rfc5389 - Session Traversal Utilities for NAT (STUN)
 * https://tools.ietf.org/html/rfc5766 - Traversal Using Relays around NAT (TURN)
 * https://tools.ietf.org/html/rfc1889 - RTP: A Transport Protocol for Real-Time Applications
 * https://tools.ietf.org/html/rfc4960 - Stream Control Transmission Protocol
 * https://tools.ietf.org/html/draft-jesup-rtcweb-data-protocol-04#page-3 - WebRTC Data Channel Protocol
 */
/**
 * For ORTC, RTCStatsType is equal to one of the following strings
 */
var RTCStatsType;
(function (RTCStatsType) {
    /**
     * Statistics for the inbound RTP stream.
     * It is accessed via the RTCInboundRTPStreamStats.
     * Local inbound RTP statistics can be obtained from the RTCRtpReceiver object;
     * remote inbound RTP statistics can be obtained from the RTCRtpSender object
    */
    RTCStatsType[RTCStatsType["inboundrtp"] = 0] = "inboundrtp";
    /**
     * Statistics for the outbound RTP stream.
     * It is accessed via the RTCOutboundRTPStreamStats.
     * Local outbound RTP statistics can be obtained from the RTCRtpSender object;
     * remote outbound RTP statistics can be obtained from the RTCRtpReceiver object.
     */
    RTCStatsType[RTCStatsType["outboundrtp"] = 1] = "outboundrtp";
    /**
     * Statistics relating to each RTCDataChannel id.
     * It is accessed via the RTCDataChannelStats.
     */
    RTCStatsType[RTCStatsType["datachannel"] = 2] = "datachannel";
    /**
     * Statistics relating to the MediaStreamTrack object.
     * It is accessed via the RTCMediaStreamTrackStats.
     */
    RTCStatsType[RTCStatsType["track"] = 3] = "track";
    /**
     * Transport statistics related to the RTCDtlsTransport object.
     * It is accessed via the RTCTransportStats.
     */
    RTCStatsType[RTCStatsType["transport"] = 4] = "transport";
    /**
     * ICE candidate pair statistics related to RTCIceTransport objects.
     * It is accessed via the RTCIceCandidatePairStats.
     */
    RTCStatsType[RTCStatsType["candidatepair"] = 5] = "candidatepair";
    /**
     * ICE local candidate statistics, related to RTCIceGatherer objects.
     * It is accessed via the RTCIceCandidateStats for the local candidate.
     */
    RTCStatsType[RTCStatsType["localcandidate"] = 6] = "localcandidate";
    /**
     * ICE remote candidate statistics, related to RTCIceTransport objects.
     * It is accessed via the RTCIceCandidateStats for the remote candidate.
     */
    RTCStatsType[RTCStatsType["remotecandidate"] = 7] = "remotecandidate";
    /**
     * Information about a certificate used by an RTCDtlsTransport object.
     * It is accessed via the RTCCertificateStats
     */
    RTCStatsType[RTCStatsType["certificate"] = 8] = "certificate";
})(RTCStatsType = exports.RTCStatsType || (exports.RTCStatsType = {}));
;
/** The RTCIceProtocol includes the protocol of the ICE candidate */
var RTCIceProtocol;
(function (RTCIceProtocol) {
    /**
     * A UDP candidate, as described in [RFC5245]
     */
    RTCIceProtocol[RTCIceProtocol["udp"] = 0] = "udp";
    /**
     * A TCP candidate, as described in [RFC6544]
     */
    RTCIceProtocol[RTCIceProtocol["tcp"] = 1] = "tcp";
})(RTCIceProtocol = exports.RTCIceProtocol || (exports.RTCIceProtocol = {}));
;
/**
 * The RTCIceTcpCandidateType includes the type of the ICE TCP candidate, as described in [RFC6544].
 * Browsers must gather active TCP candidates and only active TCP candidates.
 * Servers and other endpoints may gather active, passive or so candidates.
 */
var RTCIceTcpCandidateType;
(function (RTCIceTcpCandidateType) {
    /**
     * An active TCP candidate is one for which the transport will attempt to open an outbound
     * connection but will not receive incoming connection requests.
     */
    RTCIceTcpCandidateType[RTCIceTcpCandidateType["active"] = 0] = "active";
    /**
     * A passive TCP candidate is one for which the transport will receive incoming
     * connection attempts but not attempt a connection.
     */
    RTCIceTcpCandidateType[RTCIceTcpCandidateType["passive"] = 1] = "passive";
    /**
     * An so candidate is one for which the transport will attempt to open a connection
     * simultaneously with its peer.
     */
    RTCIceTcpCandidateType[RTCIceTcpCandidateType["so"] = 2] = "so";
})(RTCIceTcpCandidateType = exports.RTCIceTcpCandidateType || (exports.RTCIceTcpCandidateType = {}));
;
/** The RTCIceCandidateType includes the type of the ICE candidate as defined in [RFC5245] section 15.1. */
var RTCIceCandidateType;
(function (RTCIceCandidateType) {
    /**
     * A host candidate, as defined in Section 4.1.1.1 of [RFC5245].
     */
    RTCIceCandidateType[RTCIceCandidateType["host"] = 0] = "host";
    /**
     * A server reflexive candidate, as defined in Section 4.1.1.2 of [RFC5245].
     */
    RTCIceCandidateType[RTCIceCandidateType["srflx"] = 1] = "srflx";
    /**
     * A peer reflexive candidate, as defined in Section 4.1.1.2 of [RFC5245].
     */
    RTCIceCandidateType[RTCIceCandidateType["prflx"] = 2] = "prflx";
    /**
     * A relay candidate, as defined in Section 7.1.3.2.1 of [RFC5245].
     */
    RTCIceCandidateType[RTCIceCandidateType["relay"] = 3] = "relay";
})(RTCIceCandidateType = exports.RTCIceCandidateType || (exports.RTCIceCandidateType = {}));
;
/**
 * RTCIceComponent contains the component-id of the RTCIceTransport,
 * which will be rtp unless RTP and RTCP are not multiplexed and the RTCIceTransport
 * object was returned by createAssociatedTransport().
 */
var RTCIceComponent;
(function (RTCIceComponent) {
    /**
     * The RTP component ID, defined (as '1') in [RFC5245] Section 4.1.1.1.
     * Protocols multiplexed with RTP (e.g. SCTP data channel) share its component ID.
     */
    RTCIceComponent[RTCIceComponent["RTP"] = 0] = "RTP";
    /**
     * The RTCP component ID, defined (as '2') in [RFC5245] Section 4.1.1.1.
     */
    RTCIceComponent[RTCIceComponent["RTCP"] = 1] = "RTCP";
})(RTCIceComponent = exports.RTCIceComponent || (exports.RTCIceComponent = {}));
;
/** RTCIceGathererState represents the current state of the ICE gatherer. */
var RTCIceGathererState;
(function (RTCIceGathererState) {
    /**
     * The object has been created but gather() has not been called.
     */
    RTCIceGathererState[RTCIceGathererState["new"] = 0] = "new";
    /**
     * gather() has been called, and the RTCIceGatherer is in the process of gathering candidates
     * (which includes adding new candidates and removing invalidated candidates).
     */
    RTCIceGathererState[RTCIceGathererState["gathering"] = 1] = "gathering";
    /**
     * The RTCIceGatherer has completed gathering.
     * Events such as adding, updating or removing an interface, or adding, changing or
     * removing a TURN server will cause the state to go back to gathering
     * before re-entering complete once all candidate changes are finalized.
     */
    RTCIceGathererState[RTCIceGathererState["complete"] = 2] = "complete";
    /**
     * The closed state can only be entered when the RTCIceGatherer has been closed
     * intentionally by calling close(). This is a terminal state.
     */
    RTCIceGathererState[RTCIceGathererState["closed"] = 3] = "closed";
})(RTCIceGathererState = exports.RTCIceGathererState || (exports.RTCIceGathererState = {}));
;
/** RTCIceGatherPolicy denotes the policy relating to the gathering of ICE candidates. */
var RTCIceGatherPolicy;
(function (RTCIceGatherPolicy) {
    /**
     * The RTCIceGatherer gathers all types of candidates when this value is specified.
     * This will not include addresses that have been filtered by the browser.
     */
    RTCIceGatherPolicy[RTCIceGatherPolicy["all"] = 0] = "all";
    /** The RTCIceGatherer gathers all ICE candidate types except for host candidates. */
    RTCIceGatherPolicy[RTCIceGatherPolicy["nohost"] = 1] = "nohost";
    /**
     * The RTCIceGatherer must only gather media relay candidates
     * such as candidates passing through a TURN server.
     * This can be used to reduce leakage of IP addresses in certain use cases.
     */
    RTCIceGatherPolicy[RTCIceGatherPolicy["relay"] = 2] = "relay";
})(RTCIceGatherPolicy = exports.RTCIceGatherPolicy || (exports.RTCIceGatherPolicy = {}));
;
/**
 * RTCIceCredentialType represents the type of credential used by a TURN server.
 */
var RTCIceCredentialType;
(function (RTCIceCredentialType) {
    /**
     * The credential is a long-term authentication password, as described in [RFC5389], Section 10.2.
     */
    RTCIceCredentialType[RTCIceCredentialType["password"] = 0] = "password";
    /**
     * An OAuth 2.0 based authentication method, as described in [RFC7635].
     * It uses the OAuth 2.0 Implicit Grant type, with PoP (Proof-of-Possession) Token type,
     * as described in [RFC6749] and [OAUTH-POP-KEY-DISTRIBUTION].
     *
     * The OAuth Client and the Auhorization Server roles are defined in [RFC6749] Section 1.1.
     *
     * If [RFC7635] is used in ORTC then the OAuth Client is responsible for refreshing
     * the credential information, and updating the RTCIceGatherer with fresh new credentials
     * before the accessToken expires.
     *
     * For OAuth Authentication, the RTCIceGatherer requires three pieces of credential information.
     * The credential is composed of a kid, which the RTCIceServer username member is used for,
     * and macKey and accessToken, which are placed in the RTCOAuthCredential dictionary.
     * All of this information can be extracted from the OAuth response parameters,
     * which are received from the Authorization Server.
     * The relevant OAuth response parameters are the "kid", the "key", and the "access_token".
     * These can be used to extract all the necessary credential
     * infromation (the kid, macKey, and accessToken) that are required by the RTCIceGatherer
     * for the Authentication.
     *
     * The [OAUTH-POP-KEY-DISTRIBUTION] defines alg parameter in Section 4.1 and 6.
     * and describes that if the Authorization Server doesn't have prior
     * knowledge of the capabilities of the client,
     * then the OAuth Client needs to provide information about the RTCIceGatherer
     * HMAC alg capabilities. This information helps the Authorization Server to generate
     * the approriate HMAC key. The HMAC alg defines the input key length,
     * and HMAC algorithm Familly (e.g. SHA), and HMAC algorithm type (e.g. symmetric/asymmetric).
     *
     * The OAuth Client sends an OAuth Request to the Authorization Server with
     * OAuth param alg and further OAuth related parameters,
     * to get an OAuth Response with the access_token, key, kid, and further OAuth related parameters.
     *
     * However, this specification uses a simplified alg approach.
     * The length of the HMAC key (RTCOAuthCredential.macKey) may be any integer number of bytes
     * greater than 20 (160 bits). This negates the need to query the HMAC Algorithm
     * capabilities of the RTCIceGatherer, and still allows for hash agility as
     * described by [STUN-BIS], Section 15.3.
     *
     * NOTE
     * According to [RFC7635] Section 4.1, the HMAC key must be a symmetric key.
     * Currently the STUN/TURN protocols use only SHA-1 and SHA-2 family
     * hash algorithms for Message Integrity Protection,
     * as defined in [RFC5389] Section 15.4, and [STUN-BIS] Section 14.6.
     *
     * When [RFC7635] is used in ORTC, this specification adds the following additional considerations:
     *
     * The OAuth Client should obtain the mac_key by requesting an alg value of HS256.
     * This will result in a 256-bit HMAC key.
     *
     * HS256 is defined in [RFC7518] Section 3.1. It is recommended here because:
     *
     * - The OAuth respose key parameter is received in JWK format
     *   according to [OAUTH-POP-KEY-DISTRIBUTION] Section 4.2. JWK's algorithms are
     *   normatively registered in the IANA "JSON Web Signature and Encryption Algorithms" registry.
     * - STUN/TURN currently use SHA family HMAC algorithms only.
     * - The key must be symmetric, according to [RFC7635].
     * - A 256-bit key is large enough to support all currently defined STUN message integrity attributes.
     * - More details about OAuth PoP Client can be found in [OAUTH-POP-KEY-DISTRIBUTION] Section 4.
     *
     * More details about Access-Token can be found in [RFC7635], Section 6.2.
     */
    RTCIceCredentialType[RTCIceCredentialType["oauth"] = 1] = "oauth";
})(RTCIceCredentialType = exports.RTCIceCredentialType || (exports.RTCIceCredentialType = {}));
;
/** RTCIceRole contains the current role of the ICE transport. */
var RTCIceRole;
(function (RTCIceRole) {
    /** controlling state */
    RTCIceRole[RTCIceRole["controlling"] = 0] = "controlling";
    /** controlled state */
    RTCIceRole[RTCIceRole["controlled"] = 1] = "controlled";
})(RTCIceRole = exports.RTCIceRole || (exports.RTCIceRole = {}));
;
/** RTCIceTransportState represents the current state of the ICE transport. */
var RTCIceTransportState;
(function (RTCIceTransportState) {
    /**
     * The RTCIceTransport object is waiting for remote candidates to be supplied.
     * In this state the RTCIceTransport object can respond to incoming connectivity checks.
     */
    RTCIceTransportState[RTCIceTransportState["new"] = 0] = "new";
    /**
     * The RTCIceTransport has received at least one remote candidate,
     * and a local and remote RTCIceCandidateComplete dictionary
     * was not added as the last candidate.
     * In this state the RTCIceTransport is checking candidate
     * pairs but has not yet found a successful candidate pair,
     * or consent checks [RFC7675] have failed on all previously successful candidate pairs.
     */
    RTCIceTransportState[RTCIceTransportState["checking"] = 1] = "checking";
    /**
     * The RTCIceTransport has received a response to an outgoing connectivity check,
     * or has received incoming DTLS/media after a successful response to an incoming connectivity check,
     * but is still checking other candidate pairs to see if there is a better connection.
     * In this state outgoing media is permitted.
     * If consent checks [RFC7675] fail on the connection in use,
     * and there are no other successful candidate pairs available,
     * then the state transitions to checking (if there are candidate pairs remaining to be checked)
     * or disconnected (if there are no candidate pairs to check,
     * but the peer is still gathering and/or waiting for additional remote candidates).
     */
    RTCIceTransportState[RTCIceTransportState["connected"] = 2] = "connected";
    /**
     * A local and remote RTCIceCandidateComplete dictionary was added as the last candidate to the
     * RTCIceTransport and all appropriate candidate pairs have been tested and at least one functioning
     * candidate pair has been found. If consent checks [RFC7675] subsequently fail on
     * all successful candidate pairs, the state transitions to failed.
     * */
    RTCIceTransportState[RTCIceTransportState["completed"] = 3] = "completed";
    /**
     * Connectivity is currently lost for this RTCIceTransport.
     * The RTCIceTransport has received at least one local and remote candidate,
     * and a local and remote RTCIceCandidateComplete dictionary was not added as the last candidate,
     * but all appropriate candidate pairs thus far have been tested and failed,
     * or consent checks [RFC7675] once successful, have repeatedly failed to receive a response.
     * At the implementation's discretion, this state may be entered prior to consent failure,
     * and therefore could resolve itself without action.
     * Other candidate pairs may become available for testing as new candidates are trickled,
     * and a temporary consent failure could resolve itself,
     * therefore the failedstate has not been reached.
     */
    RTCIceTransportState[RTCIceTransportState["disconnected"] = 4] = "disconnected";
    /**
     * A local and remote RTCIceCandidateComplete dictionary was added as the last candidate
     * to the RTCIceTransport and all appropriate candidate pairs have either failed
     * connectivity checks or have lost consent.
     */
    RTCIceTransportState[RTCIceTransportState["failed"] = 5] = "failed";
    /** The RTCIceTransport has shut down and is no longer responding to STUN requests. */
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
/**
 * The RTCIceGatherer gathers local host,
 * server reflexive and relay candidates,
 * as well as enabling the retrieval of local Interactive Connectivity Establishment (ICE) parameters which can be exchanged in signaling.
 * By enabling an endpoint to use a set of local candidates to construct multiple RTCIceTransport objects, the RTCIceGatherer enables support for scenarios such as parallel forking
 */
var RTCIceGatherer = (function (_super) {
    __extends(RTCIceGatherer, _super);
    /*
     * onstatechange() => emit('statechange');
     * onerror(event: RTCIceGathererIceErrorEvent) => emit('error');
     * onlocalcandidate(event: RTCIceGathererEvent) => emit('localcandidate');
     */
    /**
     * An RTCIceGatherer instance is constructed from an RTCIceGatherOptions object.
     *
     * An RTCIceGatherer object in the closed state can be garbage-collected when it is no longer referenced.
     */
    function RTCIceGatherer(options) {
        return _super.call(this) || this;
    }
    Object.defineProperty(RTCIceGatherer.prototype, "component", {
        /**
         * The component-id of the RTCIceGatherer object.
         * In RTCIceGatherer objects returned by createAssociatedGatherer() the value of the component attribute is rtcp.
         * In all other RTCIceGatherer objects, the value of the component attribute is rtp.
         */
        get: function () {
            return this._component;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCIceGatherer.prototype, "state", {
        /**
         * The current state of the ICE gatherer.
         */
        get: function () {
            return this._state;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Prunes all local candidates, and closes the port.
     * Associated RTCIceTransport objects transition to the disconnected state (unless they were in the failed state).
     * Calling close() when state is closed has no effect.
     */
    RTCIceGatherer.prototype.close = function () {
        this._state = RTCIceGathererState.closed;
        this.emit('statechange');
    };
    /**
     * Gather ICE candidates.
     * If options is omitted, utilize the value of options passed in the constructor.
     * If state is closed, throw an InvalidStateError exception.
     */
    RTCIceGatherer.prototype.gather = function (options) {
    };
    /**
     * getLocalParameters() obtains the ICE parameters of the RTCIceGatherer. If state is closed, throw an InvalidStateError exception.
     */
    RTCIceGatherer.prototype.getLocalParameters = function () {
        return undefined;
    };
    /**
     * Retrieve the sequence of valid local candidates associated with the RTCIceGatherer.
     * This retrieves all unpruned local candidates currently known (except for peer reflexive candidates),
     * even if an onlocalcandidate event hasn't been processed yet.
     * Prior to calling gather() an empty list will be returned. If state is closed, throw an InvalidStateError exception.
     */
    RTCIceGatherer.prototype.getLocalCandidates = function () {
        return undefined;
    };
    /**
     * Create an associated RTCIceGatherer for RTCP, with the same RTCIceParameters and RTCIceGatherOptions.
     * If state is closed, throw an InvalidStateError exception.
     * If an RTCIceGatherer calls the method more than once, or if component is rtcp, throw an InvalidStateError exception.
     */
    RTCIceGatherer.prototype.createAssociatedGatherer = function () {
        return undefined;
    };
    /**
     * Gathers stats for the given object and reports the result asynchronously.
     * If the object has not yet begun to send or receive data,
     * the returned stats will reflect this.
     * If the object is in the closed state,
     * the returned stats will reflect the stats at the time the object transitioned to the closed state.
     *
     * When the getStats method is invoked, the user agent must queue a task to run the following steps:
     * - Let p be a new promise.
     * - Return, but continue the following steps in the background.
     * - Start gathering the stats.
     * - When the relevant stats have been gathered, return a new RTCStatsReport object,
     *   representing the gathered stats.
     */
    RTCIceGatherer.prototype.getStats = function () {
        return undefined;
    };
    return RTCIceGatherer;
}(Events.EventEmitter));
exports.RTCIceGatherer = RTCIceGatherer;
;
/**
 * The RTCIceTransport allows an application access to information about the Interactive Connectivity Establishment (ICE)
 * transport over which packets are sent and received. In particular,
 * ICE manages peer-to-peer connections which involve state which the application may want to access.
 */
var RTCIceTransport = (function (_super) {
    __extends(RTCIceTransport, _super);
    /*
     * onstatechange() => emit('statechange')
     * oncandidatepairchange(event: RTCIceCandidatePairChangedEvent) => emit('candidatepairchange')
     */
    /**
     * An RTCIceTransport instance is constructed (optionally) from an RTCIceGatherer.
     * If gatherer.state is closed or gatherer.component is rtcp, then throw an InvalidStateError exception.
     *
     * An RTCIceTransport object in the closed state can be garbage-collected when it is no longer referenced.
     */
    function RTCIceTransport(gatherer) {
        return _super.call(this) || this;
    }
    Object.defineProperty(RTCIceTransport.prototype, "iceGatherer", {
        /**
         * The iceGatherer attribute is set to the value of gatherer if passed in the constructor or in the latest call to start().
         */
        get: function () {
            return this._iceGatherer;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCIceTransport.prototype, "role", {
        /**
         * The current role of the ICE transport.
         */
        get: function () {
            return this._role;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCIceTransport.prototype, "component", {
        /**
         * The component-id of the RTCIceTransport object. In RTCIceTransport objects returned by createAssociatedTransport(),
         * the value of component is rtcp. In all other RTCIceTransport objects, the value of component is rtp.
         */
        get: function () {
            return this._component;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCIceTransport.prototype, "state", {
        /**
         * The current state of the ICE transport.
         */
        get: function () {
            return this._state;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Retrieve the sequence of candidates associated with the remote RTCIceTransport.
     * Only returns the candidates previously added using setRemoteCandidates() or addRemoteCandidate().
     * If there are no remote candidates, an empty list is returned.
     */
    RTCIceTransport.prototype.getRemoteCandidates = function () {
        return undefined;
    };
    /**
     * Retrieves the selected candidate pair on which packets are sent.
     * If there is no selected pair yet, or consent [RFC7675] is lost on the selected pair, NULL is returned.
     */
    RTCIceTransport.prototype.getSelectedCandidatePair = function () {
        return undefined;
    };
    /**
     *
     * As noted in [RFC5245] Section 7.1.2.3, an incoming connectivity check utilizes the local/remote username fragment and the local password,
     * whereas an outgoing connectivity check utilizes the local/remote username fragment and the remote password.
     * Since start() provides role information, as well as the remote username fragment and password,
     * once start() is called an RTCIceTransport object can respond to incoming connectivity checks based on its configured role.
     * Since start() enables candidate pairs to be formed, it also enables initiating connectivity checks.
     *
     * When start() is called, the following steps must be run:
     * - If gatherer.component has a value different from component, throw an InvalidParameters exception.
     * - If state or gatherer.state is closed, throw an InvalidStateError exception.
     * - If remoteParameters.usernameFragment or remoteParameters.password is unset, throw an InvalidParameters exception.
     * - If start() is called again and role is changed, throw an InvalidParameters exception.
     * - If start() is called again with the same values of gatherer and remoteParameters, this has no effect.
     * - If start() is called for the first time and either gatherer was not passed in
     *   the constructor or the value of gatherer is unchanged, if there are remote candidates,
     *   set state to checking and start connectivity checks. If there are no remote candidates, state remains new.
     * - If start() is called for the first time and the value of gatherer passed as an argument
     *   is different from that passed in the constructor, flush local candidates.
     *   If there are remote candidates, set state to checking and start connectivity checks.
     *   If there are no remote candidates, state remains new.
     * - If start() is called again with the same value of gatherer but the value of remoteParameters has changed,
     *   local candidates are kept, remote candidates are flushed, candidate pairs are flushed and state transitions to new.
     * - If start() is called again with a new value of gatherer but the value of remoteParameters is unchanged,
     *   local candidates are flushed, candidate pairs are flushed, new candidate pairs are formed with existing remote candidates, and state transitions to checking.
     * - If start() is called again with new values of gatherer and remoteParameters, local candidates are flushed,
     *   remote candidates are flushed, candidate pairs are flushed and state transitions to new.
     */
    RTCIceTransport.prototype.start = function (gatherer, remoteParameters, role) {
        if (role === void 0) { role = RTCIceRole.controlled; }
    };
    /**
     * Irreversibly stops the RTCIceTransport. When stop is called, the following steps must be run:
     *
     * - Let iceTransport be the RTCIceTransport object on which the stop method is invoked.
     * - If iceTransport.state is closed, abort these steps.
     * - Set iceTransport.state to closed.
     * - Let controller be the RTCIceTransportController object that iceTransport has been added to.
     * - Remove iceTransport from controller.
     * - Fire a simple event statechange at iceTransport.
     */
    RTCIceTransport.prototype.stop = function () {
    };
    /**
     * getRemoteParameters() obtains the current ICE parameters of the remote RTCIceTransport.
     */
    RTCIceTransport.prototype.getRemoteParameters = function () {
        return undefined;
    };
    /**
     * Create an associated RTCIceTransport for RTCP. If called more than once for the same component,
     * or if state is closed, throw an InvalidStateError exception. If called when component is rtcp,
     * throw an InvalidStateError exception.
     */
    RTCIceTransport.prototype.createAssociatedTransport = function () {
        return undefined;
    };
    /**
     * Add a remote candidate associated with the remote RTCIceTransport.
     * If state is closed, throw an InvalidStateError exception. When the remote RTCIceGatherer emits its final candidate,
     * addRemoteCandidate() should be called with an RTCIceCandidateComplete dictionary as an argument,
     * so that the local RTCIceTransport can know there are no more remote candidates expected, and can enter the completed state.
     */
    RTCIceTransport.prototype.addRemoteCandidate = function (remoteCandidate) {
    };
    /**
     * Set the sequence of candidates associated with the remote RTCIceTransport. If state is closed, throw an InvalidStateError exception.
     */
    RTCIceTransport.prototype.setRemoteCandidates = function (remoteCandidates) {
    };
    /**
     * Gathers stats for the given object and reports the result asynchronously.
     * If the object has not yet begun to send or receive data,
     * the returned stats will reflect this.
     * If the object is in the closed state,
     * the returned stats will reflect the stats at the time the object transitioned to the closed state.
     *
     * When the getStats method is invoked, the user agent must queue a task to run the following steps:
     * - Let p be a new promise.
     * - Return, but continue the following steps in the background.
     * - Start gathering the stats.
     * - When the relevant stats have been gathered, return a new RTCStatsReport object,
     *   representing the gathered stats.
     */
    RTCIceTransport.prototype.getStats = function () {
        return undefined;
    };
    return RTCIceTransport;
}(Stream.Duplex));
exports.RTCIceTransport = RTCIceTransport;
;
;
;
;
/**
 * The RTCCertificate interface enables the certificates used by an RTCDtlsTransport to be provided in the constructor.
 * This makes it possible to support forking, where the offerer will create multiple RTCDtlsTransport objects
 * using the same local certificate and fingerprint.
 */
var RTCCertificate = (function () {
    function RTCCertificate() {
    }
    Object.defineProperty(RTCCertificate.prototype, "expires", {
        /**
         * The expires attribute indicates the date and time in milliseconds relative to 1970-01-01T00:00:00Z
         * after which the certificate will be considered invalid by the browser.
         * After this time, attempts to construct an RTCDtlsTransport object using this certificate will fail.
         *
         * Note that this value might not be reflected in a notAfter parameter in the certificate itself.
         */
        get: function () {
            return this._expires;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCCertificate.prototype, "fingerprint", {
        /**
         * The fingerprint of the certificate.
         * As noted in [JSEP] Section 5.2.1, the digest algorithm used for the fingerprint matches that used in the certificate signature.
         */
        get: function () {
            return this._fingerprint;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Returns the value of keygenAlgorithm passed in the call to generateCertificate().
     */
    RTCCertificate.prototype.getAlgorithm = function () {
        return this._algorithm;
    };
    /**
     * The generateCertificate method causes the user agent to create and store an X.509 certificate [X509V3] and corresponding private key.
     * A handle to information is provided in the form of the RTCCertificate interface.
     * The returned RTCCertificate can be used to control the certificate that is offered in the DTLS session established by RTCDtlsTransport.
     *
     * The keygenAlgorithm argument is used to control how the private key associated with the certificate is generated.
     * The keygenAlgorithm argument uses the WebCrypto [WebCryptoAPI] AlgorithmIdentifier type.
     * The keygenAlgorithm value must be a valid argument to window.crypto.subtle.generateKey;
     * that is, the value must produce a non-error result when normalized according to the WebCrypto algorithm
     * normalization process [WebCryptoAPI] with an operation name of generateKey and a [[supportedAlgorithms]]
     * value specific to production of certificates for RTCDtlsTransport.
     * If the algorithm normalization process produces an error, the call to generateCertificate() must be rejected with that error.
     *
     * Signatures produced by the generated key are used to authenticate the DTLS connection.
     * The identified algorithm (as identified by the name of the normalized AlgorithmIdentifier)
     * must be an asymmetric algorithm that can be used to produce a signature.
     *
     * The certificate produced by this process also contains a signature.
     * The validity of this signature is only relevant for compatibility reasons.
     * Only the public key and the resulting certificate fingerprint are used by RTCDtlsTransport,
     * but it is more likely that a certificate will be accepted if the certificate is well formed.
     * The browser selects the algorithm used to sign the certificate; a browser should select SHA-256 [FIPS-180-4]
     * if a hash algorithm is needed.
     *
     * The resulting certificate must not include information that can be linked to a user or user agent.
     * Randomized values for distinguished name and serial number should be used.
     *
     * An optional expires attribute may be added to the keygenAlgorithm parameter.
     * If this contains a DOMTimeStamp value, it indicates the maximum time that the RTCCertificate is valid for relative to the current time.
     * A user agent sets the expires attribute of the returned RTCCertificate to the current time plus the value of the expires attribute.
     * However, a user agent may choose to limit the period over which an RTCCertificate is valid.
     *
     * A user agent must reject a call to generateCertificate() with a DOMError of type "NotSupportedError"
     * if the keygenAlgorithm parameter identifies an algorithm that the user agent cannot or will not use to generate
     * a certificate for RTCDtlsTransport.
     *
     * The following values must be supported by a
     * user agent: { name: "RSASSA-PKCS1-v1_5", modulusLength: 2048, publicExponent: new Uint8Array([1, 0, 1]), hash: "SHA-256" },
     * and { name: "ECDSA", namedCurve: "P-256" }.
     *
     * It is expected that a user agent will have a small or even fixed set of values that it will accept.
     */
    RTCCertificate.generateCertificate = function (keygenAlgorithm) {
        return undefined;
    };
    return RTCCertificate;
}());
exports.RTCCertificate = RTCCertificate;
;
/**
 * RTCDtlsTransportState indicates the state of the DTLS transport.
 */
var RTCDtlsTransportState;
(function (RTCDtlsTransportState) {
    /**
     * The RTCDtlsTransport object has been created and has not started negotiating yet.
     */
    RTCDtlsTransportState[RTCDtlsTransportState["new"] = 0] = "new";
    /**
     * DTLS is in the process of negotiating a secure connection and verifying the remote fingerprint.
     * Once a secure connection is negotiated (but prior to verification of the remote fingerprint,
     * enabled by calling start()), incoming data can flow through (and media, once DTLS-SRTP key derivation is completed).
     */
    RTCDtlsTransportState[RTCDtlsTransportState["connecting"] = 1] = "connecting";
    /**
     * DTLS has completed negotiation of a secure connection and verified the remote fingerprint. Outgoing data and media can now flow through.
     */
    RTCDtlsTransportState[RTCDtlsTransportState["connected"] = 2] = "connected";
    /**
     * The DTLS connection has been closed intentionally via a call to stop() or receipt of a close_notify alert.
     * Calling transport.stop() will also result in a transition to the closed state.
     */
    RTCDtlsTransportState[RTCDtlsTransportState["closed"] = 3] = "closed";
    /**
     * The DTLS connection has been closed as the result of an error (such as receipt of an error alert or a
     * failure to validate the remote fingerprint).
     */
    RTCDtlsTransportState[RTCDtlsTransportState["failed"] = 4] = "failed";
})(RTCDtlsTransportState = exports.RTCDtlsTransportState || (exports.RTCDtlsTransportState = {}));
;
/**
 * RTCDtlsRole indicates the role of the DTLS transport.
 *
 * To diagnose DTLS role issues, an application may wish to determine the desired and actual DTLS role of an RTCDtlsTransport.
 * For a browser implementing ORTC, a RTCDtlsTransport object assumes a DTLS role of auto upon construction.
 * This implies that the DTLS role is determined by the ICE role.
 * Since getLocalParameters().role always returns the role assigned to an RTCDtlsTransport object upon construction (auto for a browser),
 * the getLocalParameters method cannot be used to determine the desired or actual role of an RTCDtlsTransport.
 *
 * An application can determine the desired role of an RTCDtlsTransport from the value of remoteParameters.
 * role passed to RTCDtlsTransport.start(remoteParameters).
 * If remoteParameters.role is server then the desired role of the RTCDtlsTransport is client.
 * If remoteParameters.role is client then the desired role of the RTCDtlsTransport is server.
 *
 * The RTCDtlsTransport.transport.onstatechange EventHandler can be used to determine whether an RTCDtlsTransport
 * transitions to the desired role as expected. When RTCDtlsTransport.transport.state transitions to connected,
 * if RTCDtlsTransport.transport.role is controlled then the role of the RTCDtlsTransport is client.
 * If RTCDtlsTransport.transport.role is controlling then the role of the RTCDtlsTransport is server.
 */
var RTCDtlsRole;
(function (RTCDtlsRole) {
    /**
     * The DLTS role is determined based on the resolved ICE role:
     * - the ICE controlled role acts as the DTLS client and the ICE controlling role acts as the DTLS server.
     */
    RTCDtlsRole[RTCDtlsRole["auto"] = 0] = "auto";
    /**
     * The DTLS client role.
     */
    RTCDtlsRole[RTCDtlsRole["client"] = 1] = "client";
    /**
     * The DTLS server role.
     */
    RTCDtlsRole[RTCDtlsRole["server"] = 2] = "server";
})(RTCDtlsRole = exports.RTCDtlsRole || (exports.RTCDtlsRole = {}));
;
;
/**
 * The RTCDtlsTransport object includes information relating to Datagram Transport Layer Security (DTLS) transport.
 *
 * A RTCDtlsTransport instance is constructed using an RTCIceTransport and a sequence of RTCCertificate objects.
 * An RTCDtlsTransport object in the closed or failed states can be garbage-collected when it is no longer referenced.
 *
 * If an attempt is made to construct a RTCDtlsTransport instance from an RTCIceTransport in the closed state,
 * an InvalidStateError exception is thrown. If certificates is non-empty,
 * check that the expires attribute of each RTCCertificate object is in the future.
 * If a certificate has expired, throw an InvalidParameter exception; otherwise, store the certificates.
 *
 * Since the Datagram Transport Layer Security (DTLS) negotiation occurs between transport endpoints determined via ICE,
 * implementations of this specification must support multiplexing of STUN, TURN, DTLS and RTP and/or RTCP.
 * This multiplexing, originally described in [RFC5764] Section 5.1.2, is updated in [RFC7983].
 *
 * A newly constructed RTCDtlsTransport must listen and respond to incoming DTLS packets before start() is called.
 * However, to complete the negotiation it is necessary to verify the remote fingerprint,
 * which is dependent on remoteParameters, passed to start(). To verify the remote fingerprint,
 * compute the fingerprint value for the selected remote certificate using the signature digest algorithm,
 * and compare it against remoteParameters.fingerprints.
 * If the selected remote certificate RTCDtlsFingerprint.value matches remoteParameters.fingerprints[j].value
 * and RTCDtlsFingerprint.algorithm matches remoteParameters.fingerprints[j].algorithm for any value of j,
 * the remote fingerprint is verified.
 * After the DTLS handshake exchange completes (but before the remote fingerprint is verified)
 * incoming media packets may be received. A modest buffer must be provided to avoid loss of media
 * prior to remote fingerprint validation (which can begin after start() is called).
 */
var RTCDtlsTransport = (function (_super) {
    __extends(RTCDtlsTransport, _super);
    /*
     * onstatechange() => emit('statechange')
     * onerror(event: Error) => emit('error')
     */
    /**
     * An RTCDtlsTransport instance is associated to an RTCRtpSender, an RTCRtpReceiver, or an RTCSctpTransport instance.
     */
    function RTCDtlsTransport(transport, certficates) {
        return _super.call(this) || this;
    }
    Object.defineProperty(RTCDtlsTransport.prototype, "transport", {
        /**
         * The associated RTCIceTransport instance.
         */
        get: function () {
            return this._transport;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCDtlsTransport.prototype, "state", {
        /**
         * The current state of the DTLS transport.
         */
        get: function () {
            return this._state;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * getCertificates() returns the certificates provided in the constructor.
     */
    RTCDtlsTransport.prototype.getCertificates = function () {
        return undefined;
    };
    /**
     * getLocalParameters() obtains the DTLS parameters of the local RTCDtlsTransport upon construction.
     * If multiple certificates were provided in the constructor, then multiple fingerprints will be returned,
     * one for each certificate. getLocalParameters().role always returns the default role of a newly constructed RTCDtlsTransport;
     * for a browser this will be auto.
     */
    RTCDtlsTransport.prototype.getLocalParameters = function () {
        return undefined;
    };
    /**
     * getRemoteParameters() obtains the remote DTLS parameters passed in the start() method. Prior to calling start(), null is returned.
     */
    RTCDtlsTransport.prototype.getRemoteParameters = function () {
        return undefined;
    };
    /**
     * getRemoteCertificates() returns the certificate chain in use by the remote side,
     * with each certificate encoded in binary Distinguished Encoding Rules (DER) [X690].
     * getRemoteCertificates() returns an empty list prior to selection of the remote certificate,
     * which is completed once state transitions to connected.
     */
    RTCDtlsTransport.prototype.getRemoteCertificates = function () {
        return undefined;
    };
    /**
     * Start DTLS transport negotiation with the parameters of the remote DTLS transport,
     * including verification of the remote fingerprint,
     * then once the DTLS transport session is established,
     * negotiate a DTLS-SRTP [RFC5764] session to establish keys so as protect media using SRTP [RFC3711].
     * Since symmetric RTP [RFC4961] is utilized, the DTLS-SRTP session is bi-directional.
     *
     * If remoteParameters is invalid, throw an InvalidParameters exception.
     * If start() is called after a previous start() call, or if state is closed,
     * throw an InvalidStateError exception. Only a single DTLS transport can be multiplexed over an ICE transport.
     * Therefore if a RTCDtlsTransport object dtlsTransportB is constructed with an RTCIceTransport object iceTransport
     * previously used to construct another RTCDtlsTransport object dtlsTransportA,
     * then if dtlsTransportB.start() is called prior to having called dtlsTransportA.stop(),
     * then throw an InvalidStateError exception.
     */
    RTCDtlsTransport.prototype.start = function (remoteParameters) {
    };
    /**
     * Stops and closes the RTCDtlsTransport object. Calling stop() when state is closed has no effect.
     */
    RTCDtlsTransport.prototype.stop = function () {
    };
    /**
     * Gathers stats for the given object and reports the result asynchronously.
     * If the object has not yet begun to send or receive data,
     * the returned stats will reflect this.
     * If the object is in the closed state,
     * the returned stats will reflect the stats at the time the object transitioned to the closed state.
     *
     * When the getStats method is invoked, the user agent must queue a task to run the following steps:
     * - Let p be a new promise.
     * - Return, but continue the following steps in the background.
     * - Start gathering the stats.
     * - When the relevant stats have been gathered, return a new RTCStatsReport object,
     *   representing the gathered stats.
     */
    RTCDtlsTransport.prototype.getStats = function () {
        return undefined;
    };
    return RTCDtlsTransport;
}(Stream.Transform));
exports.RTCDtlsTransport = RTCDtlsTransport;
;
;
/**
 * The RTCRtpListener listens to RTP packets received from the RTCDtlsTransport,
 * determining whether an incoming RTP stream is configured to be processed by an existing RTCRtpReceiver object.
 * If no match is found, the unhandledrtp event is fired. This can be due to packets having an unknown SSRC,
 * payload type or any other error that makes it impossible to attribute an RTP packet to a specific RTCRtpReceiver object.
 * The event is not fired once for each arriving packet; multiple discarded packets for the same SSRC should result in a single event.
 *
 * Note that application handling of the unhandledrtp event may not be sufficient to enable the unhandled RTP stream to be rendered.
 * The amount of buffering to be provided for unhandled RTP streams is not mandated by this specification and is recommended
 * to be strictly limited to protect against denial of service attacks.
 * Therefore an application attempting to create additional RTCRtpReceiver objects to handle the incoming RTP stream may find
 * that portions of the incoming RTP stream were lost due to insufficient buffers, and therefore could not be rendered.
 */
var RTCRtpListener = (function () {
    /*
     * onunhandledrtp(event: RTCRtpUnhandledEvent) => emit('unhandledrtp')
     */
    /**
     * An RTCRtpListener instance is associated to an RTCDtlsTransport.
     */
    function RTCRtpListener(transport) {
    }
    Object.defineProperty(RTCRtpListener.prototype, "transport", {
        /**
         * The RTP RTCDtlsTransport instance.
         */
        get: function () {
            return this._transport;
        },
        enumerable: true,
        configurable: true
    });
    return RTCRtpListener;
}());
exports.RTCRtpListener = RTCRtpListener;
;
;
;
;
;
;
;
/**
 * RTCPriorityType can be used to indicate the relative priority of various flows.
 * This allows applications to indicate to the browser whether a particular media flow is high, medium, low
 * or of very low importance to the application.
 * WebRTC uses the priority and Quality of Service (QoS) framework described in [RTCWEB-TRANSPORT]
 * and [TSVWG-RTCWEB-QOS] to provide priority and DSCP marketing for packets that will help provide QoS in some networking environments.
 * Applications that use this API should be aware that often better overall user experience is obtained
 * by lowering the priority of things that are not as important rather than raising the the priority of the things that are.
 */
var RTCPriorityType;
(function (RTCPriorityType) {
    /**
     * See [RTCWEB-TRANSPORT], Section 4.
     */
    RTCPriorityType[RTCPriorityType["very-low"] = 0] = "very-low";
    /**
     * See [RTCWEB-TRANSPORT], Section 4.
     */
    RTCPriorityType[RTCPriorityType["low"] = 1] = "low";
    /**
     * See [RTCWEB-TRANSPORT], Section 4.
     */
    RTCPriorityType[RTCPriorityType["medium"] = 2] = "medium";
    /**
     * See [RTCWEB-TRANSPORT], Section 4.
     */
    RTCPriorityType[RTCPriorityType["high"] = 3] = "high";
})(RTCPriorityType = exports.RTCPriorityType || (exports.RTCPriorityType = {}));
;
;
;
/**
 * RTCDegradationPreference can be used to indicate the desired choice between degrading resolution
 * and degrading framerate when bandwidth is constrained.
 */
var RTCDegradationPreference;
(function (RTCDegradationPreference) {
    /**
     * Degrade resolution in order to maintain framerate.
     */
    RTCDegradationPreference[RTCDegradationPreference["maintain-framerate"] = 0] = "maintain-framerate";
    /**
     * Degrade framerate in order to maintain resolution.
     */
    RTCDegradationPreference[RTCDegradationPreference["maintain-resolution"] = 1] = "maintain-resolution";
    /**
     * Degrade a balance of framerate and resolution.
     */
    RTCDegradationPreference[RTCDegradationPreference["balanced"] = 2] = "balanced";
})(RTCDegradationPreference = exports.RTCDegradationPreference || (exports.RTCDegradationPreference = {}));
;
;
;
;
/**
 * The RTCRtpSender includes information relating to the RTP sender.
 *
 * A RTCRtpSender instance is constructed from an NodeStream object or kind and associated to an RTCDtlsTransport.
 * If an attempt is made to construct an RTCRtpSender object with transport.state or rtcpTransport.state closed,
 * or if track.readyState is ended, throw an InvalidStateError exception.
 *
 * An RTCRtpSender object can be garbage-collected once stop() is called and it is no longer referenced.
 */
var RTCRtpSender = (function (_super) {
    __extends(RTCRtpSender, _super);
    /*
     * onssrcconflict() => emit('ssrcconflict')
     */
    /**
     * An RTCRtpSender instance is associated to a sending NodeStream and provides RTC related methods to it.
     *
     * When the constructor is invoked, the user agent must run the following steps:
     * - If transport is unset, and rtcpTransport is set, throw an InvalidParameters exception and abort all further steps.
     * - If the first argument is a string, let it be kind and run the following steps:
     * - - If kind is not a legal NodeStream kind, throw a TypeError and abort these, and all further steps.
     * - - Let track be null.
     * - If the first argument is a NodeStream, let it be track and let kind be track.kind.
     * - Create an RTCRtpSender with track, transport (if provided) and rtcpTransport (if provided) and let sender be the result.
     */
    function RTCRtpSender(trackOrKind, transport, rtcpTransport) {
        return _super.call(this) || this;
    }
    Object.defineProperty(RTCRtpSender.prototype, "track", {
        /**
         * The associated MediaStreamTrack instance. If track is ended,
         * or if track.muted is set to true,
         * the RTCRtpSender sends silence (audio) or a black frame (video).
         * If track is set to null then the RTCRtpSender does not send RTP.
         */
        get: function () {
            return this._track;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCRtpSender.prototype, "transport", {
        /**
         * The RTCDtlsTransport instance over which RTCP is sent and received (if provided).
         * When BUNDLE is used,
         * many RTCRtpSender objects will share one rtcpTransport and will all send and receive RTCP over the same RTCDtlsTransport.
         * When RTCP mux is used, rtcpTransport will be null, and both RTP and RTCP traffic will flow over the RTCDtlsTransport transport.
         */
        get: function () {
            return this._transport;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCRtpSender.prototype, "rtcpTransport", {
        /**
         * The associated RTCP RTCDtlsTransport instance if one was provided in the constructor.
         * When RTCP mux is used, rtcpTransport will be null, and both RTP and RTCP traffic will flow over the RTCDtlsTransport transport.
         */
        get: function () {
            return this._rtcpTransport;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCRtpSender.prototype, "kind", {
        /**
         * The value of kind or track.kind passed in the constructor.
         */
        get: function () {
            return this._kind;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * setTransport() attempts to replace the the RTP RTCDtlsTransport transport (if set)
     * and RTCP RTCDtlsTransport rtcpTransport (if used) with the transport(s) provided.
     *
     * When the setTransport() method is invoked, the user agent must run the following steps:
     *
     * - Let sender be the RTCRtpSender object on which setTransport() is invoked.
     * - If sender.stop() has been called, throw an InvalidStateError exception.
     * - Let withTransport and withRtcpTransport be the arguments to this method.
     * - If setTransport() is called with no arguments, or if withTransport is unset, throw an InvalidParameters exception.
     * - If withTransport.transport.component is rtcp, throw an InvalidParameters exception.
     * - If withRtcpTransport is set and withRtcpTransport.transport.component is rtp, throw an InvalidParameters exception.
     * - If withTransport is set and withTransport.state is closed, throw an InvalidStateError exception.
     * - If withRtcpTransport is set and withRtcpTransport.state is closed, throw an InvalidStateError exception.
     * - Set transport to withTransport and rtcpTransport to withRtcpTransport (if provided) and seamlessly send over the new transport(s).
     */
    RTCRtpSender.prototype.setTransport = function (transport, rtcpTransport) {
    };
    /**
     * setTrack Attempts to replace the track being sent with another track provided (or with a null track).
     * The deprecated setTrack method operates identically to the replaceTrack method.
     */
    RTCRtpSender.prototype.setTrack = function (trackOrKind) {
        return undefined;
    };
    /**
     * Attempts to replace the track being sent with another track provided (or with a null track).
     *
     * When the replaceTrack method is invoked, the user agent must run the following steps:
     * - Let p be a new promise.
     * - Let sender be the RTCRtpSender object on which replaceTrack() is invoked.
     * - If sender.stop() has been called, reject p with InvalidStateError.
     * - Let withTrack be the argument to this method.
     * - If withTrack is non-null and withTrack.kind differs from sender.kind, reject p with TypeError.
     * - Run the following steps:
     * -  - Set the track attribute to withTrack. If withTrack is null, the sender stops sending. Otherwise, have the sender seamlessly switch to transmitting withTrack in place of what it is sending.
     * -  - Resolve p with undefined.
     */
    RTCRtpSender.prototype.replaceTrack = function (trackOrKind) {
        return undefined;
    };
    /**
     * getCapabilities() obtains the sender capabilities, based on kind.
     * Browsers must support kind values of "audio" and "video".
     * If there are no capabilities corresponding to the value of kind, getCapabilities returns null.
     * Capabilities that can apply to multiple values of kind (such as retransmission [RFC4588],
     * redundancy [RFC2198] and Forward Error Correction) have RTCRtpCapabilities.RTCRtpCodecCapability[i].kind
     * set to the value of the kind argument.
     */
    RTCRtpSender.getCapabilities = function (kind) {
        return undefined;
    };
    /**
     * Attempts to set the parameters controlling the sending of media.
     *
     * When the send() method is invoked, the user agent must run the following steps:
  
     * - Let sender be the RTCRtpSender object on which send() is invoked.
     * - Let p be a new promise.
     * - If sender.stop() has been called, reject p with InvalidStateError.
     * - If sender.transport is unset, reject p with InvalidStateError.
     * - Let withParameters be the argument to this method.
     * - If withParameters is invalid due to SSRC misusage or other reasons, reject p with InvalidParameters.
     * - If rtcpTransport is not set and withParameters.rtcp.mux is set to false, reject p with InvalidParameters.
     * - For each value of i from 0 to the number of codecs, check that each value of withParameters.codecs[i].payloadType is set. If any value is unset, reject p with InvalidParameters.
     * - For each value of i from 0 to the number of encodings, check whether withParameters.encodings[i].codecPayloadType (if set) corresponds to a value of withParameters.codecs[j].payloadType where j goes from 0 to the number of codecs. If there is no correspondence, or if withParameters.codecs[j].name is equal to "red", "cn", "telephone-event", "rtx" or a forward error correction codec ("ulpfec" [RFC5109] or "flexfec" [FLEXFEC]), reject p with InvalidParameters.
     * - For each value of i from 0 to the number of codecs, check that each value of withParameters.codecs[i].name not equal to "red", "rtx" or a forward error correction codec ("ulpfec" [RFC5109] or "flexfec" [FLEXFEC]), is included in getCapabilities(track.kind).codecs[j].name, where j goes from 0 to the number of codecs. If a match is not found for any value of i, reject p with InvalidParameters.
     * - Run the following steps:
     * - - If send() is called for the first time, start sending. If send() was called previously, have the sender switch to sending using withParameters.
     * - - Resolve p with undefined.
     */
    RTCRtpSender.prototype.send = function (parameters) {
        return undefined;
    };
    /**
     * The stop method irreversibly stops the RTCRtpSender. When stop called, the following steps must be run:
     * - Let sender be the RTCRtpSender on which stop is invoked.
     * - If sender.stop has previously been called, abort these steps.
     * - Let parameters be the argument provided to sender.send(parameters) the last time it was invoked.
     * - Stop sending media with sender.
     * - Send an RTCP BYE for each SSRC in parameters.encodings[i].ssrc, parameters.encodings[i].fec.ssrc and parameters.encodings[i].rtx.ssrc where i goes from 0 to encodings.length-1.
     */
    RTCRtpSender.prototype.stop = function () {
    };
    /**
     * Gathers stats for the given object and reports the result asynchronously.
     * If the object has not yet begun to send or receive data,
     * the returned stats will reflect this.
     * If the object is in the closed state,
     * the returned stats will reflect the stats at the time the object transitioned to the closed state.
     *
     * When the getStats method is invoked, the user agent must queue a task to run the following steps:
     * - Let p be a new promise.
     * - Return, but continue the following steps in the background.
     * - Start gathering the stats.
     * - When the relevant stats have been gathered, return a new RTCStatsReport object,
     *   representing the gathered stats.
     */
    RTCRtpSender.prototype.getStats = function () {
        return undefined;
    };
    return RTCRtpSender;
}(Stream.Duplex));
exports.RTCRtpSender = RTCRtpSender;
;
;
;
/**
 * The RTCRtpReceiver includes information relating to the RTP receiver.
 *
 * A RTCRtpReceiver instance is constructed from a value of kind and an RTCDtlsTransport object.
 * If an attempt is made to construct an RTCRtpReceiver object with transport.state or rtcpTransport.state with a value of closed,
 * throw an InvalidStateError exception. Upon construction, track is set,
 * and the value of track.kind is determined based on the value of kind passed in the constructor.
 *
 * An RTCRtpReceiver object can be garbage-collected once stop() is called and it is no longer referenced.
 */
var RTCRtpReceiver = (function (_super) {
    __extends(RTCRtpReceiver, _super);
    /**
     * An RTCRtpReceiver instance produces an associated receiving MediaStreamTrack and provides RTC related methods to it.
     */
    function RTCRtpReceiver(kind, transport, rtcpTransport) {
        return _super.call(this) || this;
    }
    Object.defineProperty(RTCRtpReceiver.prototype, "track", {
        /**
         * The track attribute is the MediaStreamTrack instance that is associated with this RTCRtpReceiver object receiver.
         * Upon construction, track is set. The value of track.kind is set to the value of kind passed in the constructor.
         *
         * When one of the SSRCs in parameters.encodings[i].ssrc
         * (where i goes from 0 to encodings.length-1) is removed from the source identifier table described in [RFC3550]
         * Section 8.2 (either due to BYE reception or timeout) the mute event is fired at track.
         * If and when media packets are received again for receiver, the unmute event is fired at track.
         *
         * NOTE:
         * - Prior to verification of the remote DTLS fingerprint within the RTCDtlsTransport transport, and (if set) rtcpTransport,
         * track must not emit media for rendering.
         */
        get: function () {
            return this._track;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCRtpReceiver.prototype, "transport", {
        /**
         * The associated RTP RTCDtlsTransport instance.
         */
        get: function () {
            return this._transport;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCRtpReceiver.prototype, "rtcpTransport", {
        /**
         * The RTCDtlsTransport instance over which RTCP is sent and received.
         * When BUNDLE is used,
         * multiple RTCRtpReceiver objects will share one rtcpTransport and will send and receive RTCP over the same RTCDtlsTransport.
         * When RTCP mux is used, rtcpTransport will be null, and both RTP and RTCP traffic will flow over the RTCDtlsTransport transport.
         */
        get: function () {
            return this._rtcpTransport;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * The setTransport() method attempts to replace the the RTP RTCDtlsTransport transport (and if used) RTCP RTCDtlsTransport
     * rtcpTransport with the transport(s) provided.
     *
     * When the setTransport() method is invoked, the user agent must run the following steps:
     * - Let receiver be the RTCRtpReceiver object on which setTransport() is invoked.
     * - If receiver.stop() has been called, throw an InvalidStateError exception.
     * - Let withTransport and withRtcpTransport be the arguments to this method.
     * - If setTransport() is called with no arguments, or if withTransport is unset, throw an InvalidParameters exception.
     * - If withTransport.transport.component is rtcp, throw an InvalidParameters exception.
     * - If withRtcpTransport is set and withRtcpTransport.transport.component is rtp, throw an InvalidParameters exception.
     * - If withTransport is set and withTransport.state is closed, throw an InvalidStateError exception.
     * - If withRtcpTransport is set and withRtcpTransport.state is closed, throw an InvalidStateError exception.
     * - Replace transport with withTransport and rtcpTransport (if set) with withRtcpTransport and seamlessly receive over the new transport(s).
     */
    RTCRtpReceiver.prototype.setTransport = function (transport, rtcpTransport) {
    };
    /**
     * getCapabilities() obtains the receiver capabilities, based on kind.
     * Browsers must support kind values of "audio" and "video".
     * If there are no capabilities corresponding to the value of kind, getCapabilities returns null.
     * Capabilities that can apply to multiple values of kind (such as retransmission [RFC4588],
     * redundancy [RFC2198] and Forward Error Correction) have RTCRtpCapabilities.RTCRtpCodecCapability[i].kind set to the value
     * of the kind argument.
     */
    RTCRtpReceiver.getCapabilities = function (kind) {
        return undefined;
    };
    /**
     * Attempts to set the parameters controlling the receiving of media.
     *
     * When the receive() method is invoked, the user agent must run the following steps:
     * - Let receiver be the RTCRtpReceiver object on which receive() is invoked.
     * - Let p be a new promise.
     * - If receiver.stop() has been called, reject p with InvalidStateError.
     * - Let withParameters be the argument to this method.
     * - If withParameters is invalid due to SSRC misusage, ssrc_table, muxId_table or pt_table conflicts described in Section 8.3 or other reasons, reject p with InvalidParameters.
     * - If rtcpTransport is not set and withParameters.rtcp.mux is set to false, reject p with InvalidParameters.
     * - For each value of i from 0 to the number of codecs, check that each value of withParameters.codecs[i].payloadType is set. If any value is unset, reject p with InvalidParameters.
     * - For each value of i from 0 to the number of encodings, check whether withParameters.encodings[i].codecPayloadType corresponds to a value of withParameters.codecs[j].payloadType where j goes from 0 to the number of codecs. If there is no correspondence, or if withParameters.codecs[j].name is equal to "red", "cn", "telephone-event", "rtx" or a forward error correction codec ("ulpfec" [RFC5109] or "flexfec" [FLEXFEC]), reject p with InvalidParameters.
     * - For each value of i from 0 to the number of codecs, check that each value of withParameters.codecs[i].name not equal to "red", "rtx" or a forward error correction codec is included in getCapabilities(kind).codecs[j].name, where j goes from 0 to the number of codecs, and kind takes the value passed in the RTCRtpReceiver constructor. If no match is found for any value of i, reject p with InvalidParameters.
     * - Run the following steps:
     * - - If receive() is called for the first time, start receiving. If receive() was called previously, have the receiver switch to receiving using withParameters.
     * - - Resolve p with undefined.
     */
    RTCRtpReceiver.prototype.receive = function (parameters) {
        return undefined;
    };
    /**
     * Returns an RTCRtpContributingSource for each unique CSRC identifier received by this RTCRtpReceiver.
     * The browser must keep information from RTP packets received in the last 10 seconds.
     * If no contributing sources are available, an empty list is returned.
     */
    RTCRtpReceiver.prototype.getContributingSources = function () {
        return undefined;
    };
    /**
     * Returns an RTCRtpSynchronizationSource for each unique SSRC identifier received by this RTCRtpReceiver in the last 10 seconds.
     */
    RTCRtpReceiver.prototype.getSynchronizationSources = function () {
        return undefined;
    };
    /**
     * Stops the RTCRtpReceiver receiver. receiver.stop() is final like receiver.track.stop().
     * receiver.track.stop() does not affect track clones and also does not stop receiver so that Receiver Reports continue to be sent.
     * Calling receiver.stop() does not cause the "onended" event to fire for track.
     */
    RTCRtpReceiver.prototype.stop = function () {
    };
    /**
     * Gathers stats for the given object and reports the result asynchronously.
     * If the object has not yet begun to send or receive data,
     * the returned stats will reflect this.
     * If the object is in the closed state,
     * the returned stats will reflect the stats at the time the object transitioned to the closed state.
     *
     * When the getStats method is invoked, the user agent must queue a task to run the following steps:
     * - Let p be a new promise.
     * - Return, but continue the following steps in the background.
     * - Start gathering the stats.
     * - When the relevant stats have been gathered, return a new RTCStatsReport object,
     *   representing the gathered stats.
     */
    RTCRtpReceiver.prototype.getStats = function () {
        return undefined;
    };
    return RTCRtpReceiver;
}(Stream.Duplex));
exports.RTCRtpReceiver = RTCRtpReceiver;
;
;
/**
 * The RTCDataChannelState provides information on the state of the data channel.
 */
var RTCDataChannelState;
(function (RTCDataChannelState) {
    /**
     * The user agent is attempting to establish the underlying data transport. This is the initial state of an RTCDataChannel object.
     */
    RTCDataChannelState[RTCDataChannelState["connecting"] = 0] = "connecting";
    /**
     * The underlying data transport is established and communication is possible. This is the initial state of an RTCDataChannel object dispatched as a part of an RTCDataChannelEvent.
     */
    RTCDataChannelState[RTCDataChannelState["open"] = 1] = "open";
    /**
     * The procedure to close down the underlying data transport has started.
     */
    RTCDataChannelState[RTCDataChannelState["closing"] = 2] = "closing";
    /**
     * The underlying data transport has been closed or could not be established.
     */
    RTCDataChannelState[RTCDataChannelState["closed"] = 3] = "closed";
})(RTCDataChannelState = exports.RTCDataChannelState || (exports.RTCDataChannelState = {}));
;
;
/**
 * An RTCDataChannel object allows sending data messages to/from the remote peer.
 *
 * The RTCDataChannel interface represents a bi-directional data channel between two peers.
 * There are two ways to establish a connection with RTCDataChannel.
 * The first way is to construct an RTCDataChannel at one of the peers with the RTCDataChannelParameters.negotiated
 * attribute unset or set to its default value false.
 * This will announce the new channel in-band and trigger an ondatachannel event with the corresponding RTCDataChannel object
 * at the other peer. The second way is to let the application negotiate the RTCDataChannel.
 * To do this, create an RTCDataChannel object with the RTCDataChannelParameters.negotiated dictionary member set to true,
 * and signal out-of-band (e.g. via a web server) to the other side that it should create a corresponding RTCDataChannel
 * with the RTCDataChannelParameters.negotiated dictionary member set to true and the same id.
 * This will connect the two separately created RTCDataChannel objects.
 * The second way makes it possible to create channels with asymmetric properties and to create channels in a declarative
 * way by specifying matching ids. Each RTCDataChannel has an associated underlying data transport that is used to transport
 * actual data to the other peer. The transport properties of the underlying data transport,
 * such as in order delivery settings and reliability mode, are configured by the peer as the channel is created.
 * The properties of a channel cannot change after the channel has been created.
 */
var RTCDataChannel = (function (_super) {
    __extends(RTCDataChannel, _super);
    /*
     * onopen() => emit('open')
     * onbufferedamountlow() => emit('bufferedamountlow')
     * onerror(event: Error) => emit('error')
     * onclose() => emit('close')
     * onmessage(event: ArrayBuffer) => emit('message')
     */
    /**
     * An RTCDataChannel object is constructed from a RTCDataTransport object (providing the transport for the data channel)
     * and an RTCDataChannelParameters object. If parameters is invalid, throw an InvalidParameters exception.
     * If transport.state is closed, throw an InvalidState exception.
     *
     * An RTCDataChannel object can be garbage-collected once readyState is closed and it is no longer referenced.
     */
    function RTCDataChannel(transport, parameters) {
        return _super.call(this) || this;
    }
    Object.defineProperty(RTCDataChannel.prototype, "readyState", {
        /**
         * The readyState attribute represents the state of the RTCDataChannel object.
         * It must return the value to which the user agent last set it (as defined by the processing model algorithms).
         */
        get: function () {
            return this._state;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCDataChannel.prototype, "transport", {
        /**
         * The readonly attribute referring to the related transport object.
         */
        get: function () {
            return this._transport;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCDataChannel.prototype, "bufferedAmount", {
        /**
         * The bufferedAmount attribute must return the number of bytes of application data (UTF-8 text and binary data)
         * that have been queued using send() but that, as of the last time the event loop started executing a task,
         * had not yet been transmitted to the network. This includes any text sent during the execution of the current task,
         * regardless of whether the user agent is able to transmit text asynchronously with script execution.
         * This does not include framing overhead incurred by the protocol, or buffering done by the operating system or network hardware.
         * If the channel is closed, this attribute's value will only increase with each call to the send()
         * method (the attribute does not reset to zero once the channel closes).
         */
        get: function () {
            return this._bufferedAmount;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Returns the parameters applying to this data channel.
     */
    RTCDataChannel.prototype.getParameters = function () {
        return undefined;
    };
    /**
     * Closes the RTCDataChannel.
     * It may be called regardless of whether the RTCDataChannel object was created by this peer or the remote peer.
     * When the close() method is called, the user agent must run the following steps:
     * - Let channel be the RTCDataChannel object which is about to be closed.
     * - If channel's readyState is closing or closed, then abort these steps.
     * - Set channel's readyState attribute to closing.
     * - If the closing procedure has not started yet, start it.
     */
    RTCDataChannel.prototype.close = function () {
    };
    /**
     * Run the steps described by the send() algorithm with argument type string object.
     */
    RTCDataChannel.prototype.send = function (data) {
    };
    return RTCDataChannel;
}(Stream.Duplex));
exports.RTCDataChannel = RTCDataChannel;
;
/**
 * RTCSctpTransportState indicates the state of the SCTP transport.
 */
var RTCSctpTransportState;
(function (RTCSctpTransportState) {
    /**
     * The RTCSctpTransport object has been constructed but the start method has not been called so the RTCSctpTransport has not started negotiating yet.
     */
    RTCSctpTransportState[RTCSctpTransportState["new"] = 0] = "new";
    /**
     * The start method has been called and the RTCSctpTransport is in the process of negotiating an association.
     */
    RTCSctpTransportState[RTCSctpTransportState["connecting"] = 1] = "connecting";
    /**
     * The RTCSctpTransport has completed negotiation of an association.
     */
    RTCSctpTransportState[RTCSctpTransportState["connected"] = 2] = "connected";
    /**
     * The SCTP association has been closed intentionally via a call to stop or receipt of a SHUTDOWN or ABORT chunk.
     */
    RTCSctpTransportState[RTCSctpTransportState["closed"] = 3] = "closed";
})(RTCSctpTransportState = exports.RTCSctpTransportState || (exports.RTCSctpTransportState = {}));
;
;
/**
 * The RTCSctpTransport includes information relating to Stream Control Transmission Protocol (SCTP) transport.
 */
var RTCSctpTransport = (function (_super) {
    __extends(RTCSctpTransport, _super);
    /*
     * ondatachannel() => emit('datachannel')
     * onstatechange() => emit('statechange')
     */
    /**
     * An RTCSctpTransport inherits from an RTCDataTransport object, which is associated to an RTCDataChannel object.
     *
     * An RTCSctpTransport is constructed from an RTCDtlsTransport object,
     * and optionally a port number (with a default of 5000, or the next unused port).
     * If a port already in use is provided in the constructor, throw an InvalidParameters exception.
     * An RTCSctpTransport object can be garbage-collected once stop() is called and it is no longer referenced.
     */
    function RTCSctpTransport(transport, port) {
        return _super.call(this) || this;
    }
    Object.defineProperty(RTCSctpTransport.prototype, "transport", {
        /**
         * The RTCDtlsTransport instance the RTCSctpTransport object is sending over.
         */
        get: function () {
            return this._transport;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCSctpTransport.prototype, "state", {
        /**
         * The current state of the SCTP transport.
         */
        get: function () {
            return this._state;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RTCSctpTransport.prototype, "port", {
        /**
         * The local SCTP port number used by the data channel.
         */
        get: function () {
            return this._port;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * getCapabilities() retrieves the RTCSctpCapabilities of the RTCSctpTransport.
     */
    RTCSctpTransport.getCapabilities = function () {
        return undefined;
    };
    /**
     * Starts the RTCSctpTransport instance and causes an SCTP INIT request to be issued over the RTCDtlsTransport
     * from the local RTCSctpTransport to the remote RTCSctpTransport
     * (causing the RTCSctpTransportState to transition to to connecting),
     * where the remote RTCSctpTransport responds with an SCTP INIT-ACK.
     * Since both local and remote parties must mutually create an RTCSctpTransport,
     * SCTP SO (Simultaneous Open) is used to establish a connection over SCTP.
     * If RTCSctpTransportState is not new throw an InvalidStateError exception.
     * If remotePort is not provided, a default value of port is assumed. If the remote port is in use, throw an InvalidParameters exception.
     */
    RTCSctpTransport.prototype.start = function (remoteCaps, remotePort) {
    };
    /**
     * Stops the RTCSctpTransport instance.
     */
    RTCSctpTransport.prototype.stop = function () {
    };
    /**
     * Gathers stats for the given object and reports the result asynchronously.
     * If the object has not yet begun to send or receive data,
     * the returned stats will reflect this.
     * If the object is in the closed state,
     * the returned stats will reflect the stats at the time the object transitioned to the closed state.
     *
     * When the getStats method is invoked, the user agent must queue a task to run the following steps:
     * - Let p be a new promise.
     * - Return, but continue the following steps in the background.
     * - Start gathering the stats.
     * - When the relevant stats have been gathered, return a new RTCStatsReport object,
     *   representing the gathered stats.
     */
    RTCSctpTransport.prototype.getStats = function () {
        return undefined;
    };
    return RTCSctpTransport;
}(Stream.Transform));
exports.RTCSctpTransport = RTCSctpTransport;
;
//# sourceMappingURL=ortc.js.map