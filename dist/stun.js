"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const IP = require("ipaddr.js");
const assert = require("assert");
const Crypto = require("crypto");
const CRC = require("crc");
var Stun;
(function (Stun) {
    Stun.PacketSize = 1280;
    Stun.MagicCookie = 0x2112A442;
    Stun.FingerPrint = 0x5354554e;
    var AddressFamily;
    (function (AddressFamily) {
        AddressFamily[AddressFamily["IPv4"] = 1] = "IPv4";
        AddressFamily[AddressFamily["IPv6"] = 2] = "IPv6";
    })(AddressFamily = Stun.AddressFamily || (Stun.AddressFamily = {}));
    ;
    var Type;
    (function (Type) {
        Type[Type["FULL_CONE"] = 1] = "FULL_CONE";
        Type[Type["RESTRICTED_CONE"] = 2] = "RESTRICTED_CONE";
        Type[Type["PORT_RESTRICTED_CONE"] = 3] = "PORT_RESTRICTED_CONE";
        Type[Type["SYMMETRIC"] = 4] = "SYMMETRIC";
    })(Type = Stun.Type || (Stun.Type = {}));
    ;
    var MessageType;
    (function (MessageType) {
        MessageType[MessageType["BINDING_REQUEST"] = 1] = "BINDING_REQUEST";
        MessageType[MessageType["BINDING_RESPONSE"] = 257] = "BINDING_RESPONSE";
        MessageType[MessageType["BINDING_ERROR_RESPONSE"] = 273] = "BINDING_ERROR_RESPONSE";
        MessageType[MessageType["BINDING_INDICATION"] = 17] = "BINDING_INDICATION";
        MessageType[MessageType["SHARED_SECRET_REQUEST"] = 2] = "SHARED_SECRET_REQUEST";
        MessageType[MessageType["SHARED_SECRET_RESPONSE"] = 258] = "SHARED_SECRET_RESPONSE";
        MessageType[MessageType["SHARED_SECRET_ERROR_RESPONSE"] = 274] = "SHARED_SECRET_ERROR_RESPONSE";
    })(MessageType = Stun.MessageType || (Stun.MessageType = {}));
    ;
    var Attributes;
    (function (Attributes) {
        Attributes[Attributes["MAPPED_ADDRESS"] = 1] = "MAPPED_ADDRESS";
        Attributes[Attributes["XOR_MAPPED_ADDRESS"] = 32] = "XOR_MAPPED_ADDRESS";
        Attributes[Attributes["RESPONSE_ADDRESS"] = 2] = "RESPONSE_ADDRESS";
        Attributes[Attributes["CHANGE_REQUEST"] = 3] = "CHANGE_REQUEST";
        Attributes[Attributes["SOURCE_ADDRESS"] = 4] = "SOURCE_ADDRESS";
        Attributes[Attributes["CHANGED_ADDRESS"] = 5] = "CHANGED_ADDRESS";
        Attributes[Attributes["USERNAME"] = 6] = "USERNAME";
        Attributes[Attributes["PASSWORD"] = 7] = "PASSWORD";
        Attributes[Attributes["MESSAGE_INTEGRITY"] = 8] = "MESSAGE_INTEGRITY";
        Attributes[Attributes["ERROR_CODE"] = 9] = "ERROR_CODE";
        Attributes[Attributes["UNKNOWN_ATTRIBUTES"] = 10] = "UNKNOWN_ATTRIBUTES";
        Attributes[Attributes["REFLECTED_FROM"] = 11] = "REFLECTED_FROM";
        Attributes[Attributes["REALM"] = 20] = "REALM";
        Attributes[Attributes["NONCE"] = 21] = "NONCE";
        Attributes[Attributes["SOFTWARE"] = 32802] = "SOFTWARE";
        Attributes[Attributes["ALTERNATE_SERVER"] = 32803] = "ALTERNATE_SERVER";
        Attributes[Attributes["FINGERPRINT"] = 32808] = "FINGERPRINT";
        Attributes[Attributes["PRIORITY"] = 36] = "PRIORITY";
        Attributes[Attributes["USE_CANDIDATE"] = 37] = "USE_CANDIDATE";
        Attributes[Attributes["ICE_CONTROLLED"] = 32809] = "ICE_CONTROLLED";
        Attributes[Attributes["ICE_CONTROLLING"] = 32810] = "ICE_CONTROLLING";
    })(Attributes = Stun.Attributes || (Stun.Attributes = {}));
    ;
    Stun.ErrorCode = {
        400: 'Bad Request',
        401: 'Unauthorized',
        420: 'Unknown Attribute',
        430: 'Stale Credentials',
        431: 'Integrity Check Failure',
        432: 'Missing Username',
        433: 'Use TLS',
        487: 'Role Conflict',
        500: 'Server Error',
        600: 'Global Failure',
    };
    ;
    class Packet {
        constructor(type, RFC5389) {
            this.type = MessageType.BINDING_REQUEST;
            this.attrs = {};
            this.tid = Packet.generateTransactionID(RFC5389);
            this.type = type;
        }
        transactionId(rfc5389 = true) {
            if (rfc5389) {
                return [
                    this.tid[1],
                    this.tid[2],
                    this.tid[3]
                ];
            }
            return this.tid;
        }
        getAttributes() {
            let ret = [];
            for (const attr in this.attrs) {
                if (Packet.codec[attr]) {
                    ret.push(Packet.codec[attr].name);
                }
            }
            return ret;
        }
        get isRFC3489() {
            return (this.tid[0] !== 0x2112A442);
        }
        get isRFC5389() {
            return (this.tid[0] === 0x2112A442);
        }
        static isRequest(msg_type) {
            return (((msg_type) & 0x0110) == 0x0000);
        }
        static isIndication(msg_type) {
            return (((msg_type) & 0x0110) == 0x0010);
        }
        static isResponse(msg_type) {
            return (((msg_type) & 0x0110) == 0x0100);
        }
        static isError(msg_type) {
            return (((msg_type) & 0x0110) == 0x0110);
        }
        addAttribute(type, value) {
            if (typeof type === 'string') {
                type = Packet.codec[type] ? Packet.codec[type].attribute : type;
            }
            this.attrs[type] = value;
        }
        getAttribute(type) {
            if (typeof type === 'string') {
                type = Packet.codec[type] ? Packet.codec[type].attribute : type;
            }
            return this.attrs[type];
        }
        removeAttribute(type) {
            if (typeof type === 'string') {
                type = Packet.codec[type] ? Packet.codec[type].attribute : type;
            }
            this.attrs[type] = undefined;
        }
        static validateMessageIntegrity(packet, message, password) {
            if (!packet || !Buffer.isBuffer(message) || !password || !password.length) {
                return false;
            }
            const headerLength = Packet.readHeaderLength(message);
            if ((headerLength + 20) != message.byteLength) {
                return false;
            }
            const size = message.byteLength;
            const msg = packet.getAttribute(Attributes.MESSAGE_INTEGRITY);
            if (Buffer.isBuffer(msg) && msg.byteLength == 20) {
                let current_pos = 20;
                while (current_pos + 4 <= size) {
                    const attr_type = message.readUInt16BE(current_pos);
                    const attr_length = message.readUInt16BE(current_pos + 2);
                    if (attr_type == Attributes.MESSAGE_INTEGRITY) {
                        if (attr_length != 20 || current_pos + 4 + attr_length > size) {
                            return false;
                        }
                        break;
                    }
                    current_pos += 4 + attr_length;
                    if ((attr_length % 4) != 0) {
                        current_pos += (4 - (attr_length % 4));
                    }
                }
                const tmp = Buffer.from(message.slice(0, current_pos));
                if (size > (current_pos + 24)) {
                    let offset = size - (current_pos + 24);
                    let length = size - offset - 20;
                    tmp.writeUInt16BE(length, 2);
                }
                const hmac = Crypto.createHmac('sha1', password);
                hmac.update(tmp);
                const digest = hmac.digest();
                return digest.equals(msg);
            }
            return false;
        }
        static validateFingerPrint(packet, buffer) {
            const finger = packet.getAttribute(Attributes.FINGERPRINT);
            if (finger) {
                let value = CRC.crc32(buffer.slice(0, -8));
                return (((finger ^ Stun.FingerPrint) >>> 0) === value) ? true : false;
            }
            return false;
        }
        static encode(packet, password, addFingerprint) {
            let buffer = Buffer.alloc(Stun.PacketSize);
            let msgLength = 20;
            buffer.writeUInt16BE(packet.type & 0xffff, 0);
            buffer.writeUInt32BE(packet.tid[0], 4);
            buffer.writeUInt32BE(packet.tid[1], 8);
            buffer.writeUInt32BE(packet.tid[2], 12);
            buffer.writeUInt32BE(packet.tid[3], 16);
            let attr_len = 0;
            for (const name in packet.attrs) {
                let attr;
                if (Packet.codec[name]) {
                    attr = Packet.codec[name].attribute;
                }
                else {
                    attr = parseInt(name, 16);
                }
                if (!Attributes[attr] ||
                    attr == Attributes.MESSAGE_INTEGRITY ||
                    attr == Attributes.FINGERPRINT) {
                    continue;
                }
                msgLength += Packet.writeAttribute(attr, buffer, msgLength, packet.attrs[name], packet.tid);
            }
            buffer.writeUInt16BE((msgLength - 20), 2);
            if (!packet.isRFC3489) {
                if (password && password.length) {
                    msgLength += Packet.writeAttribute(Attributes.MESSAGE_INTEGRITY, buffer, msgLength, Buffer.alloc(20, '0'), packet.tid);
                    buffer.writeUInt16BE((msgLength - 20), 2);
                    const hmac = Crypto.createHmac('sha1', password);
                    hmac.update(buffer.slice(0, msgLength - 24));
                    const digest = hmac.digest();
                    digest.copy(buffer, msgLength - 20);
                }
                if (addFingerprint) {
                    const attrPos = msgLength;
                    const valuePos = msgLength + 4;
                    msgLength += Packet.writeAttribute(Attributes.FINGERPRINT, buffer, msgLength, 0, packet.tid);
                    buffer.writeUInt16BE((msgLength - 20), 2);
                    let crc32 = CRC.crc32(buffer.slice(0, attrPos));
                    let finger = (crc32 ^ Stun.FingerPrint) >>> 0;
                    buffer.writeUInt32BE(finger, valuePos);
                }
            }
            return buffer.slice(0, msgLength);
        }
        static decode(buffer) {
            let header = buffer.readUInt32BE(0);
            let msgLength = header & 0xffff;
            if ((msgLength % 4 != 0) ||
                buffer.byteLength > Stun.PacketSize ||
                msgLength + 20 != buffer.byteLength ||
                buffer.byteLength < 20) {
                return undefined;
            }
            let type = (header >> 16) & 0x0fff;
            if (!MessageType[type]) {
                return undefined;
            }
            let packet = new Packet(type);
            packet.tid[0] = buffer.readUInt32BE(4);
            packet.tid[1] = buffer.readUInt32BE(8);
            packet.tid[2] = buffer.readUInt32BE(12);
            packet.tid[3] = buffer.readUInt32BE(16);
            let offset = 20;
            while (offset < buffer.byteLength) {
                assert((buffer.byteLength - offset) >= 4);
                header = buffer.readUInt32BE(offset);
                let length = (header & 0xffff);
                let attr = (header >> 16) & 0xffff;
                const codec = Packet.codec[attr];
                if (codec && codec.decode) {
                    packet.addAttribute(attr, codec.decode(buffer, offset + 4, length, packet.tid));
                }
                offset += ((length % 4) == 0) ? 4 + length : length + (8 - (length % 4));
            }
            return packet;
        }
        static generateCredentialHash(username, realm, password) {
            const hash = Crypto.createHash('md5');
            hash.update('' + username + ':' + realm + ':' + password);
            return hash.digest();
        }
        static generateTransactionID(rfc5389 = true) {
            return [
                (rfc5389) ? 0x2112A442 : (Math.random() * 0xffffff) >>> 0,
                (Math.random() * 0xffffff) >>> 0,
                (Math.random() * 0xffffff) >>> 0,
                (Math.random() * 0xffffff) >>> 0,
            ];
        }
        static readHeaderLength(buffer) {
            return buffer.readUInt16BE(2);
        }
        static writeHeaderLength(msgLength, buffer) {
            buffer.writeUInt16BE(msgLength & 0xffff, 2);
        }
        static writeAttributeHeader(attr, length, buffer, offset) {
            return buffer.writeUInt32BE(((attr << 16) | length & 0xffff) >>> 0, offset);
        }
        static writeAttribute(attr, buffer, offset, value, tid) {
            const codec = Packet.codec[attr];
            assert(codec);
            const ret = Packet.codec[attr].encode(buffer, offset + 4, value, tid);
            buffer.writeUInt32BE(((attr << 16) | ret & 0xffff) >>> 0, offset);
            return ((ret % 4) == 0) ? 4 + ret : ret + (8 - (ret % 4));
        }
        static decodeAddress(buffer, offset, length, tid) {
            let header = buffer.readUInt32BE(offset);
            let family = (header >> 16) & 0xff;
            assert((family == AddressFamily.IPv4 && length == 8) ||
                (family == AddressFamily.IPv6 && length == 20));
            let address = buffer.slice(offset + 4, offset + length);
            return {
                port: header & 0xffff,
                family: (family == AddressFamily.IPv4) ? 'IPv4' : 'IPv6',
                address: IP.fromByteArray(address).toString(),
            };
        }
        static encodeAddress(buffer, offset, value, tid) {
            assert(value);
            assert(value.address);
            assert(value.family);
            assert(value.port);
            const family = (value.family == 'IPv6') ? AddressFamily.IPv6 : AddressFamily.IPv4;
            const port = value.port;
            const address = Buffer.from(IP.parse(value.address).toByteArray());
            assert(address.byteLength == ((family == AddressFamily.IPv6) ? 16 : 4));
            buffer.writeUInt32BE(((family << 16) | port & 0xffff), offset);
            address.copy(buffer, offset + 4);
            return ((family == AddressFamily.IPv6) ? 20 : 8);
        }
        ;
        static decodeByteString(buffer, offset, length, tid) {
            return buffer.slice(offset, offset + length);
        }
        static encodeByteString(buffer, offset, value, tid) {
            assert(Buffer.isBuffer(value));
            const byteLength = value.byteLength;
            let ret = value.copy(buffer, offset);
            if (byteLength % 4 != 0) {
                buffer.fill(0x20, offset + ret, offset + ret + (4 - (byteLength % 4)));
            }
            return ret;
        }
        static decodeUint16List(buffer, offset, length, tid) {
            let res = [];
            const end = length + offset;
            for (let index = offset; index < end; index += 2) {
                res.push(buffer.readUInt16BE(index));
            }
            return res;
        }
        static encodeUint16List(buffer, offset, value, tid) {
            let res = offset;
            let last = 0x0000;
            assert(value && value.length);
            value.forEach(attr => {
                res = buffer.writeUInt16BE((attr & 0xffff), res);
                last = attr;
            });
            if (value.length % 4 != 0) {
                for (let index = (4 - (value.length % 4)); index > 0; index--) {
                    res = buffer.writeUInt16BE((last & 0xffff), res);
                }
            }
            return value.length * 2;
        }
        static decodeUint32(buffer, offset, length, tid) {
            assert(length == 4);
            return buffer.readUInt32BE(offset);
        }
        static encodeUint32(buffer, offset, value, tid) {
            buffer.writeUInt32BE(value, offset);
            return 4;
        }
        static decodeUint64(buffer, offset, length, tid) {
            assert(length == 8);
            return [
                buffer.readUInt32BE(offset),
                buffer.readUInt32BE(offset + 4),
            ];
        }
        static encodeUint64(buffer, offset, value, tid) {
            assert(value && value.length == 2);
            buffer.writeUInt32BE(value[0], offset);
            buffer.writeUInt32BE(value[1], offset + 4);
            return 8;
        }
        static decodeEmptyValue(buffer, offset, length, tid) {
            return true;
        }
        static encodeEmptyValue(buffer, offset, value, tid) {
            return 0;
        }
        static registerCodec(codec) {
            Packet.codec[codec.name] = codec;
            Packet.codec[codec.attribute] = codec;
        }
    }
    Packet.codec = {};
    Stun.Packet = Packet;
    ;
    Packet.registerCodec({
        name: 'MAPPED_ADDRESS',
        attribute: Attributes.MAPPED_ADDRESS,
        decode: Packet.decodeAddress,
        encode: Packet.encodeAddress,
    });
    Packet.registerCodec({
        name: 'XOR_MAPPED_ADDRESS',
        attribute: Attributes.XOR_MAPPED_ADDRESS,
        decode: (buffer, offset, length, tid) => {
            let header = buffer.readUInt32BE(offset);
            let family = (header >> 16) & 0xff;
            assert((family == AddressFamily.IPv4 && length == 8) ||
                (family == AddressFamily.IPv6 && length == 20));
            let address = buffer.slice(offset + 4, offset + length);
            if (family == AddressFamily.IPv4) {
                address.writeUInt32BE((address.readUInt32BE(0) ^ Stun.MagicCookie) >>> 0, 0);
            }
            else {
                address.writeUInt32BE((address.readUInt32BE(0) ^ Stun.MagicCookie) >>> 0, 0);
                address.writeUInt32BE((address.readUInt32BE(4) ^ tid[1]) >>> 0, 4);
                address.writeUInt32BE((address.readUInt32BE(8) ^ tid[2]) >>> 0, 8);
                address.writeUInt32BE((address.readUInt32BE(12) ^ tid[3]) >>> 0, 12);
            }
            return {
                port: (header & 0xffff) ^ (Stun.MagicCookie >> 16),
                family: (family == AddressFamily.IPv4) ? 'IPv4' : 'IPv6',
                address: IP.fromByteArray(address).toString(),
            };
        },
        encode: (buffer, offset, value, tid) => {
            assert(value);
            assert(value.address);
            assert(value.family);
            assert(value.port);
            const family = (value.family == 'IPv6') ? AddressFamily.IPv6 : AddressFamily.IPv4;
            const port = ((value.port & 0xffff) ^ (Stun.MagicCookie >> 16));
            const address = Buffer.from(IP.parse(value.address).toByteArray());
            assert(address.byteLength == ((family == AddressFamily.IPv6) ? 16 : 4));
            buffer.writeUInt32BE(((family << 16) | port & 0xffff), offset);
            if (family == AddressFamily.IPv4) {
                address.writeUInt32BE((address.readUInt32BE(0) ^ Stun.MagicCookie) >>> 0, 0);
            }
            else {
                address.writeUInt32BE((address.readUInt32BE(0) ^ Stun.MagicCookie) >>> 0, 0);
                address.writeUInt32BE((address.readUInt32BE(4) ^ tid[1]) >>> 0, 4);
                address.writeUInt32BE((address.readUInt32BE(8) ^ tid[2]) >>> 0, 8);
                address.writeUInt32BE((address.readUInt32BE(12) ^ tid[3]) >>> 0, 12);
            }
            address.copy(buffer, offset + 4);
            return ((family == AddressFamily.IPv6) ? 20 : 8);
        },
    });
    Packet.registerCodec({
        name: 'RESPONSE_ADDRESS',
        attribute: Attributes.RESPONSE_ADDRESS,
        decode: Packet.decodeAddress,
        encode: Packet.encodeAddress,
    });
    Packet.registerCodec({
        name: 'CHANGE_REQUEST',
        attribute: Attributes.CHANGE_REQUEST,
        decode: Packet.decodeAddress,
        encode: Packet.encodeAddress,
    });
    Packet.registerCodec({
        name: 'SOURCE_ADDRESS',
        attribute: Attributes.SOURCE_ADDRESS,
        decode: Packet.decodeAddress,
        encode: Packet.encodeAddress,
    });
    Packet.registerCodec({
        name: 'CHANGED_ADDRESS',
        attribute: Attributes.CHANGED_ADDRESS,
        decode: Packet.decodeAddress,
        encode: Packet.encodeAddress,
    });
    Packet.registerCodec({
        name: 'USERNAME',
        attribute: Attributes.USERNAME,
        decode: Packet.decodeByteString,
        encode: Packet.encodeByteString,
    });
    Packet.registerCodec({
        name: 'PASSWORD',
        attribute: Attributes.PASSWORD,
        decode: Packet.decodeByteString,
        encode: Packet.encodeByteString,
    });
    Packet.registerCodec({
        name: 'MESSAGE_INTEGRITY',
        attribute: Attributes.MESSAGE_INTEGRITY,
        decode: Packet.decodeByteString,
        encode: Packet.encodeByteString,
    });
    Packet.registerCodec({
        name: 'ERROR_CODE',
        attribute: Attributes.ERROR_CODE,
        decode: (buffer, offset, length, tid) => {
            let header = buffer.readUInt32BE(offset);
            assert((header >> 11) == 0);
            let errorClass = ((header >> 8) & 0x7);
            let code = (header & 0xff);
            let reason = buffer.toString('utf8', offset + 4, offset + length);
            return {
                code: errorClass * 100 + code,
                reason: reason,
            };
        },
        encode: (buffer, offset, value, tid) => {
            assert(value);
            assert(value.code);
            if (!value.reason) {
                value.reason = Stun.ErrorCode[value.code];
            }
            assert(value.reason);
            assert(Buffer.byteLength(value.reason));
            buffer.writeUInt32BE((((value.code / 100) >>> 0) << 8 | value.code % 100), offset);
            return buffer.write(value.reason, offset + 4) + 4;
        },
    });
    Packet.registerCodec({
        name: 'UNKNOWN_ATTRIBUTES',
        attribute: Attributes.UNKNOWN_ATTRIBUTES,
        decode: Packet.decodeUint16List,
        encode: Packet.encodeUint16List,
    });
    Packet.registerCodec({
        name: 'REFLECTED_FROM',
        attribute: Attributes.REFLECTED_FROM,
        decode: Packet.decodeAddress,
        encode: Packet.encodeAddress,
    });
    Packet.registerCodec({
        name: 'REALM',
        attribute: Attributes.REALM,
        decode: Packet.decodeByteString,
        encode: Packet.encodeByteString,
    });
    Packet.registerCodec({
        name: 'NONCE',
        attribute: Attributes.NONCE,
        decode: Packet.decodeByteString,
        encode: Packet.encodeByteString,
    });
    Packet.registerCodec({
        name: 'SOFTWARE',
        attribute: Attributes.SOFTWARE,
        decode: Packet.decodeByteString,
        encode: Packet.encodeByteString,
    });
    Packet.registerCodec({
        name: 'ALTERNATE_SERVER',
        attribute: Attributes.ALTERNATE_SERVER,
        decode: Packet.decodeAddress,
        encode: Packet.encodeAddress,
    });
    Packet.registerCodec({
        name: 'FINGERPRINT',
        attribute: Attributes.FINGERPRINT,
        decode: Packet.decodeUint32,
        encode: Packet.encodeUint32,
    });
    Packet.registerCodec({
        name: 'PRIORITY',
        attribute: Attributes.PRIORITY,
        decode: Packet.decodeUint32,
        encode: Packet.encodeUint32,
    });
    Packet.registerCodec({
        name: 'USE_CANDIDATE',
        attribute: Attributes.USE_CANDIDATE,
        decode: Packet.decodeEmptyValue,
        encode: Packet.encodeEmptyValue,
    });
    Packet.registerCodec({
        name: 'ICE_CONTROLLED',
        attribute: Attributes.ICE_CONTROLLED,
        decode: Packet.decodeUint64,
        encode: Packet.encodeUint64,
    });
    Packet.registerCodec({
        name: 'ICE_CONTROLLING',
        attribute: Attributes.ICE_CONTROLLING,
        decode: Packet.decodeUint64,
        encode: Packet.encodeUint64,
    });
})(Stun = exports.Stun || (exports.Stun = {}));
;
//# sourceMappingURL=stun.js.map