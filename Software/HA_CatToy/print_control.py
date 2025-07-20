# print_control.py
"""Utility to globally control ``print`` behaviour.

Importing  module replaces Python's :func:`print` with a configurable
version.  Output can be completely silenced or redirected to a log file.  The
implementation is intentionally simple so it works on both CPython and
MicroPython.

Example
-------
::

    import print_control
    print_control.set_print_enabled(False)
    print(" will not be shown")
    print_control.set_log_to_file(True, "log.txt")
    print(" goes to the log file")
"""
import sys


# Save reference to the original print
_original_print = print

# Global state flags
PRINT_ENABLED = True
LOG_TO_FILE = False
LOG_FILE_PATH = "print_log.txt"

def set_print_enabled(enabled: bool) -> None:
    """Enable or disable printing globally.

    Parameters
    ----------
    enabled : bool
        ``True`` to allow printing, ``False`` to suppress all output.
    """
    PRINT_ENABLED = bool(enabled)

def set_log_to_file(enabled: bool, filepath: str | None = None) -> None:
    """Redirect printed output to a file.

    Parameters
    ----------
    enabled : bool
        ``True`` to append print messages to a file.
    filepath : str | None
        Path of the log file; if ``None`` the existing path is used.
    """
    LOG_TO_FILE = bool(enabled)
    if filepath:
        LOG_FILE_PATH = filepath

def custom_print(*args, **kwargs) -> None:
    """Replacement for :func:`print` respecting the module settings."""
    if not PRINT_ENABLED:
        return
    output = " ".join(str(arg) for arg in args)

    if LOG_TO_FILE:
        try:
            with open(LOG_FILE_PATH, "a") as f:
                f.write(output + "\n")
        except Exception as e:
            _original_print("Log file write error:", e)
            _original_print(output, **kwargs)
    else:
        _original_print(*args, **kwargs)

# Override the built-in print when module is imported
print = custom_print
