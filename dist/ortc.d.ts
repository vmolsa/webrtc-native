/// <reference types="node" />
import Stream = require('stream');
import Events = require('events');
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
export declare enum RTCStatsType {
    /**
     * Statistics for the inbound RTP stream.
     * It is accessed via the RTCInboundRTPStreamStats.
     * Local inbound RTP statistics can be obtained from the RTCRtpReceiver object;
     * remote inbound RTP statistics can be obtained from the RTCRtpSender object
    */
    "inboundrtp" = 0,
    /**
     * Statistics for the outbound RTP stream.
     * It is accessed via the RTCOutboundRTPStreamStats.
     * Local outbound RTP statistics can be obtained from the RTCRtpSender object;
     * remote outbound RTP statistics can be obtained from the RTCRtpReceiver object.
     */
    "outboundrtp" = 1,
    /**
     * Statistics relating to each RTCDataChannel id.
     * It is accessed via the RTCDataChannelStats.
     */
    "datachannel" = 2,
    /**
     * Statistics relating to the MediaStreamTrack object.
     * It is accessed via the RTCMediaStreamTrackStats.
     */
    "track" = 3,
    /**
     * Transport statistics related to the RTCDtlsTransport object.
     * It is accessed via the RTCTransportStats.
     */
    "transport" = 4,
    /**
     * ICE candidate pair statistics related to RTCIceTransport objects.
     * It is accessed via the RTCIceCandidatePairStats.
     */
    "candidatepair" = 5,
    /**
     * ICE local candidate statistics, related to RTCIceGatherer objects.
     * It is accessed via the RTCIceCandidateStats for the local candidate.
     */
    "localcandidate" = 6,
    /**
     * ICE remote candidate statistics, related to RTCIceTransport objects.
     * It is accessed via the RTCIceCandidateStats for the remote candidate.
     */
    "remotecandidate" = 7,
    /**
     * Information about a certificate used by an RTCDtlsTransport object.
     * It is accessed via the RTCCertificateStats
     */
    "certificate" = 8,
}
/** The RTCIceProtocol includes the protocol of the ICE candidate */
export declare enum RTCIceProtocol {
    /**
     * A UDP candidate, as described in [RFC5245]
     */
    "udp" = 0,
    /**
     * A TCP candidate, as described in [RFC6544]
     */
    "tcp" = 1,
}
/**
 * The RTCIceTcpCandidateType includes the type of the ICE TCP candidate, as described in [RFC6544].
 * Browsers must gather active TCP candidates and only active TCP candidates.
 * Servers and other endpoints may gather active, passive or so candidates.
 */
export declare enum RTCIceTcpCandidateType {
    /**
     * An active TCP candidate is one for which the transport will attempt to open an outbound
     * connection but will not receive incoming connection requests.
     */
    "active" = 0,
    /**
     * A passive TCP candidate is one for which the transport will receive incoming
     * connection attempts but not attempt a connection.
     */
    "passive" = 1,
    /**
     * An so candidate is one for which the transport will attempt to open a connection
     * simultaneously with its peer.
     */
    "so" = 2,
}
/** The RTCIceCandidateType includes the type of the ICE candidate as defined in [RFC5245] section 15.1. */
export declare enum RTCIceCandidateType {
    /**
     * A host candidate, as defined in Section 4.1.1.1 of [RFC5245].
     */
    "host" = 0,
    /**
     * A server reflexive candidate, as defined in Section 4.1.1.2 of [RFC5245].
     */
    "srflx" = 1,
    /**
     * A peer reflexive candidate, as defined in Section 4.1.1.2 of [RFC5245].
     */
    "prflx" = 2,
    /**
     * A relay candidate, as defined in Section 7.1.3.2.1 of [RFC5245].
     */
    "relay" = 3,
}
/**
 * RTCIceComponent contains the component-id of the RTCIceTransport,
 * which will be rtp unless RTP and RTCP are not multiplexed and the RTCIceTransport
 * object was returned by createAssociatedTransport().
 */
export declare enum RTCIceComponent {
    /**
     * The RTP component ID, defined (as '1') in [RFC5245] Section 4.1.1.1.
     * Protocols multiplexed with RTP (e.g. SCTP data channel) share its component ID.
     */
    "RTP" = 0,
    /**
     * The RTCP component ID, defined (as '2') in [RFC5245] Section 4.1.1.1.
     */
    "RTCP" = 1,
}
/** RTCIceGathererState represents the current state of the ICE gatherer. */
export declare enum RTCIceGathererState {
    /**
     * The object has been created but gather() has not been called.
     */
    "new" = 0,
    /**
     * gather() has been called, and the RTCIceGatherer is in the process of gathering candidates
     * (which includes adding new candidates and removing invalidated candidates).
     */
    "gathering" = 1,
    /**
     * The RTCIceGatherer has completed gathering.
     * Events such as adding, updating or removing an interface, or adding, changing or
     * removing a TURN server will cause the state to go back to gathering
     * before re-entering complete once all candidate changes are finalized.
     */
    "complete" = 2,
    /**
     * The closed state can only be entered when the RTCIceGatherer has been closed
     * intentionally by calling close(). This is a terminal state.
     */
    "closed" = 3,
}
/** RTCIceGatherPolicy denotes the policy relating to the gathering of ICE candidates. */
export declare enum RTCIceGatherPolicy {
    /**
     * The RTCIceGatherer gathers all types of candidates when this value is specified.
     * This will not include addresses that have been filtered by the browser.
     */
    "all" = 0,
    /** The RTCIceGatherer gathers all ICE candidate types except for host candidates. */
    "nohost" = 1,
    /**
     * The RTCIceGatherer must only gather media relay candidates
     * such as candidates passing through a TURN server.
     * This can be used to reduce leakage of IP addresses in certain use cases.
     */
    "relay" = 2,
}
/**
 * RTCIceCredentialType represents the type of credential used by a TURN server.
 */
export declare enum RTCIceCredentialType {
    /**
     * The credential is a long-term authentication password, as described in [RFC5389], Section 10.2.
     */
    "password" = 0,
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
    "oauth" = 1,
}
/** RTCIceRole contains the current role of the ICE transport. */
export declare enum RTCIceRole {
    /** controlling state */
    "controlling" = 0,
    /** controlled state */
    "controlled" = 1,
}
/** RTCIceTransportState represents the current state of the ICE transport. */
export declare enum RTCIceTransportState {
    /**
     * The RTCIceTransport object is waiting for remote candidates to be supplied.
     * In this state the RTCIceTransport object can respond to incoming connectivity checks.
     */
    "new" = 0,
    /**
     * The RTCIceTransport has received at least one remote candidate,
     * and a local and remote RTCIceCandidateComplete dictionary
     * was not added as the last candidate.
     * In this state the RTCIceTransport is checking candidate
     * pairs but has not yet found a successful candidate pair,
     * or consent checks [RFC7675] have failed on all previously successful candidate pairs.
     */
    "checking" = 1,
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
    "connected" = 2,
    /**
     * A local and remote RTCIceCandidateComplete dictionary was added as the last candidate to the
     * RTCIceTransport and all appropriate candidate pairs have been tested and at least one functioning
     * candidate pair has been found. If consent checks [RFC7675] subsequently fail on
     * all successful candidate pairs, the state transitions to failed.
     * */
    "completed" = 3,
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
    "disconnected" = 4,
    /**
     * A local and remote RTCIceCandidateComplete dictionary was added as the last candidate
     * to the RTCIceTransport and all appropriate candidate pairs have either failed
     * connectivity checks or have lost consent.
     */
    "failed" = 5,
    /** The RTCIceTransport has shut down and is no longer responding to STUN requests. */
    "closed" = 6,
}
/**
 * An RTCStats dictionary represents the stats gathered by inspecting a specific object.
 * The RTCStats dictionary is a base type that specifies as set of default attributes,
 * such as timestamp and type. Specific stats are added by extending the RTCStats dictionary.
 *
 * Note that while stats names are standardized,
 * any given implementation may be using experimental values or values not yet known to the Web application.
 * Thus, applications must be prepared to deal with unknown stats.
 *
 * Statistics need to be synchronized with each other in order to yield reasonable values in computation;
 * for instance, if "bytesSent" and "packetsSent" are both reported,
 * they both need to be reported over the same interval,
 * so that "average packet size" can be computed as "bytes / packets"
 * - if the intervals are different, this will yield errors.
 *   Thus implementations must return synchronized values for all stats in an RTCStats dictionary.
 */
