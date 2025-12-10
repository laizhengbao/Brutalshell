def test_build_body():
    from vllm import build_body

    prompt = "$ ls -la"
    body = build_body(prompt)

    assert "messages" in body
    assert len(body["messages"]) == 2
    assert body["messages"][1]["role"] == "user"
    assert body["messages"][1]["content"] == "ls -la"
    assert body["temperature"] == 0.3

def test_completions():
    from vllm import completions, proxy_host

    prompt = "List all files in the current directory"
    response = completions(prompt, proxy_host)

    assert response is not None
    assert isinstance(response, str)
    assert len(response) > 0
    assert response.startswith("ls")  # Assuming the response is a command starting with 'ls'
