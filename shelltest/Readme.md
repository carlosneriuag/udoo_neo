# Shell Example

## Overview
This example shows how to implement a simple shell with readline

## Requierements
Readline development package is required to run this example. Install it via apt-get:

```
sudo apt-get install libreadline6 libreadline6-dev
```

## Example
Follow the next steps to run the example:

1. Compile it via   ```make```
2. Execute with the following command ```./shell_test```
3. A welcome message and prompt is shown
```
     ## # # ### #   #
    #   # # #   #   #
     #  ### ##  #   #
      # # # #   #   #
    ##  # # ### ### ###
     Nov  9 2018
shell>
```
4. The following commands are supported:
* add
* sub
* mul
* div
* help
* exit

5. Each operation command requires 2 numbers and the operation result is printed on the console

```
shell>mul 4 5
Result = 20
```