export interface RTCStats {
    /**
     * A unique id that is associated with the object that was inspected to produce this RTCStats object.
     * Two RTCStats objects, extracted from two different RTCStatsReport objects,
     * must have the same id if they were produced by inspecting the same underlying object.
     * User agents are free to pick any format for the id as long as it meets the requirements above.
     */
    id?: string;
    /**
     * The timestamp, of type DOMHighResTimeStamp [HIGHRES-TIME],
     * associated with this object. The time is relative to the UNIX epoch (Jan 1, 1970, UTC).
     * The timestamp for local measurements corresponds to the local clock and for remote
     * measurements corresponds to the timestamp indicated in the incoming RTCP Sender Report (SR),
     * Receiver Report (RR) or Extended Report (XR).
     */
    timestamp?: number;
    /**
     * The type of this object.
     * The type attribute must be initialized to the name of the most specific
     * type this RTCStats dictionary represents.
     */
    type?: RTCStatsType;
}
/**
 * The getStats() method delivers a successful result in the form of a RTCStatsReport object.
 * An RTCStatsReport represents a map between strings,
 * identifying the inspected objects (RTCStats.id), and their corresponding RTCStats objects.
 *
 * An RTCStatsReport may be composed of several RTCStats objects,
 * each reporting stats for one underlying object.
 * One achieves the total for the object by summing over all stats of a certain type; for instance,
 * if an RTCRtpSender object is sending RTP streams involving multiple SSRCs over the network,
 * the RTCStatsReport may contain one RTCStats object per SSRC
 * (which can be distinguished by the value of the ssrc stats attribute).
 */
export declare type RTCStatsReport = {
    [key: string]: RTCStats;
};
/**
 * The RTCStatsProvider interface enables the retrieval of statistics
 */
export interface RTCStatsProvider {
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
    getStats(): Promise<RTCStatsReport>;
}
/**
 * The RTCIceCandidate dictionary includes information relating to an ICE candidate.
 */
export interface RTCIceCandidate {
    /**
     * A unique identifier that allows ICE to correlate candidates that appear on multiple RTCIceTransports.
     */
    foundation: string;
    /**
     * The IP address of the candidate.
     */
    ip: string;
    /**
     * The port for the candidate.
     */
    port: number;
    /**
     * The assigned priority of the candidate. This is automatically populated by the browser.
     */
    priority: number;
    /**
     * The protocol of the candidate (udp/tcp).
     */
    protocol: RTCIceProtocol;
    /**
     * For candidates that are derived from others,
     * such as relay or reflexive candidates,
     * the relatedAddress refers to the candidate that these are derived from.
     * For host candidates, the relatedAddress is unset.
     */
    relatedAddress?: string;
    /**
     * For candidates that are derived from others,
     * such as relay or reflexive candidates,
     * the relatedPort refers to the host candidate that these are derived from.
     * For host candidates, the relatedPort is unset.
     */
    relatedPort?: number;
    /**
     * The type of TCP candidate. For UDP candidates, this attribute is unset.
     */
    tcpType?: RTCIceTcpCandidateType;
    /**
     * The type of candidate.
     */
    type?: RTCIceCandidateType;
}
/**
 * RTCIceGatherOptions provides options relating to the gathering of ICE candidates.
 */
export interface RTCIceGatherOptions {
    /**
     * The ICE gather policy.
     */
    gatherPolicy: RTCIceGatherPolicy;
    /**
     * Additional ICE servers to be configured.
     * Since implementations may provide default ICE servers,
     * and applications can desire to restrict communications to the local LAN, iceServers need not be set.
     */
    iceservers: RTCIceServer[];
}
/**
 * RTCIceCandidateComplete is a dictionary signifying that all RTCIceCandidates are gathered.
 */
export interface RTCIceCandidateComplete {
    /**
     * This attribute is always present and set to true, indicating that ICE candidate gathering is complete.
     */
    readonly complete: boolean;
}
/**
 * The RTCIceParameters dictionary includes the ICE username fragment and password and other ICE-related parameters.
 */
export interface RTCIceParameters {
    /**
     * If only ICE-lite is supported (true) or not (false or unset).
     * Since [RTCWEB-TRANSPORT] Section 3.4 requires browser support for full ICE,
     * iceLite will only be true for a remote peer such as a gateway.
     * getLocalParameters().iceLite must not be set.
     */
    iceLite: boolean;
    /**
     * ICE password.
     */
    password: string;
    /**
     * ICE username fragment.
     */
    usernameFragment: string;
}
/**
 * The icecandidateerror event of the RTCIceGatherer object uses the RTCIceGathererIceErrorEvent interface.
 */
export interface RTCIceGathererIceErrorEvent {
    /**
     * The RTCIceCandidate used to communicate with the STUN or TURN server.
     * On a multihomed system, multiple interfaces may be used to contact the server,
     * and this attribute allows the application to figure out on which one the failure occurred.
     * If the browser is in a privacy mode disallowing host candidates, this attribute will be null.
     */
    readonly hostCandidate: RTCIceCandidate;
    /**
     * The url attribute is the STUN or TURN URL identifying the server on which the failure ocurred.
     */
    readonly url: string;
    /**
     * The errorCode attribute is the numeric STUN error code returned by the STUN or TURN server [STUN-PARAMETERS].
     *
     * If no host candidate can reach the server, errorCode will be set to a value of 701,
     * as this does not conflict with the STUN error code range, and hostCandidate will be null.
     * This error is only fired once per server URL while in the RTCIceGathererState of gathering.
     */
    readonly errorCode: number;
    /**
     * The errorText attribute is the STUN reason text returned by the STUN or TURN server [STUN-PARAMETERS].
     *
     * If the server could not be reached, errorText will be set to an implementation-specific value providing details about the error.
     */
    readonly errorText: string;
}
/**
 * The RTCIceGatherCandidate provides either an RTCIceCandidate or an RTCIceCandidateComplete
 * indication that candidate gathering is complete.
 */
export declare type RTCIceGatherCandidate = RTCIceCandidate | RTCIceCandidateComplete;
/**
 * The icecandidate event of the RTCIceGatherer object uses the RTCIceGathererEvent interface.
 *
 * Firing an RTCIceGathererEvent event named e with an RTCIceGatherCandidate candidate and URL url means that an event with the name e,
 * which does not bubble (except where otherwise stated) and is not cancelable (except where otherwise stated),
 * and which uses the RTCIceGathererEvent interface with the candidate attribute set to the new ICE candidate,
 * must be created and dispatched at the given target.
 */
export interface RTCIceGathererEvent {
    /**
     * The candidate attribute is the RTCIceGatherCandidate object with the new ICE candidate that caused the event.
     * If candidate is of type RTCIceCandidateComplete, there are no additional candidates.
     */
    readonly candidate: RTCIceGatherCandidate;
    /**
     * The URL of the server from which the candidate was obtained.
     */
    readonly url: string;
}
/**
 * The RTCOAuthCredential dictionary is used to describe the OAuth auth credential
 * information which is used by the STUN/TURN client (inside the RTCIceGatherer) to authenticate against a STUN/TURN server,
 * as described in [RFC7635]. Note that the kid parameter is not located in this dictionary, but in RTCIceServer's username member.
 */
export interface RTCOAuthCredential {
    /**
     * The "mac_key", as described in [RFC7635], Section 6.2, in a base64-url encoded format.
     * It is used in STUN message integrity hash calculation (as the password is used in password based authentication).
     * Note that the OAuth response "key" parameter is a JSON Web Key (JWK) or a JWK encrypted with a JWE format.
     * Also note that this is the only OAuth parameter whose value is not used directly,
     * but must be extracted from the "k" parameter value from the JWK, which contains the needed base64-encoded "mac_key".
     */
    macKey: string;
    /**
     * The "access_token", as described in [RFC7635], Section 6.2, in a base64-encoded format.
     * This is an encrypted self-contained token that is opaque to the application.
     * Authenticated encryption is used for message encryption and integrity protection.
     * The access token contains a non-encrypted nonce value,
     * which is used by the Authorization Server for unique mac_key generation.
     * The second part of the token is protected by Authenticated Encryption.
     * It contains the mac_key, a timestamp and a lifetime.
     * The timestamp combined with lifetime provides expiry information;
     * this information describes the time window during which the token credential is valid and accepted by the TURN server.
     */
    accessToken: string;
}
/**
 * The RTCIceServer dictionary is used to configure the STUN and/or TURN servers. In network topologies with multiple layers of NATs,
 * it is desirable to have a STUN server between every layer of NATs in addition to
 * the TURN servers to minimize the peer to peer network latency.
 */
export interface RTCIceServer {
    /**
     * STUN or TURN URI(s) as defined in [RFC7064] and [RFC7065] or other URI types.
     */
    urls: string | string[];
    /**
     * If this RTCIceServer object represents a TURN server, then this attribute specifies the username to use with that TURN server.
     */
    username?: string;
    /**
     * If this RTCIceServer object represents a TURN server, then this attribute specifies the credential to use with that TURN server.
     *
     * If credentialType is "password", credential is a DOMString, and represents a long-term authentication password,
     * as described in [RFC5389], Section 10.2.
     *
     * If credentialType is "oauth", credential is a RTCOAuthCredential, which contains the OAuth access token and MAC key.
     */
    credential?: string | RTCOAuthCredential;
    /**
     * If this RTCIceServer object represents a TURN Server, then this attribute specifies how
     * credential should be used when that TURN server requests authorization.
     */
    credentialType?: RTCIceCredentialType;
}
/**
 * The RTCIceCandidatePair contains the currently selected ICE candidate pair.
 */
