/// <reference types="node" />
import Stream = require('stream');
export * from './stun';
export declare enum RTCStatsType {
    "inboundrtp" = 0,
    "outboundrtp" = 1,
    "datachannel" = 2,
    "track" = 3,
    "transport" = 4,
    "candidatepair" = 5,
    "localcandidate" = 6,
    "remotecandidate" = 7,
    "certificate" = 8,
}
export declare enum RTCIceProtocol {
    "udp" = 0,
    "tcp" = 1,
}
export declare enum RTCIceTcpCandidateType {
    "active" = 0,
    "passive" = 1,
    "so" = 2,
}
export declare enum RTCIceCandidateType {
    "host" = 0,
    "srflx" = 1,
    "prflx" = 2,
    "relay" = 3,
}
export declare enum RTCIceComponent {
    "RTP" = 0,
    "RTCP" = 1,
}
export declare enum RTCIceGathererState {
    "new" = 0,
    "gathering" = 1,
    "complete" = 2,
    "closed" = 3,
}
export declare enum RTCIceGatherPolicy {
    "all" = 0,
    "nohost" = 1,
    "relay" = 2,
}
export declare enum RTCIceCredentialType {
    "password" = 0,
    "oauth" = 1,
}
export declare enum RTCIceRole {
    "controlling" = 0,
    "controlled" = 1,
}
export declare enum RTCIceTransportState {
    "new" = 0,
    "checking" = 1,
    "connected" = 2,
    "completed" = 3,
    "disconnected" = 4,
    "failed" = 5,
    "closed" = 6,
}
export interface RTCStats {
    id?: string;
    timestamp?: number;
    type?: RTCStatsType;
}
export declare type RTCStatsReport = {
    [key: string]: RTCStats;
};
export interface RTCStatsProvider {
    getStats(): Promise<RTCStatsReport>;
}
export interface RTCIceCandidate {
    foundation: string;
    ip: string;
    port: number;
    priority: number;
    protocol: RTCIceProtocol;
    relatedAddress?: string;
    relatedPort?: number;
    tcpType?: RTCIceTcpCandidateType;
    type?: RTCIceCandidateType;
}
export interface RTCIceGatherOptions {
    gatherPolicy: string | RTCIceGatherPolicy;
    iceServers?: RTCIceServer[];
}
export interface RTCIceCandidateComplete {
    readonly complete: boolean;
}
export interface RTCIceParameters {
    iceLite: boolean;
    password: string;
    usernameFragment: string;
}
export interface RTCIceGathererIceErrorEvent {
    readonly hostCandidate: RTCIceCandidate;
    readonly url: string;
    readonly errorCode: number;
    readonly errorText: string;
}
export declare type RTCIceGatherCandidate = RTCIceCandidate | RTCIceCandidateComplete;
export interface RTCIceGathererEvent {
    readonly candidate: RTCIceGatherCandidate;
    readonly url: string;
}
export interface RTCOAuthCredential {
    macKey: string;
    accessToken: string;
}
export interface RTCIceServer {
    urls: string | string[];
    username?: string;
    credential?: string | RTCOAuthCredential;
    credentialType?: RTCIceCredentialType;
}
export interface RTCIceCandidatePair {
    local: RTCIceCandidate;
    remote: RTCIceCandidate;
}
export interface RTCIceCandidatePairChangedEvent {
    readonly pair: RTCIceCandidatePair;
}
export { RTCIceGatherer } from './rtcicegatherer';
export { RTCIceTransport } from './rtcicetransport';
export interface Algorithm {
    name: string;
}
export declare type AlgorithmIdentifier = string | Algorithm;
export interface EcKeyGenParams extends Algorithm {
    namedCurve: string;
}
export interface RTCDtlsFingerprint {
    algorithm: string;
    value: string;
}
export declare class RTCCertificate {
    private _expires;
    private _fingerprint;
    private _algorithm;
    readonly expires: number;
    readonly fingerprint: RTCDtlsFingerprint;
    getAlgorithm(): AlgorithmIdentifier;
    static generateCertificate(keygenAlgorithm: AlgorithmIdentifier): Promise<RTCCertificate>;
}
export declare enum RTCDtlsTransportState {
    "new" = 0,
    "connecting" = 1,
    "connected" = 2,
    "closed" = 3,
    "failed" = 4,
}
export declare enum RTCDtlsRole {
    "auto" = 0,
    "client" = 1,
    "server" = 2,
}
export interface RTCDtlsParameters {
    role?: RTCDtlsRole;
    fingerprints: RTCDtlsFingerprint[];
}
export declare class RTCDtlsTransport extends Stream.Transform implements RTCStatsProvider {
    constructor(transport: RTCIceTransport, certficates: RTCCertificate[]);
    private _transport;
    private _state;
    readonly transport: RTCIceTransport;
    readonly state: RTCDtlsTransportState;
    getCertificates(): RTCCertificate[];
    getLocalParameters(): RTCDtlsParameters;
    getRemoteParameters(): RTCDtlsParameters;
    getRemoteCertificates(): RTCCertificate[];
    start(remoteParameters: RTCDtlsParameters): void;
    stop(): void;
    getStats(): Promise<RTCStatsReport>;
}
export declare type payloadtype = string;
export interface RTCRtpUnhandledEvent {
    readonly muxId: string;
    readonly rid: string;
    readonly payloadType: payloadtype;
    readonly ssrc: number;
}
export declare class RTCRtpListener {
    constructor(transport: RTCDtlsTransport);
    private _transport;
    readonly transport: RTCDtlsTransport;
}
export declare type NodeStream = Stream.Duplex | Stream.Readable | Stream.Writable | Stream.Transform | Stream.PassThrough;
export interface RTCRtcpFeedback {
    type: string;
    parameter: string;
}
export interface RTCRtpCodecParameters {
    name: string;
    mimeType?: string;
    payloadType: payloadtype;
    clockRate?: number;
    maxptime?: number;
    ptime?: number;
    numChannels?: number;
    rtcpFeedback?: RTCRtcpFeedback[];
    parameters?: any;
}
export interface RTCRtpCodecCapability {
    name?: string;
    mimeType?: string;
    kind?: string;
    clockRate?: number;
    preferredPayloadType?: payloadtype;
    maxptime?: number;
    ptime?: number;
    numChannels?: number;
    rtcpFeedback?: RTCRtcpFeedback[];
    parameters?: any;
    options?: any;
    maxTemporalLayers?: number;
    maxSpatialLayers?: number;
    svcMultiStreamSupport?: boolean;
}
export interface RTCRtpHeaderExtensionParameters {
    uri: string;
    id: number;
    encrypt?: boolean;
    parameters?: any;
}
export interface RTCRtpFecParameters {
    mechanism?: string;
    ssrc?: number;
}
export interface RTCRtpRtxParameters {
    ssrc: number;
}
export declare enum RTCPriorityType {
    "very-low" = 0,
    "low" = 1,
    "medium" = 2,
    "high" = 3,
}
export interface RTCSsrcConflictEvent {
    readonly ssrc: number;
}
export interface RTCRtpEncodingParameters {
    ssrc?: number;
    codecPayloadType?: payloadtype;
    fec?: RTCRtpFecParameters;
    rtx?: RTCRtpRtxParameters;
    priority?: RTCPriorityType;
    maxBitrate?: number;
    resolutionScale?: number;
    framerateScale?: number;
    maxFramerate?: number;
    active?: boolean;
    encodingId?: string;
    dependencyEncodingIds?: string[];
}
export interface RTCRtcpParameters {
    ssrc?: number;
    cname?: string;
    reducedSize?: boolean;
    mux?: boolean;
}
export declare enum RTCDegradationPreference {
    "maintain-framerate" = 0,
    "maintain-resolution" = 1,
    "balanced" = 2,
}
export interface RTCRtpParameters {
    muxId?: string;
    codecs: RTCRtpCodecParameters[];
    headerExtensions?: RTCRtpHeaderExtensionParameters[];
    encodings?: RTCRtpEncodingParameters[];
    rtcp?: RTCRtcpParameters;
    degradationPreference?: RTCDegradationPreference;
}
export interface RTCRtpHeaderExtension {
    kind?: string;
    uri?: string;
    preferredId?: number;
    preferredEncrypt?: boolean;
}
export interface RTCRtpCapabilities {
    codecs: RTCRtpCodecCapability[];
    headerExtensions: RTCRtpHeaderExtension[];
    fecMechanisms: string;
}
export declare class RTCRtpSender extends Stream.Duplex implements RTCStatsProvider {
    constructor(trackOrKind: NodeStream, transport?: RTCDtlsTransport, rtcpTransport?: RTCDtlsTransport);
    private _track;
    private _transport;
    private _rtcpTransport;
    private _kind;
    readonly track: NodeStream;
    readonly transport: RTCDtlsTransport;
    readonly rtcpTransport: RTCDtlsTransport;
    readonly kind: string;
    setTransport(transport: RTCDtlsTransport, rtcpTransport: RTCDtlsTransport): void;
    setTrack(trackOrKind?: NodeStream): Promise<void>;
    replaceTrack(trackOrKind?: NodeStream): Promise<void>;
    static getCapabilities(kind: string): RTCRtpCapabilities;
    send(parameters: RTCRtpParameters): Promise<void>;
    stop(): void;
    getStats(): Promise<RTCStatsReport>;
}
export interface RTCRtpContributingSource {
    timestamp?: number;
    source?: number;
    audioLevel?: number;
}
export interface RTCRtpSynchronizationSource {
    timestamp?: number;
    source?: number;
    audioLevel?: number;
    voiceActivityFlag: boolean;
}
export declare class RTCRtpReceiver extends Stream.Duplex implements RTCStatsProvider {
    constructor(kind: string, transport: RTCDtlsTransport, rtcpTransport?: RTCDtlsTransport);
    private _track;
    private _transport;
    private _rtcpTransport;
    readonly track: NodeStream;
    readonly transport: RTCDtlsTransport;
    readonly rtcpTransport: RTCDtlsTransport;
    setTransport(transport: RTCDtlsTransport, rtcpTransport: RTCDtlsTransport): void;
    static getCapabilities(kind: string): RTCRtpCapabilities;
    receive(parameters: RTCRtpParameters): Promise<void>;
    getContributingSources(): RTCRtpContributingSource[];
    getSynchronizationSources(): RTCRtpSynchronizationSource[];
    stop(): void;
    getStats(): Promise<RTCStatsReport>;
}
export interface RTCDataTransport extends Stream.Duplex, RTCStatsProvider {
}
export declare enum RTCDataChannelState {
    "connecting" = 0,
    "open" = 1,
    "closing" = 2,
    "closed" = 3,
}
export interface RTCDataChannelParameters {
    label?: string;
    ordered?: boolean;
    maxPacketLifetime?: number;
    maxRetransmits?: number;
    protocol?: string;
    negotiated?: boolean;
    id: number;
}
export interface RTCDataChannelEvent {
    readonly channel: RTCDataChannel;
}
export declare class RTCDataChannel extends Stream.Duplex {
    constructor(transport: RTCDataTransport, parameters?: RTCDataChannelParameters);
    private _state;
    private _transport;
    private _bufferedAmount;
    readonly readyState: RTCDataChannelState;
    readonly transport: RTCDataTransport;
    readonly bufferedAmount: number;
    bufferedAmountLowThreshold: number;
    binaryType: string;
    getParameters(): RTCDataChannelParameters;
    close(): void;
    send(data: string | ArrayBuffer | ArrayBufferView | Buffer): void;
}
export declare enum RTCSctpTransportState {
    "new" = 0,
    "connecting" = 1,
    "connected" = 2,
    "closed" = 3,
}
export interface RTCSctpCapabilities {
    maxMessageSize: number;
}
export declare class RTCSctpTransport extends Stream.Transform implements RTCDataTransport {
    constructor(transport: RTCDtlsTransport, port?: number);
    private _transport;
    private _state;
    private _port;
    readonly transport: RTCDtlsTransport;
    readonly state: RTCSctpTransportState;
    readonly port: number;
    static getCapabilities(): RTCSctpCapabilities;
    start(remoteCaps: RTCSctpCapabilities, remotePort?: number): void;
    stop(): void;
    getStats(): Promise<RTCStatsReport>;
}
