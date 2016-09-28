cc=g++
OPTIONS	= -std=c99 -lm
INC		= ./ ../core/can/ ../core/kinetic/ ../core/misc/
CFLAGS 	= -lwiringPi -lm
DEPS   	= 
KINETIC_OBJ = leg_claw.o 
OBJ     =  gpio.o gps_test2.o sserial.o roboclaw.o 
CORE_OBJS = configuration.o leds.o can_id_list.o  

CPPOBJ = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o

claw: $(OBJ) \
	$(addprefix ../core/kinetic/, $(KINETIC_OBJ))
	g++ -o  $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ./core/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
