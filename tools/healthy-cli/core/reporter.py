"""
Rich-based progress tracking and colorful output for health checks.
"""

from typing import Optional, Dict, List
import time
from datetime import datetime

from rich.console import Console
from rich.panel import Panel
from rich.progress import Progress, TaskID, SpinnerColumn, TextColumn, BarColumn, TimeElapsedColumn
from rich.table import Table
from rich.text import Text
from rich.layout import Layout
from rich.live import Live
from rich.rule import Rule

from core.config import CheckConfig
from core.types import CheckResult, CheckStatus


class HealthReporter:
    """Reporter for health check progress and results using Rich."""
    
    def __init__(self, console: Console, config: CheckConfig):
        self.console = console
        self.config = config
        self.progress: Optional[Progress] = None
        self.current_task: Optional[TaskID] = None
        self.results: Dict[str, CheckResult] = {}
        self.start_time: Optional[datetime] = None
        self.check_times: Dict[str, float] = {}
        
    def show_header(self) -> None:
        """Display the application header."""
        header_text = Text()
        header_text.append("🏥 ", style="bold red")
        header_text.append("Healthy", style="bold blue")
        header_text.append(" - Helipad Development Environment Health Check", style="white")
        
        self.console.print(Panel(
            header_text,
            subtitle=f"Started at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            padding=(1, 2),
            style="blue"
        ))
        self.console.print()
        
    def start_checks(self, total_checks: int) -> None:
        """Start the progress tracking for checks."""
        self.start_time = datetime.now()
        
        if self.config.verbose:
            self.console.print(f"[dim]Running {total_checks} health checks...[/dim]")
            self.console.print()
        
        self.progress = Progress(
            SpinnerColumn(),
            TextColumn("[progress.description]{task.description}"),
            BarColumn(),
            TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
            TimeElapsedColumn(),
            console=self.console,
            transient=not self.config.verbose
        )
        
        self.current_task = self.progress.add_task(
            "Running health checks...",
            total=total_checks
        )
        
        if not self.config.non_interactive:
            self.progress.start()
    
    def start_check(self, check_name: str, description: str) -> None:
        """Start tracking a specific check."""
        self.check_times[check_name] = time.time()
        
        if self.progress and self.current_task:
            self.progress.update(
                self.current_task,
                description=f"Checking: {description[:50]}..."
            )
        
        if self.config.verbose:
            self.console.print(f"[dim]• Checking {check_name}...[/dim]")
    
    def complete_check(self, check_name: str, result: CheckResult) -> None:
        """Complete tracking for a specific check."""
        self.results[check_name] = result
        elapsed = time.time() - self.check_times.get(check_name, 0)
        
        if self.progress and self.current_task:
            self.progress.advance(self.current_task)
        
        # Show result immediately if verbose or if there's an issue
        if self.config.verbose or result.status in [CheckStatus.FAILED, CheckStatus.WARNING]:
            self._show_check_result(check_name, result, elapsed)
    
    def _show_check_result(self, check_name: str, result: CheckResult, elapsed: float) -> None:
        """Show the result of a single check."""
        status_styles = {
            CheckStatus.PASSED: "green",
            CheckStatus.FAILED: "red", 
            CheckStatus.WARNING: "yellow",
            CheckStatus.SKIPPED: "dim",
        }
        
        status_icons = {
            CheckStatus.PASSED: "✓",
            CheckStatus.FAILED: "✗",
            CheckStatus.WARNING: "⚠",
            CheckStatus.SKIPPED: "⏭",
        }
        
        style = status_styles.get(result.status, "white")
        icon = status_icons.get(result.status, "?")
        
        # Main result line
        result_text = Text()
        result_text.append(f"{icon} ", style=style)
        result_text.append(f"{check_name}: ", style="bold")
        result_text.append(result.message, style=style)
        
        if self.config.verbose:
            result_text.append(f" ({elapsed:.2f}s)", style="dim")
        
        self.console.print(result_text)
        
        # Show additional details if available
        if result.details and (self.config.verbose or result.status == CheckStatus.FAILED):
            details_panel = Panel(
                result.details,
                title="Details" if result.status != CheckStatus.FAILED else "Error Details",
                border_style=style,
                padding=(0, 1)
            )
            self.console.print(details_panel)
        
        # Show suggestion if available
        if result.suggestion and result.status in [CheckStatus.FAILED, CheckStatus.WARNING]:
            suggestion_panel = Panel(
                result.suggestion,
                title="💡 Suggestion",
                border_style="blue",
                padding=(0, 1)
            )
            self.console.print(suggestion_panel)
        
        if not self.config.verbose and result.status in [CheckStatus.FAILED, CheckStatus.WARNING]:
            self.console.print()
    
    def finish_checks(self) -> None:
        """Finish the progress tracking."""
        if self.progress:
            self.progress.stop()
    
    def show_summary(self, overall_success: bool) -> None:
        """Show the final summary of all checks."""
        if not self.results:
            self.console.print("[yellow]No checks were run.[/yellow]")
            return
        
        self.console.print()
        self.console.print(Rule(title="Summary", style="blue"))
        
        # Count results by status
        status_counts = {status: 0 for status in CheckStatus}
        for result in self.results.values():
            status_counts[result.status] += 1
        
        # Create summary table
        summary_table = Table(title="Health Check Results", show_header=True, header_style="bold blue")
        summary_table.add_column("Check", style="bold")
        summary_table.add_column("Status", justify="center")
        summary_table.add_column("Message")
        
        for check_name, result in self.results.items():
            status_styles = {
                CheckStatus.PASSED: "green",
                CheckStatus.FAILED: "red",
                CheckStatus.WARNING: "yellow", 
                CheckStatus.SKIPPED: "dim",
            }
            
            status_icons = {
                CheckStatus.PASSED: "✓ PASS",
                CheckStatus.FAILED: "✗ FAIL",
                CheckStatus.WARNING: "⚠ WARN",
                CheckStatus.SKIPPED: "⏭ SKIP",
            }
            
            style = status_styles.get(result.status, "white")
            status_text = status_icons.get(result.status, "? UNK")
            
            summary_table.add_row(
                check_name,
                status_text,
                result.message[:80] + "..." if len(result.message) > 80 else result.message,
                style=style if result.status == CheckStatus.FAILED else None
            )
        
        self.console.print(summary_table)
        
        # Show overall result
        self.console.print()
        total_time = (datetime.now() - self.start_time).total_seconds() if self.start_time else 0
        
        if overall_success:
            overall_panel = Panel(
                f"[green]✓ All critical checks passed![/green]\n"
                f"Completed {len(self.results)} checks in {total_time:.1f}s",
                title="🎉 Success",
                border_style="green",
                padding=(1, 2)
            )
        else:
            failed_count = status_counts[CheckStatus.FAILED]
            overall_panel = Panel(
                f"[red]✗ {failed_count} check(s) failed[/red]\n"
                f"Please address the issues above before proceeding.\n"
                f"Completed {len(self.results)} checks in {total_time:.1f}s",
                title="❌ Issues Found",
                border_style="red",
                padding=(1, 2)
            )
        
        self.console.print(overall_panel)
        
        # Show action items if there are failures or warnings
        action_items = []
        for check_name, result in self.results.items():
            if result.status in [CheckStatus.FAILED, CheckStatus.WARNING] and result.suggestion:
                action_items.append(f"• {check_name}: {result.suggestion}")
        
        if action_items and not overall_success:
            self.console.print()
            action_panel = Panel(
                "\n".join(action_items),
                title="🔧 Action Items",
                border_style="yellow",
                padding=(1, 2)
            )
            self.console.print(action_panel)
    
    def show_error(self, message: str) -> None:
        """Show an error message."""
        self.console.print(f"[red]Error: {message}[/red]")
    
    def show_warning(self, message: str) -> None:
        """Show a warning message."""
        self.console.print(f"[yellow]Warning: {message}[/yellow]")
    
    def show_info(self, message: str) -> None:
        """Show an info message."""
        self.console.print(f"[blue]Info: {message}[/blue]")
