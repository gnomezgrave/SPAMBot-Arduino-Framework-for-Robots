# SPAM-Bot #

The project is build using the Arduino UNO and Arduino duemilanove/ATmega328p boards. It makes use of continuous rotational servos for the wheels of the robot and positional rotational motors for the robot arm and gripper. IR sensors are used to detect and follow the navigational path. A Sonar sensor is used to identify the payloads across the map. A Rasberry Pi connected to a normal Webcam is used to capture the payload key and process it.

### What is this repository for? ###

This is an Autonomous Robot designed to navigate across a map and collect payloads distributed at various points. These payloads are delivered at the other end of the map in a sorted order (adjacent to each other with only the last payload on top if the previous one). The sorting is done according to a barcode number printed on each face of the payload.

### How do I get set up? ###

* The Fully Rotational Servos used for the wheels are connected to the Arduino UNO boards pins 13 and 12.
* The Servos controlling the Arm & Gripper motion are connected to pins 10 to 13 of the Duemilanove board.
* The Web Camera is connected to the Rasberry Pi board
* The Arduino boards and the Rasberry use I2C for communication
* The sonar is connected the pins 9 & 10 of the Arduino UNO board
* The Rasberry Pi runs OpenCV to process the captured images


### Contribution guidelines ###

* This is a private project between four students for a Robotics competition. Hence, at the moment we do not wish to update any external contribution. However, feel free to download the source, do the configuration and test it for your own projects.

* Nevertheless, we do welcome constructive suggestions...

### Who do I talk to? ###

* Any of the Repo Admins of **S** *(habir)* - **P** *(raneeth)* - **A** *(nushanga)* - **M** *(ilan)* - BOT