/* eslint-disable @typescript-eslint/no-explicit-any */
interface WebView {
  postMessage(message: any): void;
  addEventListener(type: string, listener: (event: MessageEvent) => void): void;
  removeEventListener(
    type: string,
    listener: (event: MessageEvent) => void,
  ): void;
}

interface Chrome {
  webview: WebView;
}

interface Window {
  chrome?: Chrome;
}
