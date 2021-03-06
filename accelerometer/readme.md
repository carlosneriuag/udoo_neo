Accelerometer Sample Code
=========================

This sample code shows how to use I2C to read data from the accelerometer.

To run it, follow the next steps:

1.- Compile the project using make

2.- Execute the application using sudo and the i2c device to use. Checking [Udoo Neo documentation](https://www.udoo.org/docs-neo/Hardware_&_Accessories/Motion_sensors.html) you'll find out the accelerometer is at I2C3

```
sudo ./accelerometer.out /dev/i2c-3
```

3.- If everything works as expected, the application confirms the sensor is found

```
FXOS8700CQ Found!
```

4.- As indicated by the application, press enter to get a new sensor reading

```
Press enter for a new reading...

X Axis: +223
Y Axis: +152
Z Axis: +4048

X Axis: +221
Y Axis: +140
Z Axis: +4048

X Axis: +223
Y Axis: +147
Z Axis: +4044
```
