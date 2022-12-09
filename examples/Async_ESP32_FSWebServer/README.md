# Async_ESP32_FSWebServer_DRD Example

Compare this efficient Async_ESP32_FSWebServer_DRD example with the so complicated twin [ESP32_FSWebServer_DRD](https://github.com/khoih-prog/ESP_WiFiManager/tree/main/examples/ESP32_FSWebServer_DRD) to appreciate the powerful AsynWebServer this [AsyncESP32_W5500_Manager Library](https://github.com/khoih-prog/AsyncESP32_W5500_Manager) is relying on.

## First, how Config Portal works?

Connect to Config Portal @ the localIP address, e.g. `192.168.2.232`


You'll see this `Main` page:

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncESP32_W5500_Manager/raw/main/Images/Main.png">
</p>

Select `Information` to enter the Info page where the board info will be shown (long page)

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncESP32_W5500_Manager/raw/main/Images/Info.png">
</p>


Select `Configuration` to enter this page where you can select an AP and specify its WiFi Credentials

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncESP32_W5500_Manager/raw/main/Images/Configuration_Standard.png">
</p>

Enter your credentials, then click `Save`.

---

## How to use this Async_ESP32_FSWebServer_DRD example?

This shows you how to use this example in Ubuntu (but you can use similar commands in other OSes)

### Download Data files

1. For example, you already downloaded data files from [Async_ESP32_FSWebServer_DRD data](https://github.com/khoih-prog/AsyncESP32_W5500_Manager/tree/main/examples/Async_ESP32_FSWebServer_DRD/data) to a local folder, for example:

```
~/Arduino/libraries/AsyncESP32_W5500_Manager-main/examples/Async_ESP32_FSWebServer_DRD/data
```

### HOWTO Upload files to ESP32 (SPIFFS or FFat)

Use one of these methods (preferable first)

1. Go to http://async-esp32fs.local/edit, then "Choose file" -> "Upload"
2. or Upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
3. or upload the contents of a folder by running the following commands: 
```
Ubuntu$ cd ~/Arduino/libraries/AsyncESP32_W5500_Manager-main/examples/Async_ESP32_FSWebServer_DRD/data
Ubuntu$ for file in \`\ls -A1\`; do curl -F "file=@$PWD/$file" http://async-esp32fs.local/edit; done
```

---

### Demonstrating pictures

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncESP32_W5500_Manager/raw/main/examples/Async_ESP32_FSWebServer_DRD/pics/async-esp32fs.local.png">
</p>

4. Edit / Delete / Download any file in the the folder by going to http://async-esp32fs.local/edit

<p align="center">
    <img src="https://github.com/khoih-prog/AsyncESP32_W5500_Manager/raw/main/examples/Async_ESP32_FSWebServer_DRD/pics/async-esp32fs.local_edit.png">
</p>


