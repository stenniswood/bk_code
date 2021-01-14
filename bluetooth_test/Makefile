CC = g++
CFLAGS = -std=gnu++11
PROG = bt
INC		   = ./ /usr/local/include

SRCS 	   = main.o sserial.o bluetooth_server.o server_cmds.o send_streams.o load_cell.o \
	scan_usb_devs.o drive_five.o
 

ALL_SRCS = $(SRCS)

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT 
else
	LIBS = 
endif

LIBS +=  -lpthread -lm -lbluetooth -L/usr/local/lib/ -L/usr/lib/arm-linux-gnueabihf/

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
