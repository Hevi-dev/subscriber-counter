# YouTube Subscriber Counter presented by Hevi

This is a simple YouTube subscriber counter based on the [W.O.P.R. Missile Launch Code Display Kit](https://unexpectedmaker.com/shop/wopr-missile-launch-code-display-kit).

## Installation

This project is built using [PlatformIO](https://platformio.org/). Install the latest version for your IDE and run a build or `pio run` in the main directory.

To set configuration variables at flash time you can add files to the `data/` directory and upload them using the Upload File-System Task in PlatformIO (`pio run --target uploadfs`). Each file should contain a single line with no carriage return.

Available Settings:
* `data/wifi-ssid`!: The SSID of the access point to connect to.
* `data/wifi-password`!: The password for the access point.

`!` These files are ignored by git to prevent accidentally revealing credentials to the public.