# Led Example

The example shows how to use a GPIO on Udoo Neo

All GPIOs are shown at
```
/sys/class/gpio
```

The list of GPIOs numbers and location on the header is shown on [Udoo Neo
Documentation](https://www.udoo.org/docs-neo/Hardware_&_Accessories/GPIO.html)

As mentioned on the documentation, all external header pins are used by the
IMX6Solo Cortex-A9 processor and configured as input.

In order to change the pin as output, open ´direction´ file from the desired pin
and write 'out'. In the example, pin 21 is used:

```
/sys/class/gpio/gpio21/direction"
```

To change the pin value, open the file 'value' file from the desired pin and
write either '1' or '0'.

```
/sys/class/gpio/gpio21/value"
```

