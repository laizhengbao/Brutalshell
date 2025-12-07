import httpx
import re

"""
Client that sends requests directly to vLLM servers.

It prompts for model (1 or 2), an optional path (default `chat/completions`),
and a prompt. It then POSTs the vLLM-style body directly to the chosen
upstream endpoint and prints the response.
"""

proxy_host = "http://100.68.65.78:8887"

def build_body(prompt: str):
    prompt = prompt.lstrip("$ ")
    
    # Minimal chat-style body compatible with many vLLM endpoints
    system_prompt = (
        "You are a smart CLI helper with two modes:\n"
        "1. AUTOCOMPLETE: If input is a partial command, output ONLY the missing suffix.\n"
        "2. TRANSLATE: If input is natural language (English/Chinese) asking for a task, output the FULL command.\n"
        "Format: Always wrap the final result strictly in [[ ]]."
    )
    # print(prompt.encode())
    return {
        "messages": [
                    {"role": "system", "content": system_prompt},
                    {"role": "user", "content": prompt}
        ],
        "temperature": 0.5
    }


def completions(prompt: str, host: str):
    path = "chat/completions"
    body = build_body(prompt)
    url = host.rstrip("/") + "/" + path.lstrip("/")

    try:
        with httpx.Client(timeout=120.0) as client:
            resp = client.post(url, json=body, headers={"Content-Type": "application/json"})
    except httpx.RequestError as e:
        print("Request error:", e)
        return

    # print(f"Upstream URL: {url}  Status: {resp.status_code}")
    try:
        suffix = re.findall(r'\[\[(.*?)\]\]', resp.json()['choices'][0]['message']["content"])[-1]
        return suffix
        # return resp.json()['choices'][0]['message']["content"]
    except Exception:
        print("exclamation mark problem occurred!")