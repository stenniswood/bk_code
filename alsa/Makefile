CC=g++
OPTIONS	= -std=c++11 -lm -fno-rtti 
INC	= ./visual/ /usr/include/freetype2
SELF_INC= ./
CFLAGS 	= -lm -g
DEPS   	= 
OBJ     =  alsa_record.o wave.o 

CPPOBJ    = alsa_record.o wave.o 

LDFLAGS = -lpthread -lrt -lasound -lcurl -lglut -lGLEW -lGL -lfreetype

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
	
alsa_record: $(OBJ)
	$(CC) -o  $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.bak 

