"""
Base health check system with modular check registration and execution.
"""

from abc import ABC, abstractmethod
from typing import Dict, List, Optional, Any, Callable, TYPE_CHECKING
import traceback

from core.config import CheckConfig
from core.types import CheckResult, CheckStatus

if TYPE_CHECKING:
    from core.reporter import HealthReporter


class HealthCheck(ABC):
    """Base class for all health checks."""
    
    def __init__(self, name: str, description: str, optional: bool = False):
        self.name = name
        self.description = description
        self.optional = optional
        self.dependencies: List[str] = []
    
    @abstractmethod
    async def run(self, config: CheckConfig) -> CheckResult:
        """Run the health check and return a result."""
        pass
    
    def add_dependency(self, check_name: str) -> None:
        """Add a dependency on another check."""
        self.dependencies.append(check_name)


class HealthCheckRegistry:
    """Registry for managing health checks."""
    
    def __init__(self):
        self._checks: Dict[str, HealthCheck] = {}
        self._categories: Dict[str, List[str]] = {}
    
    def register(self, check: HealthCheck, category: str = "general") -> None:
        """Register a health check."""
        self._checks[check.name] = check
        if category not in self._categories:
            self._categories[category] = []
        self._categories[category].append(check.name)
    
    def get_check(self, name: str) -> Optional[HealthCheck]:
        """Get a health check by name."""
        return self._checks.get(name)
    
    def get_checks_in_category(self, category: str) -> List[HealthCheck]:
        """Get all checks in a category."""
        if category not in self._categories:
            return []
        return [self._checks[name] for name in self._categories[category]]
    
    def get_all_checks(self) -> Dict[str, HealthCheck]:
        """Get all registered checks."""
        return self._checks.copy()
    
    def get_categories(self) -> List[str]:
        """Get all categories."""
        return list(self._categories.keys())


class HealthCheckRunner:
    """Main runner for executing health checks."""
    
    def __init__(self, reporter: Optional["HealthReporter"], config: CheckConfig):
        self.reporter = reporter
        self.config = config
        self.registry = HealthCheckRegistry()
        self.results: Dict[str, CheckResult] = {}
        self._load_checks()
    
    def _load_checks(self) -> None:
        """Load all available health checks."""
        # This will be implemented as we add actual checks
        # For now, register some placeholder checks
        from checks.system import SystemChecks
        from checks.tools import ToolChecks
        
        # Register system checks
        system_checks = SystemChecks()
        for check in system_checks.get_checks():
            self.registry.register(check, "system")
        
        # Register tool checks  
        tool_checks = ToolChecks()
        for check in tool_checks.get_checks():
            self.registry.register(check, "tools")
    
    def get_available_checks(self) -> Dict[str, Dict[str, Any]]:
        """Get information about all available checks."""
        checks = {}
        for name, check in self.registry.get_all_checks().items():
            checks[name] = {
                "description": check.description,
                "optional": check.optional,
                "dependencies": check.dependencies,
            }
        return checks
    
    def _should_run_check(self, check: HealthCheck) -> bool:
        """Determine if a check should be run based on configuration."""
        # If specific checks are requested, only run those
        if self.config.specific_checks:
            return check.name in self.config.specific_checks
        
        # Skip optional checks if configured to do so
        if check.optional and self.config.skip_optional:
            return False
        
        # Only run optional checks if specifically included
        if check.optional and self.config.include_optional:
            return check.name in self.config.include_optional
        
        # Run all required checks by default
        return not check.optional
    
    def _get_execution_order(self, checks: List[HealthCheck]) -> List[HealthCheck]:
        """Determine execution order based on dependencies."""
        ordered = []
        remaining = checks.copy()
        
        while remaining:
            # Find checks with no unmet dependencies
            ready = []
            for check in remaining:
                unmet_deps = [dep for dep in check.dependencies 
                            if dep not in [c.name for c in ordered]]
                if not unmet_deps:
                    ready.append(check)
            
            if not ready:
                # Circular dependency or missing dependency
                if self.reporter:
                    self.reporter.show_error("Dependency cycle detected in health checks")
                remaining_names = [c.name for c in remaining]
                ready = remaining  # Run remaining checks anyway
            
            ordered.extend(ready)
            for check in ready:
                remaining.remove(check)
        
        return ordered
    
    async def run_all_checks(self) -> bool:
        """Run all configured health checks."""
        all_checks = list(self.registry.get_all_checks().values())
        checks_to_run = [check for check in all_checks if self._should_run_check(check)]
        
        if not checks_to_run:
            if self.reporter:
                self.reporter.show_warning("No checks to run")
            return True
        
        # Order checks by dependencies
        ordered_checks = self._get_execution_order(checks_to_run)
        
        if self.reporter:
            self.reporter.start_checks(len(ordered_checks))
        
        overall_success = True
        
        for check in ordered_checks:
            if self.reporter:
                self.reporter.start_check(check.name, check.description)
            
            try:
                result = await check.run(self.config)
                self.results[check.name] = result
                
                if self.reporter:
                    self.reporter.complete_check(check.name, result)
                
                # Stop on critical failures unless configured otherwise
                if result.status == CheckStatus.FAILED and not check.optional:
                    overall_success = False
                    if not self.config.continue_on_failure:
                        break
                        
            except Exception as e:
                error_result = CheckResult(
                    status=CheckStatus.FAILED,
                    message=f"Check failed with exception: {str(e)}",
                    error=e,
                    details=traceback.format_exc() if self.config.verbose else None
                )
                self.results[check.name] = error_result
                overall_success = False
                
                if self.reporter:
                    self.reporter.complete_check(check.name, error_result)
                
                if not self.config.continue_on_failure:
                    break
        
        if self.reporter:
            self.reporter.finish_checks()
        
        return overall_success
    
    def get_results(self) -> Dict[str, CheckResult]:
        """Get all check results."""
        return self.results.copy()


# Global registry instance
_global_registry = HealthCheckRegistry()


def register_check(check: HealthCheck, category: str = "general") -> None:
    """Register a health check in the global registry."""
    _global_registry.register(check, category)


def get_registry() -> HealthCheckRegistry:
    """Get the global health check registry."""
    return _global_registry
