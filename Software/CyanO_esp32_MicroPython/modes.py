class Modes:
    def __init__(self):
        # Initialize modes
        self.modes = {
            "mode1_aeration": {},
            "mode2_eau": {},
            "sleep": {},
            "mode_3_vidange": {},
            "mode4_nutriments": {}
        }

    def setup_modes(self, settings):
        # Setup modes based on settings
        for mode, setting in settings.items():
            self.modes[mode] = setting

    def update_modes(self):
        # Update modes based on sensor readings
        # This is where you would put the logic for controlling the relays and servomotor
        pass
