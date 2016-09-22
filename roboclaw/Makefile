cc=g++
OPTIONS	= -std=c99 -lm
INC		= ../core/can/ 
CFLAGS 	= -lwiringPi -lm
DEPS   	= 
OBJ     =  gpio.o gps_test2.o sserial.o roboclaw.o 
CORE_OBJS = configuration.o leds.o can_id_list.o  

CPPOBJ = 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o

claw: $(OBJ)
	g++ -o  $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ./core/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
