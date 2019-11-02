#!/bin/bash
FILE="main.hex"
avrdude -c usbasp -p m8 -U flash:w:$FILE -U lfuse:w:0xE4:m -U hfuse:w:0xD9:m -y
