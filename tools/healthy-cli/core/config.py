"""
Configuration management for health checks.
"""

from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any
import json
import os
from pathlib import Path


@dataclass
class CheckConfig:
    """Configuration for health check execution."""
    
    # Check selection
    specific_checks: Optional[List[str]] = None
    include_optional: Optional[List[str]] = None  
    skip_optional: bool = False
    
    # Output and behavior
    verbose: bool = False
    non_interactive: bool = False
    continue_on_failure: bool = True
    report_file: Optional[str] = None
    
    # Platform-specific settings
    platform_overrides: Dict[str, Any] = field(default_factory=dict)
    
    @classmethod
    def from_file(cls, config_path: str) -> 'CheckConfig':
        """Load configuration from a JSON file."""
        try:
            with open(config_path, 'r') as f:
                data = json.load(f)
            return cls(**data)
        except (FileNotFoundError, json.JSONDecodeError, TypeError) as e:
            raise ValueError(f"Failed to load config from {config_path}: {e}")
    
    def to_file(self, config_path: str) -> None:
        """Save configuration to a JSON file."""
        # Ensure directory exists
        Path(config_path).parent.mkdir(parents=True, exist_ok=True)
        
        with open(config_path, 'w') as f:
            json.dump(self.__dict__, f, indent=2)
    
    def should_run_check(self, check_name: str, is_optional: bool = False) -> bool:
        """Determine if a specific check should be run."""
        # If specific checks are requested, only run those
        if self.specific_checks:
            return check_name in self.specific_checks
        
        # Handle optional checks
        if is_optional:
            if self.skip_optional:
                return False
            if self.include_optional:
                return check_name in self.include_optional
            return False  # Optional checks are opt-in by default
        
        # Run all required checks by default
        return True


@dataclass
class ReportConfig:
    """Configuration for generating health reports."""
    
    format: str = "json"  # json, yaml, html, text
    include_details: bool = True
    include_suggestions: bool = True
    include_system_info: bool = True
    output_path: Optional[str] = None
    
    @classmethod
    def from_check_config(cls, check_config: CheckConfig) -> 'ReportConfig':
        """Create report config from check config."""
        if not check_config.report_file:
            return cls()
        
        # Determine format from file extension
        ext = Path(check_config.report_file).suffix.lower()
        format_map = {
            '.json': 'json',
            '.yaml': 'yaml', 
            '.yml': 'yaml',
            '.html': 'html',
            '.txt': 'text'
        }
        
        return cls(
            format=format_map.get(ext, 'json'),
            include_details=check_config.verbose,
            output_path=check_config.report_file
        )


class ConfigManager:
    """Manages configuration loading and validation."""
    
    DEFAULT_CONFIG_LOCATIONS = [
        ".healthy.json",
        "~/.config/healthy/config.json",
        "~/.healthy.json"
    ]
    
    @classmethod
    def load_config(cls, config_path: Optional[str] = None) -> CheckConfig:
        """Load configuration from file or defaults."""
        if config_path:
            return CheckConfig.from_file(config_path)
        
        # Try default locations
        for location in cls.DEFAULT_CONFIG_LOCATIONS:
            expanded_path = os.path.expanduser(location)
            if os.path.exists(expanded_path):
                try:
                    return CheckConfig.from_file(expanded_path)
                except ValueError:
                    continue  # Try next location
        
        # Return default config if no file found
        return CheckConfig()
    
    @classmethod
    def validate_config(cls, config: CheckConfig) -> List[str]:
        """Validate configuration and return list of issues."""
        issues = []
        
        # Validate report file path
        if config.report_file:
            report_dir = Path(config.report_file).parent
            if not report_dir.exists():
                try:
                    report_dir.mkdir(parents=True, exist_ok=True)
                except OSError as e:
                    issues.append(f"Cannot create report directory {report_dir}: {e}")
        
        # Validate check names (this would require the registry to be loaded)
        # For now, we'll skip this validation
        
        return issues


# Environment variable support
def get_env_config() -> Dict[str, Any]:
    """Get configuration overrides from environment variables."""
    env_config = {}
    
    # Boolean flags
    verbose_env = os.getenv("HEALTHY_VERBOSE")
    if verbose_env:
        env_config["verbose"] = verbose_env.lower() in ("1", "true", "yes")
    
    non_interactive_env = os.getenv("HEALTHY_NON_INTERACTIVE")
    if non_interactive_env:
        env_config["non_interactive"] = non_interactive_env.lower() in ("1", "true", "yes")
    
    skip_optional_env = os.getenv("HEALTHY_SKIP_OPTIONAL")
    if skip_optional_env:
        env_config["skip_optional"] = skip_optional_env.lower() in ("1", "true", "yes")
    
    # String/list values
    report_file_env = os.getenv("HEALTHY_REPORT_FILE")
    if report_file_env:
        env_config["report_file"] = report_file_env
    
    specific_checks_env = os.getenv("HEALTHY_SPECIFIC_CHECKS")
    if specific_checks_env:
        env_config["specific_checks"] = specific_checks_env.split(",")
    
    include_optional_env = os.getenv("HEALTHY_INCLUDE_OPTIONAL")
    if include_optional_env:
        env_config["include_optional"] = include_optional_env.split(",")
    
    return env_config


def create_default_config_file(path: str) -> None:
    """Create a default configuration file."""
    config = CheckConfig(
        verbose=False,
        skip_optional=True,
        include_optional=["doxygen", "ccache"]
    )
    config.to_file(path)