export interface RTCIceCandidatePair {
    /**
     * The local ICE candidate.
     */
    local: RTCIceCandidate;
    /**
     * The remote ICE candidate
     */
    remote: RTCIceCandidate;
}
/**
 * The icecandidatepairchange event of the RTCIceTransport object uses the RTCIceCandidatePairChangedEvent interface.
 * Firing an RTCIceCandidatePairChangedEvent event named e with an RTCIceCandidatePair pair means that an event with the name e,
 * which does not bubble (except where otherwise stated) and is not cancelable (except where otherwise stated),
 * and which uses the RTCIceCandidatePairChangedEvent interface with pair set to the selected RTCIceCandidatePair,
 * must be created and dispatched at the given target.
 */
export interface RTCIceCandidatePairChangedEvent {
    /**
     * The pair attribute is the selected RTCIceCandidatePair that caused the event.
     */
    readonly pair: RTCIceCandidatePair;
}
/**
 * The RTCIceGatherer gathers local host,
 * server reflexive and relay candidates,
 * as well as enabling the retrieval of local Interactive Connectivity Establishment (ICE) parameters which can be exchanged in signaling.
 * By enabling an endpoint to use a set of local candidates to construct multiple RTCIceTransport objects, the RTCIceGatherer enables support for scenarios such as parallel forking
 */
export declare class RTCIceGatherer extends Events.EventEmitter implements RTCStatsProvider {
    /**
     * An RTCIceGatherer instance is constructed from an RTCIceGatherOptions object.
     *
     * An RTCIceGatherer object in the closed state can be garbage-collected when it is no longer referenced.
     */
    constructor(options: RTCIceGatherOptions);
    private _component;
    private _state;
    /**
     * The component-id of the RTCIceGatherer object.
     * In RTCIceGatherer objects returned by createAssociatedGatherer() the value of the component attribute is rtcp.
     * In all other RTCIceGatherer objects, the value of the component attribute is rtp.
     */
    readonly component: RTCIceComponent;
    /**
     * The current state of the ICE gatherer.
     */
    readonly state: RTCIceGathererState;
    /**
     * Prunes all local candidates, and closes the port.
     * Associated RTCIceTransport objects transition to the disconnected state (unless they were in the failed state).
     * Calling close() when state is closed has no effect.
     */
    close(): void;
    /**
     * Gather ICE candidates.
     * If options is omitted, utilize the value of options passed in the constructor.
     * If state is closed, throw an InvalidStateError exception.
     */
    gather(options?: RTCIceGatherOptions): void;
    /**
     * getLocalParameters() obtains the ICE parameters of the RTCIceGatherer. If state is closed, throw an InvalidStateError exception.
     */
    getLocalParameters(): RTCIceParameters;
    /**
     * Retrieve the sequence of valid local candidates associated with the RTCIceGatherer.
     * This retrieves all unpruned local candidates currently known (except for peer reflexive candidates),
     * even if an onlocalcandidate event hasn't been processed yet.
     * Prior to calling gather() an empty list will be returned. If state is closed, throw an InvalidStateError exception.
     */
    getLocalCandidates(): RTCIceCandidate[];
    /**
     * Create an associated RTCIceGatherer for RTCP, with the same RTCIceParameters and RTCIceGatherOptions.
     * If state is closed, throw an InvalidStateError exception.
     * If an RTCIceGatherer calls the method more than once, or if component is rtcp, throw an InvalidStateError exception.
     */
    createAssociatedGatherer(): RTCIceGatherer;
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
    getStats(): Promise<RTCStatsReport>;
}
/**
 * The RTCIceTransport allows an application access to information about the Interactive Connectivity Establishment (ICE)
 * transport over which packets are sent and received. In particular,
 * ICE manages peer-to-peer connections which involve state which the application may want to access.
 */
export declare class RTCIceTransport extends Stream.Duplex implements RTCStatsProvider {
    /**
     * An RTCIceTransport instance is constructed (optionally) from an RTCIceGatherer.
     * If gatherer.state is closed or gatherer.component is rtcp, then throw an InvalidStateError exception.
     *
     * An RTCIceTransport object in the closed state can be garbage-collected when it is no longer referenced.
     */
    constructor(gatherer?: RTCIceGatherer);
    private _iceGatherer;
    private _role;
    private _component;
    private _state;
    /**
     * The iceGatherer attribute is set to the value of gatherer if passed in the constructor or in the latest call to start().
     */
    readonly iceGatherer: RTCIceGatherer;
    /**
     * The current role of the ICE transport.
     */
    readonly role: RTCIceRole;
    /**
     * The component-id of the RTCIceTransport object. In RTCIceTransport objects returned by createAssociatedTransport(),
     * the value of component is rtcp. In all other RTCIceTransport objects, the value of component is rtp.
     */
    readonly component: RTCIceComponent;
    /**
     * The current state of the ICE transport.
     */
    readonly state: RTCIceTransportState;
    /**
     * Retrieve the sequence of candidates associated with the remote RTCIceTransport.
     * Only returns the candidates previously added using setRemoteCandidates() or addRemoteCandidate().
     * If there are no remote candidates, an empty list is returned.
     */
    getRemoteCandidates(): RTCIceCandidate[];
    /**
     * Retrieves the selected candidate pair on which packets are sent.
     * If there is no selected pair yet, or consent [RFC7675] is lost on the selected pair, NULL is returned.
     */
    getSelectedCandidatePair(): RTCIceCandidatePair;
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
    start(gatherer: RTCIceGatherer, remoteParameters: RTCIceParameters, role?: RTCIceRole): void;
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
    stop(): void;
    /**
     * getRemoteParameters() obtains the current ICE parameters of the remote RTCIceTransport.
     */
    getRemoteParameters(): RTCIceParameters;
    /**
     * Create an associated RTCIceTransport for RTCP. If called more than once for the same component,
     * or if state is closed, throw an InvalidStateError exception. If called when component is rtcp,
     * throw an InvalidStateError exception.
     */
    createAssociatedTransport(): RTCIceTransport;
    /**
     * Add a remote candidate associated with the remote RTCIceTransport.
     * If state is closed, throw an InvalidStateError exception. When the remote RTCIceGatherer emits its final candidate,
     * addRemoteCandidate() should be called with an RTCIceCandidateComplete dictionary as an argument,
     * so that the local RTCIceTransport can know there are no more remote candidates expected, and can enter the completed state.
     */
    addRemoteCandidate(remoteCandidate: RTCIceGatherCandidate): void;
    /**
     * Set the sequence of candidates associated with the remote RTCIceTransport. If state is closed, throw an InvalidStateError exception.
     */
    setRemoteCandidates(remoteCandidates: RTCIceCandidate[]): void;
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
    getStats(): Promise<RTCStatsReport>;
}
/**
 * The Algorithm object is a inteface object which is used to specify an algorithm and any additional
 * parameters required to fully specify the desired operation.
 */
export interface Algorithm {
    /**
     * The name of the registered algorithm to use.
     */
    name: string;
}
export declare type AlgorithmIdentifier = string | Algorithm;
/**
 * The "ECDSA" algorithm identifier is used to perform signing and verification using the ECDSA algorithm specified in [RFC6090]
 * and using the SHA hash functions and elliptic curves defined in this specification.
 *
 * Other specifications may specify the use of additional elliptic curves and hash algorithms with ECDSA.
 * To specify additional hash algorithms to be used with ECDSA,
 * a specification must define a registered algorithm that supports the digest operation.
 * To specify an additional elliptic curve a specification must define the curve name,
 * ECDSA signature steps, ECDSA verification steps, ECDSA generation steps, ECDSA key import steps and ECDSA key export steps.
 */
export interface EcKeyGenParams extends Algorithm {
    /**
     * The NamedCurve type represents named elliptic curves,
     * which are a convenient way to specify the domain parameters of well-known elliptic curves.
     * The following values defined by this specification:
     *
     * - "P-256" NIST recommended curve P-256, also known as secp256r1.
     * - "P-384" NIST recommended curve P-384, also known as secp384r1.
     * - "P-521" NIST recommended curve P-521, also known as secp521r1.
     */
    namedCurve: string;
}
/**
 * The RTCDtlsFingerprint dictionary includes the hash function algorithm and certificate fingerprint as described in [RFC4572].
 */
export interface RTCDtlsFingerprint {
    /**
     * One of the the hash function algorithms defined in the 'Hash function Textual Names' registry, initially specified in [RFC4572] Section 8. As noted in [JSEP] Section 5.2.1, the digest algorithm used for the fingerprint matches that used in the certificate signature.
     */
    algorithm: string;
    value: string;
}
/**
 * The RTCCertificate interface enables the certificates used by an RTCDtlsTransport to be provided in the constructor.
 * This makes it possible to support forking, where the offerer will create multiple RTCDtlsTransport objects
 * using the same local certificate and fingerprint.
 */
