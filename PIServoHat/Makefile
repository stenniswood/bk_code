CC = g++
CFLAGS = -std=gnu++11 `pkg-config --cflags gtk+-3.0`
PROG = arm_servs
INC		   = ./ /usr/local/include

SRCS  = main.o sserial.o scan_usb_devs.o drive_five.o i2c_switching.o stance_catagory.o \
	arm.o vector.o servo.o dfServo.o rcServo.o limb.o robot.o MPU6050.o jog_mode.o \
	graph.o dataseries.o read_pendant.o interpolator.o
 

ALL_SRCS = $(SRCS)

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT 
else
	LIBS = 
endif

LIBS +=  -lpthread -lm -lcurl -L/usr/local/lib/ -L/usr/lib/arm-linux-gnueabihf/ -li2c \
	`pkg-config --libs gtk+-3.0`


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
