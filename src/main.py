import os
import socket
import sys
import vllm
from threading import Thread
import time


WRAPPER_PATH = "/tmp/brutalshell.sock"
HELPER_PATH = "/path/to/helper"


class Buffer:
    def __init__(self):
        self.data = b''
        self.last_update = 0
        
def llm(buffer: Buffer, host: str):
    
    start_time = time.time()
    time.sleep(1)
    if(buffer.last_update > start_time): return
    
    suffix = vllm.completions(buffer.data.decode(), host)
    if(buffer.last_update > start_time): return
    
    print(suffix)
    # send suffix to helper
    
    
def main():
    default_host = "http://100.68.65.78:8887"
    host_env = os.getenv("VLLM_SERVER_URL")
    if host_env:
        host = host_env
    else:
        host = default_host
    if os.path.exists(WRAPPER_PATH):
        os.remove(WRAPPER_PATH)

    server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    server.bind(WRAPPER_PATH)
    server.listen(1)
    
    print(f"Listening on {WRAPPER_PATH}...")

    while True:
        try:
            conn, _ = server.accept()
            
            buffer = Buffer()
            while True:
                data = conn.recv(1024)
                
                if(not data):
                    break
                if(b'\r\n' in data):
                    buffer = Buffer()
                    continue
                buffer.data += data
                buffer.last_update = time.time()
                if(buffer.data != "$ "):
                    Thread(target=llm, args=(buffer,host)).start()
                
                
                # sys.stdout.buffer.write(data)
                # sys.stdout.flush()
            conn.close()
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(f"Error: {e}")

    os.remove(WRAPPER_PATH)

if __name__ == "__main__":
    main()