export declare class RTCCertificate {
    private _expires;
    private _fingerprint;
    private _algorithm;
    /**
     * The expires attribute indicates the date and time in milliseconds relative to 1970-01-01T00:00:00Z
     * after which the certificate will be considered invalid by the browser.
     * After this time, attempts to construct an RTCDtlsTransport object using this certificate will fail.
     *
     * Note that this value might not be reflected in a notAfter parameter in the certificate itself.
     */
    readonly expires: number;
    /**
     * The fingerprint of the certificate.
     * As noted in [JSEP] Section 5.2.1, the digest algorithm used for the fingerprint matches that used in the certificate signature.
     */
    readonly fingerprint: RTCDtlsFingerprint;
    /**
     * Returns the value of keygenAlgorithm passed in the call to generateCertificate().
     */
    getAlgorithm(): AlgorithmIdentifier;
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
    static generateCertificate(keygenAlgorithm: AlgorithmIdentifier): Promise<RTCCertificate>;
}
/**
 * RTCDtlsTransportState indicates the state of the DTLS transport.
 */
export declare enum RTCDtlsTransportState {
    /**
     * The RTCDtlsTransport object has been created and has not started negotiating yet.
     */
    "new" = 0,
    /**
     * DTLS is in the process of negotiating a secure connection and verifying the remote fingerprint.
     * Once a secure connection is negotiated (but prior to verification of the remote fingerprint,
     * enabled by calling start()), incoming data can flow through (and media, once DTLS-SRTP key derivation is completed).
     */
    "connecting" = 1,
    /**
     * DTLS has completed negotiation of a secure connection and verified the remote fingerprint. Outgoing data and media can now flow through.
     */
    "connected" = 2,
    /**
     * The DTLS connection has been closed intentionally via a call to stop() or receipt of a close_notify alert.
     * Calling transport.stop() will also result in a transition to the closed state.
     */
    "closed" = 3,
    /**
     * The DTLS connection has been closed as the result of an error (such as receipt of an error alert or a
     * failure to validate the remote fingerprint).
     */
    "failed" = 4,
}
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
export declare enum RTCDtlsRole {
    /**
     * The DLTS role is determined based on the resolved ICE role:
     * - the ICE controlled role acts as the DTLS client and the ICE controlling role acts as the DTLS server.
     */
    "auto" = 0,
    /**
     * The DTLS client role.
     */
    "client" = 1,
    /**
     * The DTLS server role.
     */
    "server" = 2,
}
/**
 * The RTCDtlsParameters dictionary includes information relating to DTLS configuration.
 */
export interface RTCDtlsParameters {
    /**
     * The DTLS role, with a default of auto.
     */
    role?: RTCDtlsRole;
    /**
     * Array of fingerprints, one fingerprint for each certificate.
     */
    fingerprints: RTCDtlsFingerprint[];
}
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
export declare class RTCDtlsTransport extends Stream.Transform implements RTCStatsProvider {
    /**
     * An RTCDtlsTransport instance is associated to an RTCRtpSender, an RTCRtpReceiver, or an RTCSctpTransport instance.
     */
    constructor(transport: RTCIceTransport, certficates: RTCCertificate[]);
    private _transport;
    private _state;
    /**
     * The associated RTCIceTransport instance.
     */
    readonly transport: RTCIceTransport;
    /**
     * The current state of the DTLS transport.
     */
    readonly state: RTCDtlsTransportState;
    /**
     * getCertificates() returns the certificates provided in the constructor.
     */
    getCertificates(): RTCCertificate[];
    /**
     * getLocalParameters() obtains the DTLS parameters of the local RTCDtlsTransport upon construction.
     * If multiple certificates were provided in the constructor, then multiple fingerprints will be returned,
     * one for each certificate. getLocalParameters().role always returns the default role of a newly constructed RTCDtlsTransport;
     * for a browser this will be auto.
     */
    getLocalParameters(): RTCDtlsParameters;
    /**
     * getRemoteParameters() obtains the remote DTLS parameters passed in the start() method. Prior to calling start(), null is returned.
     */
    getRemoteParameters(): RTCDtlsParameters;
    /**
     * getRemoteCertificates() returns the certificate chain in use by the remote side,
     * with each certificate encoded in binary Distinguished Encoding Rules (DER) [X690].
     * getRemoteCertificates() returns an empty list prior to selection of the remote certificate,
     * which is completed once state transitions to connected.
     */
    getRemoteCertificates(): RTCCertificate[];
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
    start(remoteParameters: RTCDtlsParameters): void;
    /**
     * Stops and closes the RTCDtlsTransport object. Calling stop() when state is closed has no effect.
     */
    stop(): void;
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
    getStats(): Promise<RTCStatsReport>;
}
/**
 * Throughout this specification, the identifier payloadtype is used to refer to the octet type.
 */
export declare type payloadtype = string;
/**
 * The unhandledrtp event of the RTCRtpListener object uses the RTCRtpUnhandledEvent interface.
 *
 * Firing an RTCRtpUnhandledEvent event named e means that an event with the name e,
 * which does not bubble (except where otherwise stated) and is not cancelable (except where otherwise stated),
 * and which uses the RTCRtpUnhandledEvent interface must be created and dispatched at the given target.
 */
export interface RTCRtpUnhandledEvent {
    /**
     * The value of the MID RTP header extension [BUNDLE] in the RTP stream triggering the unhandledrtp event.
     * If receive() has not been called, the MID header extension cannot be decoded, so that muxId will be unset.
     */
    readonly muxId: string;
    /**
     * The value of the RID RTP header extension [RID] in the RTP stream triggering the unhandledrtp event.
     * If receive() has not been called, the RID header extension cannot be decoded, so that rid will be unset.
     */
    readonly rid: string;
    /**
     * The Payload Type value in the RTP stream triggering the unhandledrtp event.
     */
    readonly payloadType: payloadtype;
    /**
     * The SSRC in the RTP stream triggering the unhandledrtp event.
     */
    readonly ssrc: number;
}
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
export declare class RTCRtpListener {
    /**
     * An RTCRtpListener instance is associated to an RTCDtlsTransport.
     */
    constructor(transport: RTCDtlsTransport);
    private _transport;
    /**
     * The RTP RTCDtlsTransport instance.
     */
    readonly transport: RTCDtlsTransport;
}
/**
 * On NodeJS environment there is no MediaStream / MediaStreamTrack so that's why NodeStream is replacing those.
 */
export declare type NodeStream = Stream.Duplex | Stream.Readable | Stream.Writable | Stream.Transform | Stream.PassThrough;
/**
 * RTCRtcpFeedback provides information on RTCP feedback messages.
 */
export interface RTCRtcpFeedback {
    /**
     * Valid values for type are the "RTCP Feedback" Attribute Values enumerated in [IANA-SDP-14] ("ack", "ccm", "nack", etc.),
     * as well as "goog-remb" [REMB] and "transport-cc" [TRANSPORT-CC].
     */
    type: string;
    /**
     * For a type value of "ack" or "nack", valid values for parameter are the "ack" and "nack"
     * Attribute Values enumerated in [IANA-SDP-15] ("sli", "rpsi", etc.).
     * For the Generic NACK feedback message defined in [RFC4585] Section 6.2.1,
     * the type attribute is set to "nack" and the parameter attribute is unset. For a type value of "ccm",
     * valid values for parameter are the "Codec Control Messages" enumerated in [IANA-SDP-19] ("fir", "tmmbr" (includes "tmmbn"), etc.).
     */
    parameter: string;
}
/**
 * RTCRtpCodecParameters provides information on codec settings.
 */
export interface RTCRtpCodecParameters {
    /**
     * The codec MIME subtype. Valid subtypes are listed in [IANA-RTP-2].
     */
    name: string;
    /**
     * The codec MIME media type/subtype. Valid media types and subtypes are listed in [IANA-RTP-2].
     */
    mimeType?: string;
    /**
     * The value that goes in the RTP Payload Type Field [RFC3550]. The payloadType must always be provided, and must be unique.
     */
    payloadType: payloadtype;
    /**
     * Codec clock rate expressed in Hertz. If unset, the implementation default is assumed.
     */
    clockRate?: number;
    /**
     * The maximum packetization time set on the RTCRtpSender. Not specified if unset. If ptime is also set, maxptime is ignored.
     */
    maxptime?: number;
    /**
     * The duration of media represented by a packet in milliseconds for the RTCRtpSender.
     * If unset, the RTCRtpSender may select any value up to maxptime.
     */
    ptime?: number;
    /**
     * The number of channels supported (e.g. two for stereo). If unset for audio, use the codec default. For video, this can be left unset.
     */
    numChannels?: number;
    /**
     * Transport layer and codec-specific feedback messages for this codec.
     */
    rtcpFeedback?: RTCRtcpFeedback[];
    /**
     * Codec-specific parameters available for signaling.
     */
    parameters?: any;
}
/**
 * The RTCRtpCodecCapability dictionary provides information on the capabilities of a codec.
 * Exactly one RTCRtpCodecCapability will be present for each supported combination of parameters that requires
 * a distinct value of preferredPayloadType. For example:
 * - Multiple "h264" codecs, each with their own distinct packetization-mode values.
 * - "cn" codecs, each with distinct clockRate values.
 */
