cc		  = g++
OPTIONS	  = -std=gnu99 -lm 
INC		  = ../core/can/ ../core/msg/ ../core/misc/ ../core/fusion/ ../core/ipc/ ./
SELF_INC  = ../../aquad/ ./
CFLAGS 	  = -lwiringPi -lm -lrt 
DEPS   	  = 
OBJ        = quad_main.o quad_ini.o 
#can_buff.o 
CAN_OBJS   = CAN_Interface.o can_instance.o can_eid.o can_txbuff.o can_rxbuff.o   \
	         can_board_msg.o bk_system_defs.o can_id_list.o system_msgs_callback.o \
	         configuration.o 
MSG_OBJS   = packer_lcd.o packer.o parser_tilt.o packer_motor.o
MISC_OBJS  = buttons.o leds.o vector_math.o timer.o board_list.o 
FUSION_OBJS= catagorization.o fuse_ag.o power.o fuse_accel.o fuse_gyro.o fuse_magnet.o
CPPOBJ     = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)

../core/misc/%.o:%.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)

#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o 

quad: $(OBJ)  $(addprefix   ../core/can/, $(CAN_OBJS)) 	 \
				$(addprefix ../core/misc/,$(MISC_OBJS))  \
				$(addprefix ../core/msg/, $(MSG_OBJS))   \
				$(addprefix ../core/fusion/, $(FUSION_OBJS))
	$(cc) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
