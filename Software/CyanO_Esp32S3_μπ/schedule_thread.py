import schedule
import utime
import _thread 


def scheduler_loop(): 
    while True: 
        schedule.run_pending() 
        utime.sleep(1)


_thread.start_new_thread(scheduler_loop, ())