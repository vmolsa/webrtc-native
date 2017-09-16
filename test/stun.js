require('source-map-support').install();

const ORTC = require('../');
const assert = require('assert');
const ipaddr = require('ipaddr.js');
const Dgram = require('dgram');

describe('STUN Protocol', function() {
  describe('Stun message with IPv6 mapped Address', function() {
    let message = Buffer.from([
      0x00, 0x01, 0x00, 0x18,  // message header
      0x21, 0x12, 0xa4, 0x42,  // transaction id
      0x29, 0x1f, 0xcd, 0x7c,
      0xba, 0x58, 0xab, 0xd7,
      0xf2, 0x41, 0x01, 0x00,
      0x00, 0x01, 0x00, 0x14,  // Address type (mapped), length
      0x00, 0x02, 0xb8, 0x81,  // family (IPv6), port
      0x24, 0x01, 0xfa, 0x00,  // an IPv6 address
      0x00, 0x04, 0x10, 0x00,
      0xbe, 0x30, 0x5b, 0xff,
      0xfe, 0xe5, 0x00, 0xc3
    ]);

    let address = '2401:fa00:4:1000:be30:5bff:fee5:c3';    
    let port = 47233;
    let family = 'IPv6';
    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(message);
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_REQUEST);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0x291fcd7c);
      assert(tid[1] == 0xba58abd7);
      assert(tid[2] == 0xf2410100); 
    });

    let attr;

    it('GetAttribute(MAPPED_ADDRESS)', function() {
      attr = packet.getAttribute(ORTC.Stun.Attributes.MAPPED_ADDRESS);
      assert(attr);
    });

    it('Checking MAPPED_ADDRESS Family', function() {
      assert(attr.family == family);
    });

    it('Checking MAPPED_ADDRESS Port', function() {
      assert(attr.port == port);
    });

    it('Checking MAPPED_ADDRESS Address', function() {
      assert(attr.address == address);
    });

    it('Encode MAPPED_ADDRESS Address', function() {
      let res = ORTC.Stun.Packet.encode(packet);

      assert(res);
      assert(message.compare(res) == 0);
    });
  });

  describe('Stun message with IPv4 mapped Address', function() {
    let message = Buffer.from([
      0x01, 0x01, 0x00, 0x0c,   // binding response, length 12
      0x21, 0x12, 0xa4, 0x42,   // magic cookie
      0x29, 0x1f, 0xcd, 0x7c,   // transaction ID
      0xba, 0x58, 0xab, 0xd7,
      0xf2, 0x41, 0x01, 0x00,
      0x00, 0x01, 0x00, 0x08,  // Mapped, 8 byte length
      0x00, 0x01, 0x9d, 0xfc,  // AF_INET, unxor-ed port
      0xac, 0x17, 0x44, 0xe6   // IPv4 address
    ]);

    let address = '172.23.68.230';
    let port = 40444;
    let family = 'IPv4';
    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_RESPONSE);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0x291fcd7c);
      assert(tid[1] == 0xba58abd7);
      assert(tid[2] == 0xf2410100); 
    });

    let attr;

    it('GetAttribute(MAPPED_ADDRESS)', function() {
      attr = packet.getAttribute(ORTC.Stun.Attributes.MAPPED_ADDRESS);
      assert(attr);
    });

    it('Checking MAPPED_ADDRESS Family', function() {
      assert(attr.family == family);
    });

    it('Checking MAPPED_ADDRESS Port', function() {
      assert(attr.port == port);
    });

    it('Checking MAPPED_ADDRESS Address', function() {
      assert(attr.address == address);
    });

    it('Encode MAPPED_ADDRESS Address', function() {
      let res = ORTC.Stun.Packet.encode(packet);

      assert(res);
      assert(message.compare(res) == 0);
    });
  });

  describe('Stun message with IPv4 XOR mapped Address', function() {
    let message = Buffer.from([
      0x01, 0x01, 0x00, 0x0c,  // message header (binding response)
      0x21, 0x12, 0xa4, 0x42,  // magic cookie
      0x29, 0x1f, 0xcd, 0x7c,  // transaction ID
      0xba, 0x58, 0xab, 0xd7,
      0xf2, 0x41, 0x01, 0x00,
      0x00, 0x20, 0x00, 0x08,  // address type (xor), length
      0x00, 0x01, 0xfc, 0xb5,  // family (AF_INET), XOR-ed port
      0x8d, 0x05, 0xe0, 0xa4   // IPv4 address
    ]);

    let address = '172.23.68.230'; 
    let port = 56743;
    let family = 'IPv4';
    let transaction = 0x291fcd7c;

    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_RESPONSE);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0x291fcd7c);
      assert(tid[1] == 0xba58abd7);
      assert(tid[2] == 0xf2410100); 
    });

    let attr;

    it('GetAttribute(XOR_MAPPED_ADDRESS)', function() {
      attr = packet.getAttribute(ORTC.Stun.Attributes.XOR_MAPPED_ADDRESS);
      assert(attr);
    });

    it('Checking XOR_MAPPED_ADDRESS Family', function() {
      assert(attr.family == family);
    });

    it('Checking XOR_MAPPED_ADDRESS Port', function() {
      assert(attr.port == port);
    });

    it('Checking XOR_MAPPED_ADDRESS Address', function() {
      assert(attr.address == address);
    });

    it('Encode XOR_MAPPED_ADDRESS Address', function() {
      let res = ORTC.Stun.Packet.encode(packet);

      assert(res);
      assert(message.compare(res) == 0);
    });
  });

  describe('Stun message with IPv6 XOR mapped Address', function() {
    let message = Buffer.from([
      0x01, 0x01, 0x00, 0x18,  // message header (binding response)
      0x21, 0x12, 0xa4, 0x42,  // magic cookie (rfc5389)
      0xe3, 0xa9, 0x46, 0xe1,  // transaction ID
      0x7c, 0x00, 0xc2, 0x62,
      0x54, 0x08, 0x01, 0x00,
      0x00, 0x20, 0x00, 0x14,  // Address Type (XOR), length
      0x00, 0x02, 0xcb, 0x5b,  // family, XOR-ed port
      0x05, 0x13, 0x5e, 0x42,  // XOR-ed IPv6 address
      0xe3, 0xad, 0x56, 0xe1,
      0xc2, 0x30, 0x99, 0x9d,
      0xaa, 0xed, 0x01, 0xc3
    ]);

    let address = '2401:fa00:4:1000:be30:5bff:fee5:c3';  
    let port = 59977;
    let family = 'IPv6';
    let transaction = 0x0e3a946e1;
    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_RESPONSE);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0xe3a946e1);
      assert(tid[1] == 0x7c00c262);
      assert(tid[2] == 0x54080100); 
    });

    let attr;

    it('GetAttribute(XOR_MAPPED_ADDRESS)', function() {
      attr = packet.getAttribute(ORTC.Stun.Attributes.XOR_MAPPED_ADDRESS);
      assert(attr);
    });

    it('Checking XOR_MAPPED_ADDRESS Family', function() {
      assert(attr.family == family);
    });

    it('Checking XOR_MAPPED_ADDRESS Port', function() {
      assert(attr.port == port);
    });

    it('Checking XOR_MAPPED_ADDRESS Address', function() {
      assert(attr.address == address);
    });

    it('Encode XOR_MAPPED_ADDRESS Address', function() {
      let res = ORTC.Stun.Packet.encode(packet);

      assert(res);
      assert(message.compare(res) == 0);
    });
  });

  describe('Stun message with Error response', function() {
    let message = Buffer.from([
      0x01, 0x11, 0x00, 0x14,
      0x21, 0x12, 0xa4, 0x42,
      0x29, 0x1f, 0xcd, 0x7c,
      0xba, 0x58, 0xab, 0xd7,
      0xf2, 0x41, 0x01, 0x00,
      0x00, 0x09, 0x00, 0x10,
      0x00, 0x00, 0x04, 0x01,
      0x55, 0x6e, 0x61, 0x75,
      0x74, 0x68, 0x6f, 0x72,
      0x69, 0x7a, 0x65, 0x64
    ]);

    let transaction = 0x291fcd7c;
    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_ERROR_RESPONSE);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0x291fcd7c);
      assert(tid[1] == 0xba58abd7);
      assert(tid[2] == 0xf2410100); 
    });

    let attr;

    it('GetAttribute(ERROR_CODE)', function() {
      attr = packet.getAttribute(ORTC.Stun.Attributes.ERROR_CODE);
      assert(attr);
    });

    it('Checking error code', function() {
      assert(attr.code == 401);
    });

    it('Encode Error response Address', function() {
      let res = ORTC.Stun.Packet.encode(packet);

      assert(res);
      assert(message.compare(res) == 0);
    });
  });

  describe('Stun message with Unknown attributes + USERNAME', function() {
    let message = Buffer.from([
      0x00, 0x01, 0x00, 0x14,
      0x21, 0x12, 0xa4, 0x42,
      0xe3, 0xa9, 0x46, 0xe1,
      0x7c, 0x00, 0xc2, 0x62,
      0x54, 0x08, 0x01, 0x00,
      0x00, 0xaa, 0x00, 0x07,  // Unknown attribute, length 7 (needs padding!)
      0x61, 0x62, 0x63, 0x64,  // abcdefg + padding
      0x65, 0x66, 0x67, 0x00,
      0x00, 0x06, 0x00, 0x03,  // Followed by a known attribute we can
      0x61, 0x62, 0x63, 0x00   // check for (username of length 3)
    ]);

    let transaction = 0x291fcd7c;
    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_REQUEST);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0xe3a946e1);
      assert(tid[1] == 0x7c00c262);
      assert(tid[2] == 0x54080100);
    });

    let attr;

    it('GetAttribute(USERNAME)', function() {
      attr = packet.getAttribute(ORTC.Stun.Attributes.USERNAME);
      assert(attr);
    });

    it('Checking USERNAME', function() {
      assert(attr == 'abc');
    });
  });

  describe('Stun message with UNKNOWN_ATTRIBUTE', function() {
    let message = Buffer.from([
      0x00, 0x01, 0x00, 0x0c,
      0x21, 0x12, 0xa4, 0x42,
      0xe3, 0xa9, 0x46, 0xe1,
      0x7c, 0x00, 0xc2, 0x62,
      0x54, 0x08, 0x01, 0x00,
      0x00, 0x0a, 0x00, 0x06,  // username attribute (length 6)
      0x00, 0x01, 0x10, 0x00,  // three attributes plus padding
      0xAB, 0xC, 0xAB, 0xC
    ]);

    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_REQUEST);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0xe3a946e1);
      assert(tid[1] == 0x7c00c262);
      assert(tid[2] == 0x54080100); 
    });

    let attr;

    it('GetAttribute(UNKNOWN_ATTRIBUTES)', function() {
      attr = packet.getAttribute(ORTC.Stun.Attributes.UNKNOWN_ATTRIBUTES);
      assert(attr);
    });

    it('Checking UNKNOWN_ATTRIBUTES', function() {
      assert(attr.length == 3);
      assert(attr[0] == 0x01);
      assert(attr[1] == 0x1000);
      assert(attr[2] == 0xAB0C);
    });

    it('Encode UNKNOWN_ATTRIBUTES', function() {
      let res = ORTC.Stun.Packet.encode(packet);

      assert(res);
      assert(message.compare(res) == 0);
    });
  });

  describe('Stun message with USERNAME', function() {
    let message = Buffer.from([
      0x00, 0x01, 0x00, 0x08,
      0x21, 0x12, 0xa4, 0x42,
      0xe3, 0xa9, 0x46, 0xe1,
      0x7c, 0x00, 0xc2, 0x62,
      0x54, 0x08, 0x01, 0x00,
      0x00, 0x06, 0x00, 0x03,  // username attribute (length 3)
      0x61, 0x62, 0x63, 0x20   // abc
    ]);

    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_REQUEST);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0xe3a946e1);
      assert(tid[1] == 0x7c00c262);
      assert(tid[2] == 0x54080100); 
    });

    let attr;

    it('GetAttribute(USERNAME)', function() {
      attr = packet.getAttribute(ORTC.Stun.Attributes.USERNAME);
      assert(attr);
    });

    it('Checking USERNAME', function() {
      assert(attr == 'abc');
    });

    it('Encode USERNAME', function() {
      let res = ORTC.Stun.Packet.encode(packet);

      assert(res);
      assert(message.compare(res) == 0);
    });
  });

  describe('Stun message with RFC5769 BINDING_REQUEST', function() {
    let message = Buffer.from([
      0x00, 0x01, 0x00, 0x58,   //    Request type and message length
      0x21, 0x12, 0xa4, 0x42,   //    Magic cookie
      0xb7, 0xe7, 0xa7, 0x01,   // }
      0xbc, 0x34, 0xd6, 0x86,   // }  Transaction ID
      0xfa, 0x87, 0xdf, 0xae,   // }
      0x80, 0x22, 0x00, 0x10,   //    SOFTWARE attribute header
      0x53, 0x54, 0x55, 0x4e,   // }
      0x20, 0x74, 0x65, 0x73,   // }  User-agent...
      0x74, 0x20, 0x63, 0x6c,   // }  ...name
      0x69, 0x65, 0x6e, 0x74,   // }
      0x00, 0x24, 0x00, 0x04,   //    PRIORITY attribute header
      0x6e, 0x00, 0x01, 0xff,   //    ICE priority value
      0x80, 0x29, 0x00, 0x08,   //    ICE-CONTROLLED attribute header
      0x93, 0x2f, 0xf9, 0xb1,   // }  Pseudo-random tie breaker...
      0x51, 0x26, 0x3b, 0x36,   // }   ...for ICE control
      0x00, 0x06, 0x00, 0x09,   //    USERNAME attribute header
      0x65, 0x76, 0x74, 0x6a,   // }
      0x3a, 0x68, 0x36, 0x76,   // }  Username (9 bytes) and padding (3 bytes)
      0x59, 0x20, 0x20, 0x20,   // }
      0x00, 0x08, 0x00, 0x14,   //    MESSAGE-INTEGRITY attribute header
      0x9a, 0xea, 0xa7, 0x0c,   // }
      0xbf, 0xd8, 0xcb, 0x56,   // }
      0x78, 0x1e, 0xf2, 0xb5,   // }  HMAC-SHA1 fingerprint
      0xb2, 0xd3, 0xf2, 0x49,   // }
      0xc1, 0xb5, 0x71, 0xa2,   // }
      0x80, 0x28, 0x00, 0x04,   //    FINGERPRINT attribute header
      0xe5, 0x7a, 0x3b, 0xcf    //    CRC32 fingerprint
    ]);

    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_REQUEST);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0xb7e7a701);
      assert(tid[1] == 0xbc34d686);
      assert(tid[2] == 0xfa87dfae);
    });

    it('GetAttributes().length == 6', function() {
      let attr = packet.getAttributes();
      assert(attr);
      assert(attr.length == 6);
    });

    it('GetAttribute(USERNAME)', function() {
      let attr = packet.getAttribute(ORTC.Stun.Attributes.USERNAME);
      assert(attr);
      assert(attr == 'evtj:h6vY');
    });

    it('GetAttribute(SOFTWARE)', function() {
      let attr = packet.getAttribute(ORTC.Stun.Attributes.SOFTWARE);
      assert(attr);
      assert(attr == 'STUN test client');
    });

    it('GetAttribute(PRIORITY)', function() {
      let attr = packet.getAttribute(ORTC.Stun.Attributes.PRIORITY);
      assert(attr);
      assert(attr == 1845494271);
    });

    it('GetAttribute(ICE-CONTROLLED)', function() {
      let attr = packet.getAttribute(ORTC.Stun.Attributes.ICE_CONTROLLED);
      assert(attr);
    });

    it('validateMessageIntegrity()', function() {
      assert(ORTC.Stun.Packet.validateMessageIntegrity(packet, message, 'VOkJxbRl1RmTxUk/WvJxBt'));
    });

    it('validateFingerPrint()', function() {
      assert(ORTC.Stun.Packet.validateFingerPrint(packet, message));
    });

    describe('Encode Packet', function() {
      let buffer;

      it('Building Packet', function() {
        buffer = ORTC.Stun.Packet.encode(packet, 'VOkJxbRl1RmTxUk/WvJxBt', true);
        assert(buffer);
      });

      it('Parsing Stun Message', function() {
        packet = ORTC.Stun.Packet.decode(Buffer.from(buffer));
        assert(packet);
      });
  
      it('Checking RFC5389', function() {
        assert(packet.isRFC5389);
      });
  
      it('Checking Message Type', function() {
        assert(packet.type == ORTC.Stun.MessageType.BINDING_REQUEST);
      });
  
      it('Checking Transaction Id', function() {
        let tid = packet.transactionId();
  
        assert(tid.length == 3);
        assert(tid[0] == 0xb7e7a701);
        assert(tid[1] == 0xbc34d686);
        assert(tid[2] == 0xfa87dfae);
      });
  
      it('GetAttributes().length == 6', function() {
        let attr = packet.getAttributes();
        assert(attr);
        assert(attr.length == 6);
      });
  
      it('GetAttribute(USERNAME)', function() {
        let attr = packet.getAttribute(ORTC.Stun.Attributes.USERNAME);
        assert(attr);
        assert(attr == 'evtj:h6vY');
      });
  
      it('GetAttribute(SOFTWARE)', function() {
        let attr = packet.getAttribute(ORTC.Stun.Attributes.SOFTWARE);
        assert(attr);
        assert(attr == 'STUN test client');
      });
  
      it('GetAttribute(PRIORITY)', function() {
        let attr = packet.getAttribute(ORTC.Stun.Attributes.PRIORITY);
        assert(attr);
        assert(attr == 1845494271);
      });
  
      it('GetAttribute(ICE-CONTROLLED)', function() {
        let attr = packet.getAttribute(ORTC.Stun.Attributes.ICE_CONTROLLED);
        assert(attr);
      });
  
      it('validateMessageIntegrity()', function() {
        assert(ORTC.Stun.Packet.validateMessageIntegrity(packet, buffer, 'VOkJxbRl1RmTxUk/WvJxBt'));
      });
  
      it('validateFingerPrint()', function() {
        assert(ORTC.Stun.Packet.validateFingerPrint(packet, buffer));
      }); 
    });
  });

  describe('Stun message with RFC5769 BINDING_REQUEST with Long-Term Authentication', function() {
    let message = Buffer.from([
      0x00, 0x01, 0x00, 0x60,  //    Request type and message length
      0x21, 0x12, 0xa4, 0x42,  //    Magic cookie
      0x78, 0xad, 0x34, 0x33,  // }
      0xc6, 0xad, 0x72, 0xc0,  // }  Transaction ID
      0x29, 0xda, 0x41, 0x2e,  // }
      0x00, 0x06, 0x00, 0x12,  //    USERNAME attribute header
      0xe3, 0x83, 0x9e, 0xe3,  // }
      0x83, 0x88, 0xe3, 0x83,  // }
      0xaa, 0xe3, 0x83, 0x83,  // }  Username value (18 bytes) and padding (2 bytes)
      0xe3, 0x82, 0xaf, 0xe3,  // }
      0x82, 0xb9, 0x00, 0x00,  // }
      0x00, 0x15, 0x00, 0x1c,  //    NONCE attribute header
      0x66, 0x2f, 0x2f, 0x34,  // }
      0x39, 0x39, 0x6b, 0x39,  // }
      0x35, 0x34, 0x64, 0x36,  // }
      0x4f, 0x4c, 0x33, 0x34,  // }  Nonce value
      0x6f, 0x4c, 0x39, 0x46,  // }
      0x53, 0x54, 0x76, 0x79,  // }
      0x36, 0x34, 0x73, 0x41,  // }
      0x00, 0x14, 0x00, 0x0b,  //    REALM attribute header
      0x65, 0x78, 0x61, 0x6d,  // }
      0x70, 0x6c, 0x65, 0x2e,  // }  Realm value (11 bytes) and padding (1 byte)
      0x6f, 0x72, 0x67, 0x00,  // }
      0x00, 0x08, 0x00, 0x14,  //    MESSAGE-INTEGRITY attribute header
      0xf6, 0x70, 0x24, 0x65,  // }
      0x6d, 0xd6, 0x4a, 0x3e,  // }
      0x02, 0xb8, 0xe0, 0x71,  // }  HMAC-SHA1 fingerprint
      0x2e, 0x85, 0xc9, 0xa2,  // }
      0x8c, 0xa8, 0x96, 0x66   // }
    ]);

    let packet;

    it('Parsing Stun Message', function() {
      packet = ORTC.Stun.Packet.decode(Buffer.from(message));
      assert(packet);
    });

    it('Checking RFC5389', function() {
      assert(packet.isRFC5389);
    });

    it('Checking Message Type', function() {
      assert(packet.type == ORTC.Stun.MessageType.BINDING_REQUEST);
    });

    it('Checking Transaction Id', function() {
      let tid = packet.transactionId();

      assert(tid.length == 3);
      assert(tid[0] == 0x78ad3433);
      assert(tid[1] == 0xc6ad72c0);
      assert(tid[2] == 0x29da412e);
    });

    it('GetAttributes().length == 4', function() {
      let attr = packet.getAttributes();
      assert(attr);
      assert(attr.length == 4);
    });

    it('GetAttribute(USERNAME)', function() {
      let attr = packet.getAttribute(ORTC.Stun.Attributes.USERNAME);
      assert(attr);

      let username = Buffer.from([0xe3, 0x83, 0x9e, 0xe3, 
                                  0x83, 0x88, 0xe3, 0x83, 
                                  0xaa, 0xe3, 0x83, 0x83, 
                                  0xe3, 0x82, 0xaf, 0xe3, 
                                  0x82, 0xb9]);

      assert(username.compare(Buffer.from(attr)) == 0);
    });

    it('GetAttribute(NONCE)', function() {
      let attr = packet.getAttribute(ORTC.Stun.Attributes.NONCE);
      assert(attr);
      assert(attr == 'f//499k954d6OL34oL9FSTvy64sA');
    });

    it('GetAttribute(REALM)', function() {
      let attr = packet.getAttribute(ORTC.Stun.Attributes.REALM);
      assert(attr);
      assert(attr == 'example.org');
    });

    let key;

    it('generateCredentialHash()', function() {
      key = ORTC.Stun.Packet.generateCredentialHash(
        packet.getAttribute(ORTC.Stun.Attributes.USERNAME),
        packet.getAttribute(ORTC.Stun.Attributes.REALM),
        'TheMatrIX'
      );
    });

    it('validateMessageIntegrity()', function() {
      assert(ORTC.Stun.Packet.validateMessageIntegrity(packet, message, key));
    });

    describe('Encode Packet', function() {
      let buffer;

      it('Building Packet', function() {
        buffer = ORTC.Stun.Packet.encode(packet, key);
        assert(buffer);
      });

      it('Parsing Stun Message', function() {
        packet = ORTC.Stun.Packet.decode(Buffer.from(buffer));
        assert(packet);
      });
  
      it('Checking RFC5389', function() {
        assert(packet.isRFC5389);
      });
  
      it('Checking Message Type', function() {
        assert(packet.type == ORTC.Stun.MessageType.BINDING_REQUEST);
      });
  
      it('Checking Transaction Id', function() {
        let tid = packet.transactionId();
  
        assert(tid.length == 3);
        assert(tid[0] == 0x78ad3433);
        assert(tid[1] == 0xc6ad72c0);
        assert(tid[2] == 0x29da412e);
      });
  
      it('GetAttributes().length == 4', function() {
        let attr = packet.getAttributes();
        assert(attr);
        assert(attr.length == 4);
      });
  
      it('GetAttribute(USERNAME)', function() {
        let attr = packet.getAttribute(ORTC.Stun.Attributes.USERNAME);
        assert(attr);
  
        let username = Buffer.from([0xe3, 0x83, 0x9e, 0xe3, 
                                    0x83, 0x88, 0xe3, 0x83, 
                                    0xaa, 0xe3, 0x83, 0x83, 
                                    0xe3, 0x82, 0xaf, 0xe3, 
                                    0x82, 0xb9]);
  
        assert(username.compare(Buffer.from(attr)) == 0);
      });
  
      it('GetAttribute(NONCE)', function() {
        let attr = packet.getAttribute(ORTC.Stun.Attributes.NONCE);
        assert(attr);
        assert(attr == 'f//499k954d6OL34oL9FSTvy64sA');
      });
  
      it('GetAttribute(REALM)', function() {
        let attr = packet.getAttribute(ORTC.Stun.Attributes.REALM);
        assert(attr);
        assert(attr == 'example.org');
      });
  
      it('validateMessageIntegrity()', function() {
        assert(ORTC.Stun.Packet.validateMessageIntegrity(packet, buffer, key));
      }); 
    });
  });

  describe('STUN BINDING_REQUEST', function() {
    this.timeout(10000);
  
    let client;

    it('Binding socket', function(done) {
      client = Dgram.createSocket('udp4');

      client.bind(() => {
        done();
      });
    });

    let buffer;

    it('Building packet BINDING_REQUEST', function() {
      const packet = new ORTC.Stun.Packet(ORTC.Stun.MessageType.BINDING_REQUEST, true);
      buffer = ORTC.Stun.Packet.encode(packet);
    });

    it('Sending packet to stun.l.google.com:19302', function(done) {
      client.send(buffer, 19302, 'stun.l.google.com', () => {
        done();
      });
    });

    it('Waiting response from stun.l.google.com:19302', function(done) {
      const callback = (error) => {
        if (done) {
          done(error)
        }
  
        done = undefined;
      };

      client.on('close', () => {
        callback();
      });
      
      client.on('message', (msg, rinfo) => {
        const packet = ORTC.Stun.Packet.decode(msg);
        const mapped = packet.getAttribute(ORTC.Stun.Attributes.MAPPED_ADDRESS);
  
        if (!mapped) {
          const xor_mapped = packet.getAttribute(ORTC.Stun.Attributes.XOR_MAPPED_ADDRESS);
  
          if (!xor_mapped) {
            callback(new Error('Missing STUN Attributes [ MAPPED_ADDRESS || XOR_MAPPED_ADDRESS ]'));
          }
        }
        
        client.close();
      });
    });
  });
});
