# Crazyflie micro-XRCE-DDS demo:

## How to set-up the demo

You need the next dependencies:

- Toolchain:
```bash
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt-get update
sudo apt install gcc-arm-embedded
```

- Bridge:
  - Give UDev permissions to the radio:
  ```bash
  sudo groupadd plugdev
  sudo usermod -a -G plugdev $USER
  ```
  - You will need to log out and log in again in order to be a member of the plugdev group.

  - Create a file named /etc/udev/rules.d/99-crazyradio.rules and add the following:
  ```
  # Crazyradio (normal operation)
  SUBSYSTEM=="usb", ATTRS{idVendor}=="1915", ATTRS{idProduct}=="7777", MODE="0664", GROUP="plugdev"
  # Bootloader
  SUBSYSTEM=="usb", ATTRS{idVendor}=="1915", ATTRS{idProduct}=="0101", MODE="0664", GROUP="plugdev"
  ```
  To connect Crazyflie 2.0 via usb, create a file name /etc/udev/rules.d/99-crazyflie.rules and add the following:
  ```SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="5740", MODE="0664", GROUP="plugdev"```
  - You can reload the udev-rules using the following:
    - `sudo udevadm control --reload-rules`
    - `sudo udevadm trigger`

  - Install the bridge utility
  ```bash
  sudo apt-get install libusb-1.0-0-dev
  git clone https://github.com/jfm92/micro-XRCE-DDS_PX4_Bridge
  cd micro-XRCE-DDS_PX4_Bridge
  git submodule update --init
  make build
  ```

- Firmware:
```bash
https://github.com/eProsima/crazyflie-firmware -b cf_micro-xrce-dds
cd crazyflie-firmware
git submodule init
git submodule update
make PLATFORM=cf2
```

## How to flash:

- Compile the firmware by executing on the firmware folder the next command:
`make build`
- Unplug the battery.
- Push the reset button, and at the same time connect the USB cable.
- Only one blue LED must be on and blinky. After a few seconds it will start blinking faster and now is on DFU mode.
- To check if it's on DUF mode, execute the next command: `lsusb`
  - This should return somenthing like this:
  ```bash
  Bus 001 Device 051: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
  ```
- Now flash using the next command: ``sudo dfu-util -d 0483:df11 -a 0 -s 0x08000000 -D cf2.bin``
  - This should return somenthing like this:
```bash
Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
Copyright 2010-2019 Tormod Volden and Stefan Schmidt
This program is Free Software and has ABSOLUTELY NO WARRANTY
Please report bugs to http://sourceforge.net/p/dfu-util/tickets/
dfu-util: Invalid DFU suffix signature
dfu-util: A valid DFU suffix will be required in a future dfu-util release!!!
Opening DFU capable USB device...
ID 0483:df11
Run-time device DFU version 011a
Claiming USB DFU Interface...
Setting Alternate Setting #0 ...
Determining device status: state = dfuERROR, status = 10
dfuERROR, clearing status
Determining device status: state = dfuIDLE, status = 0
dfuIDLE, continuing
DFU mode device DFU version 011a
Device returned transfer size 2048
DfuSe interface name: "Internal Flash  "
Downloading to address = 0x08000000, size = 350776
Download	[=========================] 100%       350776 bytes
Download done.
File downloaded successfully
```
- Now the drone is correctly flashed, if you reset, it should start properly.

## Set-up the demo:

**Is important to respect the order of execution, if not the demo won't work**
- Connect the antenna to the USB port of the PC.
- Execute the bridge by typing the next command on the bridge folder: `make run`
  - This should return somenthing like this:
  ```bash
  ./build/bridge
  Pseudo-Serial device opend at /dev/pts/0
  Waiting for messages...
  ```
  (It could return an error of libusb, but is not problem.)
- Execute the micro-XRCE-DDS Agent by typing the next command: `MicroXRCEAgent serial --dev /dev/pts/0`
- Finally turn on the drone and after a few seconds, it should connect to the Agent and start the publication process.
