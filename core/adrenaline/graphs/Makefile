CC	= gcc
TARGET  = adrenaline_graphs.a

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

cpp_files := $(wildcard *.cpp)
c_files   := $(wildcard *.c)
all_files = $(cpp_files) $(c_files)

cpp_objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
c_objects   := $(patsubst %.c,%.o,$(wildcard *.c))
all_objects = $(cpp_objects) $(c_objects)

all:
	g++ -c $(OPTIONS) $(CFLAGS) $(addprefix -I,$(INC)) $(all_files)
	ar rc $(TARGET) $(all_objects)
