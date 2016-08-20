#!/bin/bash

arm-none-eabi-gdb -s $(basename $(pwd)).elf
