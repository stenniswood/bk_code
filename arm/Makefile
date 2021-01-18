
CC=g++
OPTIONS	= -std=gnu99 -lm -fno-rtti 
INC	    = /usr/include/
SELF_INC= ./
CFLAGS 	= -lm -g -I/home/pi/glm/
DEPS   	= 
OBJ     =  arm.o moving_object.o verbal_object.o gl_object.o gl_atom.o \
		   glSphere.o Texture.o vector.o ibeam.o cylinder.o extrusion.o tk_colors.o \
		   key_press.o

CPPOBJ    = 

LDFLAGS = -lpthread -lrt -lGL -lGLU -lglut -lSOIL

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
	
arms: $(OBJ)
	$(CC) -o  $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.bak 
	rm -rf *.lst *.map
