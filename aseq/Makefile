RPI = 1
linux_desktop = 2
Mac = 3

USE_ROBOCLAW = 1
USE_BK_QM    = 2
USE_BK_CAN   = 3
MOTOR_DRIVER = USE_ROBOCLAW
#MOTOR_DRIVER= USE_BK_QM
#MOTOR_DRIVER= USE_BK_CAN

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

MACROS = -DPLATFORM=$(PLATFORM) -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac) \
	     -DMOTOR_DRIVER=$(USE_ROBOCLAW) -DUSE_ROBOCLAW=$(USE_ROBOCLAW) -DUSE_BK_QM=$(USE_BK_QM) -DUSE_BK_CAN=$(USE_BK_CAN)

cc		 = g++
OPTIONS	 = -lm
INC		 = ../core/can ../core/msg ../core/misc ../core/ipc ../core/mysql/ ../core/nlp/
SELF_INC = ./
INCLUDE  = $(INC) $(SELF_INC)
CFLAGS 	 = -lwiringPi -lm -lrt $(MACROS)
DEPS   	 = 
OBJ      = seq_main.o board_list.o motor_vector.o cmd_process.o leds.o  config_file.o \
	      string_util.o vector_file.o motor.o motor_control.o Appendage.o robot.o \
	      angular_vectors_isolated_hip.o bigmotor_callbacks.o teach_pendant.o seq_init.o \
			SEQUENCER_protocol.o roboclaw.o
NLP_OBJS =  nlp_sentence.o super_string.o
CAN_OBJS =  CAN_Interface.o can_instance.o can_eid.o can_rxbuff.o can_txbuff.o   \
	        bk_system_defs.o can_id_list.o system_msgs_callback.o
MISC_OBJS = buttons.o leds.o vector_math.o timer.o preferences.o sserial.o
MSG_OBJS  = packer_lcd.o packer_motor.o packer.o 
IPC_OBJS  = sequencer_memory.o shm_base.o
MYSQL_OBJS = machine_defs.o
CPPOBJ 	  = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INCLUDE)) -c -o $@ $< $(CFLAGS) -std=c++11
%.o: %.c $(DEPS)
	$(CC) -std=gnu99 $(addprefix -I,$(INCLUDE)) -c -o $@ $< $(OPTIONS) $(CFLAGS)

#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o
LD_FLAGS = -lpthread

aseq: $(OBJ) $(addprefix ../core/can/, $(CAN_OBJS)) \
			$(addprefix ../core/mysql/, $(MYSQL_OBJS)) \
			$(addprefix ../core/msg/, $(MSG_OBJS)) \
			$(addprefix ../core/ipc/, $(IPC_OBJS)) \
			$(addprefix ../core/nlp/, $(NLP_OBJS)) \
			$(addprefix ../core/misc/,$(MISC_OBJS))
	$(cc) -o  $@ $^ $(CFLAGS) $(LD_FLAGS)

clean:
	rm -rf *.o $(PRG).elf 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf ../core/ipc/*.o 	
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

