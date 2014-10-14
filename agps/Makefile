cc=g++
OPTIONS	= -std=c99 -lm
INC		= ./core/
CFLAGS 	= -lwiringPi -lm
DEPS   	= 
OBJ     = CAN_Interface_.o gpio.o  \
			gps_test.o 
CORE_OBJS = can.o can_instance.o can_eid.o can_board_msg.o can_buff.o can_txbuff.o \
		    bk_system_defs.o  configuration.o leds.o can_id_list.o  

CPPOBJ = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o
	
gps: $(OBJ) $(addprefix ./core/, $(CORE_OBJS)) 
	gcc -o  $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ./core/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

