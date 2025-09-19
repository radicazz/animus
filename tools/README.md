# helipad/tools

This directory contains various optional scripts and utilities to assist you during development.

## Requirements

You will need the following to run the tools:

- Python installation (3.12+)
- Package manager for managing dependencies

We recommend [uv](https://github.com/astral-sh/uv), a fast package manager for Python.

## Installation

If you already have Python installed, follow these instructions:

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

---

If you do not have Python installed, follow these instructions:

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
