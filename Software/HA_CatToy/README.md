# HA_CatToy

This project controls an ESP32S3 based cat toy using MicroPython.  Wi‑Fi and MQTT credentials are no longer stored in the repository.  Instead they are loaded from environment variables or local configuration files that are ignored by git.

## Configuration

### Wi‑Fi
Wi‑Fi networks are read from a JSON configuration file.  The path can be specified with the environment variable `WIFI_CONFIG_PATH`.  If not set, `/networks.json` is used.  A template file is provided as `networks.example.json`:

```json
{
    "schema": 2,
    "known_networks": [
        { "ssid": "YOUR_WIFI_SSID", "password": "YOUR_WIFI_PASSWORD", "enables_webrepl": false }
    ],
    "access_point": {
        "config": {
            "essid": "Micropython-Dev2",
            "channel": 11,
            "hidden": false,
            "password": "CHANGE_ME"
        },
        "enables_webrepl": false,
        "start_policy": "fallback"
    }
}
```

Copy this file to `networks.json` and fill in your credentials.  Because `networks.json` is listed in `.gitignore`, it will not be committed to version control.

### MQTT
The MQTT client reads its connection settings from environment variables:

- `MQTT_BROKER` – hostname or IP of the broker
- `MQTT_CLIENT_ID` – client identifier (defaults to a value derived from `machine.unique_id` if unset)
- `MQTT_USERNAME` – MQTT username
- `MQTT_PASSWORD` – MQTT password

You can create an optional `.env` or `mqtt_config.json` (both ignored by git) and load the variables before running the code.

An example configuration file is provided as `mqtt_config.example.json`:

```json
{
    "MQTT_BROKER": "mqtt.example.com",
    "MQTT_CLIENT_ID": "ESP32S3_example",
    "MQTT_USERNAME": "homeassistant",
    "MQTT_PASSWORD": "YOUR_MQTT_PASSWORD"
}
```

Copy this file to `mqtt_config.json` and update it with your credentials.

## Running Tests
Run `pytest -q` to execute the test suite (currently empty).

## Print Control
The `print_control` module allows global control over the built-in
`print` function.  Importing the module overrides `print` so you can
disable all output or log it to a file::

    import print_control
    print_control.set_print_enabled(False)
    print("This won't appear on stdout")
    print_control.set_log_to_file(True, "mylog.txt")
    print("This will be written to mylog.txt")

## Stopping Timers
The application schedules most of its work using ``machine.Timer`` instances.
Once ``boot.py`` finishes execution, these timers continue to run in the
background. Pressing ``Ctrl-C`` immediately after boot does **not** stop them
because the boot script has already returned. To clean up running timers from
the REPL, call::

    import bootproc
    bootproc.kill_all_timers()

Alternatively enable automatic cleanup by starting the keyboard interrupt
monitor::

    bootproc.start_keyboard_interrupt_timer()
