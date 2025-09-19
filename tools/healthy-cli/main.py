"""
Healthy - Health check tool for the Helipad game engine project.

A comprehensive tool that validates your development environment,
dependencies, and configuration to ensure a smooth setup and build process.
"""

import platform
from typing import List, Optional

import typer
from rich.console import Console
from rich.panel import Panel
from rich.table import Table

from core.checker import HealthCheckRunner
from core.reporter import HealthReporter
from core.config import CheckConfig

app = typer.Typer(
    name="healthy",
    help="Health check tool for the Helipad game engine project",
    add_completion=False,
)
console = Console()


@app.command()
def check(
    checks: Optional[List[str]] = typer.Option(None, "--check", help="Specific checks to run"),
    include_optional: Optional[List[str]] = typer.Option(None, "--include-optional", help="Include optional checks"),
    skip_optional: bool = typer.Option(False, "--skip-optional", help="Skip all optional checks"),
    verbose: bool = typer.Option(False, "--verbose", "-v", help="Verbose output with detailed diagnostics"),
    non_interactive: bool = typer.Option(False, "--non-interactive", help="Skip interactive prompts"),
    report: Optional[str] = typer.Option(None, "--report", help="Generate health report file"),
):
    """Run health checks for the Helipad development environment."""
    config = CheckConfig(
        specific_checks=checks,
        include_optional=include_optional,
        skip_optional=skip_optional,
        verbose=verbose,
        non_interactive=non_interactive,
        report_file=report,
    )
    
    reporter = HealthReporter(console, config)
    runner = HealthCheckRunner(reporter, config)
    
    reporter.show_header()
    
    import asyncio
    success = asyncio.run(runner.run_all_checks())
    
    reporter.show_summary(success)
    
    if not success:
        raise typer.Exit(1)


@app.command()
def list_checks():
    """List all available health checks."""
    runner = HealthCheckRunner(None, CheckConfig())
    checks = runner.get_available_checks()
    
    table = Table(title="Available Health Checks")
    table.add_column("Check", style="cyan", no_wrap=True)
    table.add_column("Type", style="green")
    table.add_column("Description", style="white")
    
    for check_name, check_info in checks.items():
        check_type = "Required" if not check_info.get("optional", False) else "Optional"
        description = check_info.get("description", "")
        table.add_row(check_name, check_type, description)
    
    console.print(table)


@app.command()
def install_help(
    tool: str = typer.Argument(..., help="Tool to show installation help for"),
):
    """Show platform-specific installation help for a tool."""
    from utils.installers import get_install_instructions
    from rich.markdown import Markdown
    
    instructions = get_install_instructions(tool)
    if instructions:
        # Render markdown with Rich for better formatting
        markdown = Markdown(instructions)
        console.print(Panel(
            markdown,
            title=f"[bold cyan]Installing {tool}[/bold cyan]",
            border_style="cyan",
            padding=(1, 2)
        ))
    else:
        console.print(f"[red]No installation instructions found for '{tool}'[/red]")
        raise typer.Exit(1)


@app.command()
def version():
    """Show version information."""
    console.print(Panel(
        "[bold blue]Healthy v1.0.0[/bold blue]\n"
        "Health check tool for the Helipad game engine project\n"
        f"Platform: {platform.system()} {platform.machine()}",
        title="Version"
    ))


# Legacy command for backward compatibility
@app.command(hidden=True)
def info():
    """Legacy command - use 'check' instead."""
    console.print("[yellow]Warning: 'info' command is deprecated. Use 'healthy check' instead.[/yellow]")
    check()


if __name__ == "__main__":
    app()
