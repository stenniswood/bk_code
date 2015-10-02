RPI = 1
linux_desktop = 2
Mac = 3

PLATFORM  = $(RPI)
#PLATFORM = $(linux_desktop)
#PLATFORM  = $(Mac)

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
INC		   =  ../core/can/ ../core/msg/ ../core/fusion/ ../core/misc/ ../core/ipc/ ../core/wifi/ \
			  ./nlp_protocol/ ../nlp_protocol/  ./nlp_adrenaline/ ./voice/ \
			  ./threads/ /home/pi/abkInstant/  ./hardware/ ../core/libs/ilclient/ \
			  /opt/vc/include/  /opt/vc/include/interface/vmcs_host/linux/ 	\
			  $(HARDWARE_PATH)
#			  /opt/vc/include/interface/vcos/pthreads/ 
SELF_INC   = ../../abkInstant/ ./ ../
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
			 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
LIBFLAGS   =   -lpthread  -lm
			


DEPS   	   = 
#CFLAGS    = -fpermissive -Wwrite-strings 
#CFLAGS    += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

CFLAGS+=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX \
		-D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
		-U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize \
		-pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST \
		-DUSE_VCHIQ_ARM -Wno-psabi  -DPLATFORM=$(PLATFORM) \
		-DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac)

#LDFLAGS    = -L/opt/vc/lib/ -lGLESv2 -lEGL -ljpeg -lkhrn_static -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt  

# CORE CODE: can_rxbuff.o CAN_Interface.o buttons.o leds.o can_instance.o can_txbuff.o
#VG_OBJS    =  libshapes.o oglinit.o
CAN_OBJS   =  can_eid.o bk_system_defs.o can_id_list.o 
MSG_OBJS   =  packer_lcd.o packer_motor.o packer.o parser_tilt.o 
MISC_OBJS  =  vector_math.o ini_file.o 
FUSE_OBJS  =  fuse_gyro.o fuse_ag.o fuse_log.o fuse_accel.o fuse_magnet.o
APP_CPP_OBJS	= instant_main.o string_util.o
IPC_OBJS    	= picamscan_memory.o sway_memory.o visual_memory.o audio_memory.o \
				CAN_memory.o client_memory.o CAN_memory.o
THREAD_OBJS 	= audio_thread.o video_thread.o can_thread.o file_thread.o \
			    file_tx_thread.o hmi_thread.o thread_control.o serverthread.o 
HARDWARE_OBJS = GPIO_device.o AUDIO_device.o IMAGE_device.o CAMERA_device.o 
ILCLIENT_OBJS = ilclient.o ilcore.o
WIFI_OBJS     = udp_transponder.o client.o

# APP CODE
NLP_PROTOCOL_OBJS = GENERAL_protocol.o	CAN_protocol.o	GPIO_protocol.o	\
	AUDIO_protocol.o	CAN_util.o			CAMERA_protocol.o \
	AUDIO_util.o		FILE_protocol.o		IMAGE_protocol.o	CAMERA_util.o	  \
	package_commands.o		IMAGE_util.o		HMI_protocol.o	  \
			

NLP_ADRENALINE_OBJS  =	 Adrenaline_nlp.o     	Adren_bigmotor_talk.o  		\
	Adren_lcd_talk.o     Adren_pwm_talk.o	 	Adren_analog_talk.o  		\
	Adren_button_talk.o  Adren_picamscan_talk.o Adren_tiltsensor_talk.o

ifeq ($(PLATFORM), $(Mac))
LIBFLAGS 	  += -L./hardware/mac/ -framework AudioToolbox -framework CoreFoundation
#HARDWARE_OBJS += AUDIO_util.o
endif
ifeq ($(PLATFORM), $(RPI))
HARDWARE_OBJS += CAN_device.o 
#LIB_ILCLIENT = $(addprefix ../core/libs/ilclient/, $(ILCLIENT_OBJS))	
LIBFLAGS 	+= -L../core/libs/ilclient -lopenmaxil -lvchiq_arm -lilclient -lvcos -lbcm_host -L./hardware/rpi/ -L/opt/vc/lib -lrt -L../core/libs/vgfont
endif
ifeq ($(PLATFORM), $(linux_desktop))
LIBFLAGS 	+= -L../core/libs/ilclient -L./hardware/rpi/ -L/opt/vc/lib -lrt
endif


%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

instant: $(APP_CPP_OBJS) 										\
		$(addprefix ./nlp_adrenaline/, 	$(NLP_ADRENALINE_OBJS)) \
		$(addprefix ./nlp_protocol/, 	$(NLP_PROTOCOL_OBJS)) 	\
		$(addprefix ./threads/, 	$(THREAD_OBJS)) 		\
		$(addprefix $(HARDWARE_PATH), 	$(HARDWARE_OBJS)) \
		$(addprefix ../core/ipc/, 	$(IPC_OBJS)) 			\
		$(addprefix ../core/can/, 	$(CAN_OBJS)) 			\
		$(addprefix ../core/msg/, 	$(MSG_OBJS)) 			\
		$(addprefix ../core/wifi/, 	$(WIFI_OBJS)) 			\
		$(addprefix ../core/misc/,	$(MISC_OBJS))
	g++ -o $@   $^  $(LIBFLAGS)  -rdynamic

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/ipc/*.o 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/wifi/*.o 	
	rm -rf ../core/misc/*.o
	rm -rf ../core/fusion/*.o
	rm -rf ./adrenaline/*.o 
	rm -rf ./hardware/*/*.o 	
	rm -rf ./threads/*.o 			
	rm -rf ./nlp_protocol/*.o 				
	rm -rf ./nlp_adrenaline/*.o 					
	rm -rf ./voice/*.o 	
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