export interface RTCRtpCodecCapability {
    /**
     * The MIME media subtype. Valid subtypes are listed in [IANA-RTP-2].
     */
    name?: string;
    /**
     * The codec MIME media type/subtype. Valid media types and subtypes are listed in [IANA-RTP-2].
     */
    mimeType?: string;
    /**
     * The media supported by the codec: "audio", "video", etc.
     */
    kind?: string;
    /**
     * Codec clock rate expressed in Hertz. If unset, the codec is applicable to any clock rate.
     */
    clockRate?: number;
    /**
     * The preferred RTP payload type for the codec denoted by RTCRtpCodecCapability.name.
     * This attribute was added to make it possible for the sender and receiver to pick
     * a matching payload type when creating sender and receiver parameters.
     * When returned by RTCRtpSender.getCapabilities(),
     * RTCRtpCapabilities.codecs.preferredPayloadtype represents the preferred RTP payload type for sending.
     * When returned by RTCRtpReceiver.getCapabilities(),
     * RTCRtpCapabilities.codecs.preferredPayloadtype represents the preferred RTP payload type for receiving.
     * To avoid payload type conflicts, each value of preferredPayloadType must be unique.
     */
    preferredPayloadType?: payloadtype;
    /**
     * The maximum packetization time supported by the RTCRtpReceiver.
     */
    maxptime?: number;
    /**
     * The preferred duration of media represented by a packet in milliseconds for the RTCRtpSender or RTCRtpReceiver.
     */
    ptime?: number;
    /**
     * The number of channels supported (e.g. two for stereo). For video, this attribute is unset.
     */
    numChannels?: number;
    /**
     * Transport layer and codec-specific feedback messages for this codec.
     */
    rtcpFeedback?: RTCRtcpFeedback[];
    /**
     * Codec-specific parameters that must be signaled to the remote party.
     */
    parameters?: any;
    /**
     * Codec-specific parameters that may be optionally signalled and are available as
     * additional supported information or settings about the codec.
     */
    options?: any;
    /**
     * Maximum number of temporal layer extensions supported by this codec
     * (e.g. a value of 1 indicates support for up to 2 temporal layers).
     * A value of 0 indicates no support for temporal scalability.
     */
    maxTemporalLayers?: number;
    /**
     * Maximum number of spatial layer extensions supported by this codec
     * (e.g. a value of 1 indicates support for up to 2 spatial layers).
     * A value of 0 indicates no support for spatial scalability.
     */
    maxSpatialLayers?: number;
    /**
     * Whether the implementation can send/receive SVC layers utilizing distinct SSRCs.
     * Unset for audio codecs. For video codecs, only set if the codec supports scalable video coding with MRST.
     */
    svcMultiStreamSupport?: boolean;
}
/**
 * The RTCRtpHeaderExtensionParameters dictionary enables a header extension to be configured for use within an RTCRtpSender
 * or RTCRtpReceiver. In order to provide the equivalent of the "direction" parameter defined in [RFC5285] Section 5,
 * an application can do the following:
 *
 * - sendonly: Include the header extension only when calling send(parameters).
 * - recvonly: Include the header extension only when calling receive(parameters).
 * - sendrecv: Include the header extension when calling send(parameters) and receive(parameters).
 * - inactive: Don't include the header extension when calling either send(parameters) or receive(parameters).
 */
export interface RTCRtpHeaderExtensionParameters {
    /**
     * The URI of the RTP header extension, as defined in [RFC5285].
     */
    uri: string;
    /**
     * The value that goes in the packet.
     */
    id: number;
    /**
     * If true, the value in the header is encrypted as per [RFC6904]. Default is unencrypted.
     */
    encrypt?: boolean;
    /**
     * Configuration parameters for the header extension.
     * An example is the "vad" extension attribute in the client-to-mixer header extension,
     * described in [RFC6464] Section 4.
     */
    parameters?: any;
}
/**
 * The RTCRtpFecParameters dictionary contains information relating to Forward Error Correction (FEC) settings.
 */
export interface RTCRtpFecParameters {
    /**
     * The Forward Error Correction (FEC) mechanism to use: "red", "red+ulpfec" or "flexfec".
     */
    mechanism?: string;
    /**
     * The SSRC to use for FEC. If unset in an RTCRtpSender object, the browser will choose.
     */
    ssrc?: number;
}
/**
 * The RTCRtpRtxParameters dictionary contains information relating to retransmission (RTX) settings.
 */
export interface RTCRtpRtxParameters {
    /**
     * The SSRC to use for retransmission, as specified in [RFC4588]. If unset when passed to RTCRtpSender.send(), the browser will choose.
     */
    ssrc: number;
}
/**
 * RTCPriorityType can be used to indicate the relative priority of various flows.
 * This allows applications to indicate to the browser whether a particular media flow is high, medium, low
 * or of very low importance to the application.
 * WebRTC uses the priority and Quality of Service (QoS) framework described in [RTCWEB-TRANSPORT]
 * and [TSVWG-RTCWEB-QOS] to provide priority and DSCP marketing for packets that will help provide QoS in some networking environments.
 * Applications that use this API should be aware that often better overall user experience is obtained
 * by lowering the priority of things that are not as important rather than raising the the priority of the things that are.
 */
export declare enum RTCPriorityType {
    /**
     * See [RTCWEB-TRANSPORT], Section 4.
     */
    "very-low" = 0,
    /**
     * See [RTCWEB-TRANSPORT], Section 4.
     */
    "low" = 1,
    /**
     * See [RTCWEB-TRANSPORT], Section 4.
     */
    "medium" = 2,
    /**
     * See [RTCWEB-TRANSPORT], Section 4.
     */
    "high" = 3,
}
/**
 * RTCRtpEncodingParameters provides information relating to an encoding.
 * Note that all encoding parameters (such as maxBitrate, maxFramerate and resolutionScale)
 * are applied prior to codec-specific constraints.
 */
export interface RTCRtpEncodingParameters {
    /**
     * The SSRC for this layering/encoding.
     * Multiple RTCRtpEncodingParameters objects can share the same ssrc value
     * (useful, for example, to indicate that different RTX payload types associated to different codecs are carried over the same stream).
     * If ssrc is unset in a RTCRtpEncodingParameters object passed to the receive() method,
     * the next unhandled SSRC will match, and an RTCRtpUnhandledEvent will not be fired.
     * If ssrc is unset in a RTCRtpEncodingParameters object passed to the send() method,
     * the browser will choose, and the chosen value is not reflected in RTCRtpEncodingParameters.ssrc.
     * If the browser chooses the ssrc, it may change it due to a collision without firing an RTCSsrcConflictEvent.
     * If ssrc is set in an RTCRtpEncodingParameters object passed to the send() method
     * and an SSRC conflict is detected within the RTP session, then an RTCSsrcConflictEvent is fired (see Section 5.4).
     */
    ssrc?: number;
    /**
     * For per-encoding codec specifications,
     * give the codec payload type here. If unset,
     * the browser will choose the first codec in parameters.codecs[] of the appropriate kind.
     */
    codecPayloadType?: payloadtype;
    /**
     * Specifies the FEC mechanism if set.
     */
    fec?: RTCRtpFecParameters;
    /**
     * Specifies the RTX [RFC4588] parameters if set.
     */
    rtx?: RTCRtpRtxParameters;
    /**
     * Indicates the priority of this encoding.
     * It is specified in [RTCWEB-TRANSPORT], Section 4.
     * For scalable video coding, this parameter is only relevant for the base layer.
     * This parameter is ignored in an RTCRtpReceiver object.
     */
    priority?: RTCPriorityType;
    /**
     * Ramp up resolution/quality/framerate until this bitrate,
     * if set. maxBitrate is the Transport Independent Application Specific (TIAS)
     * maximum bandwidth defined in [RFC3890] Section 6.2.2,
     * which is the maximum bandwidth needed without counting IP or other transport layers like TCP or UDP.
     * Summed when using dependent layers. This parameter is ignored in scalable video coding,
     * or in an RTCRtpReceiver object. If unset, there is no maximum bitrate.
     */
    maxBitrate?: number;
    /**
     * If sender.track.kind is "video", the encoder will scale down the resolution of sender.track
     * in each dimension before sending. For example,
     * if the value is 2.0, the video will be scaled down by a factor of at least 2 in each dimension,
     * resulting in sending a video no greater than one quarter size. If the value is 1.0 or unset,
     * the sender will attempt to encode with the resolution of track. For scalable video coding,
     * resolutionScale refers to the aggregate scale down of this layer when combined with all dependent layers.
     *
     * If resolutionScale is less than 1.0, reject the promise with RangeError when send() or receive() is called.
     * If sender.track.kind is "audio", the value is ignored.
     */
    resolutionScale?: number;
    /**
     * Inverse of the input framerate fraction to be encoded.
     * Example: 1.0 = full framerate, 2.0 = one half of the full framerate. For scalable video coding,
     * framerateScale refers to the inverse of the aggregate fraction of input framerate achieved by this layer when combined
     * with all dependent layers.
     */
    framerateScale?: number;
    /**
     * The maximum framerate to use for this encoding, in frames per second.
     * This setting is not used for scalable video coding.
     * If framerateScale is set, then maxFramerate is ignored.
     */
    maxFramerate?: number;
    /**
     * For an RTCRtpSender, indicates whether this encoding is actively being sent.
     * Setting it to false causes this encoding to no longer be sent.
     * Setting it to true causes this encoding to be sent.
     * If unset, the default (true) is assumed. For an RTCRtpReceiver, indicates that this encoding is being decoded.
     * Setting it to false causes this encoding to no longer be decoded.
     * Setting it to true causes this encoding to be decoded. If unset, the default (true) is assumed.
     * Setting active to false is different than omitting the encoding,
     * since it can keep resources available to re-activate more quickly than re-adding the encoding.
     * As noted in [RFC3264] Section 5.1, RTCP is still sent, regardless of the value of the active attribute.
     */
    active?: boolean;
    /**
     * An identifier for the encoding object.
     * This identifier should be unique within the scope of the localized sequence of RTCRtpEncodingParameters
     * for any given RTCRtpParameters object.
     * Values must be composed only of case-sensitive alphanumeric characters (a-z, A-Z, 0-9) up to a maximum of 16 characters.
     * An RTCRtpSender places the value of encodingId into the RID header extension [RID],
     * and an RTCRtpReceiver determines the encodingId that an RTP packet belongs to based on the value of the RID header extension.
     * For a codec (such as VP8) using SRST transport which requires a compliant decoder to be able to decode anything
     * that an encoder can send,
     * it is not required that the encodingId and dependencyEncodingIds be set in order to enable a receiver to decode scalable video coding,
     * and if set, they are ignored by the receiver.
     */
    encodingId?: string;
    /**
     * The encodingIds on which this layer depends.
     * Within this specification encodingIds are permitted only within the same RTCRtpEncodingParameters sequence.
     * In the future if MST were to be supported, then if searching within an encodings[] sequence did not produce a match,
     * then a global search would be carried out. In order to send scalable video coding (SVC),
     * both the encodingId and dependencyEncodingIds are required.
     */
    dependencyEncodingIds?: string[];
}
/**
 * RTCRtcpParameters provides information on RTCP settings.
 */
