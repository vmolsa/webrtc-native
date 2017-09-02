declare namespace Stun {
  export enum messageType {
    BINDING_REQUEST = 0x0001,
    BINDING_INDICATION = 0x0011,
    BINDING_RESPONSE = 0x0101,
    BINDING_ERROR_RESPONSE = 0x0111
  }
}

declare module "stun" {
  export = Stun;
}