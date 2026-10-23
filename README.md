# ESP Monitor

Water-level alarm built on an ESP8285 with a GSM module. When the level sensor (a reed switch) trips, the device sends an SMS alert and repeats it every 5 minutes while the level stays high. Alerts go over the cellular network, so it works where there is no Wi-Fi. It is configured from a small web app served over its own Wi-Fi access point.

## How it works

- **Firmware** (`full/full.ino`): reads the level sensor, sends SMS alerts through the GSM module (TinyGSM) and exposes a REST API on the device, including endpoints to place test calls and send messages.
- **Configuration** is stored as JSON on the device's SPIFFS (`full/data/`), with a default file used for hard resets.
- **Web app** (`app/`): a Vue app to set up the access point, Wi-Fi, alarm phone numbers and alert message, check GSM status (operator, signal, battery) and send test calls or messages.

## Hardware

Based on the AI Thinker A20 board (ESP8285 plus GSM module). Any Arduino-compatible controller works if you wire the GSM module's TX and RX to the right pins.

| Pin    | Use               |
| ------ | ----------------- |
| `WI00` | GND (flash mode)  |
| `WI04` | Status LED        |
| `WI05` | Level sensor      |
| `WI13` | TX                |
| `WI15` | RX                |

To flash the ESP8285, pull `WI00` to GND. On the A20 board, set switches 2 and 4 to ON so `WI13` and `WI15` connect to the GSM module's internal TX and RX.

## Firmware setup

1. In the Arduino IDE, add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` under **File > Preferences > Additional Boards Manager URLs**.
2. Install the ESP8266 boards package and select **Generic ESP8285 Module** (Flash size: 1M, 512K SPIFFS).
3. Install the **ArduinoJson** and **TinyGSM** libraries.
4. Copy `full/data/config.default.json` to `full/data/config.json` and fill in your values.
5. Upload the sketch, then upload the `full/data` folder to SPIFFS.

You can test the device through the serial console once both are uploaded.

### Configuration

| Key           | Description                                  |
| ------------- | -------------------------------------------- |
| `apssid`      | SSID of the device's own access point        |
| `appass`      | Access point password                        |
| `wifissid`    | Wi-Fi network to join                        |
| `wifipass`    | Wi-Fi password                               |
| `wificonnect` | Join Wi-Fi automatically (`true` / `false`)  |
| `phoneno`     | Phone number for alarms                      |
| `phoneno2`    | Second phone number for alarms               |
| `alertmsg`    | Text sent in the alarm message               |

`config.json` holds the user's settings and `config.default.json` is restored on a hard reset. `config.json` is git-ignored so personal data never ends up in the repository.

## REST API

The device serves the API at `http://192.168.4.1` on its access point.

| Endpoint          | Method | Description                                  |
| ----------------- | ------ | -------------------------------------------- |
| `/config`         | GET    | Full configuration, Wi-Fi and GSM status     |
| `/config/save`    | POST   | Save the configuration                       |
| `/config/reset`   | ANY    | Hard reset to the default configuration      |
| `/config/restart` | ANY    | Restart the device                           |
| `/wifi`           | GET    | Wi-Fi status and available networks          |
| `/wifi/save`      | POST   | Save Wi-Fi settings                          |
| `/gsm`            | GET    | GSM status: SIM, IMEI, operator, signal, battery |
| `/call`           | POST   | Call a phone number                          |
| `/msg`            | POST   | Send a text message                          |
| `/files`          | GET    | List files on SPIFFS                         |

## Web app

```bash
cd app
npm install
npm run watch   # development
npm run build   # production
```

The API address is set in `app/src/main.js` and defaults to the access point IP, `http://192.168.4.1/`.
