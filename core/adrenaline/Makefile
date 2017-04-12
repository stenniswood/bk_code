CC		= gcc
TARGET  = adrenaline_system.a

OPTIONS	= -Wwrite-strings -fpermissive -Wreturn-type -Wformat
CFLAGS  = -g -Wall 

CORE_INC   = ../../can/  ../../fusion/ ../../hardware/ ../../kinetic 	\
	     ../../mysql/ ../../misc/ ../../msg/ ../../wifi/ ../../ipc/    	\
	     ../../libs/vgfont/ ../../adrenaline/graphs/ ../../libs/ilclient/ \
	     ../../adrenaline/ ../../adrenaline/atoms/ ../../adrenaline/molecules/ \
	     ../../adrenaline/molecules/calendar/ ../../adrenaline/system/

SQL_INC    = /usr/include/mysql
INC = $(SQL_INC) $(CORE_INC) ./ ../../can/ ../ ../system/ ../atoms/ ../graphs/ /opt/vc/include /opt/vc/include/interface/vmcs_host/linux

src= $(wildcard *.cpp)

all:
	mkdir -p lib
	mkdir -p include
	cp atoms/adrenaline_atoms.a ./lib/adrenaline_atoms.a
	cp molecules/adrenaline_molecules.a ./lib/adrenaline_molecules.a
	cp system/adrenaline_system.a ./lib/adrenaline_system.a
	cp graphs/adrenaline_graphs.a ./lib/adrenaline_graphs.a
	cp $(wildcard atoms/*.hpp) ./include/
	cp $(wildcard molecules/*.hpp) ./include/
	cp $(wildcard system/*.hpp) ./include/
	cp $(wildcard graphs/*.hpp) ./include/
