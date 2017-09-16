declare namespace CRC {
  export function crc32(value: Buffer): number;
}
  
declare module "crc" {
  export = CRC;
}

declare namespace IP {
  export function parse(address: string): any;
  export function fromByteArray(array: any): any;
}

declare module "ipaddr.js" {
  export = IP;
}