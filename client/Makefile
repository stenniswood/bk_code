RPI   = 1
linux_desktop = 2
Mac   = 3
PLATFORM = $(RPI)

ifeq ($(PLATFORM), $(RPI))
HARDWARE_PATH = ./hardware/rpi/
endif
ifeq ($(PLATFORM), $(linux_desktop))
HARDWARE_PATH = ./hardware/linux_desktop/
endif
ifeq ($(PLATFORM), $(Mac))
HARDWARE_PATH = ./hardware/mac/
endif

CC		   = g++
OPTIONS	   = -fpermissive -Wwrite-strings -Wno-deprecated -w
INC		   =  ../core/can/ ../core/msg/ ../core/fusion/ ../core/misc/ ../core/ipc/ ../core/kinetic \
			   ../core/libs/ilclient/  

SELF_INC   = ../../abkInstant/ ./ ../
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
			 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
LIBFLAGS   = -L../core/libs/vgfont -lpthread -lm

# -L../core/libs/ilclient -lilclient 
			 
DEPS   	   = 
#CFLAGS 	   = -fpermissive -Wwrite-strings 
#CFLAGS    += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

CFLAGS+=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX \
		-D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
		-U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize \
		-pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -g \
		-DUSE_VCHIQ_ARM -Wno-psabi  -DRPI=$(RPI) -DMac=$(Mac) -Dlinux_desktop=$(linux_desktop) -DPLATFORM=$(PLATFORM)

#LDFLAGS    = -L/opt/vc/lib/ -lGLESv2 -lEGL -ljpeg -lkhrn_static -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt  

# CORE CODE:
CAN_OBJS   =  CAN_Interface.o can_instance.o can_eid.o can_rxbuff.o can_txbuff.o \
	          bk_system_defs.o can_id_list.o 
MSG_OBJS   =  packer_lcd.o packer_motor.o packer.o parser_tilt.o 
MISC_OBJS  =  buttons.o leds.o vector_math.o ini_file.o
FUSE_OBJS  =  fuse_gyro.o fuse_ag.o fuse_log.o fuse_accel.o fuse_magnet.o
VG_OBJS    =  libshapes.o oglinit.o
APP_CPP_OBJS= client_main.o client.o package_commands.o 
#picamscan_memory.o 
IPC_OBJS    = client_memory.o vision_memory.o
THREAD_OBJS = audio_thread.o video_thread.o can_thread.o file_thread.o \
			  file_tx_thread.o hmi_thread.o thread_control.o serverthread.o 
HARDWARE_OBJS = AUDIO_device.o 
ILCLIENT_OBJS = ilclient.o ilcore.o

# APP CODE
NLP_PROTOCOL_OBJS  		= udp_transponder.o GENERAL_protocol.o	\
	AUDIO_device.o		CAN_protocol.o		GPIO_protocol.o		CAMERA_device.o	  \
	AUDIO_protocol.o	CAN_util.o			IMAGE_device.o		CAMERA_protocol.o \
	AUDIO_util.o		FILE_protocol.o		IMAGE_protocol.o	CAMERA_util.o	  \
	CAN_device.o	package_commands.o		IMAGE_util.o		HMI_protocol.o	  \
	client.o		GPIO_device.o

NLP_ADRENALINE_OBJS  =	 Adrenaline_nlp.o     	Adren_bigmotor_talk.o  		\
	Adren_lcd_talk.o     Adren_pwm_talk.o	 	Adren_analog_talk.o  		\
	Adren_button_talk.o  Adren_picamscan_talk.o Adren_tiltsensor_talk.o

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

client: $(APP_CPP_OBJS) 										\
		$(addprefix ../core/ipc/, $(IPC_OBJS))
	g++ -o $@   $^  $(LIBFLAGS)  -rdynamic

#-Wl,--whole-archive  -Wl,--no-whole-archive

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf ../core/ipc/*.o 
	rm -rf ../core/fusion/*.o 	
	rm -rf ./adrenaline/*.o 
	rm -rf ./hardware/*.o 	
	rm -rf ./threads/*.o 			
	rm -rf ./nlp_protocol/*.o 				
	rm -rf ./nlp_adrenaline/*.o 					
	rm -rf ./voice/*.o 	
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)


#  /opt/vc/include/  /opt/vc/include/interface/vmcs_host/linux/
# -lrt -lshapes  -lvcfiled_check  -lvmcs_rpc_client -lvchostif
#-lGLESv2	-lEGL -ljpeg \ -L/opt/vc/lib -lopenmaxil -lwiringPi  -lEGL_static -lbcm_host
#  -lvcos -lvchiq_arm
