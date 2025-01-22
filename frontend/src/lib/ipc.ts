type IpcResponse<T> = {
  success: boolean;
  payload?: T;
  error?: string;
};

class IpcClient {
  private static instance: IpcClient;
  private messageHandlers: Map<string, (response: any) => void>;
  private messageCounter: number;

  private constructor() {
    console.log("Initializing IpcClient");
    this.messageHandlers = new Map();
    this.messageCounter = 0;

    if (typeof window !== "undefined" && window.chrome?.webview) {
      console.log("WebView detected, setting up message listener");
      window.chrome.webview.addEventListener("message", (event) => {
        const { id, payload } = event.data;
        const handler = this.messageHandlers.get(id);
        if (handler) {
          handler(payload);
          this.messageHandlers.delete(id);
        }
      });
    } else {
      console.log("WebView not detected during IpcClient initialization");
    }
  }

  static getInstance(): IpcClient {
    if (!IpcClient.instance) {
      IpcClient.instance = new IpcClient();
    }
    return IpcClient.instance;
  }

  // async send<T>(type: string, payload: any = {}): Promise<IpcResponse<T>> {
  //   return new Promise((resolve) => {
  //     const id = `msg_${++this.messageCounter}`;

  //     this.messageHandlers.set(id, (response) => {
  //       resolve(response as IpcResponse<T>);
  //     });

  //     window.chrome?.webview.postMessage({
  //       type,
  //       id,
  //       payload,
  //     });
  //   });
  // }
  async send<T>(type: string, payload: any = {}): Promise<IpcResponse<T>> {
    return new Promise((resolve) => {
      const id = `msg_${++this.messageCounter}`;
      console.log(`Sending message ${id} of type ${type}`);

      this.messageHandlers.set(id, (response) => {
        console.log(`Received response for message ${id}:`, response);
        resolve(response as IpcResponse<T>);
      });

      // Add event listener for this specific message
      window.chrome?.webview.addEventListener("message", (event) => {
        const response = event.data;
        console.log("Received WebView message:", response);
        if (response.id === id) {
          const handler = this.messageHandlers.get(id);
          if (handler) {
            handler(response.payload);
            this.messageHandlers.delete(id);
          }
        }
      });

      window.chrome?.webview.postMessage({
        type,
        id,
        payload,
      });
    });
  }
}

export const ipc = IpcClient.getInstance();