export interface RTCRtcpParameters {
    /**
     * The SSRC to be used in the "SSRC of packet sender" field defined in [RFC3550]
     * Section 6.4.2 (Receiver Report) and [RFC4585] Section 6.1 (Feedback Messages),
     * as well as the "SSRC" field defined in [RFC3611] Section 2 (Extended Reports).
     * It can only be set for an RTCRtpReceiver. Other than for debugging,
     * or situations where receive() is called before send() on the same RTCDtlsTransport it is best to leave it unset,
     * in which case ssrc is chosen by the browser, though the chosen value is not reflected in RTCRtcpParameters.ssrc.
     * If the browser chooses the ssrc it may change it in event of a collision, as described in [RFC3550].
     * Where send(parameters) is called before receive() on the same RTCDtlsTransport,
     * the browser can choose one of the SSRCs allocated to an RTCRtpSender of the same kind. Where receive() is called first,
     * a random SSRC value can be chosen.
     */
    ssrc?: number;
    /**
     * The Canonical Name (CNAME) used by RTCP (e.g. in SDES messages).
     * Guidelines for CNAME generation are provided in [RTP-USAGE] Section 4.9 and [RFC7022]. By default,
     * ORTC implementations should set the CNAME to be the same within all RTCRtcpParameter objects created within the same
     * Javascript sandbox. For backward compatibility with WebRTC 1.0,
     * applications may set the CNAME only for an RTCRtpReceiver; if unset, the CNAME is chosen by the browser.
     */
    cname?: string;
    /**
     * Whether reduced size RTCP [RFC5506] is configured (if true) or compound RTCP as specified in [RFC3550] (if false).
     * The default is false.
     */
    reducedSize?: boolean;
    /**
     * Whether RTP and RTCP are multiplexed, as specified in [RFC5761].
     * The default is true. If set to false, the RTCIceTransport must have an associated RTCIceTransport object with a component of rtcp,
     * in which case RTCP will be sent on the associated RTCIceTransport.
     */
    mux?: boolean;
}
/**
 * RTCDegradationPreference can be used to indicate the desired choice between degrading resolution
 * and degrading framerate when bandwidth is constrained.
 */
export declare enum RTCDegradationPreference {
    /**
     * Degrade resolution in order to maintain framerate.
     */
    "maintain-framerate" = 0,
    /**
     * Degrade framerate in order to maintain resolution.
     */
    "maintain-resolution" = 1,
    /**
     * Degrade a balance of framerate and resolution.
     */
    "balanced" = 2,
}
/**
 * RTCRtcpParameters provides information on RTCP settings.
 */
export interface RTCRtpParameters {
    /**
     * The muxId assigned to the RTP stream, if any, empty string if unset.
     * In an RTCRtpReceiver or RTCRtpSender object, this corresponds to MID RTP header extension defined in [BUNDLE].
     * This is a stable identifier that permits the track corresponding to an RTP stream to be identified,
     * rather than relying on an SSRC. An SSRC is randomly generated and can change arbitrarily due to conflicts with other SSRCs,
     * whereas the muxId has a value whose meaning can be defined in advance between RTP sender and receiver, a
     * ssisting in RTP demultiplexing. Since muxId is included in RTCRtpParameters but not in RTCRtpEncodingParameters,
     * if it is desired to send simulcast streams with different muxId values for each stream,
     * then multiple RTCRtpSender objects are needed.
     */
    muxId?: string;
    /**
     * The codecs to send or receive (could include "red" [RFC2198], "rtx" [RFC4588] and "cn" [RFC3389]).
     * codecs must be set for an RTCRtpParameters object to be a valid argument passed to send() or receive().
     */
    codecs: RTCRtpCodecParameters[];
    /**
     * Configured header extensions. If unset, no header extensions are configured.
     */
    headerExtensions?: RTCRtpHeaderExtensionParameters[];
    /**
     * The "encodings" or "layers" to be used for things like simulcast,
     * Scalable Video Coding, RTX, FEC, etc. A sender may send fewer layers than what is specified in encodings[],
     * but must not send more. When unset in a call to send(), the browser behaves as though a single encodings[0] entry was provided,
     * with encodings[0].ssrc set to a browser-determined value, encodings[0].active set to true,
     * encodings[0].codecPayloadType set to codecs[j].payloadType where j
     * is the index of the first codec that is not "cn", "telephone-event", "red", "rtx" or a forward error correction codec
     * ("ulpfec" [RFC5109] or "flexfec" [FLEXFEC]),
     * and all the other parameters.encodings[0] attributes (e.g. fec, rtx, priority, maxBitrate, resolutionScale, etc.) unset.
     * When unset in a call to receive(), the behavior is described in Section 8.3.
     */
    encodings?: RTCRtpEncodingParameters[];
    /**
     * Parameters to configure RTCP. If unset, the default values described in Section 9.6.1 are assumed.
     */
    rtcp?: RTCRtcpParameters;
    /**
     * When bandwidth is constrained and the RTCRtpSender needs to choose between degrading resolution or degrading framerate,
     * degradationPreference indicates which is preferred.
     * degradationPreference is ignored in an RTCRtpReceiver object. If unset, "balanced" is assumed.
     */
    degradationPreference?: RTCDegradationPreference;
}
/**
 * The RTCRtpHeaderExtension dictionary provides information relating to supported header extensions.
 */
export interface RTCRtpHeaderExtension {
    /**
     * The media supported by the header extension: "audio" for an audio codec, "video" for a video codec, etc.
     */
    kind?: string;
    /**
     * The URI of the RTP header extension, as defined in [RFC5285].
     */
    uri?: string;
    /**
     * The preferred ID value that goes in the packet.
     */
    preferredId?: number;
    /**
     * If true, it is preferred that the value in the header be encrypted as per [RFC6904]. Default is to prefer unencrypted.
     */
    preferredEncrypt?: boolean;
}
/**
 * The RTCRtpCapabilities object expresses the capabilities of RTCRtpSender and RTCRtpReceiver objects.
 * Features which are mandatory to implement in [RTP-USAGE],
 * such as RTP/RTCP multiplexing [RFC5761],
 * audio/video multiplexing [RTP-MULTI-STREAM] and reduced size RTCP [RFC5506] are assumed to be
 * available and are therefore not included in RTCRtpCapabilities,
 * although these features can be set via RTCRtpParameters.
 */
export interface RTCRtpCapabilities {
    /**
     * Supported codecs.
     */
    codecs: RTCRtpCodecCapability[];
    /**
     * Supported RTP header extensions.
     */
    headerExtensions: RTCRtpHeaderExtension[];
    /**
     * Supported Forward Error Correction (FEC) mechanisms and combinations.
     * Supported values are "red" [RFC2198], "red+ulpfec" [RFC5109] and "flexfec" [FLEXFEC].
     * Note that supported mechanisms also need to be included within RTCRtpCapabilities.codecs[].
     * [FEC] summarizes requirements relating to FEC mechanisms.
     */
    fecMechanisms: string;
}
/**
 * The RTCRtpSender includes information relating to the RTP sender.
 *
 * A RTCRtpSender instance is constructed from an NodeStream object or kind and associated to an RTCDtlsTransport.
 * If an attempt is made to construct an RTCRtpSender object with transport.state or rtcpTransport.state closed,
 * or if track.readyState is ended, throw an InvalidStateError exception.
 *
 * An RTCRtpSender object can be garbage-collected once stop() is called and it is no longer referenced.
 */
