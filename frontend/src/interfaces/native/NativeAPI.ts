export interface NativeAPI {
  send: (event: string, data: any) => Promise<any>;
  on: (event: string, callback: (data: any) => void) => void;
}
