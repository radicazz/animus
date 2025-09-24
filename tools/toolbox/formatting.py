import sys
import subprocess
import shutil
from pathlib import Path
from typing import Iterable

EXCLUDE_DIRS = {".git", "build", "external", "private", ".venv", ".env"}

def _repo_root() -> Path:
    return Path(__file__).resolve().parents[2]

def _iter_cmake_files(root: Path) -> Iterable[Path]:
    # Find CMakeLists.txt and *.cmake files, excluding certain directories.
    for p in root.rglob("CMakeLists.txt"):
        if not _is_excluded(p):
            yield p
    for p in root.rglob("*.cmake"):
        if not _is_excluded(p):
            yield p

def _is_excluded(path: Path) -> bool:
    parts = set(part.lower() for part in path.parts)
    return any(d.lower() in parts for d in EXCLUDE_DIRS)

def _run_gersemi_on_batch(files: list[Path]) -> None:
    if not files:
        return

    gersemi_exe = shutil.which("gersemi")
    if gersemi_exe:
        cmd = [gersemi_exe, "--in-place", *[str(p) for p in files]]
    else:
        raise RuntimeError("Gersemi is not available in your current environment.")

    try:
        subprocess.run(cmd, check=True)
    except FileNotFoundError as e:
        raise RuntimeError(
            "Gersemi is not available. Re-run with 'uv run --extra formatting format_cmake'"
        ) from e
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"gersemi failed with exit code {e.returncode}") from e


def _format_all_cmake_files() -> int:
    """Format all CMake files in the repository using gersemi.

    Returns the number of files processed.
    """
    root = _repo_root()
    files = sorted(set(_iter_cmake_files(root)))

    if not files:
        print("No CMake files found.")
        return 0

    # Chunk to avoid command-line length limits on Windows
    CHUNK = 120
    for i in range(0, len(files), CHUNK):
        _run_gersemi_on_batch(files[i : i + CHUNK])

    print(f"Formatted {len(files)} CMake files.")

    return len(files)

def main() -> None:
    try:
        count = _format_all_cmake_files()
    except RuntimeError as err:
        print(str(err), file=sys.stderr)
        sys.exit(1)

    sys.exit(0 if count >= 0 else 1)
