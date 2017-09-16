import IP = require('ipaddr.js');
import assert = require('assert');
import OS = require('os');
import Crypto = require('crypto');
import CRC = require('crc');

// TODO(): Decode Ignore all attributes after FINGERPRINT. (-1) 
// TODO(): Check MESSAGE_INTEGRITY order when it is applied. (-2)

export namespace Stun {

/**
 * All STUN messages sent over UDP SHOULD be less than the path MTU, if
 * known.  If the path MTU is unknown, messages SHOULD be the smaller of
 * 576 bytes and the first-hop MTU for IPv4 [RFC1122] and 1280 bytes for
 * IPv6 [RFC2460].  This value corresponds to the overall size of the IP
 * packet.  Consequently, for IPv4, the actual STUN message would need
 * to be less than 548 bytes (576 minus 20-byte IP header, minus 8-byte
 * UDP header, assuming no IP options are used).  STUN provides no
 * ability to handle the case where the request is under the MTU but the
 * response would be larger than the MTU.  It is not envisioned that
 * this limitation will be an issue for STUN.  The MTU limitation is a
 * SHOULD, and not a MUST, to account for cases where STUN itself is
 * being used to probe for MTU characteristics [BEHAVE-NAT].  Outside of
 * this or similar applications, the MTU constraint MUST be followed.
 */

export var PacketSize: number = 1280;

/**
 * The magic cookie field MUST contain the fixed value 0x2112A442 in
 * network byte order.  In RFC 3489 [RFC3489], this field was part of
 * the transaction ID; placing the magic cookie in this location allows
 * a server to detect if the client will understand certain attributes
 * that were added in this revised specification.  In addition, it aids
 * in distinguishing STUN packets from packets of other protocols when
 * STUN is multiplexed with those other protocols on the same port.
 */

export var MagicCookie: number = 0x2112A442;

/**
 * The value of the attribute is computed as the CRC-32 of the STUN message
 * up to (but excluding) the FINGERPRINT attribute itself, XOR'ed with
 * the 32-bit value 0x5354554e (the XOR helps in cases where an
 * application packet is also using CRC-32 in it).
 */

export var FingerPrint: number = 0x5354554e;

/**
 * The address family can take on the following
 */

export enum AddressFamily {
  IPv4 = 0x01,
  IPv6 = 0x02,
};

/**
 * It is assumed that the reader is familiar with NATs.  It has been
 * observed that NAT treatment of UDP varies among implementations.  The
 * four treatments observed in implementations are:
 */

export enum Type {
  /**
   * A full cone NAT is one where all requests from the
   * same internal IP address and port are mapped to the same external
   * IP address and port.  Furthermore, any external host can send a
   * packet to the internal host, by sending a packet to the mapped
   * external address. [RFC3489]
   */

  FULL_CONE = 0x01, 
  
  /**
   * A restricted cone NAT is one where all requests
   * from the same internal IP address and port are mapped to the same
   * external IP address and port.  Unlike a full cone NAT, an external
   * host (with IP address X) can send a packet to the internal host
   * only if the internal host had previously sent a packet to IP
   * address X. [RFC3489]
   */

  RESTRICTED_CONE = 0x02,

  /**
   * A port restricted cone NAT is like a
   * restricted cone NAT, but the restriction includes port numbers.
   * Specifically, an external host can send a packet, with source IP
   * address X and source port P, to the internal host only if the
   * internal host had previously sent a packet to IP address X and
   * port P. [RFC3489]
   */

  PORT_RESTRICTED_CONE = 0x03,

  /**
   * A symmetric NAT is one where all requests from the
   * same internal IP address and port, to a specific destination IP
   * address and port, are mapped to the same external IP address and
   * port.  If the same host sends a packet with the same source
   * address and port, but to a different destination, a different
   * mapping is used.  Furthermore, only the external host that
   * receives a packet can send a UDP packet back to the internal host. [RFC3489]
   */

  SYMMETRIC = 0x04,
};

/**
 * After the agent has done the basic processing of a message, the agent
 * performs the checks listed below in order specified:
 *
 * - If the message does not contain both a MESSAGE-INTEGRITY and a
 *   USERNAME attribute:
 * 
 * - If the message is a request, the server MUST reject the request
 *   with an error response.  This response MUST use an error code
 *   of 400 (Bad Request).
 *
 * - If the message is an indication, the agent MUST silently
 *   discard the indication.
 *
 * - If the USERNAME does not contain a username value currently valid
 *   within the server:
 *
 * - If the message is a request, the server MUST reject the request
 *   with an error response.  This response MUST use an error code
 *   of 401 (Unauthorized).
 * 
 * - If the message is an indication, the agent MUST silently
 *   discard the indication.
 * 
 * - Using the password associated with the username, compute the value
 *   for the message integrity as described in Section 15.4.  If the
 *   resulting value does not match the contents of the MESSAGE-
 *   INTEGRITY attribute:
 * 
 * - If the message is a request, the server MUST reject the request
 *   with an error response.  This response MUST use an error code
 *   of 401 (Unauthorized).
 * 
 * - If the message is an indication, the agent MUST silently
 *   discard the indication.
 * 
 * If these checks pass, the agent continues to process the request or
 * indication.  Any response generated by a server MUST include the
 * MESSAGE-INTEGRITY attribute, computed using the password utilized to
 * authenticate the request.  The response MUST NOT contain the USERNAME
 * attribute.
 * 
 * If any of the checks fail, a server MUST NOT include a MESSAGE-
 * INTEGRITY or USERNAME attribute in the error response.  This is
 * because, in these failure cases, the server cannot determine the
 * shared secret necessary to compute MESSAGE-INTEGRITY. [RFC5389]
 */

export enum MessageType {
  /**
   * Binding Request
   * 
   * For a request or indication message, the agent MUST include the
   * USERNAME and MESSAGE-INTEGRITY attributes in the message.  The HMAC
   * for the MESSAGE-INTEGRITY attribute is computed as described in
   * Section 15.4.  Note that the password is never included in the
   * request or indication. [RFC5389]
   */

