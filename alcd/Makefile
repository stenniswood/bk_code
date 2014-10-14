cc=g++
OPTIONS	= -std=c99 -lm
INC	    = ./ ../core/can/ ../core/msg/ ../core/misc/ 
SELF_INC= ./
CFLAGS 	= -lwiringPi -lm
DEPS   	= 
OBJ       = alcd_main.o help.o OS_Dispatch.o 
CAN_OBJS  = can_instance.o can_eid.o can_rxbuff.o can_txbuff.o  \
		    bk_system_defs.o OS_timers.o configuration.o		\
		    system_msgs_callback.o CAN_Interface.o can_id_list.o 
MSG_OBJS  = packer_motor.o packer_lcd.o packer.o  chevy_codes.o 
MISC_OBJS = buttons.o leds.o board_list.o 


%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
	
alcd: $(OBJ)  \
    $(addprefix ../core/can/,  $(CAN_OBJS))  \
    $(addprefix ../core/misc/, $(MISC_OBJS)) \
    $(addprefix ../core/msg/,  $(MSG_OBJS))  
	gcc -o  $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o
	rm -rf ../core/msg/*.o
	rm -rf ../core/misc/*.o
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

