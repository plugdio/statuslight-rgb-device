This is a reference implementation for the [Statuslight Online](https://statuslight.online/device) service: the light of the device will be green, red or yellow depending on your status.

You need:

- LOLIN (WEMOS) D1 mini
- RGB LED Shield for LOLIN (WEMOS) D1 mini
- Soldering iron and tin

It’s recommended to use the repository with PlatformIO, but you can also use the Arduino IDE too. The code is based on the homie-esp8266 (https://github.com/homieiot/homie-esp8266) so we can use all the nice features from it, like the UI for configuration. (Don’t forget to upload the data folder to the device.)

After you have built the code and uploaded it to your device:

1. Turn on the device. It will create a new wifi network with the name SL-xxxxxxx.
2. Connect to that network and follow the steps in the configuration UI: select the wifi network that the device should use, enter the password for it, and enter your PIN number.
3. Then your device will reboot and it’s ready to be used.
