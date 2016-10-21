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
OPTIONS	   = -std=c99 -g -fpermissive -Wwrite-strings -Wno-deprecated -w

INC_CORE   = ../core/can/ ../core/msg/ ../core/fusion/ ../core/misc/ ../core/ipc/   \
			 ../core/wifi/ ../core/libs/ilclient/ ../core/kinetic/ ../core/hardware/ \
			 ../core/nlp/ ../core/adrenaline/molecules/calendar/
LOCAL_INC  =  ./threads/ ./nlp_protocol/ ./nlp_adrenaline/ ./voice/ ./util/ ./nlp_core/ $(HARDWARE_PATH) \
SYSTEM_INC =  /home/pi/raspicam/interface \
			 /opt/vc/include/  /opt/vc/include/interface/vmcs_host/linux/
SELF_INC   = /home/pi/bk_code/abkInstant/
MYSQL_INC    = /usr/include/mysql /usr/include/
INC 	   = $(INC_CORE) $(LOCAL_INC) $(SYSTEM_INC) $(SELF_INC) $(MYSQL_INC)
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
		     -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
SQL_LIB = -L/usr/lib/arm-linux-gnueabihf -lmysqlclient -lz -ldl
LIBFLAGS   = -lpthread  -lm $(SQL_LIB)

#			  /opt/vc/include/interface/vcos/pthreads/
#CFLAGS    =  -fpermissive -Wwrite-strings 
#CFLAGS    += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi
# CORE CODE:  can_rxbuff.o leds.o can_instance.o can_txbuff.o
#VG_OBJS   =  libshapes.o oglinit.o

CFLAGS+=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX \
		-D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
		-U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize \
		-pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST \
		-DUSE_VCHIQ_ARM -Wno-psabi  -DPLATFORM=$(PLATFORM) \
		-DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac) -std=c++11


KINETIC_OBJS = vector.o
CAN_OBJS   =  can_eid.o bk_system_defs.o can_id_list.o
MSG_OBJS   =  packer_lcd.o packer_motor.o packer.o
#parser_tilt.o
MISC_OBJS  =  vector_math.o ini_file.o super_string.o string_util.o
FUSE_OBJS  =  fuse_gyro.o fuse_ag.o fuse_log.o fuse_accel.o fuse_magnet.o
APP_CPP_OBJS	= instant_main.o 
IPC_OBJS    	= picamscan_memory.o sway_memory.o vision_memory.o visual_memory.o audio_memory.o \
		  		  CAN_memory.o client_memory.o simulator_memory.o sequencer_memory.o  
THREAD_OBJS 	= video_thread.o can_thread.o file_thread.o listen_thread.o \
				  file_tx_thread.o hmi_thread.o thread_control.o
HARDWARE_OBJS = GPIO_device.o AUDIO_device.o IMAGE_device.o CAMERA_device.o
ILCLIENT_OBJS = ilclient.o ilcore.o
WIFI_OBJS     = udp_transponder.o
#client.o
VIDEO_OBJS    = vc_vchi_gencmd.c vc_vchi_filesys.c vc_vchi_dispmanx.c vc_vchi_cecservice.c \
				vc_service_common.c vcilcs_out.c vcilcs_in.c vcilcs_common.c vcilcs.c
CALENDAR_OBJS = calendar_entry.o 
# APP CODE calendar.o calendar_summary.o 
UTIL_OBJS = tk_colors.o client_to_socket.o AUDIO_file_util.o CAMERA_util.o CAN_util.o IMAGE_file_util.o VIDEO_file_util.o

NLP_CORE_OBJS = nlp_extraction.o nlp_sentence.o alias.o serverthread.o
#prefilter.o 
#ordering_protocol.o CAN_util.o IMAGE_file_util.o 
#othello_ai_player.o
#HARDWARE_OBJS += AUDIO_util.o
#RaspiCamControl.o RaspiCLI.o RaspiPreview.o CAMERA_device_mmal.o 
#LIB_ILCLIENT = $(addprefix ../core/libs/ilclient/, $(ILCLIENT_OBJS))	
#LDFLAGS    = -L/opt/vc/lib/ -lGLESv2 -lEGL -ljpeg -lkhrn_static -lopenmaxil -lbcm_host  -lpthread -lrt  
# Following is for video on RPI
#-lvmcs_rpc_client rectangle.o 

