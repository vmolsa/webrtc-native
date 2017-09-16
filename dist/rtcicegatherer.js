"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const Events = require("events");
const OS = require("os");
const CRC = require("crc");
const Dgram = require("dgram");
const Net = require("net");
const ortc_1 = require("./ortc");
function once(callback) {
    let done = false;
    return () => {
        if (!done) {
            done = true;
            callback();
        }
    };
}
function getRandomString(len) {
    const iceChars = [
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    ];
    let str = '';
    for (let index = 0; index < len; index++) {
        const pos = ((Math.random() * 62) >>> 0) % 62;
        str += iceChars[pos];
    }
    return str;
}
function RTCIceGatherPolicyToString(policy) {
    switch (policy) {
        case ortc_1.RTCIceGatherPolicy.all:
            return 'all';
        case ortc_1.RTCIceGatherPolicy.nohost:
            return 'nohost';
        case ortc_1.RTCIceGatherPolicy.relay:
            return 'relay';
    }
}
function RTCIceGatherPolicyFrom(policy) {
    if (typeof policy == 'string') {
        switch (policy) {
            case 'relay':
                return ortc_1.RTCIceGatherPolicy.relay;
            case 'nohost':
                return ortc_1.RTCIceGatherPolicy.nohost;
            default:
                return ortc_1.RTCIceGatherPolicy.all;
        }
    }
    else {
        return policy;
    }
}
;
function getLocalPreference(family, address) {
    if (family == 'IPv4') {
        return 30;
    }
    return 40;
}
function getPriority(type, protocol, local_preference, component, relay_protocol) {
    let type_preference = 0;
    switch (type) {
        case ortc_1.RTCIceCandidateType.host:
            type_preference = (protocol == ortc_1.RTCIceProtocol.tcp) ? 90 : 126;
            break;
        case ortc_1.RTCIceCandidateType.prflx:
            type_preference = (protocol == ortc_1.RTCIceProtocol.tcp) ? 80 : 110;
            break;
        case ortc_1.RTCIceCandidateType.srflx:
            type_preference = 100;
            break;
        case ortc_1.RTCIceCandidateType.relay:
            type_preference = (relay_protocol == ortc_1.RTCIceProtocol.udp) ? 2 :
                (relay_protocol == ortc_1.RTCIceProtocol.tcp) ? 1 : 0;
            break;
    }
    return ((type_preference << 24) +
        ((local_preference << 8) & 0x00ffffff) +
        (256 - ((component == ortc_1.RTCIceComponent.RTP) ? 1 : 2)));
}
function getFoundation(ip, type, protocol, relay_protocol) {
    let str = (type == ortc_1.RTCIceCandidateType.host) ? 'host' :
        (type == ortc_1.RTCIceCandidateType.prflx) ? 'prflx' :
            (type == ortc_1.RTCIceCandidateType.srflx) ? 'srflx' :
                (type == ortc_1.RTCIceCandidateType.relay) ? 'relay' : '';
    str += ip;
    str += (protocol == ortc_1.RTCIceProtocol.tcp) ? 'tcp' :
        (protocol == ortc_1.RTCIceProtocol.udp) ? 'udp' : '';
    str += (relay_protocol == ortc_1.RTCIceProtocol.tcp) ? 'tcp' :
        (relay_protocol == ortc_1.RTCIceProtocol.udp) ? 'udp' : '';
    return CRC.crc32(Buffer.from(str));
}
function RTCIceCandidateFrom(socket, component, type, protocol) {
    const info = socket.address();
    let preference = getLocalPreference(info.family, info.address);
    let priority = getPriority(type, protocol, preference, component);
    let foundation = '' + getFoundation(info.address, type, protocol);
    let candidate = {
        foundation: foundation,
        ip: info.address,
        port: info.port,
        priority: priority,
        protocol: protocol,
        type: type,
    };
    if (socket instanceof Net.Server) {
        candidate.tcpType = ortc_1.RTCIceTcpCandidateType.passive;
    }
    else if (socket instanceof Net.Socket) {
        candidate.tcpType = ortc_1.RTCIceTcpCandidateType.active;
    }
    return candidate;
}
exports.RTCIceCandidateFrom = RTCIceCandidateFrom;
class RTCIceGatherer extends Events.EventEmitter {
    constructor(options) {
        super();
        this._iceLite = false;
        this._iceLocalCandidates = [];
        this._policy = ortc_1.RTCIceGatherPolicy.all;
        this._component = ortc_1.RTCIceComponent.RTP;
        this._state = ortc_1.RTCIceGathererState.new;
        this._iceServers = options.iceServers;
        this._policy = RTCIceGatherPolicyFrom(options.gatherPolicy);
        this._iceUfrag = getRandomString(4);
        this._icePwd = getRandomString(24);
    }
    get component() {
        return this._component;
    }
    get state() {
        return this._state;
    }
    close() {
        if (this._state != ortc_1.RTCIceGathererState.closed) {
            setImmediate(() => {
                this._state = ortc_1.RTCIceGathererState.closed;
                this.emit('statechange', {
                    state: ortc_1.RTCIceGathererState.closed
                });
            });
        }
    }
    gatherStunCandidate(validatedServer, callback) {
        console.log('Gathering Stun Candidate');
        callback(false);
    }
    gatherTurnCandidate(validatedServer, callback) {
        console.log('Gathering Turn Candidate');
        callback(false);
    }
    gatherCandidateFrom(validatedServers, index) {
        return new Promise((resolve, reject) => {
            const policy = this._policy;
            if (index < validatedServers.length) {
                const validatedServer = validatedServers[index];
                if (policy == ortc_1.RTCIceGatherPolicy.relay &&
                    (validatedServer.protocol != 'turn' && validatedServer.protocol != 'turns')) {
                    return resolve(this.gatherCandidateFrom(validatedServers, index + 1));
                }
                if (validatedServer.protocol == 'stun' || validatedServer.protocol == 'stuns') {
                    return this.gatherStunCandidate(validatedServer, (complete) => {
                        if (!complete) {
                            return resolve(this.gatherCandidateFrom(validatedServers, index + 1));
                        }
                        resolve();
                    });
                }
                return this.gatherTurnCandidate(validatedServer, (complete) => {
                    if (!complete) {
                        return resolve(this.gatherCandidateFrom(validatedServers, index + 1));
                    }
                    resolve();
                });
            }
            else {
                resolve();
            }
        });
    }
    gatherHostUdpCandidate(address) {
        return new Promise((resolve, reject) => {
            const socket = Dgram.createSocket('udp4');
            socket.on('error', (error) => {
                socket.close();
                resolve();
            });
            socket.bind(0, address, () => {
                let event = {
                    candidate: RTCIceCandidateFrom(socket, this._component, ortc_1.RTCIceCandidateType.host, ortc_1.RTCIceProtocol.udp),
                    url: 'host',
                };
                socket.unref();
                if (!this.emit('localcandidate', event)) {
                    socket.close();
                }
                resolve();
            });
        });
    }
    gatherHostTcpCandidate(address) {
        return new Promise((resolve, reject) => {
            const socket = Net.createServer();
            socket.on('error', (error) => {
                socket.close();
                resolve();
            });
            socket.listen(0, address, () => {
                let event = {
                    candidate: RTCIceCandidateFrom(socket, this._component, ortc_1.RTCIceCandidateType.host, ortc_1.RTCIceProtocol.tcp),
                    url: 'host',
                };
                socket.unref();
                if (!this.emit('localcandidate', event)) {
                    socket.close();
                }
                resolve();
            });
        });
    }
    gather(options) {
        if (this._state == ortc_1.RTCIceGathererState.closed) {
            throw new Error('InvalidStateError');
        }
        if (options) {
            this._iceServers = options.iceServers;
            this._policy = RTCIceGatherPolicyFrom(options.gatherPolicy);
        }
        let promises = [];
        if (!this._iceLite) {
            let validatedServers = [];
            this._iceServers = (this._iceServers) ? this._iceServers : [];
            for (let iceServerIndex = 0; iceServerIndex < this._iceServers.length; iceServerIndex++) {
                const iceServer = this._iceServers[iceServerIndex];
                iceServer.urls = (iceServer.urls) ?
                    (typeof iceServer.urls === 'string') ?
                        [iceServer.urls] : iceServer.urls :
                    [];
                for (let iceUrlIndex = 0; iceUrlIndex < iceServer.urls.length; iceUrlIndex++) {
                    const iceUrl = iceServer.urls[iceUrlIndex];
                    const url = iceUrl.split(':');
                    if (!url || url.length < 2) {
                        throw new Error('SyntaxError');
                    }
                    const scheme = url[0];
                    const address = url[1];
                    let port = (url.length == 3) ? parseInt(url[2]) : 0;
                    let cred;
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
                            if (iceServer.credentialType == ortc_1.RTCIceCredentialType.password) {
                                if (!iceServer.username ||
                                    !iceServer.credential ||
                                    typeof iceServer.credential !== 'string') {
                                    throw new Error('InvalidAccessError');
                                }
                            }
                            else if (iceServer.credentialType == ortc_1.RTCIceCredentialType.oauth) {
                                if (typeof iceServer.credential !== 'string') {
                                    cred = iceServer.credential;
                                    if (!cred.macKey || !cred.accessToken) {
                                        throw new Error('InvalidAccessError');
                                    }
                                }
                                else {
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
            if (validatedServers.length) {
                validatedServers.sort((src, dst) => {
                    let srcScore = (src.protocol == 'stun') ? 4 :
                        (src.protocol == 'stuns') ? 3 :
                            (src.protocol == 'turns') ? 2 : 1;
                    let dstScore = (dst.protocol == 'stun') ? 4 :
                        (dst.protocol == 'stuns') ? 3 :
                            (dst.protocol == 'turns') ? 2 : 1;
                    return dstScore - srcScore;
                });
                promises.push(this.gatherCandidateFrom(validatedServers, 0));
            }
            else if (this._policy == ortc_1.RTCIceGatherPolicy.nohost || this._policy == ortc_1.RTCIceGatherPolicy.relay) {
                throw new Error(`RTCIceServers[]: can't be empty for policy "${RTCIceGatherPolicyToString(this._policy)}"`);
            }
            else {
                this._iceLite = true;
            }
        }
        if (this._policy == ortc_1.RTCIceGatherPolicy.all) {
            const ifaces = OS.networkInterfaces();
            for (const dev in ifaces) {
                const iface = ifaces[dev];
                iface.forEach((address) => {
                    if ((address.family == 'IPv4' || address.family == 'IPv6') && !address.internal) {
                        promises.push(this.gatherHostUdpCandidate(address.address));
                    }
                });
            }
        }
        if (promises.length) {
            this._state == ortc_1.RTCIceGathererState.gathering;
            this.emit('statechange', {
                state: ortc_1.RTCIceGathererState.gathering
            });
        }
        Promise.all(promises).then(() => {
            let event = {
                complete: true,
            };
            this._state == ortc_1.RTCIceGathererState.complete;
            this.emit('localcandidate', event);
            this.emit('statechange', {
                state: ortc_1.RTCIceGathererState.complete
            });
        }, (error) => {
            error = undefined;
        });
    }
    getLocalParameters() {
        if (this._state == ortc_1.RTCIceGathererState.closed) {
            throw new Error('InvalidStateError');
        }
        return {
            iceLite: this._iceLite,
            password: this._icePwd,
            usernameFragment: this._iceUfrag,
        };
    }
    getLocalCandidates() {
        if (this._state == ortc_1.RTCIceGathererState.closed) {
            throw new Error('InvalidStateError');
        }
        return this._iceLocalCandidates;
    }
    createAssociatedGatherer() {
        if (this._state == ortc_1.RTCIceGathererState.closed ||
            this._component == ortc_1.RTCIceComponent.RTCP ||
            this._associated) {
            throw new Error('InvalidStateError');
        }
        let gatherer = new RTCIceGatherer({
            gatherPolicy: this._policy,
            iceServers: this._iceServers,
        });
        gatherer._component = ortc_1.RTCIceComponent.RTCP;
        gatherer._iceLite = this._iceLite;
        gatherer._icePwd = this._icePwd;
        gatherer._iceUfrag = this._iceUfrag;
        this._associated = gatherer;
        return gatherer;
    }
    getStats() {
        return undefined;
    }
}
exports.RTCIceGatherer = RTCIceGatherer;
//# sourceMappingURL=rtcicegatherer.js.map