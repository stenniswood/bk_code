CC = g++
CFLAGS = 
PROG = cube

SRCS = main.cpp imageloader.cpp gl_container.cpp cylinder.cpp room.cpp  \
		table.cpp stairway.cpp cabinet.cpp extrusion.cpp ibeam.cpp \
		robot_leg_piece.cpp txt_container.cpp 

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT 
else
	LIBS = -lglut
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
