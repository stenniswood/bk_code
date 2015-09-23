cc		 = g++
OPTIONS	 =  -lm
INC		 = ../core/can ../core/msg ../core/misc ../core/ipc
SELF_INC = ./
CFLAGS 	= -lwiringPi -lm -lrt
DEPS   	= 
OBJ     = seq_main.o board_list.o motor_vector.o cmd_process.o leds.o  config_file.o \
	      string_util.o vector_file.o motor.o motor_control.o Appendage.o robot.o \
	      angular_vectors_isolated_hip.o bigmotor_callbacks.o teach_pendant.o seq_init.o
	      

CAN_OBJS =  CAN_Interface.o can_instance.o can_eid.o can_rxbuff.o can_txbuff.o   \
	        bk_system_defs.o can_id_list.o system_msgs_callback.o 
MISC_OBJS = buttons.o leds.o vector_math.o timer.o preferences.o 
MSG_OBJS  = packer_lcd.o packer_motor.o packer.o 
CPPOBJ 	  = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) -std=gnu99 $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)

#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o

seq: $(OBJ) $(addprefix ../core/can/, $(CAN_OBJS)) \
			$(addprefix ../core/msg/, $(MSG_OBJS)) \
			$(addprefix ../core/misc/,$(MISC_OBJS)) 
	$(cc) -o  $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