  BINDING_REQUEST = 0x0001,

  /**
   * Binding Response
   */

  BINDING_RESPONSE = 0x0101,

  /**
   * Binding Error Response
   */

  BINDING_ERROR_RESPONSE = 0x0111,

  /**
   * For a request or indication message, the agent MUST include the
   * USERNAME and MESSAGE-INTEGRITY attributes in the message.  The HMAC
   * for the MESSAGE-INTEGRITY attribute is computed as described in
   * Section 15.4.  Note that the password is never included in the
   * request or indication. [RFC5389]
   */

  BINDING_INDICATION = 0x0011,

  /**
   * Shared Secret Request [RFC3489]
   */

  SHARED_SECRET_REQUEST = 0x0002,

  /**
   * Shared Secret Response [RFC3489]
   */

  SHARED_SECRET_RESPONSE = 0x0102,

  /**
   * Shared Secret Error Response [RFC3489]
   */

  SHARED_SECRET_ERROR_RESPONSE = 0x0112
};

/**
 * The following types are defined
 */

export enum Attributes {
  /**
   * The MAPPED-ADDRESS attribute indicates the mapped IP address and
   * port.  It consists of an eight bit address family, and a sixteen bit
   * port, followed by a fixed length value representing the IP address. [RFC3489]
   */

  MAPPED_ADDRESS = 0x0001,

  /**
   * X-Port is computed by taking the mapped port in host byte order,
   * XOR'ing it with the most significant 16 bits of the magic cookie, and
   * then the converting the result to network byte order.  If the IP
   * address family is IPv4, X-Address is computed by taking the mapped IP
   * address in host byte order, XOR'ing it with the magic cookie, and
   * converting the result to network byte order.  If the IP address
   * family is IPv6, X-Address is computed by taking the mapped IP address
   * in host byte order, XOR'ing it with the concatenation of the magic
   * cookie and the 96-bit transaction ID, and converting the result to
   * network byte order.
   * 
   * The rules for encoding and processing the first 8 bits of the
   * attribute's value, the rules for handling multiple occurrences of the
   * attribute, and the rules for processing address families are the same
   * as for MAPPED-ADDRESS.
   * 
   * Note: XOR-MAPPED-ADDRESS and MAPPED-ADDRESS differ only in their
   * encoding of the transport address.  The former encodes the transport
   * address by exclusive-or'ing it with the magic cookie.  The latter
   * encodes it directly in binary.  RFC 3489 originally specified only
   * MAPPED-ADDRESS.  However, deployment experience found that some NATs
   * rewrite the 32-bit binary payloads containing the NAT's public IP
   * address, such as STUN's MAPPED-ADDRESS attribute, in the well-meaning
   * but misguided attempt at providing a generic ALG function.  Such
   * behavior interferes with the operation of STUN and also causes
   * failure of STUN's message-integrity checking. [RFC5389]
   */

  XOR_MAPPED_ADDRESS = 0x0020,

  /**
   * The RESPONSE-ADDRESS attribute indicates where the response to a
   * Binding Request should be sent.  Its syntax is identical to MAPPED-
   * ADDRESS. [RFC3489]
   */

  RESPONSE_ADDRESS = 0x0002,
  
  /**
   * The CHANGE-REQUEST attribute is used by the client to request that
   * the server use a different address and/or port when sending the
   * response.  The attribute is 32 bits long, although only two bits (A
   * and B) are used: [RFC3489]
   */

  CHANGE_REQUEST = 0x0003,
  
  /**
   * The SOURCE-ADDRESS attribute is present in Binding Responses.  It
   * indicates the source IP address and port that the server is sending
   * the response from.  Its syntax is identical to that of MAPPED-
   * ADDRESS. [RFC3489]
   */

  SOURCE_ADDRESS = 0x0004,
  
  /**
   * The CHANGED-ADDRESS attribute indicates the IP address and port where
   * responses would have been sent from if the "change IP" and "change
   * port" flags had been set in the CHANGE-REQUEST attribute of the
   * Binding Request.  The attribute is always present in a Binding
   * Response, independent of the value of the flags.  Its syntax is
   * identical to MAPPED-ADDRESS. [RFC3489]
   */

  CHANGED_ADDRESS = 0x0005,
  
  /**
   * The USERNAME attribute is used for message integrity.  It serves as a
   * means to identify the shared secret used in the message integrity
   * check.  The USERNAME is always present in a Shared Secret Response,
   * along with the PASSWORD.  It is optionally present in a Binding
   * Request when message integrity is used.
   * 
   * The value of USERNAME is a variable length opaque value.  Its length
   * MUST be a multiple of 4 (measured in bytes) in order to guarantee
   * alignment of attributes on word boundaries. [RFC3489]
   */

  USERNAME = 0x0006,
  
  /**
   * The PASSWORD attribute is used in Shared Secret Responses.  It is
   * always present in a Shared Secret Response, along with the USERNAME.
   * 
   * The value of PASSWORD is a variable length value that is to be used
   * as a shared secret.  Its length MUST be a multiple of 4 (measured in
   * bytes) in order to guarantee alignment of attributes on word
   * boundaries. [RFC3489]
   */

  PASSWORD = 0x0007,
  
  /**
   * The MESSAGE-INTEGRITY attribute contains an HMAC-SHA1 [13] of the
   * STUN message.  It can be present in Binding Requests or Binding
   * Responses.  Since it uses the SHA1 hash, the HMAC will be 20 bytes.
   * The text used as input to HMAC is the STUN message, including the
   * header, up to and including the attribute preceding the MESSAGE-
   * INTEGRITY attribute. That text is then padded with zeroes so as to be
   * a multiple of 64 bytes.  As a result, the MESSAGE-INTEGRITY attribute
   * MUST be the last attribute in any STUN message.  The key used as
   * input to HMAC depends on the context. [RFC3489]
   * 
   * The MESSAGE-INTEGRITY attribute MUST be the last attribute within a
   * message.  Any attributes that are known, but are not supposed to be
   * present in a message (MAPPED-ADDRESS in a request, for example) MUST
   * be ignored. [RFC3489]
   */