export declare class RTCRtpSender extends Stream.Duplex implements RTCStatsProvider {
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
    constructor(trackOrKind: NodeStream, transport?: RTCDtlsTransport, rtcpTransport?: RTCDtlsTransport);
    private _track;
    private _transport;
    private _rtcpTransport;
    private _kind;
    /**
     * The associated MediaStreamTrack instance. If track is ended,
     * or if track.muted is set to true,
     * the RTCRtpSender sends silence (audio) or a black frame (video).
     * If track is set to null then the RTCRtpSender does not send RTP.
     */
    readonly track: NodeStream;
    /**
     * The RTCDtlsTransport instance over which RTCP is sent and received (if provided).
     * When BUNDLE is used,
     * many RTCRtpSender objects will share one rtcpTransport and will all send and receive RTCP over the same RTCDtlsTransport.
     * When RTCP mux is used, rtcpTransport will be null, and both RTP and RTCP traffic will flow over the RTCDtlsTransport transport.
     */
    readonly transport: RTCDtlsTransport;
    /**
     * The associated RTCP RTCDtlsTransport instance if one was provided in the constructor.
     * When RTCP mux is used, rtcpTransport will be null, and both RTP and RTCP traffic will flow over the RTCDtlsTransport transport.
     */
    readonly rtcpTransport: RTCDtlsTransport;
    /**
     * The value of kind or track.kind passed in the constructor.
     */
    readonly kind: string;
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
    setTransport(transport: RTCDtlsTransport, rtcpTransport: RTCDtlsTransport): void;
    /**
     * setTrack Attempts to replace the track being sent with another track provided (or with a null track).
     * The deprecated setTrack method operates identically to the replaceTrack method.
     */
    setTrack(trackOrKind?: NodeStream): Promise<void>;
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
    replaceTrack(trackOrKind?: NodeStream): Promise<void>;
    /**
     * getCapabilities() obtains the sender capabilities, based on kind.
     * Browsers must support kind values of "audio" and "video".
     * If there are no capabilities corresponding to the value of kind, getCapabilities returns null.
     * Capabilities that can apply to multiple values of kind (such as retransmission [RFC4588],
     * redundancy [RFC2198] and Forward Error Correction) have RTCRtpCapabilities.RTCRtpCodecCapability[i].kind
     * set to the value of the kind argument.
     */
    static getCapabilities(kind: string): RTCRtpCapabilities;
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
    send(parameters: RTCRtpParameters): Promise<void>;
    /**
     * The stop method irreversibly stops the RTCRtpSender. When stop called, the following steps must be run:
     * - Let sender be the RTCRtpSender on which stop is invoked.
     * - If sender.stop has previously been called, abort these steps.
     * - Let parameters be the argument provided to sender.send(parameters) the last time it was invoked.
     * - Stop sending media with sender.
     * - Send an RTCP BYE for each SSRC in parameters.encodings[i].ssrc, parameters.encodings[i].fec.ssrc and parameters.encodings[i].rtx.ssrc where i goes from 0 to encodings.length-1.
     */
    stop(): void;
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
    getStats(): Promise<RTCStatsReport>;
}
/**
 * The RTCRtpContributingSource dictionary contains information about a given contributing source (CSRC),
 *  including the most recent time a packet that the source contributed to was played out.
 *  The browser must keep information from RTP packets received in the previous 10 seconds.
 *
 * When the first audio frame contained in an RTP packet is delivered to the RTCRtpReceiver's MediaStreamTrack for playout,
 * the user agent must queue a task to update the relevant RTCRtpContributingSource and RTCRtpSynchronizationSource objects
 * based on the contents of the packet. The RTCRtpSynchronizationSource dictionary corresponding to the SSRC identifier is updated
 * each time, and the level value for the SSRC is updated based on the client-mixer header extension [RFC6464] if present.
 *
 * If the RTP packet contains CSRC identifiers, then the RTCRtpContributingSource dictionaries corresponding to those CSRC identifiers
 * are also updated, and the level values for those CSRCs are updated based on the mixer-client header extension [RFC6464] if present.
 */
export interface RTCRtpContributingSource {
    /**
     * The timestamp of type DOMHighResTimeStamp [HIGHRES-TIME],
     * indicating the most recent time of playout of an RTP packet containing the source.
     * The timestamp is defined in [HIGHRES-TIME] and corresponds to a local clock.
     */
    timestamp?: number;
    /**
     * The CSRC identifier of the contributing source.
     */
    source?: number;
    /**
     * The audio level contained in the last RTP packet played from this source.
     * audioLevel will be the level value defined in [RFC6465] if the RFC 6465 header extension is present,
     * and otherwise null. RFC 6465 defines the level as a integral value from 0 to 127 representing
     * the audio level in negative decibels relative to the loudest signal that the system could possibly encode.
     * Thus, 0 represents the loudest signal the system could possibly encode, and 127 represents silence.
     */
    audioLevel?: number;
}
/**
 * The RTCRtpSynchronizationSource dictionary contains information about a given synchronization source (SSRC),
 * including the most recent time a packet that the source contributed to was played out.
 * The browser must keep information from RTP packets received in the previous 10 seconds.
 */
export interface RTCRtpSynchronizationSource {
    /**
     * The timestamp of type DOMHighResTimeStamp [HIGHRES-TIME],
     * indicating the most recent time of playout of an RTP packet from the source.
     * The timestamp is defined in [HIGHRES-TIME] and corresponds to a local clock.
     */
    timestamp?: number;
    /**
     * The SSRC identifier of the synchronization source.
     */
    source?: number;
    /**
     * The audio level contained in the last RTP packet played from this source.
     * audioLevel will be the level value defined in [RFC6464], if the RFC 6464 header extension is present.
     */
    audioLevel?: number;
    /**
     * Whether the last RTP packet received from this source contains voice activity (true) or not (false).
     * Since the "V" bit is supported in [RFC6464] but not [RFC6465],
     * the voiceActivityFlag attribute will only be set when receivers enable the client-mixer
     * header extension (setting the vad attribute to true),
     * and when RTP packets are received from senders enabling the client-mixer header extension (setting the vad attribute to true).
     */
    voiceActivityFlag: boolean;
}
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
export declare class RTCRtpReceiver extends Stream.Duplex implements RTCStatsProvider {
    /**
     * An RTCRtpReceiver instance produces an associated receiving MediaStreamTrack and provides RTC related methods to it.
     */
    constructor(kind: string, transport: RTCDtlsTransport, rtcpTransport?: RTCDtlsTransport);
    private _track;
    private _transport;
    private _rtcpTransport;
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
    readonly track: NodeStream;
    /**
     * The associated RTP RTCDtlsTransport instance.
     */
    readonly transport: RTCDtlsTransport;
    /**
     * The RTCDtlsTransport instance over which RTCP is sent and received.
     * When BUNDLE is used,
     * multiple RTCRtpReceiver objects will share one rtcpTransport and will send and receive RTCP over the same RTCDtlsTransport.
     * When RTCP mux is used, rtcpTransport will be null, and both RTP and RTCP traffic will flow over the RTCDtlsTransport transport.
     */
    readonly rtcpTransport: RTCDtlsTransport;
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
    setTransport(transport: RTCDtlsTransport, rtcpTransport: RTCDtlsTransport): void;
    /**
     * getCapabilities() obtains the receiver capabilities, based on kind.
     * Browsers must support kind values of "audio" and "video".
     * If there are no capabilities corresponding to the value of kind, getCapabilities returns null.
     * Capabilities that can apply to multiple values of kind (such as retransmission [RFC4588],
     * redundancy [RFC2198] and Forward Error Correction) have RTCRtpCapabilities.RTCRtpCodecCapability[i].kind set to the value
     * of the kind argument.
     */
    static getCapabilities(kind: string): RTCRtpCapabilities;
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
    receive(parameters: RTCRtpParameters): Promise<void>;
    /**
     * Returns an RTCRtpContributingSource for each unique CSRC identifier received by this RTCRtpReceiver.
     * The browser must keep information from RTP packets received in the last 10 seconds.
     * If no contributing sources are available, an empty list is returned.
     */
    getContributingSources(): RTCRtpContributingSource[];
    /**
     * Returns an RTCRtpSynchronizationSource for each unique SSRC identifier received by this RTCRtpReceiver in the last 10 seconds.
     */
    getSynchronizationSources(): RTCRtpSynchronizationSource[];
    /**
     * Stops the RTCRtpReceiver receiver. receiver.stop() is final like receiver.track.stop().
     * receiver.track.stop() does not affect track clones and also does not stop receiver so that Receiver Reports continue to be sent.
     * Calling receiver.stop() does not cause the "onended" event to fire for track.
     */
    stop(): void;
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
    getStats(): Promise<RTCStatsReport>;
}
export interface RTCDataTransport extends Stream.Duplex, RTCStatsProvider {
}
/**
 * The RTCDataChannelState provides information on the state of the data channel.
 */
