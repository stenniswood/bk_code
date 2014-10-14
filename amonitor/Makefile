cc      =g++
OPTIONS	= -std=c99
INC		= ./ ../core/can/ ../core/misc/ ../core/msg/ 
CFLAGS 	= -lwiringPi -lrt
DEPS   	= 
OBJ     =   amon_main.o gpio.o filter_file.o OS_Dispatch.o

CAN_OBJS =  can_instance.o can_eid.o can_rxbuff.o can_txbuff.o 		\
		    bk_system_defs.o OS_timers.o configuration.o   			\
		    system_msgs_callback.o CAN_Interface.o can_id_list.o
MISC_OBJS = leds.o ini_file.o board_list.o 
MSG_OBJS  = chevy_codes.o packer.o packer_lcd.o packer_motor.o 
CPPOBJ    = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o

amon: $(OBJ) \
		$(addprefix ../core/can/,  $(CAN_OBJS))  \
		$(addprefix ../core/misc/, $(MISC_OBJS)) \
		$(addprefix ../core/msg/,  $(MSG_OBJS))  
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.bak 
	rm -rf ../core/*/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

