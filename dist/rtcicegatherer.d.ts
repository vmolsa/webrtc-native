/// <reference types="node" />
import Events = require('events');
import Dgram = require('dgram');
import Net = require('net');
import { RTCStatsProvider, RTCStatsReport, RTCIceComponent, RTCIceGathererState, RTCIceGatherOptions, RTCIceParameters, RTCIceCandidate as IceCandidate, RTCIceProtocol, RTCIceTcpCandidateType, RTCIceCandidateType } from './ortc';
export declare class RTCIceCandidate implements IceCandidate {
    constructor(foundation: string | number, ip: string, port: number, priority: number, protocol: RTCIceProtocol, type?: RTCIceCandidateType, relatedAddress?: string, relatedPort?: number, tcpType?: RTCIceTcpCandidateType);
    static from(socket: Dgram.Socket | Net.Server | Net.Socket, component: RTCIceComponent, type: RTCIceCandidateType, protocol: RTCIceProtocol): RTCIceCandidate;
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
    private gatherCandidate(validatedServer, callback);
    private gatherCandidateFrom(validatedServers, index, callback);
    private gatherHostUdpCandidate(address);
    private gatherHostTcpCandidate(address);
    gather(options?: RTCIceGatherOptions): void;
    getLocalParameters(): RTCIceParameters;
    getLocalCandidates(): RTCIceCandidate[];
    createAssociatedGatherer(): RTCIceGatherer;
    getStats(): Promise<RTCStatsReport>;
}
