declare namespace CRC {
  export function crc32(value: Buffer): number;
}
  
declare module "crc" {
  export = CRC;
}