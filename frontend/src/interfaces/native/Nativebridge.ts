/* eslint-disable @typescript-eslint/no-explicit-any */
/* eslint-disable @typescript-eslint/no-unsafe-assignment */
/* eslint-disable @typescript-eslint/no-unsafe-argument */
/* eslint-disable @typescript-eslint/no-unsafe-call */
/* eslint-disable @typescript-eslint/no-unsafe-member-access */
import { type NativeAPI } from "./NativeAPI";

interface Chrome {
  webview: {
    addEventListener: (event: string, callback: (event: any) => void) => void;
    postMessage: (message: any) => void;
  };
}

declare global {
  interface Window {
    chrome: Chrome;
  }
}

export class Nativebridge implements NativeAPI {
  private callbacks = new Map<string, ((data: any) => void)[]>();

  constructor() {
    // Handle messages from native code
    window.chrome.webview.addEventListener("message", (event) => {
      const { type, data } = JSON.parse(event.data);
      this.callbacks.get(type)?.forEach((callback) => callback(data));
    });
  }

  async send(event: string, data: any): Promise<any> {
    return new Promise((resolve) => {
      const messageId = Date.now().toString();

      // One-time handler for this message's response
      this.on(`${event}:response:${messageId}`, (responseData) => {
        resolve(responseData);
      });

      // Send to native
      window.chrome.webview.postMessage({
        type: event,
        id: messageId,
        data,
      });
    });
  }

  on(event: string, callback: (data: any) => void) {
    if (!this.callbacks.has(event)) {
      this.callbacks.set(event, []);
    }
    this.callbacks.get(event)?.push(callback);
  }
}
