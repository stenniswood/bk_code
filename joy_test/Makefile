CC=g++
OPTIONS	= -lm -fno-rtti 
INC	= 
SELF_INC= ./
CFLAGS 	= -lm -g
DEPS   	= 
OBJ     =   joystick_raw.o crc_32.o ps4_hid_outputs.o ff_main.o 

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

