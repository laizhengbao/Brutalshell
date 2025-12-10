# Brutalshell

This is a AI Agent command-line shell wrapper for accelerated problem solving and task automation. It leverages large language models (LLMs) to interpret user commands and execute them in a shell environment, providing a more intuitive and efficient way to interact with the command line.

It also supports lagacy AI intergration like command prediction, natural language to command translation and auto-completion.

## Features

- [x] Natural language command execution
- [ ] Command prediction and auto-completion
- [ ] Integration with popular LLMs
- [ ] Cross-platform/shell support (Unix-like shells)

## Build

To set up the development environment, follow these steps:

1. Install uv (If you don't have it already)

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

## Development

This project has 3 main components:

1. **Wrapper**: The command-line interface that users interact with.
2. **Helper**: A utility that processes natural language commands and translates them into shell commands.
3. **Daemon**: A background service that manages communication between the wrapper and helper, and handles LLM interactions.

Before pushing any changes, make sure to test your code thoroughly.

```bash
# Daemon tests
uv run pytest

# Wrapper tests
cd wrapper
rm -rf build
cmake -B build
cmake --build build
ctest --test-dir build
cd ..

# Helper tests
cd helper
rm -rf build
cmake -B build
cmake --build build
ctest --test-dir build
cd ..
```
