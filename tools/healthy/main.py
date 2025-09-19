import platform
import subprocess
from typer import Typer

from rich.console import Console
from rich.panel import Panel
from rich.table import Table

app = Typer()
console = Console()

def check_tool(name: str, command: str) -> str:
    """Check if a tool is installed by running its version command."""
    try:
        result = subprocess.run([command, "--version"], capture_output=True, text=True, check=True)
        return f"[green]✓ {name} is installed[/green]\n{result.stdout.strip()}"
    except (subprocess.CalledProcessError, FileNotFoundError):
        return f"[red]✗ {name} is not installed or not in PATH[/red]"



@app.command()
def info():
    """Print platform info and check for CMake/Git."""
    plat = platform.system()
    console.print(Panel(f"[bold blue]Platform:[/] {plat}", title="System Info"))
    
    cmake_status = check_tool("CMake", "cmake")
    git_status = check_tool("Git", "git")
    
    console.print(Panel(f"{cmake_status}\n\n{git_status}", title="Tool Checks"))


if __name__ == "__main__":
    app()
