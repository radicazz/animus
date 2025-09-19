"""
Shared data structures for health checks.
"""

from dataclasses import dataclass
from enum import Enum
from typing import Optional


class CheckStatus(Enum):
    """Status of a health check."""
    PENDING = "pending"
    RUNNING = "running" 
    PASSED = "passed"
    FAILED = "failed"
    SKIPPED = "skipped"
    WARNING = "warning"


@dataclass
class CheckResult:
    """Result of a health check."""
    status: CheckStatus
    message: str
    details: Optional[str] = None
    suggestion: Optional[str] = None
    error: Optional[Exception] = None
