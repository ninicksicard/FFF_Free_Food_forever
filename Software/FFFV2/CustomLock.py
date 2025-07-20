from _thread import allocate_lock

class CustomLock:
    def __init__(self):
        """Initialize the underlying lock."""
        self.lock = allocate_lock()

    def __enter__(self):
        """Acquire the lock when entering the context."""
        self.lock.acquire()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Release the lock when exiting the context."""
        self.lock.release()
