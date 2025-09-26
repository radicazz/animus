# helipad/tools

Various tools to assist in various aspects of development.

## Commands

```sh
# Format CMake files in the repository.
uv run --extra formatting format_cmake
```

```sh
# Generate Doxygen documentation under build/docs/doxygen/.
uv run --extra documentation docs_source

# Build and serve MkDocs documentation.
uv run --extra documentation docs_user

# The two steps above in one go.
uv run --extra documentation docs_all
```

## Setup

These tools have been written for Python 3.12 and will run on any platform that supports it, just remember the dependencies. I recommend using [`uv`](<https://pypi.org/project/uv/>) for environment management as it is very fast, lightweight and easy to use.

```sh
# On macOS and Linux.
curl -LsSf https://astral.sh/uv/install.sh | sh
```

```sh
# On Windows.
powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"
```

Once you have `uv` installed, make sure its on your path by running `uv --version`. Now you just need to make you have the correct Python version installed:

```sh
# Check if you have Python 3.12 installed.
uv python list
```

```sh
# If you don't see Python 3.12 in the list, install it.
uv python install 3.12
```

The first time you run one of the tools in this directory, `uv` will automatically create a virtual environment and install the dependencies for you but if you want to manually set them up you can run:

```sh
uv sync --all-extras
```

---

More tools will be added in the future as needed.
