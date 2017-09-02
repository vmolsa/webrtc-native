import Events = require('events');
import Stun = require('stun');
import OS = require('os');
import CRC = require('crc');
import IpAddr = require('ipaddr.js');
import Dgram = require('dgram');
import assert = require('assert');
import Net = require('net');

import {
  RTCIceServer,
  RTCStatsProvider,
  RTCStatsReport,
  RTCIceGatherPolicy,
  RTCIceComponent,
  RTCIceGathererState,
  RTCOAuthCredential,
  RTCIceGatherOptions,
  RTCIceCredentialType,
  RTCIceParameters,
  RTCIceCandidate as IceCandidate,
  RTCIceProtocol,
  RTCIceTcpCandidateType,
  RTCIceCandidateType,
  RTCIceGathererEvent,
  RTCIceCandidateComplete,
} from './ortc'

function getRandomString(len: number): string {
  const iceChars = [
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
  ];

  let str: string = '';

  for (let index = 0; index < len; index++) {
    const pos = ((Math.random() * 62) >>> 0)  % 62;
    str += iceChars[pos];
  }
 
  return str;
}

function RTCIceGatherPolicyToString(policy: RTCIceGatherPolicy): string {
  switch (policy) {
    case RTCIceGatherPolicy.all:
      return 'all';
    case RTCIceGatherPolicy.nohost:
      return 'nohost';
    case RTCIceGatherPolicy.relay:
      return 'relay';
  }
}

function RTCIceGatherPolicyFrom(policy: string | RTCIceGatherPolicy): RTCIceGatherPolicy {
  if (typeof policy == 'string') {
    switch (policy) {
      case 'relay':
        return RTCIceGatherPolicy.relay;
      case 'nohost':
        return RTCIceGatherPolicy.nohost;
      default:
        return RTCIceGatherPolicy.all;
    }
  } else {
    return policy;
  }
}

interface ValidatedServers {
  url: string;
  protocol: string;
  address: string;
  port: number;
  iceServer: RTCIceServer;
};

function getLocalPreference(family: string, address: string): number {
  if (family == 'IPv4') {
    return 30;
  }

  // TODO(): Check default address for ipv6 - RFC 3484

  return 40;
}

/**
 *  When using the formula, an agent computes the priority by determining
 *  a preference for each type of candidate (server reflexive, peer
 *  reflexive, relayed, and host), and, when the agent is multihomed,
 *  choosing a preference for its IP addresses.  These two preferences
 *  are then combined to compute the priority for a candidate.  That
 *  priority is computed using the following formula:
 * 
 *  priority = (2^24)*(type preference) +
 *             (2^8)*(local preference) +
 *             (2^0)*(256 - component ID)
 * 
 *  The type preference MUST be an integer from 0 to 126 inclusive, and
 *  represents the preference for the type of the candidate (where the
 *  types are local, server reflexive, peer reflexive, and relayed).  A
 *  126 is the highest preference, and a 0 is the lowest.  Setting the
 *  value to a 0 means that candidates of this type will only be used as
 *  a last resort.  The type preference MUST be identical for all
 *  candidates of the same type and MUST be different for candidates of
 *  different types.  The type preference for peer reflexive candidates
 *  MUST be higher than that of server reflexive candidates.  Note that
 *  candidates gathered based on the procedures of Section 4.1.1 will
 *  never be peer reflexive candidates; candidates of these type are
 *  learned from the connectivity checks performed by ICE.
 *
 *  The local preference MUST be an integer from 0 to 65535 inclusive.
 *  It represents a preference for the particular IP address from which
 *  the candidate was obtained, in cases where an agent is multihomed.
 *  65535 represents the highest preference, and a zero, the lowest.
 *  When there is only a single IP address, this value SHOULD be set to 65535. 
 *  More generally, if there are multiple candidates for a
 *  particular component for a particular media stream that have the same
 *  type, the local preference MUST be unique for each one.  In this
 *  specification, this only happens for multihomed hosts.  If a host is
 *  multihomed because it is dual stack, the local preference SHOULD be
 *  set equal to the precedence value for IP addresses described in RFC
 *  3484 [RFC3484].
 *
 *  The component ID is the component ID for the candidate, and MUST be
 *  between 1 and 256 inclusive.
 */

