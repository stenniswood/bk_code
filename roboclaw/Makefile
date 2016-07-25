RPI = 1
linux_desktop = 2
Mac = 3

PLATFORM  = $(RPI)
#PLATFORM = $(linux_desktop)
#PLATFORM  = $(Mac)
ifeq ($(PLATFORM), $(RPI))
HOME_PATH = /home/pi/     
endif
ifeq ($(PLATFORM), $(linux_desktop))
HOME_PATH = ~/                
endif
ifeq ($(PLATFORM), $(Mac))
HOME_PATH = /Users/stephentenniswood/
endif
MACROS = -DPLATFORM=$(PLATFORM) -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac)

cc		 = g++
OPTIONS	 =  -lm
INC	 = ../core/can ../core/msg ../core/misc ../core/ipc
SELF_INC = ./
CFLAGS 	= -lwiringPi -lm -lrt $(MACROS)
DEPS   	= 
OBJ = RoboClaw	      

CAN_OBJS =  CAN_Interface.o can_instance.o can_eid.o can_rxbuff.o can_txbuff.o   \
	        bk_system_defs.o can_id_list.o system_msgs_callback.o 
MISC_OBJS = buttons.o leds.o vector_math.o timer.o preferences.o 
MSG_OBJS  = packer_lcd.o packer_motor.o packer.o 
IPC_OBJS  = sequencer_memory.o
CPPOBJ 	  = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) -std=gnu99 $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)

#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o
LD_FLAGS = -lpthread

seq: $(OBJ) $(addprefix ../core/can/, $(CAN_OBJS)) \
	$(cc) -o  $@ $^ $(CFLAGS) $(LD_FLAGS)

clean:
	rm -rf *.o $(PRG).elf 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf ../core/ipc/*.o 	
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

