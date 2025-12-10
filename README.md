# Brutalshell

This is a AI Agent command-line shell wrapper for accelerated problem solving and task automation. It leverages large language models (LLMs) to interpret user commands and execute them in a shell environment, providing a more intuitive and efficient way to interact with the command line.

It also supports lagacy AI intergration like command prediction, natural language to command translation and auto-completion.

## Features

- [ ] Natural language command execution
- [ ] Command prediction and auto-completion
- [ ] Integration with popular LLMs
- [ ] Cross-platform/shell support (Unix-like shells)

## Development

To set up the development environment, follow these steps:

1. Install uv

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

2. Clone the repository

```bash
git clone https://github.com/AMD-AI-Agent-Online-Hackthon/Brutalshell.git
cd Brutalshell
```

3. Install dependencies

```bash
uv sync
```

4. Build the wrapper

```bash
cd wrapper
cmake -B build
cmake --build build
cd ..
```

5. Build the helper

```bash
cd helper
cmake -B build
cmake --build build
cd ..
```

### Run

To execute wrapper + daemon, it's recommended to use tmux.

On first tmux pane, run the daemon:

```bash
uv run src/main.py
```

On second tmux pane, run the wrapper:

```bash
./wrapper/build/bsh
```

## Usage

If you have installed the helper, you can launch wrapper and run:

```bash
helper list files in current directory
```

If not, you can run:

```bash
./helper/build/helper list files in current directory
```
