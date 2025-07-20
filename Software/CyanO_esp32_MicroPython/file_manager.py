import ujson

class FileManager:
    def __init__(self):
        pass

    def load_settings(self):
        # Load settings from JSON file
        try:
            with open('settings.json', 'r') as f:
                return ujson.load(f)
        except Exception as e:
            print("Error loading settings: ", e)
            return {}

    def save_settings(self, settings):
        # Save settings to JSON file
        try:
            with open('settings.json', 'w') as f:
                ujson.dump(settings, f)
        except Exception as e:
            print("Error saving settings: ", e)
