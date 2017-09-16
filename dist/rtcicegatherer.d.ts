/// <reference types="node" />
import Events = require('events');
import Dgram = require('dgram');
import Net = require('net');
import { RTCStatsProvider, RTCStatsReport, RTCIceComponent, RTCIceGathererState, RTCIceGatherOptions, RTCIceParameters, RTCIceCandidate, RTCIceProtocol, RTCIceCandidateType } from './ortc';
export declare function RTCIceCandidateFrom(socket: Dgram.Socket | Net.Server | Net.Socket, component: RTCIceComponent, type: RTCIceCandidateType, protocol: RTCIceProtocol): RTCIceCandidate;
export declare class RTCIceGatherer extends Events.EventEmitter implements RTCStatsProvider {
    private _iceLite;
    private _icePwd;
    private _iceUfrag;
    private _iceServers;
    private _iceLocalCandidates;
    private _policy;
    private _component;
    private _state;
    private _associated;
    constructor(options: RTCIceGatherOptions);
    readonly component: RTCIceComponent;
    readonly state: RTCIceGathererState;
    close(): void;
    private gatherStunCandidate(validatedServer, callback);
    private gatherTurnCandidate(validatedServer, callback);
    private gatherCandidateFrom(validatedServers, index);
    private gatherHostUdpCandidate(address);
    private gatherHostTcpCandidate(address);
    gather(options?: RTCIceGatherOptions): void;
    getLocalParameters(): RTCIceParameters;
    getLocalCandidates(): RTCIceCandidate[];
    createAssociatedGatherer(): RTCIceGatherer;
    getStats(): Promise<RTCStatsReport>;
}
