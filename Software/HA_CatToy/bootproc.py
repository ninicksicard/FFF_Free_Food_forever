"""Startup helper for delayed execution of :mod:`main`.

 module sets up a timer that calls :func:`main.run` a short while after boot
unless a "NO-GO" pin is held high.  It allows the device to remain in the REPL
for debugging by connecting DI10 to 3.3&nbsp;V.
"""

import machine
from ESP32S3_IO_MAPPING import DI10
import sys


# Import print_control so the custom print replacement is active
try:
    from print_control import custom_print as print #noqa: F401
except Exception:
    pass

sys.modules[__name__]._test_timer = None
# Timer used to detect Ctrl-C and clean up active timers
sys.modules[__name__]._kbd_timer = None
NO_GO_PIN = DI10  # Use DI10 from IO mapping

def _run_main(timer: machine.Timer) -> None:
    """Timer callback used by :func:`run`.

    Parameters
    ----------
    timer : machine.Timer
        The timer instance invoking the callback.  It is unused but included for
        API compatibility.

    The function checks :data:`NO_GO_PIN`.  If the pin reads high the call to
    :func:`main.run` is skipped, leaving the REPL available.
    """
    pin = machine.Pin(NO_GO_PIN, machine.Pin.IN, machine.Pin.PULL_DOWN)
    if pin.value():
        print("bootproc: NO-GO flag detected (NO_GO_PIN HIGH, tied to 3.3V); skipping main.run()")
        return
    try:
        import main
        if hasattr(main, "run"):
            main.run()
        else:
            print("bootproc: main.run() not found")
    except Exception as e:
        try:
            import sys
            sys.print_exception(e)
        except Exception:
            pass
        print("bootproc: exception running main.run()")

def run() -> None:
    """Schedule a one-shot timer to execute :func:`main.run` if allowed."""
    print("bootproc.run() scheduling one-shot call with NO-GO pin check (using DI10 from mapping)")
    sys.modules[__name__]._test_timer = machine.Timer(1)
    sys.modules[__name__]._test_timer.init(period=2000, mode=machine.Timer.ONE_SHOT, callback=_run_main)


def kill_all_timers() -> None:
    """Attempt to deinitialise every active :class:`machine.Timer` instance."""
    import sys

    count = 0
    for mod in list(sys.modules.values()) + [globals()]:
        if mod is None:
            continue
        attrs = mod if isinstance(mod, dict) else dir(mod)
        for name in attrs:
            try:
                timer_obj = mod[name] if isinstance(mod, dict) else getattr(mod, name)
            except Exception:
                continue
            try:
                if isinstance(timer_obj, machine.Timer):
                    timer_obj.deinit()
                    count += 1
            except Exception:
                pass

    print(f"bootproc.kill_all_timers: deinitialised {count} timer(s)")


def start_keyboard_interrupt_timer(period_ms: int = 100) -> None:
    """Start periodic checks to clean up timers on ``Ctrl-C``.

    MicroPython raises ``KeyboardInterrupt`` when ``Ctrl-C`` is received.
     function sets up a timer executing a no-op callback at regular
    intervals so the exception can be caught even if the main code is idle.
    When the interrupt is detected, :func:`kill_all_timers` is invoked and the
    exception is re-raised.

    Parameters
    ----------
    period_ms : int
        Interval in milliseconds between callback executions.
    """

    def _check(_timer: machine.Timer) -> None:
        try:
            # The ``pass`` statement runs so ``KeyboardInterrupt`` can surface
            pass
        except KeyboardInterrupt:
            print("bootproc: KeyboardInterrupt detected; cleaning up timers")
            kill_all_timers()
            raise

    sys.modules[__name__]._kbd_timer = machine.Timer(2)
    sys.modules[__name__]._kbd_timer.init(period=period_ms,
                         mode=machine.Timer.PERIODIC,
                         callback=_check)


def stop_keyboard_interrupt_timer() -> None:
    """Disable the keyboard interrupt monitoring timer if active."""

    if sys.modules[__name__]._kbd_timer is not None:
        try:
            sys.modules[__name__]._kbd_timer.deinit()
        except Exception:
            pass
        sys.modules[__name__]._kbd_timer = None