  MESSAGE_INTEGRITY = 0x0008,
  
  /**
   * The ERROR-CODE attribute is present in the Binding Error Response and
   * Shared Secret Error Response.  It is a numeric value in the range of
   * 100 to 699 plus a textual reason phrase encoded in UTF-8, and is
   * consistent in its code assignments and semantics with SIP [10] and
   * HTTP [15].  The reason phrase is meant for user consumption, and can
   * be anything appropriate for the response code.  The lengths of the
   * reason phrases MUST be a multiple of 4 (measured in bytes).  This can
   * be accomplished by added spaces to the end of the text, if necessary.
   * Recommended reason phrases for the defined response codes are
   * presented below.
   * 
   * To facilitate processing, the class of the error code (the hundreds
   * digit) is encoded separately from the rest of the code. [RFC3489]
   */

  ERROR_CODE = 0x0009,
  
  /**
   * The UNKNOWN-ATTRIBUTES attribute is present only in a Binding Error
   * Response or Shared Secret Error Response when the response code in
   * the ERROR-CODE attribute is 420.
   * 
   * The attribute contains a list of 16 bit values, each of which
   * represents an attribute type that was not understood by the server.
   * If the number of unknown attributes is an odd number, one of the
   * attributes MUST be repeated in the list, so that the total length of
   * the list is a multiple of 4 bytes. [RFC3489]
   */

  UNKNOWN_ATTRIBUTES = 0x000a,
   
  /**
   * The REFLECTED-FROM attribute is present only in Binding Responses,
   * when the Binding Request contained a RESPONSE-ADDRESS attribute.  The
   * attribute contains the identity (in terms of IP address) of the
   * source where the request came from.  Its purpose is to provide
   * traceability, so that a STUN server cannot be used as a reflector for
   * denial-of-service attacks.
   * 
   * Its syntax is identical to the MAPPED-ADDRESS attribute. [RFC3489]
   */

  REFLECTED_FROM = 0x000b,

  /**
   * The REALM attribute may be present in requests and responses.  It
   * contains text that meets the grammar for "realm-value" as described
   * in RFC 3261 [RFC3261] but without the double quotes and their
   * surrounding whitespace.  That is, it is an unquoted realm-value (and
   * is therefore a sequence of qdtext or quoted-pair).  It MUST be a
   * UTF-8 [RFC3629] encoded sequence of less than 128 characters (which
   * can be as long as 763 bytes), and MUST have been processed using
   * SASLprep [RFC4013].
   * 
   * Presence of the REALM attribute in a request indicates that long-term
   * credentials are being used for authentication.  Presence in certain
   * error responses indicates that the server wishes the client to use a
   * long-term credential for authentication. [RFC5389]
   */

  REALM = 0x0014,

  /**
   * The NONCE attribute may be present in requests and responses.  It
   * contains a sequence of qdtext or quoted-pair, which are defined in
   * RFC 3261 [RFC3261].  Note that this means that the NONCE attribute
   * will not contain actual quote characters.  See RFC 2617 [RFC2617],
   * Section 4.3, for guidance on selection of nonce values in a server.
   * 
   * It MUST be less than 128 characters (which can be as long as 763 bytes). [RFC5389]
   */

  NONCE = 0x0015,

  /**
   * The SOFTWARE attribute contains a textual description of the software
   * being used by the agent sending the message.  It is used by clients
   * and servers.  Its value SHOULD include manufacturer and version
   * number.  The attribute has no impact on operation of the protocol,
   * and serves only as a tool for diagnostic and debugging purposes.  The
   * value of SOFTWARE is variable length.  It MUST be a UTF-8 [RFC3629]
   * encoded sequence of less than 128 characters (which can be as long as 763 bytes). 
   * [RFC5226] [RFC5389]
   */

  SOFTWARE = 0x8022,

  /**
   * The alternate server represents an alternate transport address
   * identifying a different STUN server that the STUN client should try.
   * 
   * It is encoded in the same way as MAPPED-ADDRESS, and thus refers to a
   * single server by IP address.  The IP address family MUST be identical
   * to that of the source IP address of the request. [RFC5226] [RFC5389]
   */
  
  ALTERNATE_SERVER = 0x8023,

  /**
   * The FINGERPRINT attribute MAY be present in all STUN messages.  The
   * value of the attribute is computed as the CRC-32 of the STUN message
   * up to (but excluding) the FINGERPRINT attribute itself, XOR'ed with
   * the 32-bit value 0x5354554e (the XOR helps in cases where an
   * application packet is also using CRC-32 in it).  The 32-bit CRC is
   * the one defined in ITU V.42 [ITU.V42.2002], which has a generator
   * polynomial of x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1.
   * When present, the FINGERPRINT attribute MUST be the last attribute in
   * the message, and thus will appear after MESSAGE-INTEGRITY.
   * 
   * The FINGERPRINT attribute can aid in distinguishing STUN packets from
   * packets of other protocols.  See Section 8.
   * 
   * As with MESSAGE-INTEGRITY, the CRC used in the FINGERPRINT attribute
   * covers the length field from the STUN message header.  Therefore,
   * this value must be correct and include the CRC attribute as part of
   * the message length, prior to computation of the CRC.  When using the
   * FINGERPRINT attribute in a message, the attribute is first placed
   * into the message with a dummy value, then the CRC is computed, and
   * then the value of the attribute is updated.  If the MESSAGE-INTEGRITY
   * attribute is also present, then it must be present with the correct
   * message-integrity value before the CRC is computed, since the CRC is
   * done over the value of the MESSAGE-INTEGRITY attribute as well. 
   * [RFC5226] [RFC5389]
   */

