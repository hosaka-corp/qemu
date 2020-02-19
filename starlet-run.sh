#!/bin/bash

build/arm-softmmu/qemu-system-arm \
	-machine starlet \
	-bios /tmp/boot0.bin \
	-nographic \
	-s -S -singlestep \
	-d in_asm,int,cpu_reset,cpu,exec,mmu \


