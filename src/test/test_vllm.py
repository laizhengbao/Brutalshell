def test_build_body():
    from vllm import build_body

    prompt = "$ ls -la"
    body = build_body(prompt)

    assert "messages" in body
    assert len(body["messages"]) == 2
    assert body["messages"][1]["role"] == "user"
    assert body["messages"][1]["content"] == "ls -la"
    assert body["temperature"] == 0.3
