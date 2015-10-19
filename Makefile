#
#  makefile
#
#  Created on: Oct 16, 2012
#      Authors: Nicolo Rivetti, Eleonora Calore
#

srcDir:=../src

obj-m += module.o

module-objs += $(srcDir)/module.o $(srcDir)/barrierSyscall.o $(srcDir)/barrierDataStructure.o $(srcDir)/tagDataStructure.o $(srcDir)/processDataStructure.o

syscallSrc:=  $(srcDir)/syscallUsr.c

all: module syscall

syscall:
	gcc $(syscallSrc) -o syscall

module:
	make -C /lib/modules/3.2.0-31-generic-pae/build M=$(PWD) modules
	rm $(srcDir)/module.o
	rm $(srcDir)/barrierSyscall.o
	rm $(srcDir)/barrierDataStructure.o
	rm $(srcDir)/tagDataStructure.o
	rm $(srcDir)/processDataStructure.o
	
clean:
	make -C /lib/modules/3.2.0-31-generic-pae/build M=$(PWD) clean
