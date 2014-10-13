cc		  = g++
OPTIONS	  = -std=gnu99 -lm 
INC		  = ../core/can/ ../core/msg/ ../core/misc/ ../core/fusion/ ./
SELF_INC  = ../../orient/ ./
CFLAGS 	  = -lwiringPi -lm -lrt -lstdc++
DEPS   	  = 
OBJ        = segway_main.o board_list.o help.o sway_config.o app_tilt.o 
CAN_OBJS   = CAN_Interface.o can_instance.o can_eid.o can_rxbuff.o can_txbuff.o   \
	         bk_system_defs.o can_id_list.o system_msgs_callback.o 
MSG_OBJS   = packer_lcd.o packer.o parser_tilt.o packer_motor.o
MISC_OBJS  = leds.o vector_math.o timer.o preferences.o sway_memory.o
FUSE_OBJS  = fuse_ag.o fuse_accel.o fuse_gyro.o fuse_magnet.o fuse_log.o catagorization.o 
CPPOBJ     = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)

#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o 

sway: $(OBJ)    $(addprefix   ../core/can/, $(CAN_OBJS)) 	 \
		$(addprefix ../core/misc/,$(MISC_OBJS))  	 \
		$(addprefix ../core/fusion/,$(FUSE_OBJS))  	 \
		$(addprefix ../core/msg/, $(MSG_OBJS)) 
	gcc -o  $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
