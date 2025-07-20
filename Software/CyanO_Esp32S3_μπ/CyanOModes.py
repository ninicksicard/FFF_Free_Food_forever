import _thread
import utime
import ServoController


class Mode:
    _mode_queue = []
    _lock = _thread.allocate_lock()
    _default_mode = None  # Holds the actual default mode instance

    def __init__(self, name, servo_position, pump_speed, duration=None, delay=None, conditions:list=[True], follow_up_mode=None, fallback_mode=None):
        self.name = name
        self.servo_position = servo_position
        self.pump_speed = pump_speed
        self.duration = duration
        self.delay = delay
        self.print_count = 5
        self.conditions = conditions
        
        with Mode._lock:
            if Mode._default_mode is None:
                Mode._default_mode = self  # Set the first created mode as the default mode
        # Directly use Mode.default_mode as a "carrier" if no follow-up mode is provided
        if follow_up_mode:
            self.follow_up_mode = follow_up_mode
        else:
            self.follow_up_mode = self
            
        if fallback_mode :
            self.fallback_mode = fallback_mode
        else:
            self.fallback_mode = Mode._default_mode
               
        

    @classmethod
    @property
    def default_mode(cls):
        # This property returns the current default mode
        return cls._default_mode

    @classmethod
    def set_default_mode(cls, mode):
        with cls._lock:
            cls._default_mode = mode

    def start(self):
        self.running=False        
        
        print("\nStarting ", self.name, " : \n",
              "    Servo to ",self.servo_position, "\n",
              "    Pump to ", self.pump_speed)

        if self.duration and all(self.conditions):
            if self.delay:
                print("     with delay : ",self.delay," seconds")
            _thread.start_new_thread(self._parallel_tasks, ())
        else:
            self._complete()

    def _parallel_tasks(self):
        if self.delay and all(self.conditions):
            utime.sleep(self.delay)
        start_time = utime.time()
        # set servo
        # set pump
        while utime.time() - start_time < self.duration:
            if not all(self.conditions):
                # set pump
                # set servo
                self._fallback()  # Exit and fallback if conditions not met
                return
            utime.sleep(0.1)  # Check conditions every 0.1 seconds, adjust as necessary
        # set pump
        # set servo
        self._complete()        

    def _complete(self):
        print(f"     Completing ",self.name)
        self.end()
        # No need for conditional logic here anymore, as follow_up_mode is always set
        next_mode = self.follow_up_mode
        next_mode.start()
        
        
    def _fallback(self):
        print(f"     conditions not met during :  ",self.name)
        print("     fallback to :", self.fallback_mode)
        self.end()
        # No need for conditional logic here anymore, as follow_up_mode is always set
        next_mode = self.fallback_mode
        next_mode.start()

    def end(self):
        print("     Ending ",self.name)
        self.running=False

    @classmethod
    def queue_mode(cls, mode):
        with cls._lock:
            cls._mode_queue.append(mode)
            if len(cls._mode_queue) == 1 and mode != cls._default_mode:
                # Start the first mode in the queue if it's not the default mode
                mode.start()
                
    def __str__(self):
        return f"name: {self.name}: \n     Servo: {self.servo_position}, \n    Pump: {self.pump_speed}, \n    follow_up_mode : {self.follow_up_mode.name} "
        
            