function getPriority(type: RTCIceCandidateType, 
                     protocol: RTCIceProtocol,
                     local_preference: number,
                     component: RTCIceComponent,
                     relay_protocol?: RTCIceProtocol): number 
{
  let type_preference = 0;

  switch (type) {
    case RTCIceCandidateType.host:
      type_preference = (protocol == RTCIceProtocol.tcp) ? 90 : 126;
      break;
    case RTCIceCandidateType.prflx:
      type_preference = (protocol == RTCIceProtocol.tcp) ? 80 : 110;
      break;
    case RTCIceCandidateType.srflx:
      type_preference = 100;
      break;
    case RTCIceCandidateType.relay:
      type_preference = (relay_protocol == RTCIceProtocol.udp) ? 2 :
                        (relay_protocol == RTCIceProtocol.tcp) ? 1 : 0;

      break;
  }

  return ((type_preference << 24) + 
         ((local_preference << 8) & 0x00ffffff) +
          (256 - ((component == RTCIceComponent.RTP) ? 1 : 2)));
}

function getFoundation(ip: string,
                       type: RTCIceCandidateType,
                       protocol: RTCIceProtocol,
                       relay_protocol?: RTCIceProtocol): number
{
  let str: string = (type == RTCIceCandidateType.host) ? 'host' : 
                    (type == RTCIceCandidateType.prflx) ? 'prflx' : 
                    (type == RTCIceCandidateType.srflx) ? 'srflx' : 
                    (type == RTCIceCandidateType.relay) ? 'relay' : ''; 

  str += ip;

  str += (protocol == RTCIceProtocol.tcp) ? 'tcp' : 
         (protocol == RTCIceProtocol.udp) ? 'udp' : '';

  str += (relay_protocol == RTCIceProtocol.tcp) ? 'tcp' : 
         (relay_protocol == RTCIceProtocol.udp) ? 'udp' : '';

  return CRC.crc32(Buffer.from(str));
}

export class RTCIceCandidate implements IceCandidate {
  constructor(foundation: string | number,
              ip: string,
              port: number,
              priority: number,
              protocol: RTCIceProtocol,
              type?: RTCIceCandidateType,
              relatedAddress?: string, 
              relatedPort?: number,
              tcpType?: RTCIceTcpCandidateType)
  {
    this.foundation = (typeof foundation == 'string') ? foundation : '' + foundation;
    this.ip = ip;
    this.port = port;
    this.priority = priority;
    this.protocol = protocol;
    this.type = type;
    this.relatedAddress = relatedAddress;
    this.relatedPort = relatedPort;
    this.tcpType = tcpType;
  }

  public static from(socket: Dgram.Socket | Net.Server | Net.Socket,  
                     component: RTCIceComponent,
                     type: RTCIceCandidateType,
                     protocol: RTCIceProtocol): RTCIceCandidate
  {
    const info = socket.address(); 
    let preference = getLocalPreference(info.family, info.address);    
    let priority = getPriority(type, protocol, preference, component);
    let foundation = getFoundation(info.address, type, protocol);

    let candidate: RTCIceCandidate = new RTCIceCandidate(foundation, 
                                                        info.address, 
                                                        info.port, 
                                                        priority, 
                                                        protocol, 
                                                        type);

    if (socket instanceof Net.Server) {
      candidate.tcpType = RTCIceTcpCandidateType.passive;
    } else if (socket instanceof Net.Socket) {
      candidate.tcpType = RTCIceTcpCandidateType.active;
    }

    return candidate;
  }

  public foundation: string;
  public ip: string;
  public port: number;  
  public priority: number;  
  public protocol: RTCIceProtocol;
  public relatedAddress?: string;  
  public relatedPort?: number;
  public tcpType?: RTCIceTcpCandidateType;  
  public type?: RTCIceCandidateType;
};

