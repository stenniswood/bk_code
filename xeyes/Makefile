RPI = 1
linux_desktop = 2
Mac = 3

PLATFORM  = $(RPI)
#PLATFORM = $(linux_desktop)
#PLATFORM = $(Mac)

ifeq ($(PLATFORM), $(RPI))
HARDWARE_PATH = ./hardware/rpi/
endif
ifeq ($(PLATFORM), $(linux_desktop))
HARDWARE_PATH = ./hardware/linux_desktop/
endif
ifeq ($(PLATFORM), $(Mac))
HARDWARE_PATH = ./hardware/mac/
endif

cc      =g++
OPTIONS	   = -std=c99
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
		 	 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
INC	       = ./ ../core/can/ ../core/misc/ ../core/msg/ ../core/ipc/

CFLAGS = -lwiringPi -lrt -DPLATFORM=$(PLATFORM) \
	  -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac)
LDFLAGS = -lilclient -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host \
			-lvcos -lvchiq_arm -lpthread -lrt -L./vgfont
LIBFLAGS= -L../core/libs/ilclient/ -L/opt/vc/lib -L../core/libs/vgfont/  \
			-ljpeg -lshapes -lwiringPi -lm $(SQL_LIB)
OBJ     = mouse.o serial.o x_eyes.o x_eyes_main.o

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(VG_INCLUDE) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(VG_INCLUDE) -c -o $@ $< $(OPTIONS) $(CFLAGS)

xeyes: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