  FINGERPRINT = 0x8028,

  /**
   * An agent MUST include the PRIORITY attribute in its Binding request.
   * The attribute MUST be set equal to the priority that would be
   * assigned, based on the algorithm in Section 4.1.2, to a peer
   * reflexive candidate, should one be learned as a consequence of this
   * check (see Section 7.1.3.2.1 for how peer reflexive candidates are
   * learned).  This priority value will be computed identically to how
   * the priority for the local candidate of the pair was computed, except
   * that the type preference is set to the value for peer reflexive
   * candidate types. [RFC5245]
   */

  PRIORITY = 0x0024,

  /**
   * The controlling agent MAY include the USE-CANDIDATE attribute in the
   * Binding request.  The controlled agent MUST NOT include it in its
   * Binding request.  This attribute signals that the controlling agent
   * wishes to cease checks for this component, and use the candidate pair
   * resulting from the check for this component.  Section 8.1.1 provides
   * guidance on determining when to include it. [RFC5245]
   */

  USE_CANDIDATE = 0x0025,

  /**
   * The agent MUST include the ICE-CONTROLLED attribute in the request if
   * it is in the controlled role, and MUST include the ICE-CONTROLLING
   * attribute in the request if it is in the controlling role.  The
   * content of either attribute MUST be the tie-breaker that was
   * determined in Section 5.2.  These attributes are defined fully in
   * Section 19.1. [RFC5226] [RFC5245]
   */

  ICE_CONTROLLED = 0x8029,

  /**
   * The agent MUST include the ICE-CONTROLLED attribute in the request if
   * it is in the controlled role, and MUST include the ICE-CONTROLLING
   * attribute in the request if it is in the controlling role.  The
   * content of either attribute MUST be the tie-breaker that was
   * determined in Section 5.2.  These attributes are defined fully in
   * Section 19.1. [RFC5226] [RFC5245]
   */

  ICE_CONTROLLING = 0x802A,
};

/**
 * The following response codes, along with their recommended reason
 * phrases (in brackets) are defined at this time: [RFC3489]
 */

export const ErrorCode: any = {
  /**
   * (Bad Request): The request was malformed.  The client should not
   * retry the request without modification from the previous attempt. [RFC3489]
   */

  400: 'Bad Request',

  /**
   * (Unauthorized): The Binding Request did not contain a MESSAGE-INTEGRITY attribute. [RFC3489]
   */

  401: 'Unauthorized',

  /**
   * (Unknown Attribute): The server did not understand a mandatory
   * attribute in the request. [RFC3489]
   */
  
  420: 'Unknown Attribute',

  /**
   * (Stale Credentials): The Binding Request did contain a MESSAGE-INTEGRITY attribute, 
   * but it used a shared secret that has expired.  
   * The client should obtain a new shared secret and try again. [RFC3489]
   */

  430: 'Stale Credentials',

  /**
   * (Integrity Check Failure): The Binding Request contained a
   * MESSAGE-INTEGRITY attribute, but the HMAC failed verification.
   * This could be a sign of a potential attack, or client implementation error. [RFC3489]
   */

  431: 'Integrity Check Failure',

  /**
   * (Missing Username): The Binding Request contained a MESSAGE-INTEGRITY attribute, 
   * but not a USERNAME attribute.  Both must be present for integrity checks. [RFC3489]
   */

  432: 'Missing Username',

  /**
   * (Use TLS): The Shared Secret request has to be sent over TLS, but was not received over TLS.
   * [RFC3489]
   */

  433: 'Use TLS',

  /**
   * (Role Conflict): The client asserted an ICE role (controlling or controlled) that is in conflict 
   * with the role of the server. [RFC5245]
   */

  487: 'Role Conflict',

  /**
   * (Server Error): The server has suffered a temporary error. The client should try again.
   * [RFC3489]
   */

  500: 'Server Error',

  /**
   * (Global Failure:) The server is refusing to fulfill the request.
   * The client should not retry. [RFC3489]
   */

  600: 'Global Failure',
};

export interface CodecInterface {
  name: string;
  attribute: Attributes;
  decode: (buffer: Buffer, offset: number, length: number, tid: number[]) => any;
  encode: (buffer: Buffer, offset: number, value: any, tid: number[]) => number;
};

/**
 * After the header are 0 or more attributes.  Each attribute is TLV
 * encoded, with a 16 bit type, 16 bit length, and variable value:
 */

export class Packet {
  public type: MessageType = MessageType.BINDING_REQUEST;
  public tid: number[];
  public attrs: { [key: string]: any } = {};

  constructor(type: MessageType, RFC5389?: boolean) {
    this.tid = Packet.generateTransactionID(RFC5389);
    this.type = type;
  }

  public transactionId(rfc5389: boolean = true): number[] {
    if (rfc5389) {
      return [
        this.tid[1],
        this.tid[2],
        this.tid[3]
      ];
    }
    
    return this.tid;
  }

  public getAttributes(): string[] {
    let ret: string[] = [];

    for (const attr in this.attrs) {
      if (Packet.codec[attr]) {
        ret.push(Packet.codec[attr].name);
      }
    }

    return ret;
  }

  /**
   * The magic cookie field MUST contain the fixed value 0x2112A442 in
   * network byte order.  In RFC 3489 [RFC3489], this field was part of
   * the transaction ID; placing the magic cookie in this location allows
   * a server to detect if the client will understand certain attributes
   * that were added in this revised specification.  In addition, it aids
   * in distinguishing STUN packets from packets of other protocols when
   * STUN is multiplexed with those other protocols on the same port.
   */

  public get isRFC3489(): boolean {
    return (this.tid[0] !== 0x2112A442); 
  }

  /**
   * The magic cookie field MUST contain the fixed value 0x2112A442 in
   * network byte order.  In RFC 3489 [RFC3489], this field was part of
   * the transaction ID; placing the magic cookie in this location allows
   * a server to detect if the client will understand certain attributes
   * that were added in this revised specification.  In addition, it aids
   * in distinguishing STUN packets from packets of other protocols when
   * STUN is multiplexed with those other protocols on the same port.
   */

