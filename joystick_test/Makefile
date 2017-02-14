CC=g++
OPTIONS	= -std=gnu99 -lm -fno-rtti
INC	= 
SELF_INC= ./
CFLAGS 	= -lm
DEPS   	= 
OBJ     =   joystick.o joystick_pendant.o main.o sserial.o roboclaw.o 

CPPOBJ    = 

LDFLAGS = -lpthread -lrt

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
	
joys: $(OBJ)
	$(CC) -o  $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.bak 
	rm -rf ../core/can/*.o
	rm -rf ../core/msg/*.o
	rm -rf ../core/misc/*.o
	rm -rf *.lst *.map

