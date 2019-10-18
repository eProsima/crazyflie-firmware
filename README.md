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

  - Install the Crazyflie client bridge
  ```bash
  sudo apt-get install libusb-1.0-0-dev
  sudo apt-get install python3 python3-pip python3-pyqt5 python3-pyqt5.qtsvg
  git clone https://github.com/eProsima/crazyflie-clients-python -b Micro-XRCE-DDS_Bridge
  cd crazyflie-clients-python
  pip3 install -e .
  ```

- Firmware:
```bash
git clone https://github.com/eProsima/crazyflie-firmware -b cf_micro-xrce-dds
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

- Attach the [flowdeck](https://www.bitcraze.io/flow-deck-v2/) sensor to the drone.

- Execute the Crazyflie client by typing the next command on the client folder: `python3 bin/cfclient`

- Once you execute the command, this should return somenthing like this:
```bash
INFO:cfclient.gui:Disabling STL printouts
INFO:cfclient.utils.input.inputreaders:Input readers: ['linuxjsdev', 'pysdl2']
INFO:cfclient.utils.input.inputreaders:Successfully initialized [linuxjsdev]
INFO:cfclient.utils.input.inputreaders:Could not initialize [pysdl2]: No SDL2 support on Linux
INFO:cfclient.utils.input.inputinterfaces:Found interfaces: ['leapmotion', 'wiimote', 'zmqpull']
INFO:cfclient.utils.input.inputinterfaces:Could not initialize [leapmotion]: Leap Motion library probably not installed (No module named 'leapsdk')
INFO:cfclient.utils.input.inputinterfaces:Could not initialize [wiimote]: Missing cwiid (wiimote) driver No module named 'cwiid'
INFO:cfclient.utils.config:Dist config read from /home/juan/client_bridge/crazyflie-clients-python/src/cfclient/configs/config.json
INFO:cfclient.utils.config:Config file read from [/home/juan/.config/cfclient/config.json]
INFO:cfclient.utils.input.inputinterfaces:Could not initialize [zmqpull]: ZMQ input disabled in config file
============= Micro-XRCE-DDS bridge port: /dev/pts/0 =============
INFO:cfclient.utils.zmq_param:Biding ZMQ for parameters at tcp://*:1213
INFO:cfclient.utils.zmq_led_driver:Biding ZMQ for LED driverat tcp://*:1214
INFO:cfclient.utils.input:Using device blacklist [(VirtualBox|VMware)]
INFO:cflib.crtp.radiodriver:v0.53 dongle with serial N/A found
INFO:cfclient.ui.tabs.QualisysTab:Switching Flight Mode to: FlightModeStates.DISCONNECTED
INFO:cflib.drivers.cfusb:Looking for devices....
```
- The serial port which will handle the micro-XRCE-DDS communication is the `/dev/pts/0`, as you can see on the console.

- On another console, open a micro-XRCE-DDS Agent which use serial communications, and connects to the bridge port: `MicroXRCEAgent serial --dev /dev/pts/0`

- Go to the graphical interface and push on the on scan button. If the radio is set-up properly, on the dropdown menu of the left you should the radio: `radio://0/80/2M`.

- Push on connect.

- Automatically the micro-XRCE-DDS client on the drone will set-up the communication and it will start the sending process of the topics.

## How to fly the drone with assisted fly:

Fly the drone without assisted flight is a very difficult task. To avoid this problem the client offer us different ways of flight.
- **Altitude hold:** This mode will try to maintain the same altitude base on the measures of the barometer. It as a precision of +-15 cm, so the drone can bounce while is flying due to the atmospheric pressure variations.

- **Position hold:** This mode will try to maintain the position set, compensating external force that can modify the position.

- **Hover:** This mode requires the flowdeck module, to be use. Basically, it will maintain constantly the same distance from the floor. This will make very easy to fly the drone, avoiding constantly compensate the altitude.

All of the previous mode need a controller to be used. So, you can use a PS3/PS4, Xbox 360/Xbox One or a regular Joystick controller. First we need to connect to the computer, you can use by wired or Bluetooth connection.

After connect it, is necessary to configure it. To do so, go to `Input Device -> Normal -> Input map ` and check the controller that you want to use. (Note: Xbox One controller use the Xbox 360 input map).
On the other hand, if you want to set your custom controls, go to: `Input Device -> Configure device mapping`

Now is time to flight, theses are the input map by default:
- Left joystick: Controls pitch(Vertical axis) and roll(Horizontal Axis).
- Right joystick: Controls thrust(Vertical axis)  and yaw(Horizontal Axis).
- A button(Xbox) or X button(PS): Controls the special function, that you set on the client.
  - If you are on altitude mode, you can press the special button and set your altitude by increasing or decreasing the thrust.
  - If you are on position mode, when you push the special button, will set this position and will try to maintain.
  - If you are on hover mode, and push the special button, will perform a take off and will be stable at 40 cm.
