
declare namespace IP {
  export function parse(address: string): any;
}

declare module "ipaddr.js" {
  export = IP;
}