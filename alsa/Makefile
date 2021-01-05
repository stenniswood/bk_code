CC=g++
OPTIONS	= -std=c++14 -lm -fno-rtti 
INC	= 
SELF_INC= ./
CFLAGS 	= -lm -g
DEPS   	= 
OBJ     =   alsa_prep.o wave.o sound_playback.o 

CPPOBJ    = 

LDFLAGS = -lpthread -lrt -lasound

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
	
asynth: $(OBJ)
	$(CC) -o  $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.bak 