export declare enum RTCDataChannelState {
    /**
     * The user agent is attempting to establish the underlying data transport. This is the initial state of an RTCDataChannel object.
     */
    "connecting" = 0,
    /**
     * The underlying data transport is established and communication is possible. This is the initial state of an RTCDataChannel object dispatched as a part of an RTCDataChannelEvent.
     */
    "open" = 1,
    /**
     * The procedure to close down the underlying data transport has started.
     */
    "closing" = 2,
    /**
     * The underlying data transport has been closed or could not be established.
     */
    "closed" = 3,
}
/**
 * The RTCDataChannelParameters dictionary describes the configuration of the RTCDataChannel.
 * An RTCDataChannel can be configured to operate in different reliability modes.
 * A reliable channel ensures that the data is delivered at the other peer through retransmissions.
 * An unreliable channel is configured to either limit the number of retransmissions (maxRetransmits)
 * or set a time during which transmissions (including retransmissions) are allowed (maxPacketLifeTime).
 * These properties can not be used simultaneously and an attempt to do so will result in an error.
 * Not setting any of these properties results in a reliable channel.
 */
export interface RTCDataChannelParameters {
    /**
     * The label attribute represents a label that can be used to distinguish this RTCDataChannel object from other RTCDataChannel objects.
     * The attribute must return the value to which it was set when the RTCDataChannel object was constructed.
     * For an SCTP data channel, the label is carried in the DATA_CHANNEL_OPEN message defined in [DATA-PROT] Section 5.1.
     */
    label?: string;
    /**
     * The ordered attribute returns true if the RTCDataChannel is ordered, and false if out of order delivery is allowed.
     * Default is true. The attribute must return the value to which it was set when the RTCDataChannel was constructed.
     */
    ordered?: boolean;
    /**
     * The maxPacketLifetime attribute represents the length of the time window (in milliseconds) during which retransmissions
     * may occur in unreliable mode. The attribute must return the value to which it was set when the RTCDataChannel was constructed.
     */
    maxPacketLifetime?: number;
    /**
     * The maxRetransmits attribute returns the maximum number of retransmissions that are attempted in unreliable mode.
     * The attribute must be initialized to null by default and must return the value to which it was set when the RTCDataChannel
     * was constructed.
     */
    maxRetransmits?: number;
    /**
     * The name of the sub-protocol used with this RTCDataChannelif any,
     * or the empty string otherwise (in which case the protocol is unspecified).
     * The attribute must return the value to which it was set when the RTCDataChannel was constucted.
     * Sub-protocols are registered in the 'Websocket Subprotocol Name Registry' created in [RFC6455] Section 11.5.
     */
    protocol?: string;
    /**
     * The negotiated attribute returns true if this RTCDataChannel was negotiated by the application, or false otherwise.
     * The attribute must be initialized to false by default and must return the value to which it was set when
     * the RTCDataChannel was constructed. If set to true, the application developer must signal to the remote peer to construct
     * an RTCDataChannel object with the same id for the data channel to be open.
     * As noted in [DATA-PROT], DATA_CHANNEL_OPEN is not sent to the remote peer nor is DATA_CHANNEL_ACK expected in return.
     * If set to false, the remote party will receive an ondatachannel event with a system constructed RTCDataChannel object.
     */
    negotiated?: boolean;
    /**
     * The id attribute returns the id for this RTCDataChannel.
     * The id was either assigned by the user agent at channel creation time or was selected by the script. For SCTP,
     * the id represents a stream identifier, as discussed in [DATA] Section 6.5.
     * The attribute must return the value to which it was set when the RTCDataChannel was constructed.
     */
    id: number;
}
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
export declare class RTCDataChannel extends Stream.Duplex {
    /**
     * An RTCDataChannel object is constructed from a RTCDataTransport object (providing the transport for the data channel)
     * and an RTCDataChannelParameters object. If parameters is invalid, throw an InvalidParameters exception.
     * If transport.state is closed, throw an InvalidState exception.
     *
     * An RTCDataChannel object can be garbage-collected once readyState is closed and it is no longer referenced.
     */
    constructor(transport: RTCDataTransport, parameters?: RTCDataChannelParameters);
    private _state;
    private _transport;
    private _bufferedAmount;
    /**
     * The readyState attribute represents the state of the RTCDataChannel object.
     * It must return the value to which the user agent last set it (as defined by the processing model algorithms).
     */
    readonly readyState: RTCDataChannelState;
    /**
     * The readonly attribute referring to the related transport object.
     */
    readonly transport: RTCDataTransport;
    /**
     * The bufferedAmount attribute must return the number of bytes of application data (UTF-8 text and binary data)
     * that have been queued using send() but that, as of the last time the event loop started executing a task,
     * had not yet been transmitted to the network. This includes any text sent during the execution of the current task,
     * regardless of whether the user agent is able to transmit text asynchronously with script execution.
     * This does not include framing overhead incurred by the protocol, or buffering done by the operating system or network hardware.
     * If the channel is closed, this attribute's value will only increase with each call to the send()
     * method (the attribute does not reset to zero once the channel closes).
     */
    readonly bufferedAmount: number;
    /**
     * The bufferedAmountLowThreshold attribute sets the threshold at which the bufferedAmount is considered to be low.
     * When the bufferedAmount decreases from above this threshold to equal or below it,
     * the bufferedamountlow event fires. The bufferedAmountLowThreshold is initially zero on each new RTCDataChannel,
     * but the application may change its value at any time.
     */
    bufferedAmountLowThreshold: number;
    /**
     * The binaryType attribute must, on getting, return the value to which it was last set.
     * On setting, the user agent must set the IDL attribute to the new value. When an RTCDataChannel object is constructed,
     * the binaryType attribute must be initialized to the string 'blob'.
     * This attribute controls how binary data is exposed to scripts. See the [WEBSOCKETS-API] for more information.
     */
    binaryType: string;
    /**
     * Returns the parameters applying to this data channel.
     */
    getParameters(): RTCDataChannelParameters;
    /**
     * Closes the RTCDataChannel.
     * It may be called regardless of whether the RTCDataChannel object was created by this peer or the remote peer.
     * When the close() method is called, the user agent must run the following steps:
     * - Let channel be the RTCDataChannel object which is about to be closed.
     * - If channel's readyState is closing or closed, then abort these steps.
     * - Set channel's readyState attribute to closing.
     * - If the closing procedure has not started yet, start it.
     */
    close(): void;
    /**
     * Run the steps described by the send() algorithm with argument type string object.
     */
    send(data: string | ArrayBuffer | ArrayBufferView | Buffer): void;
}
/**
 * RTCSctpTransportState indicates the state of the SCTP transport.
 */
export declare enum RTCSctpTransportState {
    /**
     * The RTCSctpTransport object has been constructed but the start method has not been called so the RTCSctpTransport has not started negotiating yet.
     */
    "new" = 0,
    /**
     * The start method has been called and the RTCSctpTransport is in the process of negotiating an association.
     */
    "connecting" = 1,
    /**
     * The RTCSctpTransport has completed negotiation of an association.
     */
    "connected" = 2,
    /**
     * The SCTP association has been closed intentionally via a call to stop or receipt of a SHUTDOWN or ABORT chunk.
     */
    "closed" = 3,
}
/**
 * The RTCSctpCapabilities dictionary provides information about the capabilities of the RTCSctpTransport.
 */
export interface RTCSctpCapabilities {
    /**
     * The maximum size of data that can be passed to RTCDataChannel's send() method.
     */
    maxMessageSize: number;
}
/**
 * The RTCSctpTransport includes information relating to Stream Control Transmission Protocol (SCTP) transport.
 */
export declare class RTCSctpTransport extends Stream.Transform implements RTCDataTransport {
    /**
     * An RTCSctpTransport inherits from an RTCDataTransport object, which is associated to an RTCDataChannel object.
     *
     * An RTCSctpTransport is constructed from an RTCDtlsTransport object,
     * and optionally a port number (with a default of 5000, or the next unused port).
     * If a port already in use is provided in the constructor, throw an InvalidParameters exception.
     * An RTCSctpTransport object can be garbage-collected once stop() is called and it is no longer referenced.
     */
    constructor(transport: RTCDtlsTransport, port?: number);
    private _transport;
    private _state;
    private _port;
    /**
     * The RTCDtlsTransport instance the RTCSctpTransport object is sending over.
     */
    readonly transport: RTCDtlsTransport;
    /**
     * The current state of the SCTP transport.
     */
    readonly state: RTCSctpTransportState;
    /**
     * The local SCTP port number used by the data channel.
     */
    readonly port: number;
    /**
     * getCapabilities() retrieves the RTCSctpCapabilities of the RTCSctpTransport.
     */
    static getCapabilities(): RTCSctpCapabilities;
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
    start(remoteCaps: RTCSctpCapabilities, remotePort?: number): void;
    /**
     * Stops the RTCSctpTransport instance.
     */
    stop(): void;
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
    getStats(): Promise<RTCStatsReport>;
}
