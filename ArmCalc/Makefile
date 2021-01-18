

CC=g++
OPTIONS	=
INC	= 
SELF_INC= ./
CFLAGS 	= -g
OBJ     =   APP_CoordinateTransform.o APP_InverseCoordinates.o APP_Servos.o APP_Calibrations.o drive_five.o main.o

LDFLAGS = -lm 

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
	
ArmXforms: $(OBJ)
	$(CC) -o  $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.bak 
	rm -rf *.lst *.map
	
