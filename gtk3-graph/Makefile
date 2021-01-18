CC = g++
CFLAGS = -std=gnu++11
PROG = graph
INC  = ./ /usr/local/include /usr/lib/arm-linux-gnueabihf/perl/5.28.1/CORE/ \
       /usr/local/include/opencv4/

SRCS  = main.o axis.o graph.o histogram.o dataseries.o
 

ALL_SRCS = $(SRCS)

LIBS_opencv = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_objdetect -lopencv_videoio


ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT $(LIBS_opencv) 
else
	LIBS = 
endif

LIBS +=  -lpthread -lm -L/usr/local/lib/ -L/usr/lib/arm-linux-gnueabihf/ $(LIBS_opencv)
LIBS = `pkg-config --libs gtk+-3.0`
CFLAGS = `pkg-config --cflags gtk+-3.0`


%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

all: $(PROG)

$(PROG): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $(PROG)  $(LIBS)

clean:
	rm -f $(PROG)
	rm -rf ./*.o
