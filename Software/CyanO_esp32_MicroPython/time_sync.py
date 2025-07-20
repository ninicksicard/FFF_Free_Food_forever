import ntptime
import machine

class TimeSync:
    def __init__(self):
        # Initialize RTC
        self.rtc = machine.RTC()

    def sync_time(self, settings):
        # Synchronize time with NTP server
        try:
            ntptime.settime(settings.get('timezone', 8), settings.get('server', 'ntp.ntsc.ac.cn'))
            print("Time synchronized with NTP server")
        except:
            print("Error synchronizing time")

    def get_time(self):
        # Get the current time
        return self.rtc.datetime()
