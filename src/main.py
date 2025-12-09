import os
import socket
import sys
import vllm
from threading import Thread
import time
import pyte
import uuid

DEFAULT_HOST = "http://100.68.65.78:8887"

DAEMON_PATH = "/tmp/brutalshell.sock"

wrapper_sessions = {}
'''
{
    "session_id1": {
        "server": thread1,
        "connection": conn1,
        "buffer": buffer1
    }
    "session_id2": {
        "server": thread2,
        "connection": conn2,
        "buffer": buffer2
    }
    ...
}
'''



# Filter the control characters.
def content_filter(data: bytes):
    screen = pyte.Screen(80, 24)
    stream = pyte.Stream(screen)

    text_data = data.decode('utf-8', errors="replace")
    stream.feed(text_data)
    
    lines = []
    for y in range(screen.lines):
        line_str = ""
        if y in screen.buffer:
            line_data = screen.buffer[y]
            for x in range(screen.columns):
                char = line_data.get(x)
                if char and char.data:
                    line_str += char.data
        
        lines.append(line_str.rstrip())

    return "".join(lines)

'''
Receive WRAPPER_SESSION_ID, USER_MSG from helper.
Send prompt to vllm, and send the response to wrapper
'''
def handle_helper(conn, data: bytes, host: str):
    wrapper_session_id, user_msg = data.decode(encoding='utf-8').split('\x1f') # WRAPPER_SESSION_ID + \x1f + USER_MSG
    context = content_filter(wrapper_sessions[wrapper_session_id]["buffer"])
    prompt = f"Context: {context}\nUser input: {user_msg}"
    suffix = vllm.completions(prompt, host)
    wrapper_sessions[wrapper_session_id]["connection"].send(suffix.encode())
    conn.close()

'''
Create a session id for wrapper.
Get the bsh content from wrapper, and add to buffer.
'''     
def wrapper_server(session_id: str):
    while True:
        data = wrapper_sessions[session_id]["connection"].recv(1024)
        
        if(not data):
            break
        wrapper_sessions[session_id]["buffer"] += data
    
    wrapper_sessions[session_id]["connection"].close()
    del wrapper_sessions[session_id]
    
    
def main():
    
    host_env = os.getenv("VLLM_SERVER_URL")
    if host_env:
        host = host_env
    else:
        host = DEFAULT_HOST
    if os.path.exists(DAEMON_PATH):
        os.remove(DAEMON_PATH)
    
    server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    server.bind(DAEMON_PATH)
    server.listen(10)
    
    print(f"Listening on {DAEMON_PATH}...")
    
    while True:
        try:
            conn, _ = server.accept()
            
            session_id = str(uuid.uuid4())
            data = conn.recv(1024)
            if(b'NEW_SESSION_ID' in data):
                conn.send(session_id.encode())
            
            if(b'\x1f' in data):
                Thread(target=handle_helper, args=(conn, data, host)).start()
            else :
                wrapper_sessions[session_id] = {
                    "server": Thread(target=wrapper_server, args=(session_id,)),
                    "connection": conn,
                    "buffer": b''
                }
                wrapper_sessions[session_id]["server"].start()
                
            
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(f"Error: {e}")

    os.remove(DAEMON_PATH)

if __name__ == "__main__":
    main()
