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
OPTIONS	   = -std=c99 -g 
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
	         -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
SQL_INC    = -I/usr/include/mysql 
INC	   = ./ ../core/can/ ../core/kinetic/ ../core/misc/ ../core/msg/ ../core/ipc/ 
ALL_INCLUSIVE = $(SQL_INC) $(VG_INCLUDE)


SQL_LIB = -L/usr/lib/arm-linux-gnueabihf -lmysqlclient -lz -ldl
CFLAGS = -lwiringPi -lrt -DPLATFORM=$(PLATFORM) \
	     -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac)
LDFLAGS = -lilclient -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host \
		  -lvcos -lvchiq_arm -lpthread -lrt -L./vgfont
LIBFLAGS= -L../core/libs/ilclient/ -L/usr/local/lib -L/opt/vc/lib -L../core/libs/vgfont/  \
	      -lshapes -lwiringPi -lm $(SQL_LIB)
OPENCV_LIBS = -lopencv_video -lopencv_videoio -lopencv_imgcodecs -lopencv_ts -lopencv_face \
	-lopencv_objdetect -l opencv_ml -lopencv_calib3d -lopencv_imgproc -lopencv_highgui \
	-lopencv_core
OBJ       = mouse.o serial.o x_eyes.o x_eyes_main.o roboclaw.o findEyeCenter.o findEyeCorner.o \
	    	helpers.o face_detect.o face_recog.o serial_synchronous.o vision_logger.o \
	    	neck_thread.o
IPC_OBJS  = vision_memory.o


%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(ALL_INCLUSIVE) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(ALL_INCLUSIVE) -c -o $@ $< $(OPTIONS) $(CFLAGS)

xeyes: $(OBJ)	\
	$(addprefix ../core/ipc/, $(IPC_OBJS))
	g++ -o $@ $^ $(CFLAGS) $(LIBFLAGS) $(LDFLAGS) $(OPENCV_LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