NLP_PROTOCOL_OBJS = adrenaline_creator.o color_protocol.o IMAGE_protocol.o parameter.o SEQUENCER_protocol.o \
 adrenaline_protocol.o easter_eggs_protocol.o jets_pizza.o positioner_2d.o specific_restaurants.o \
 AUDIO_interface.o family_relationships.o  positioner_3d.o sql_people.o \
 AUDIO_protocol.o FILE_protocol.o math_protocol.o qualitative_time.o system_configuration.o \
 calendar_protocol.o GENERAL_protocol.o menu_items.o question_to_user.o ThreeD_object_protocol.o \
 CAMERA_protocol.o GPIO_protocol.o menus_orders.o \
 CAN_protocol.o hearts_game.o robot_arms_protocol.o \
 HMI_protocol.o  robot_legs_protocol.o \
 card_player.o othello_board.o self_identity.o
NLP_ADRENALINE_OBJS  =	 Adrenaline_nlp.o     	Adren_bigmotor_talk.o  		\
	Adren_lcd_talk.o     Adren_pwm_talk.o	 Adren_analog_talk.o  		\
	Adren_button_talk.o  Adren_picamscan_talk.o Adren_tiltsensor_talk.o

ifeq ($(PLATFORM), $(Mac))
LIBFLAGS 	  += -L./hardware/mac/ -framework AudioToolbox -framework CoreFoundation

endif
ifeq ($(PLATFORM), $(RPI))
HARDWARE_OBJS += CAN_device.o 
LIBFLAGS 	+= -L../core/libs/ilclient -L./hardware/rpi/ -L../core/libs/vgfont \
				-L/opt/vc/lib -lopenmaxil -lvchiq_arm -lilclient -lbcm_host -lvcos -lrt
endif
ifeq ($(PLATFORM), $(linux_desktop))
LIBFLAGS 	+= -L../core/libs/ilclient -L./hardware/rpi/ -L/opt/vc/lib -lrt
endif


ifeq ($(PLATFORM), $(RPI))
LDFLAGS = -lvchostif -lmmal -lmmal_core -lmmal_util -lvcos -lvcsm -lvcilcs -lvchiq_arm -lvcfiled_check  
endif

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

instant: $(APP_CPP_OBJS) 						\
		$(addprefix ../core/nlp/, 	$(NLP_CORE_OBJS)) 	\
		$(addprefix ./threads/, 	$(THREAD_OBJS)) 	\
		$(addprefix ../core/adrenaline/molecules/calendar/, $(CALENDAR_OBJS)) 	\
		$(addprefix ./nlp_protocol/, $(NLP_PROTOCOL_OBJS)) 	\
		$(addprefix ./util/, 		$(UTIL_OBJS)) 		\
		$(addprefix $(HARDWARE_PATH), 	$(HARDWARE_OBJS)) 	\
		$(addprefix ../core/ipc/, 	$(IPC_OBJS)) 		\
		$(addprefix ../core/kinetic/, 	$(KINETIC_OBJS)) 	\
		$(addprefix ../core/can/, 	$(CAN_OBJS)) 		\
		$(addprefix ../core/msg/, 	$(MSG_OBJS)) 		\
		$(addprefix ../core/wifi/, 	$(WIFI_OBJS)) 		\
		$(addprefix ../core/misc/,	$(MISC_OBJS))		\
		../core/hardware/tone_generator.o
	g++  -o $@ $^ $(LIBFLAGS) $(LDFLAGS) -rdynamic

#		//$(addprefix /home/pi/raspicam/interface/vmcs_host/, $(VIDEO_OBJS))	\

clean:
	rm -rf *.o $(PRG).elf *.eps  *.bak 
	rm -rf ../core/*/*.o 
	rm -rf ../core/ipc/*.o 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/wifi/*.o 	
	rm -rf ../core/misc/*.o
	rm -rf ../core/fusion/*.o
	rm -rf ./adrenaline/*.o 
	rm -rf ./adrenaline/*/*.o 
	rm -rf ./adrenaline/*/*/*.o 	
	rm -rf ./hardware/*/*.o 	
	rm -rf ./threads/*.o 			
	rm -rf ./nlp_protocol/*.o 				
	rm -rf ./nlp_adrenaline/*.o 					
	rm -rf ./nlp_core/*.o	
	rm -rf ./voice/*.o 	
	rm -rf ./util/*.o
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

