cc=g++
OPTIONS	= -std=gnu99 -lm
INC	= ../core/can/ ../core/msg/ ../core/misc/ ../core/ipc/
SELF_INC= ./
CFLAGS 	= -lwiringPi -lm
DEPS   	= 
OBJ     =   aconf_main.o help.o help_configs.o config_options.o board_list.o \
			motor_vector.o cmd_process.o OS_Dispatch.o

CORE_OBJS = CAN_Interface.o can_instance.o can_board_msg.o can_eid.o 		 \
			can_rxbuff.o    can_txbuff.o   configuration.o bk_system_defs.o  \
			can_id_list.o   system_msgs_callback.o OS_timers.o

MSG_OBJS  = packer_motor.o packer_lcd.o packer.o parser_tilt.o 
MISC_OBJS = buttons.o leds.o vector_math.o 
CPPOBJ    = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o
	
aconf: $(OBJ) $(addprefix ../core/can/, $(CORE_OBJS)) $(addprefix ../core/misc/, $(MISC_OBJS)) $(addprefix ../core/msg/, $(MSG_OBJS)) 
	gcc -o  $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o
	rm -rf ../core/msg/*.o
	rm -rf ../core/misc/*.o
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

