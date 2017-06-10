CC		= gcc
TARGET  = adrenaline_system.a

OPTIONS	= -Wwrite-strings -fpermissive -Wreturn-type -Wformat
CFLAGS  = -g -Wall 

CORE_INC   = ../can/  ../fusion/ ../hardware/ ../kinetic 	\
	     ../mysql/ ../misc/ ../msg/ ../wifi/ ../ipc/    	\
	     ../libs/vgfont/ ../adrenaline/graphs/ ../libs/ilclient/ \
	     molecules/calendar/ molecules/CAN/
DCORE_INC = $(addprefix ../, $(CORE_INC))

SQL_INC    = /usr/include/mysql
#INC = $(SQL_INC) $(CORE_INC) ./ ../can/ ../ system/ atoms/ molecules/ graphs/ \
#	  /opt/vc/include /opt/vc/include/interface/vmcs_host/linux
DINC = $(SQL_INC) $(DCORE_INC) ./ ../ ../../can/ ../system/ ../atoms/ ../molecules/ ../graphs/ \
	  /opt/vc/include /opt/vc/include/interface/vmcs_host/linux

src= $(wildcard *.cpp)

atoms_cpp_files 	:= $(patsubst atoms/%, %, $(wildcard atoms/*.cpp))
atoms_c_files   	:= $(patsubst atoms/%, %, $(wildcard atoms/*.c))

molecules_cpp_files := $(patsubst molecules/%, %, $(wildcard molecules/*.cpp))
molecules_c_files   := $(patsubst molecules/%, %, $(wildcard molecules/*.cpp))

system_cpp_files 	:= $(patsubst system/%, %, $(wildcard system/*.cpp))
system_c_files   	:= $(patsubst system/%, %, $(wildcard system/*.cpp))

graphs_cpp_files 	:= $(patsubst graphs/%, %, $(wildcard graphs/*.cpp))
graphs_c_files   	:= $(patsubst graphs/%, %, $(wildcard graphs/*.cpp))

all_files = $(atoms_cpp_files) $(atoms_c_files) \
			$(molecules_cpp_files) $(molecules_c_files) \
			$(system_cpp_files) $(system_c_files) \
			$(graphs_cpp_files) $(graphs_c_files)

#cpp_objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
#c_objects   := $(patsubst %.c,%.o,$(wildcard *.c))
all_objects0 = $(patsubst %.cpp,%.o,$(all_files))
all_objects = $(patsubst %.c,%.o,$(all_objects0))
	
all: 
	cd atoms/ && g++ -c $(OPTIONS) $(CFLAGS) $(addprefix -I,$(DINC)) $(atoms_cpp_files) $(atoms_c_files)
	cd molecules/ && g++ -c $(OPTIONS) $(CFLAGS) $(addprefix -I,$(DINC)) $(molecules_cpp_files) $(molecules_c_files)
	cd system/ && g++ -c $(OPTIONS) $(CFLAGS) $(addprefix -I,$(DINC)) $(system_cpp_files) $(system_c_files)
	cd graphs/ && g++ -c $(OPTIONS) $(CFLAGS) $(addprefix -I,$(DINC)) $(graphs_cpp_files) $(graphs_c_files)
	echo $(wildcard atoms/*.o)
	ar rc adrenaline_atoms.a $(wildcard atoms/*.o)
	ar rc adrenaline_molecules.a $(wildcard molecules/*.o)
	ar rc adrenaline_system.a $(wildcard system/*.o)
	ar rc adrenaline_graphs.a $(wildcard graphs/*.o)
	ar rc adrenaline.a $(wildcard atoms/*.o) $(wildcard molecules/*.o) $(wildcard system/*.o) $(wildcard graphs/*.o)
	mkdir -p lib
	mkdir -p include
	mv adrenaline*.a ./lib/
	cp $(wildcard atoms/*.hpp) ./include/
	cp $(wildcard molecules/*.hpp) ./include/
	cp $(wildcard molecules/CAN/*.hpp) ./include/
	cp $(wildcard molecules/calendar/*.hpp) ./include/
	cp $(wildcard system/*.hpp) ./include/
	cp $(wildcard graphs/*.hpp) ./include/

clean:
	rm atoms/*.o molecules/*.o system/*.o graphs/*.o lib/*.a include/*
