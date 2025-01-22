// frontend/src/lib/ipc.ts
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
    this.messageHandlers = new Map();
    this.messageCounter = 0;

   window.chrome?.webview.addEventListener("message", (event) => {
      const { id, payload } = event.data;
      const handler = this.messageHandlers.get(id);
      if (handler) {
        handler(payload);
        this.messageHandlers.delete(id);
      }
    });
  }

  static getInstance(): IpcClient {
    if (!IpcClient.instance) {
      IpcClient.instance = new IpcClient();
    }
    return IpcClient.instance;
  }

  async send<T>(type: string, payload: any = {}): Promise<IpcResponse<T>> {
    return new Promise((resolve) => {
      const id = `msg_${++this.messageCounter}`;

      this.messageHandlers.set(id, (response) => {
        resolve(response as IpcResponse<T>);
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