/** 
 * The RTCIceGatherer gathers local host, 
 * server reflexive and relay candidates, 
 * as well as enabling the retrieval of local Interactive Connectivity Establishment (ICE) parameters which can be exchanged in signaling. 
 * By enabling an endpoint to use a set of local candidates to construct multiple RTCIceTransport objects, the RTCIceGatherer enables support for scenarios such as parallel forking 
 */

export class RTCIceGatherer extends Events.EventEmitter implements RTCStatsProvider {
/*
 * onstatechange() => emit('statechange');
 * onerror(event: RTCIceGathererIceErrorEvent) => emit('error');
 * onlocalcandidate(event: RTCIceGathererEvent) => emit('localcandidate');
 */

  private _iceLite: boolean = false;
  private _icePwd: string;
  private _iceUfrag: string;
  private _iceServers: RTCIceServer[];
  private _iceLocalCandidates: RTCIceCandidate[] = [];
  private _policy: RTCIceGatherPolicy = RTCIceGatherPolicy.all;
  private _component: RTCIceComponent = RTCIceComponent.RTP;
  private _state: RTCIceGathererState = RTCIceGathererState.new;
  private _associated: RTCIceGatherer;

/**
  * An RTCIceGatherer instance is constructed from an RTCIceGatherOptions object.
  * 
  * An RTCIceGatherer object in the closed state can be garbage-collected when it is no longer referenced.
  *
  * To force enable iceLite option, set RTCIceServers length to zero.
  */

  constructor(options: RTCIceGatherOptions) {
    super();

    this._iceServers = options.iceServers;
    this._policy = RTCIceGatherPolicyFrom(options.gatherPolicy);

    this._iceUfrag = getRandomString(4);
    this._icePwd = getRandomString(24);
  }

  /**
    * The component-id of the RTCIceGatherer object. 
    * In RTCIceGatherer objects returned by createAssociatedGatherer() the value of the component attribute is rtcp. 
    * In all other RTCIceGatherer objects, the value of the component attribute is rtp.
    */

  public get component(): RTCIceComponent {
    return this._component;
  }

  /**
    * The current state of the ICE gatherer.
    */

  public get state(): RTCIceGathererState {
    return this._state;
  }

  /**
    * Prunes all local candidates, and closes the port. 
    * Associated RTCIceTransport objects transition to the disconnected state (unless they were in the failed state). 
    * Calling close() when state is closed has no effect.
    */

  public close(): void {
    if (this._state != RTCIceGathererState.closed) {
      setImmediate(() => {
        this._state = RTCIceGathererState.closed;
        this.emit('statechange', this);
      });
    } 
  }

  private gatherCandidate(validatedServer: ValidatedServers, callback: (complete: boolean) => void) {
    console.log('Gathering from:', validatedServer.url);
    callback(false);
  }

  private gatherCandidateFrom(validatedServers: ValidatedServers[], 
                              index: number, 
                              callback: () => void): void 
  {
    const policy = this._policy;

    if (index < validatedServers.length) {
      const validatedServer: ValidatedServers = validatedServers[index];

      if (policy == RTCIceGatherPolicy.relay && 
         (validatedServer.protocol != 'turn' && validatedServer.protocol != 'turns')) 
      {
        return this.gatherCandidateFrom(validatedServers, index + 1, callback);
      }

      this.gatherCandidate(validatedServer, (complete: boolean) => {
        if (!complete) {
          return this.gatherCandidateFrom(validatedServers, index + 1, callback);
        }

        callback();
      });      
    } else {
      callback();
    }
  }

