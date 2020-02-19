#!/bin/bash

PREFIX="arm-none-eabi"
${PREFIX}-gdb -ex 'target remote :1234'
