from CustomLock import CustomLock
import time


class PrintQueueHandler:
    def __init__(self, verbose=False, max_queue_size=100):
        """Initialize the print queue."""
        self.default_verbose = verbose  # Global default verbosity
        self.lock_print_queue = CustomLock()
        self.print_queue = []
        self.max_queue_size = max_queue_size
        self.log_file = "log_file.txt"  # Path to the log file, if enabled
        
    def append(self, *content, _v=None):
        """
        Append content to the queue or print it.

        - `_v=None` uses the default verbosity.
        - `_v=True/False` explicitly overrides the default.
        """
        effective_verbose = self.default_verbose if _v is None else _v
        with self.lock_print_queue:
            if effective_verbose:
                print(*self.print_queue, content)
                self.print_queue = []
            else:
                self.print_queue.append(content)

            while len(self.print_queue) > self.max_queue_size:
                self.print_queue.pop(0)

        # Log the appended content
        self._log_content(content)

    def dump(self):
        """Print all items currently in the queue, then clear the queue."""
        with self.lock_print_queue:
            if self.print_queue:
                print("Queue Dump:", *self.print_queue)
                self.print_queue = []
            else:
                print("Queue is empty.")
            
    def format_and_print(self):
        """Print all items in the queue line by line without clearing it."""
        with self.lock_print_queue:
            if self.print_queue:
                print("Formatted Queue Dump:")
                for item in self.print_queue:
                    # Convert item to a string and print it
                    print(item if isinstance(item, str) else " ".join(map(str, item)))
            else:
                print("Queue is empty.")


    def set_default_verbose(self, verbose):
        """Set the default verbosity."""
        self.default_verbose = verbose
        
    def set_log_file(self, file_path):
        """Set or update the log file for the queue."""
        self.log_file = file_path
        print(f"Logging enabled. Messages will be written to: {file_path}")

    def _log_content(self, content):
        """Log content to the specified file if logging is enabled."""
        if self.log_file:
            try:
                with open(self.log_file, 'a') as file:
                    for item in content:
                        file.write(f"{item}\n")
            except Exception as e:
                print(f"Error writing to log file: {e}")

# Singleton instance
print_queue = PrintQueueHandler(verbose=True)

# Convenience functions for REPL usage
def append_to_queue(*content, _v=None):
    """Convenient wrapper to append content to the global print queue."""
    print_queue.append(*content, _v=_v)

def dump_queue():
    """Dump the contents of the global print queue."""
    print_queue.dump()
    
def format_and_print():
    """Dump the contents of the global print queue."""
    print_queue.format_and_print()

def set_default_verbose(verbose):
    """Set the default verbosity for the global print queue."""
    print_queue.set_default_verbose(verbose)

def set_max_queue_size(size):
    """Set the maximum size of the queue."""
    print_queue.max_queue_size = size
