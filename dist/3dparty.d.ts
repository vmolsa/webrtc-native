/// <reference types="node" />
declare namespace CRC {
    function crc32(value: Buffer): number;
}
declare module "crc" {
    export = CRC;
}
declare namespace IP {
    function parse(address: string): any;
    function fromByteArray(array: any): any;
}
declare module "ipaddr.js" {
    export = IP;
}