  private gatherHostUdpCandidate(address: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      const socket = Dgram.createSocket('udp4');
      
      socket.on('error', (error: any) => {
        socket.close();

        if (error.errno != 'EINVAL') {
          return reject(error);
        }

        resolve();
      });

      socket.bind(0, address, () => {
        let event: RTCIceGathererEvent = {
          candidate: RTCIceCandidate.from(socket, 
                                          this._component, 
                                          RTCIceCandidateType.host,
                                          RTCIceProtocol.udp),
          url: 'host',
        };
      
        if (!this.emit('localcandidate', event)) {
          socket.close();
        }

        resolve();
      });

      socket.unref();
    });
  }

  private gatherHostTcpCandidate(address: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      const socket = Net.createServer();

      socket.on('error', (error: any) => {
        socket.close();

        if (error.errno != 'EADDRNOTAVAIL') {
          return reject(error);
        }

        resolve();
      });
      
      socket.listen(0, address, () => {
        let event: RTCIceGathererEvent = {
          candidate: RTCIceCandidate.from(socket, 
                                          this._component, 
                                          RTCIceCandidateType.host,
                                          RTCIceProtocol.tcp),
          url: 'host',
        };
      
        if (!this.emit('localcandidate', event)) {
          socket.close();
        }

        resolve();
      });

      socket.unref();
    });
  }

  /**
    * Gather ICE candidates. 
    * If options is omitted, utilize the value of options passed in the constructor. 
    * If state is closed, throw an InvalidStateError exception.
    *
    * To force enable iceLite option, set RTCIceServers length to zero.
    */

  public gather(options?: RTCIceGatherOptions): void {
    if (this._state == RTCIceGathererState.closed) {
      throw new Error('InvalidStateError');
    }

    if (options) {
      this._iceServers = options.iceServers;
      this._policy = RTCIceGatherPolicyFrom(options.gatherPolicy);
    }

    this._state = RTCIceGathererState.gathering;
    this.emit('statechange', this);

    let validatedServers: ValidatedServers[] = [];
    this._iceServers = (this._iceServers) ? this._iceServers : [];

    for (let iceServerIndex = 0; iceServerIndex < this._iceServers.length; iceServerIndex++) {
      const iceServer = this._iceServers[iceServerIndex];

      iceServer.urls = (iceServer.urls) ? 
                       (typeof iceServer.urls === 'string') ? 
                       [ iceServer.urls ] : iceServer.urls : 
                       [];

      for (let iceUrlIndex = 0; iceUrlIndex < iceServer.urls.length; iceUrlIndex++) {
        const iceUrl = iceServer.urls[iceUrlIndex];
        const url = iceUrl.split(':');

        if (!url || url.length < 2) {
          throw new Error('SyntaxError');
        }

        const scheme: string = url[0];
        const address: string = url[1];

        // The default port for STUN requests is 3478, for both TCP and UDP.
        // By default, TURN runs on the same ports as STUN: 3478 for TURN over
        // UDP and TCP, and 5349 for TURN over TLS.  However, TURN has its own
        // set of Service Record (SRV) names: "turn" for UDP and TCP, and
        // "turns" for TLS.

        let port: number = (url.length == 3) ? parseInt(url[2]) : 0;
        let cred: RTCOAuthCredential;

        switch (scheme) {
          case 'stun':
          case 'stuns':
            port = port ? port : 3478;
            break;
          case 'turn':
            port = port ? port : 3478;
          case 'turns':
            if (!port) { 
              port = 5349; 
            }
  
            if (iceServer.credentialType == RTCIceCredentialType.password) {
              if (!iceServer.username || 
                  !iceServer.credential || 
                  typeof iceServer.credential !== 'string') 
              {
                throw new Error('InvalidAccessError');
              }
            } else if (iceServer.credentialType == RTCIceCredentialType.oauth) {
              if (typeof iceServer.credential !== 'string') {
                cred = iceServer.credential;
  
                if (!cred.macKey || !cred.accessToken) {
                  throw new Error('InvalidAccessError');
                }
              } else {
                throw new Error('SyntaxError');
              }
            }

            break;
          default:
            throw new Error('NotSupportedError');
        }

        validatedServers.push({
          url: iceUrl,
          protocol: scheme,
          address: address,
          port: port,
          iceServer: iceServer,
        });
      }
    }

    let gatherers: Promise<void>[] = [];

    if (validatedServers.length) {
      validatedServers.sort((src: ValidatedServers, dst: ValidatedServers): number => {
        let srcScore = (src.protocol == 'stun') ? 4 :
                       (src.protocol == 'stuns') ? 3 :
                       (src.protocol == 'turns') ? 2 : 1;

        let dstScore = (dst.protocol == 'stun') ? 4 :
                       (dst.protocol == 'stuns') ? 3 :
                       (dst.protocol == 'turns') ? 2 : 1;
        
        return dstScore - srcScore;
      });
     
      gatherers.push(new Promise<void>((resolve, reject) => {   
        this.gatherCandidateFrom(validatedServers, 0, () => {
          resolve();
        });
      }));
    } else if (this._policy == RTCIceGatherPolicy.nohost || this._policy == RTCIceGatherPolicy.relay) {
      throw new Error(`RTCIceServers[]: can't be empty for policy "${RTCIceGatherPolicyToString(this._policy)}"`);
    } else {
      this._iceLite = true;
    }

    if (this._policy == RTCIceGatherPolicy.all) {
      let ifaces = OS.networkInterfaces();

      for (const name in ifaces) {
        const iface: OS.NetworkInterfaceInfo[] = ifaces[name];

        for (let index = 0; index < iface.length; index++) {
          const info = iface[index];

          if (info.internal) {
            continue;
          }

          gatherers.push(this.gatherHostTcpCandidate(info.address));
          gatherers.push(this.gatherHostUdpCandidate(info.address));
        }
      }
    }

    Promise.all(gatherers).then(() => {
      let event: RTCIceCandidateComplete = {
        complete: true,
      };

      this._state = RTCIceGathererState.complete;

      this.emit('localcandidate', event);
      this.emit('statechange', this);
    }, (error: any) => {
      // TODO(): This should never happen!
      assert.ifError(error);
    });
  }

  /**
    * getLocalParameters() obtains the ICE parameters of the RTCIceGatherer. 
    * If state is closed, throw an InvalidStateError exception.
    */

  public getLocalParameters(): RTCIceParameters {
    if (this._state == RTCIceGathererState.closed) {
      throw new Error('InvalidStateError');
    }

    return {
      iceLite: this._iceLite,
      password: this._icePwd,
      usernameFragment: this._iceUfrag,
    };
  }

  /**
    * Retrieve the sequence of valid local candidates associated with the RTCIceGatherer. 
    * This retrieves all unpruned local candidates currently known (except for peer reflexive candidates), 
    * even if an onlocalcandidate event hasn't been processed yet. 
    * Prior to calling gather() an empty list will be returned. 
    * If state is closed, throw an InvalidStateError exception.
    */

  public getLocalCandidates(): RTCIceCandidate[] {
    if (this._state == RTCIceGathererState.closed) {
      throw new Error('InvalidStateError');
    }
    
    return this._iceLocalCandidates;
  }

  /**
    * Create an associated RTCIceGatherer for RTCP, with the same RTCIceParameters and RTCIceGatherOptions. 
    * If state is closed, throw an InvalidStateError exception. 
    * If an RTCIceGatherer calls the method more than once, or if component is rtcp, 
    * throw an InvalidStateError exception.
    */

  public createAssociatedGatherer(): RTCIceGatherer {
    if (this._state == RTCIceGathererState.closed || 
        this._component == RTCIceComponent.RTCP ||
        this._associated) 
    {
      throw new Error('InvalidStateError');
    }

    let gatherer = new RTCIceGatherer({
      gatherPolicy: this._policy,
      iceServers: this._iceServers,
    });

    gatherer._component = RTCIceComponent.RTCP;
    gatherer._iceLite = this._iceLite;
    gatherer._icePwd = this._icePwd;
    gatherer._iceUfrag = this._iceUfrag;

    this._associated = gatherer;

    return gatherer;
  }

  public getStats(): Promise<RTCStatsReport> {
    return undefined;
  }
}