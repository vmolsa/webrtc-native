/// <reference types="node" />
export declare namespace Stun {
    var PacketSize: number;
    var MagicCookie: number;
    var FingerPrint: number;
    enum AddressFamily {
        IPv4 = 1,
        IPv6 = 2,
    }
    enum Type {
        FULL_CONE = 1,
        RESTRICTED_CONE = 2,
        PORT_RESTRICTED_CONE = 3,
        SYMMETRIC = 4,
    }
    enum MessageType {
        BINDING_REQUEST = 1,
        BINDING_RESPONSE = 257,
        BINDING_ERROR_RESPONSE = 273,
        BINDING_INDICATION = 17,
        SHARED_SECRET_REQUEST = 2,
        SHARED_SECRET_RESPONSE = 258,
        SHARED_SECRET_ERROR_RESPONSE = 274,
    }
    enum Attributes {
        MAPPED_ADDRESS = 1,
        XOR_MAPPED_ADDRESS = 32,
        RESPONSE_ADDRESS = 2,
        CHANGE_REQUEST = 3,
        SOURCE_ADDRESS = 4,
        CHANGED_ADDRESS = 5,
        USERNAME = 6,
        PASSWORD = 7,
        MESSAGE_INTEGRITY = 8,
        ERROR_CODE = 9,
        UNKNOWN_ATTRIBUTES = 10,
        REFLECTED_FROM = 11,
        REALM = 20,
        NONCE = 21,
        SOFTWARE = 32802,
        ALTERNATE_SERVER = 32803,
        FINGERPRINT = 32808,
        PRIORITY = 36,
        USE_CANDIDATE = 37,
        ICE_CONTROLLED = 32809,
        ICE_CONTROLLING = 32810,
    }
    const ErrorCode: any;
    interface CodecInterface {
        name: string;
        attribute: Attributes;
        decode: (buffer: Buffer, offset: number, length: number, tid: number[]) => any;
        encode: (buffer: Buffer, offset: number, value: any, tid: number[]) => number;
    }
    class Packet {
        type: MessageType;
        tid: number[];
        attrs: {
            [key: string]: any;
        };
        constructor(type: MessageType, RFC5389?: boolean);
        transactionId(rfc5389?: boolean): number[];
        getAttributes(): string[];
        readonly isRFC3489: boolean;
        readonly isRFC5389: boolean;
        static isRequest(msg_type: number): boolean;
        static isIndication(msg_type: number): boolean;
        static isResponse(msg_type: number): boolean;
        static isError(msg_type: number): boolean;
        addAttribute(type: Attributes | string, value: any): void;
        getAttribute(type: Attributes | string): any;
        removeAttribute(type: Attributes | string): void;
        static validateMessageIntegrity(packet: Packet, message: Buffer, password: string): boolean;
        static validateFingerPrint(packet: Packet, buffer: Buffer): boolean;
        static encode(packet: Packet, password?: string | Buffer, addFingerprint?: boolean): Buffer;
        static decode(buffer: Buffer): Packet;
        static generateCredentialHash(username: string | Buffer, realm: string | Buffer, password: string | Buffer): Buffer;
        static generateTransactionID(rfc5389?: boolean): number[];
        static readHeaderLength(buffer: Buffer): number;
        static writeHeaderLength(msgLength: number, buffer: Buffer): void;
        static writeAttributeHeader(attr: Attributes, length: number, buffer: Buffer, offset: number): number;
        static writeAttribute(attr: Attributes, buffer: Buffer, offset: number, value: any, tid: number[]): number;
        static decodeAddress(buffer: Buffer, offset: number, length: number, tid: number[]): any;
        static encodeAddress(buffer: Buffer, offset: number, value: any, tid: number[]): number;
        static decodeByteString(buffer: Buffer, offset: number, length: number, tid: number[]): any;
        static encodeByteString(buffer: Buffer, offset: number, value: Buffer, tid: number[]): number;
        static decodeUint16List(buffer: Buffer, offset: number, length: number, tid: number[]): any[];
        static encodeUint16List(buffer: Buffer, offset: number, value: number[], tid: number[]): number;
        static decodeUint32(buffer: Buffer, offset: number, length: number, tid: number[]): number;
        static encodeUint32(buffer: Buffer, offset: number, value: number, tid: number[]): number;
        static decodeUint64(buffer: Buffer, offset: number, length: number, tid: number[]): number[];
        static encodeUint64(buffer: Buffer, offset: number, value: number[], tid: number[]): number;
        static decodeEmptyValue(buffer: Buffer, offset: number, length: number, tid: number[]): boolean;
        static encodeEmptyValue(buffer: Buffer, offset: number, value: number, tid: number[]): number;
        private static codec;
        static registerCodec(codec: CodecInterface): void;
    }
}
