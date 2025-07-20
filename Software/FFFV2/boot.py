"""boot.py

Minimal launcher that configures and uses logutil for messages.
Runs bootproc.run() if import is successful. All imports and execution are wrapped in try blocks.
Keeps REPL responsive if anything fails.
"""

import sys

try:
    from logutil import log
except Exception:
    def log(msg):
        pass  # Silent fallback if logutil import fails

try:
    import bootproc
    bootproc.run()
except Exception as e:
    sys.print_exception(e)
    log("boot.py: Failed to import or run bootproc. Staying in REPL.")
