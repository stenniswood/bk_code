CC = g++
CFLAGS = -std=gnu++11
PROG = balance
INC		   = ./ /usr/local/include

SRCS 	   = main.o motor_mapping.o drive_five.o sserial.o vector_file.o cal_file.o \
		load_cell.o scan_usb_devs.o cal_motor.o 

ALL_SRCS = $(SRCS)

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT 
else
	LIBS = 
endif

LIBS +=  -lpthread -lm  -L/usr/local/lib/

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
