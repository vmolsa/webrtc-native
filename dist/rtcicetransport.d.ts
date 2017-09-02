/// <reference types="node" />
import Stream = require('stream');
import { RTCStatsReport, RTCStatsProvider, RTCIceCandidate, RTCIceGatherer, RTCIceComponent, RTCIceRole, RTCIceTransportState, RTCIceCandidatePair, RTCIceParameters, RTCIceGatherCandidate } from './ortc';
export declare class RTCIceTransport extends Stream.Duplex implements RTCStatsProvider {
    constructor(gatherer?: RTCIceGatherer);
    private _iceGatherer;
    private _role;
    private _component;
    private _state;
    readonly iceGatherer: RTCIceGatherer;
    readonly role: RTCIceRole;
    readonly component: RTCIceComponent;
    readonly state: RTCIceTransportState;
    getRemoteCandidates(): RTCIceCandidate[];
    getSelectedCandidatePair(): RTCIceCandidatePair;
    start(gatherer: RTCIceGatherer, remoteParameters: RTCIceParameters, role?: RTCIceRole): void;
    stop(): void;
    getRemoteParameters(): RTCIceParameters;
    createAssociatedTransport(): RTCIceTransport;
    addRemoteCandidate(remoteCandidate: RTCIceGatherCandidate): void;
    setRemoteCandidates(remoteCandidates: RTCIceCandidate[]): void;
    getStats(): Promise<RTCStatsReport>;
}