  public get isRFC5389(): boolean {
    return (this.tid[0] === 0x2112A442); 
  }

  /**
   * Given a 16-bit STUN message type value in host byte order in msg_type
   * parameter, to determine the STUN message types [rfc5389]
   */

  public static isRequest(msg_type: number) {
    return (((msg_type) & 0x0110) == 0x0000);
  }

  /**
   * Given a 16-bit STUN message type value in host byte order in msg_type
   * parameter, to determine the STUN message types [rfc5389]
   */
  
  public static isIndication(msg_type: number) {
    return (((msg_type) & 0x0110) == 0x0010);
  }

  /**
   * Given a 16-bit STUN message type value in host byte order in msg_type
   * parameter, to determine the STUN message types [rfc5389]
   */

  public static isResponse(msg_type: number) {
    return (((msg_type) & 0x0110) == 0x0100);
  }

  /**
   * Given a 16-bit STUN message type value in host byte order in msg_type
   * parameter, to determine the STUN message types [rfc5389]
   */

  public static isError(msg_type: number) {
    return (((msg_type) & 0x0110) == 0x0110);
  }

  public addAttribute(type: Attributes | string, value: any): void {
    if (typeof type === 'string') {
      type = Packet.codec[type] ? Packet.codec[type].attribute : type;
    }

    this.attrs[type] = value;
  }

  public getAttribute(type: Attributes | string): any {
    if (typeof type === 'string') {
      type = Packet.codec[type] ? Packet.codec[type].attribute : type;
    }

    return this.attrs[type];
  }

  public removeAttribute(type: Attributes | string): void {
    if (typeof type === 'string') {
      type = Packet.codec[type] ? Packet.codec[type].attribute : type;
    }

    this.attrs[type] = undefined;
  }

  public static validateMessageIntegrity(packet: Packet, message: Buffer, password: string): boolean {
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
        let offset: number = size - (current_pos + 24);
        let length: number = size - offset - 20;
        tmp.writeUInt16BE(length, 2);
      }

      const hmac = Crypto.createHmac('sha1', password);
      hmac.update(tmp);
      const digest = hmac.digest();      
      return digest.equals(msg);
    }

