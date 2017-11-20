USE_KINECT = 1

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

cc      = g++
OPTIONS	= -std=c99 -g  
CFLAGS  = -lwiringPi -lrt -DPLATFORM=$(PLATFORM) -DUSE_KINECT_SENSOR=$(USE_KINECT) \
	     -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac)

# ***** INCLUDE PATHS : 
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
	         -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
SQL_INC    = -I/usr/include/mysql 
INC	   	   = ./ ../core/can/ ../core/kinetic/ ../core/misc/ ../core/msg/ ../core/ipc/ \
	         /usr/local/include/libfreenect/ ../abkInstant/threads/ ./vision/ ./nlp/ \
	         ./control/ ../core/dbase/ ../core/vision/
ALL_INCLUSIVE = $(addprefix -I,$(INC)) $(SQL_INC) $(VG_INCLUDE)

# LIBRARY PATHS : 
SQL_LIB = -L/usr/lib/arm-linux-gnueabihf -lmysqlclient -lz -ldl
LDFLAGS = -lilclient -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host \
	  -lvcos -lvchiq_arm -lpthread -lrt -L./vgfont -lfreenect
LIBFLAGS= -L../core/libs/ilclient/ -L/usr/local/lib -L/opt/vc/lib -L../core/libs/vgfont/  \
	      -lshapes -lwiringPi -lm $(SQL_LIB)
OPENCV_LIBS = -lopencv_video -lopencv_videoio -lopencv_imgcodecs -lopencv_ts -lopencv_face \
	-lopencv_objdetect -l opencv_ml -lopencv_calib3d -lopencv_imgproc -lopencv_highgui \
	-lopencv_core -lopencv_plot

# MAIN DIRECTORY OBJECTS:
OBJ     = mouse.o x_eyes_main.o plot.o \
	      cal_screen.o key_config.o 
NLP_OBJ = nlp_thread.o  nlp_vision_general_protocol.o nlp_vision_thing_protocol.o color_protocol.o \
		  qualitative_time.o nlp_sentence.o tk_colors.o serverthread.o
CONTROL_OBJS = serial.o serial_synchronous.o roboclaw.o x_eyes.o neck_thread.o
VISION_OBJS  = findEyeCenter.o findEyeCorner.o face_detect.o face_recog.o face_summary.o \
			   misc_detect.o  helpers.o 
CORE_VISION_OBJS = optical_flow.o image_proc_misc.o frame_grabber.o neighborhood_tricks.o
#				depth_plane_detect.o 

DBASE_OBJS    = viki_logger.o vision_logger.o
MISC_OBJS     = super_string.o
IPC_OBJS      = vision_memory.o
INSTANT_OBJS  = 
KINETIC_OBJS  = vector.o

%.o: %.cpp $(DEPS)
	$(CC) $(ALL_INCLUSIVE) -c -o $@ $< -g $(CFLAGS) -std=gnu++11
%.o: %.c $(DEPS)
	$(CC) $(ALL_INCLUSIVE) -c -o $@ $< $(OPTIONS) -g $(CFLAGS)

xeyes: $(OBJ) \
	$(addprefix ./nlp/,     $(NLP_OBJ)) 	 \
	$(addprefix ./control/, $(CONTROL_OBJS)) \
	$(addprefix ./vision/, $(VISION_OBJS))   \
	$(addprefix ../core/vision/, $(CORE_VISION_OBJS)) \
    $(addprefix ../core/mysql/, $(DBASE_OBJS)) \
	$(addprefix ../core/nlp/, $(MISC_OBJS)) \
	$(addprefix ../core/ipc/,  $(IPC_OBJS))	 \
	$(addprefix ../abkInstant/threads/, $(INSTANT_OBJS)) \
	$(addprefix ../core/kinetic/, $(KINETIC_OBJS))
	g++ -o $@ $^ $(CFLAGS) $(LIBFLAGS) $(LDFLAGS) $(OPENCV_LIBS)

clean:
	rm ./nlp/*.o
	rm ./control/*.o
	rm ./vision/*.o	
	rm ../core/dbase/*.o
	rm ../core/misc/*.o	
	rm ../core/ipc/*.o
	rm ../core/kinetic/*.o
	rm -rf *.o $(PRG).elf *.eps *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
