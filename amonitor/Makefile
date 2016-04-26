RPI = 1
linux_desktop = 2
Mac = 3

PLATFORM  = $(RPI)
#PLATFORM = $(linux_desktop)
#PLATFORM  = $(Mac)

ifeq ($(PLATFORM), $(RPI))
HARDWARE_PATH = ./hardware/rpi/
endif
ifeq ($(PLATFORM), $(linux_desktop))
HARDWARE_PATH = ./hardware/linux_desktop/
endif
ifeq ($(PLATFORM), $(Mac))
HARDWARE_PATH = ./hardware/mac/
endif

cc      =g++
OPTIONS	= -std=c99
INC		= ./ ../core/can/ ../core/misc/ ../core/msg/ ../core/ipc/
CFLAGS 	= -lwiringPi -lrt -DPLATFORM=$(PLATFORM) \
		-DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac)
LDFLAGS = -lpthread -lrt
DEPS   	= 
OBJ     = amon_main.o gpio.o OS_Dispatch.o 

#can_rxbuff.o ipc_can_rxbuff.o filter_file.o
CAN_OBJS =  can_instance.o can_eid.o  can_txbuff.o 	 \
		    bk_system_defs.o OS_timers.o configuration.o   	\
		    system_msgs_callback.o CAN_Interface.o can_id_list.o
MISC_OBJS = leds.o ini_file.o board_list.o
MSG_OBJS  = chevy_codes.o packer.o packer_lcd.o packer_motor.o
CPPOBJ    = 
IPC_OBJS  = CAN_memory.o 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o

amon: $(OBJ) \
		$(addprefix ../core/can/,  $(CAN_OBJS))  \
		$(addprefix ../core/misc/, $(MISC_OBJS)) \
		$(addprefix ../core/ipc/,  $(IPC_OBJS))
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.bak 
	rm -rf ../core/*/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	