    return false;
  }

  public static validateFingerPrint(packet: Packet, buffer: Buffer): boolean {
    const finger = packet.getAttribute(Attributes.FINGERPRINT);
    
    if (finger) {
      let value = CRC.crc32(buffer.slice(0, -8));
      return (((finger ^ FingerPrint) >>> 0) === value) ? true : false;
    }
    
    return false;
  }

  public static encode(packet: Packet, password?: string | Buffer, addFingerprint?: boolean) : Buffer {
    let buffer: Buffer = Buffer.alloc(PacketSize);
    let msgLength: number = 20;

    buffer.writeUInt16BE(packet.type & 0xffff, 0);

    buffer.writeUInt32BE(packet.tid[0], 4);
    buffer.writeUInt32BE(packet.tid[1], 8);
    buffer.writeUInt32BE(packet.tid[2], 12);
    buffer.writeUInt32BE(packet.tid[3], 16);

    let attr_len: number = 0;

    for (const name in packet.attrs) {
      let attr: number;

      if (Packet.codec[name]) {
        attr = Packet.codec[name].attribute;
      } else {
        attr = parseInt(name, 16);        
      }
      
      if (!Attributes[attr] || 
          attr == Attributes.MESSAGE_INTEGRITY ||
          attr == Attributes.FINGERPRINT) 
      {
        continue;
      }
      
      msgLength += Packet.writeAttribute(attr, buffer, msgLength, packet.attrs[name], packet.tid);
    }

    buffer.writeUInt16BE((msgLength - 20), 2);

    if (!packet.isRFC3489) {
      if (password && password.length) {
        msgLength += Packet.writeAttribute(Attributes.MESSAGE_INTEGRITY, 
                                           buffer, msgLength, 
                                           Buffer.alloc(20, '0'), 
                                           packet.tid);

        buffer.writeUInt16BE((msgLength - 20), 2);

        const hmac = Crypto.createHmac('sha1', password);
        hmac.update(buffer.slice(0, msgLength - 24));

        const digest = hmac.digest();
        digest.copy(buffer, msgLength - 20);
      }

      if (addFingerprint) {
        const attrPos = msgLength;
        const valuePos = msgLength + 4;

        msgLength += Packet.writeAttribute(Attributes.FINGERPRINT, 
                                           buffer, msgLength, 
                                           0, 
                                           packet.tid);

        buffer.writeUInt16BE((msgLength - 20), 2);

        let crc32 = CRC.crc32(buffer.slice(0, attrPos));
        let finger = (crc32 ^ FingerPrint) >>> 0;

        buffer.writeUInt32BE(finger, valuePos);
      }
    }

    return buffer.slice(0, msgLength);
  }

  public static decode(buffer: Buffer) : Packet {
    let header: number = buffer.readUInt32BE(0);

    let msgLength = header & 0xffff;

    if ((msgLength % 4 != 0) ||
         buffer.byteLength > PacketSize ||
         msgLength + 20 != buffer.byteLength || 
         buffer.byteLength < 20)
    {      
      return undefined;
    }

    let type = (header >> 16) & 0x0fff;
    
    if (!MessageType[type]) {      
      return undefined;
    }

    let packet: Packet = new Packet(type);

    packet.tid[0] = buffer.readUInt32BE(4);
    packet.tid[1] = buffer.readUInt32BE(8);
    packet.tid[2] = buffer.readUInt32BE(12);
    packet.tid[3] = buffer.readUInt32BE(16);

    let offset = 20;

    while (offset < buffer.byteLength) {
      assert((buffer.byteLength - offset) >= 4);
      header = buffer.readUInt32BE(offset);

      let length = (header & 0xffff);
      let attr: Attributes = (header >> 16) & 0xffff;
      const codec = Packet.codec[attr];

      if (codec && codec.decode) {
        packet.addAttribute(attr, codec.decode(buffer, offset + 4, length, packet.tid)); 
      }

      offset += ((length % 4) == 0) ? 4 + length : length + (8 - (length % 4));
    }

    return packet;
  }

  /**
   * The key for the HMAC depends on whether long-term or short-term
   * credentials are in use.  For long-term credentials, the key is 16 bytes:
   *
   *    key = MD5(username ":" realm ":" SASLprep(password))
   *
   * That is, the 16-byte key is formed by taking the MD5 hash of the
   * result of concatenating the following five fields: (1) the username,
   * with any quotes and trailing nulls removed, as taken from the
   * USERNAME attribute (in which case SASLprep has already been applied);
   * (2) a single colon; (3) the realm, with any quotes and trailing nulls
   * removed; (4) a single colon; and (5) the password, with any trailing
   * nulls removed and after processing using SASLprep.  For example, if
   * the username was 'user', the realm was 'realm', and the password was
   * 'pass', then the 16-byte HMAC key would be the result of performing
   * an MD5 hash on the string 'user:realm:pass', the resulting hash being
   * 0x8493fbc53ba582fb4c044c456bdc40eb.
   *
   * For short-term credentials:
   * 
   *    key = SASLprep(password)
   *
   * where MD5 is defined in RFC 1321 [RFC1321] and SASLprep() is defined
   * in RFC 4013 [RFC4013].
   * 
   * The structure of the key when used with long-term credentials
   * facilitates deployment in systems that also utilize SIP.  Typically,
   * SIP systems utilizing SIP's digest authentication mechanism do not
   * actually store the password in the database.  Rather, they store a
   * value called H(A1), which is equal to the key defined above.
   * 
   * Based on the rules above, the hash used to construct MESSAGE-
   * INTEGRITY includes the length field from the STUN message header.
   * Prior to performing the hash, the MESSAGE-INTEGRITY attribute MUST be
   * inserted into the message (with dummy content).  The length MUST then
   * be set to point to the length of the message up to, and including,
   * the MESSAGE-INTEGRITY attribute itself, but excluding any attributes
   * after it.  Once the computation is performed, the value of the
   * MESSAGE-INTEGRITY attribute can be filled in, and the value of the
   * length in the STUN header can be set to its correct value -- the
   * length of the entire message.  Similarly, when validating the
   * MESSAGE-INTEGRITY, the length field should be adjusted to point to
   * the end of the MESSAGE-INTEGRITY attribute prior to calculating the
   * HMAC.  Such adjustment is necessary when attributes, such as
   * FINGERPRINT, appear after MESSAGE-INTEGRITY.
   */

  public static generateCredentialHash(username: string | Buffer, 
                                       realm: string | Buffer, 
                                       password: string | Buffer): Buffer 
  {
    const hash = Crypto.createHash('md5');
    hash.update('' + username + ':' + realm + ':' + password);
    return hash.digest();
  }

  public static generateTransactionID(rfc5389: boolean = true) {
    return [
      (rfc5389) ? 0x2112A442 : (Math.random() * 0xffffff) >>> 0,
      (Math.random() * 0xffffff) >>> 0,
      (Math.random() * 0xffffff) >>> 0,
      (Math.random() * 0xffffff) >>> 0,
    ];
  }

  public static readHeaderLength(buffer: Buffer): number {
    return buffer.readUInt16BE(2);
  }

  public static writeHeaderLength(msgLength: number, buffer: Buffer) {
    buffer.writeUInt16BE(msgLength & 0xffff, 2);
  }

  public static writeAttributeHeader(attr: Attributes, length: number, buffer: Buffer, offset: number): number {
    return buffer.writeUInt32BE(((attr << 16) | length & 0xffff) >>> 0, offset);
  }

  public static writeAttribute(attr: Attributes, buffer: Buffer, offset: number, value: any, tid: number[]): number {
    const codec = Packet.codec[attr];
    assert(codec);

    const ret: number = Packet.codec[attr].encode(buffer, offset + 4, value, tid);
    buffer.writeUInt32BE(((attr << 16) | ret & 0xffff) >>> 0, offset);

    return ((ret % 4) == 0) ? 4 + ret : ret + (8 - (ret % 4));
  }

  public static decodeAddress(buffer: Buffer, offset: number, length: number, tid: number[]): any {
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

  public static encodeAddress(buffer: Buffer, offset: number, value: any, tid: number[]): number {
    assert(value);
    assert(value.address);
    assert(value.family);
    assert(value.port);

    const family: number = (value.family == 'IPv6') ? AddressFamily.IPv6 : AddressFamily.IPv4;
    const port: number = value.port;
    const address = Buffer.from(IP.parse(value.address).toByteArray());

    assert(address.byteLength == ((family == AddressFamily.IPv6) ? 16 : 4));

    buffer.writeUInt32BE(((family << 16) | port & 0xffff), offset);
    address.copy(buffer, offset + 4);

    return ((family == AddressFamily.IPv6) ? 20 : 8);
  };

  public static decodeByteString(buffer: Buffer, offset: number, length: number, tid: number[]): any {
    return buffer.slice(offset, offset + length);
  }

  public static encodeByteString(buffer: Buffer, offset: number, value: Buffer, tid: number[]): number {
    assert(Buffer.isBuffer(value));

    const byteLength: number = value.byteLength;
    let ret: number = value.copy(buffer, offset);

    if (byteLength % 4 != 0) {
      buffer.fill(0x20, offset + ret, offset + ret + (4 - (byteLength % 4)));
    }

    return ret;
  }

  public static decodeUint16List(buffer: Buffer, offset: number, length: number, tid: number[]): any[] {
    let res = [];
    const end = length + offset;
    
    for (let index = offset; index < end; index += 2) {
      res.push(buffer.readUInt16BE(index));
    }

    return res;
  }

  public static encodeUint16List(buffer: Buffer, offset: number, value: number[], tid: number[]): number {
    let res: number = offset;
    let last: number = 0x0000;

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

  public static decodeUint32(buffer: Buffer, offset: number, length: number, tid: number[]): number {
    assert(length == 4);
    return buffer.readUInt32BE(offset);
  }

  public static encodeUint32(buffer: Buffer, offset: number, value: number, tid: number[]): number {
    buffer.writeUInt32BE(value, offset);
    return 4;
  }

  public static decodeUint64(buffer: Buffer, offset: number, length: number, tid: number[]): number[] {
    assert(length == 8);

    return [
      buffer.readUInt32BE(offset),
      buffer.readUInt32BE(offset + 4),
    ];
  }

  public static encodeUint64(buffer: Buffer, offset: number, value: number[], tid: number[]): number {
    assert(value && value.length == 2);

    buffer.writeUInt32BE(value[0], offset);
    buffer.writeUInt32BE(value[1], offset + 4);

    return 8;
  }

  public static decodeEmptyValue(buffer: Buffer, offset: number, length: number, tid: number[]): boolean {
    return true;
  }

  public static encodeEmptyValue(buffer: Buffer, offset: number, value: number, tid: number[]): number {
    return 0;
  }

  private static codec: { [key: string]: CodecInterface } = {};

  public static registerCodec(codec: CodecInterface) {
    Packet.codec[codec.name] = codec;
    Packet.codec[codec.attribute] = codec;
  }
};

/**
 * MAPPED_ADDRESS
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.1
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.1
 */

Packet.registerCodec({
  name: 'MAPPED_ADDRESS',
  attribute: Attributes.MAPPED_ADDRESS,
  decode: Packet.decodeAddress,
  encode: Packet.encodeAddress,
});

/**
 * XOR_MAPPED_ADDRESS
 * 
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.2
 */

Packet.registerCodec({
  name: 'XOR_MAPPED_ADDRESS',
  attribute: Attributes.XOR_MAPPED_ADDRESS,
  decode: (buffer: Buffer, offset: number, length: number, tid: number[]): any => {
    let header = buffer.readUInt32BE(offset);
    let family = (header >> 16) & 0xff;

    assert((family == AddressFamily.IPv4 && length == 8) ||
           (family == AddressFamily.IPv6 && length == 20));
    
    let address = buffer.slice(offset + 4, offset + length);

    if (family == AddressFamily.IPv4) {
      address.writeUInt32BE((address.readUInt32BE(0) ^ MagicCookie) >>> 0, 0);
    } else {
      address.writeUInt32BE((address.readUInt32BE(0) ^ MagicCookie) >>> 0, 0);
      address.writeUInt32BE((address.readUInt32BE(4) ^ tid[1]) >>> 0, 4);
      address.writeUInt32BE((address.readUInt32BE(8) ^ tid[2]) >>> 0, 8);
      address.writeUInt32BE((address.readUInt32BE(12) ^ tid[3]) >>> 0, 12);
    }

    return {
      port: (header & 0xffff) ^ (MagicCookie >> 16),
      family: (family == AddressFamily.IPv4) ? 'IPv4' : 'IPv6',
      address: IP.fromByteArray(address).toString(),
    };
  },

  encode:(buffer: Buffer, offset: number, value: any, tid: number[]): number => {
    assert(value);
    assert(value.address);
    assert(value.family);
    assert(value.port);

    const family: number = (value.family == 'IPv6') ? AddressFamily.IPv6 : AddressFamily.IPv4;
    const port: number = ((value.port & 0xffff) ^ (MagicCookie >> 16));
    const address = Buffer.from(IP.parse(value.address).toByteArray());

    assert(address.byteLength == ((family == AddressFamily.IPv6) ? 16 : 4));

    buffer.writeUInt32BE(((family << 16) | port & 0xffff), offset);

    if (family == AddressFamily.IPv4) {
      address.writeUInt32BE((address.readUInt32BE(0) ^ MagicCookie) >>> 0, 0);         
    } else {
      address.writeUInt32BE((address.readUInt32BE(0) ^ MagicCookie) >>> 0, 0);
      address.writeUInt32BE((address.readUInt32BE(4) ^ tid[1]) >>> 0, 4);
      address.writeUInt32BE((address.readUInt32BE(8) ^ tid[2]) >>> 0, 8);
      address.writeUInt32BE((address.readUInt32BE(12) ^ tid[3]) >>> 0, 12);
    }

    address.copy(buffer, offset + 4);

    return ((family == AddressFamily.IPv6) ? 20 : 8);
  },
});

/**
 * RESPONSE_ADDRESS
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.2
 */

Packet.registerCodec({
  name: 'RESPONSE_ADDRESS',
  attribute: Attributes.RESPONSE_ADDRESS,
  decode: Packet.decodeAddress,
  encode: Packet.encodeAddress,
});

/**
 * CHANGE_REQUEST
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.4
 */

Packet.registerCodec({
  name: 'CHANGE_REQUEST',
  attribute: Attributes.CHANGE_REQUEST,
  decode: Packet.decodeAddress,
  encode: Packet.encodeAddress,
});

/**
 * SOURCE_ADDRESS
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.5
 */

Packet.registerCodec({
  name: 'SOURCE_ADDRESS',
  attribute: Attributes.SOURCE_ADDRESS,
  decode: Packet.decodeAddress,
  encode: Packet.encodeAddress,
});

/**
 * CHANGED_ADDRESS
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.3
 */

Packet.registerCodec({
  name: 'CHANGED_ADDRESS',
  attribute: Attributes.CHANGED_ADDRESS,
  decode: Packet.decodeAddress,
  encode: Packet.encodeAddress,
});

/**
 * USERNAME
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.6
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.3
 */

Packet.registerCodec({
  name: 'USERNAME',
  attribute: Attributes.USERNAME,
  decode: Packet.decodeByteString,
  encode: Packet.encodeByteString,
});

/**
 * PASSWORD
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.7
 */

Packet.registerCodec({
  name: 'PASSWORD',
  attribute: Attributes.PASSWORD,
  decode: Packet.decodeByteString,
  encode: Packet.encodeByteString,
});

/**
 * MESSAGE_INTEGRITY
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.8
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.4
 */

Packet.registerCodec({
  name: 'MESSAGE_INTEGRITY',
  attribute: Attributes.MESSAGE_INTEGRITY,
  decode: Packet.decodeByteString,
  encode: Packet.encodeByteString,
});

/**
 * ERROR_CODE
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.9
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.6
 */

Packet.registerCodec({
  name: 'ERROR_CODE',
  attribute: Attributes.ERROR_CODE,
  decode: (buffer: Buffer, offset: number, length: number, tid: number[]) : any => {
    let header = buffer.readUInt32BE(offset);

    assert((header >> 11) == 0);

    let errorClass: number = ((header >> 8) & 0x7);
    let code: number = (header & 0xff);
    let reason: string = buffer.toString('utf8', offset + 4, offset + length);

    return {
      code: errorClass * 100 + code,
      reason: reason,
    };
  },
  encode: (buffer: Buffer, offset: number, value: any, tid: number[]) : number => {
    assert(value);
    assert(value.code);

    if (!value.reason) {
      value.reason = ErrorCode[value.code];
    }

    assert(value.reason);
    assert(Buffer.byteLength(value.reason));

    buffer.writeUInt32BE((((value.code / 100) >>> 0) << 8 | value.code % 100), offset);
    return buffer.write(value.reason, offset + 4) + 4;
  },
});

/**
 * UNKNOWN_ATTRIBUTES
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.10
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.9
 */

Packet.registerCodec({
  name: 'UNKNOWN_ATTRIBUTES',
  attribute: Attributes.UNKNOWN_ATTRIBUTES,
  decode: Packet.decodeUint16List,
  encode: Packet.encodeUint16List,
});

/**
 * REFLECTED_FROM
 * 
 * - RFC3489: https://tools.ietf.org/html/rfc3489#section-11.2.11
 */

Packet.registerCodec({
  name: 'REFLECTED_FROM',
  attribute: Attributes.REFLECTED_FROM,
  decode: Packet.decodeAddress,
  encode: Packet.encodeAddress,
});

/**
 * REALM
 * 
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.7
 */

Packet.registerCodec({
  name: 'REALM',
  attribute: Attributes.REALM,
  decode: Packet.decodeByteString,
  encode: Packet.encodeByteString,
});

/**
 * NONCE
 * 
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.8
 */

Packet.registerCodec({
  name: 'NONCE',
  attribute: Attributes.NONCE,
  decode: Packet.decodeByteString,
  encode: Packet.encodeByteString,
});

/**
 * SOFTWARE
 * 
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.10
 */

Packet.registerCodec({
  name: 'SOFTWARE',
  attribute: Attributes.SOFTWARE,
  decode: Packet.decodeByteString,
  encode: Packet.encodeByteString,
});

/**
 * ALTERNATE_SERVER
 * 
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.11
 */

Packet.registerCodec({
  name: 'ALTERNATE_SERVER',
  attribute: Attributes.ALTERNATE_SERVER,
  decode: Packet.decodeAddress,
  encode: Packet.encodeAddress,
});

/**
 * FINGERPRINT
 * 
 * - RFC5389: https://tools.ietf.org/html/rfc5389#section-15.5
 */

Packet.registerCodec({
  name: 'FINGERPRINT',
  attribute: Attributes.FINGERPRINT,
  decode: Packet.decodeUint32,
  encode: Packet.encodeUint32,
});

/**
 * PRIORITY
 * 
 * - RFC5245: https://tools.ietf.org/html/rfc5245#section-19.1
 * - RFC5245: https://tools.ietf.org/html/rfc5245#section-21.2
 */

Packet.registerCodec({
  name: 'PRIORITY',
  attribute: Attributes.PRIORITY,
  decode: Packet.decodeUint32,
  encode: Packet.encodeUint32,
});

/**
 * USE_CANDIDATE
 * 
 * - RFC5245: https://tools.ietf.org/html/rfc5245#section-19.1
 * - RFC5245: https://tools.ietf.org/html/rfc5245#section-21.2
 */

Packet.registerCodec({
  name: 'USE_CANDIDATE',
  attribute: Attributes.USE_CANDIDATE,
  decode: Packet.decodeEmptyValue,
  encode: Packet.encodeEmptyValue,
});

/**
 * ICE_CONTROLLED
 * 
 * - RFC5245: https://tools.ietf.org/html/rfc5245#section-19.1
 * - RFC5245: https://tools.ietf.org/html/rfc5245#section-21.2
 */

Packet.registerCodec({
  name: 'ICE_CONTROLLED',
  attribute: Attributes.ICE_CONTROLLED,
  decode: Packet.decodeUint64,
  encode: Packet.encodeUint64,
});

/**
 * ICE_CONTROLLING
 * 
 * The agent MUST include the ICE-CONTROLLED attribute in the request if
 * it is in the controlled role, and MUST include the ICE-CONTROLLING
 * attribute in the request if it is in the controlling role.  The
 * content of either attribute MUST be the tie-breaker that was
 * determined in Section 5.2.  These attributes are defined fully in
 * Section 19.1.
 * 
 * - RFC5245: https://tools.ietf.org/html/rfc5245#section-19.1
 * - RFC5245: https://tools.ietf.org/html/rfc5245#section-21.2
 */

Packet.registerCodec({
  name: 'ICE_CONTROLLING',
  attribute: Attributes.ICE_CONTROLLING,
  decode: Packet.decodeUint64,
  encode: Packet.encodeUint64,
});
};