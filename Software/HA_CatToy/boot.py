"""boot.py

Minimal launcher that runs :mod:`bootproc` if available.
All imports and execution are wrapped in try blocks so the REPL
remains accessible if anything fails.
"""

import sys

# Import print_control to override global print behavior
try:
   from print_control import custom_print as print #noqa: F401
except Exception:
    pass  # If import fails, fallback to normal print


try:
    import bootproc
    bootproc.run()
except Exception as e:
    sys.print_exception(e)
    print("boot.py: Failed to import or run bootproc. Staying in REPL.")
