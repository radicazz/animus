# helipad/tools

This directory contains various optional scripts and utilities to assist you during development.

## Requirements

The requirements for them are as follows:

- Python development environemt (3.12+)
- Package manager for dependencies

> We recommend [uv](https://github.com/astral-sh/uv), a fast package manager for Python.

## Installation

Follow the correct steps below, depending on whether or not you have Python installed already.

> If you are not sure, simply open up a terminal window and run `python`.
>> You will experience an error if you do not have Python installed.

<details>
<summary>Python is installed</summary>

1. Install [pipx](https://pypa.github.io/pipx/):

   ```bash
   python -m pip install --user pipx
   python -m pipx ensurepath
   ```

2. Install **uv** using pipx:

   ```bash
   pipx install uv
   ```

</details>

<details>
<summary>Python is NOT installed</summary>

1. Install **uv** (see [docs](https://github.com/astral-sh/uv#installation) for your platform):

   ```bash
   curl -LsSf https://astral.sh/uv/install.sh | sh
   ```

2. Use uv to install Python:

   ```bash
   uv python install
   ```

</details>

## Usage

*Nothing to see here yet... come back later.*

---

> [!NOTE]  
> This directory and its scripts are under development and will change in future releases.
