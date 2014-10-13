CC		   = g++
OPTIONS	   = -fpermissive -Wwrite-strings  
INC		   =  ../core/can/ ../core/msg/ ../core/fusion/ ../core/misc/  \
			  ./nlp_protocol/ ../nlp_protocol/  ./nlp_adrenaline/ ./voice/ \
			  ./threads/ /home/pi/abkInstant/  
SELF_INC   = ../../abkInstant/ ./ ../
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
			 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
LIBFLAGS   = -L/opt/vc/lib -lGLESv2 -lEGL -ljpeg -lshapes -lwiringPi -lm 
CFLAGS 	   =  
DEPS   	   = 

# CORE CODE:
CAN_OBJS   = CAN_Interface.o can_instance.o can_eid.o can_rxbuff.o can_txbuff.o \
	         bk_system_defs.o can_id_list.o 
MSG_OBJS   =  packer_lcd.o packer_motor.o packer.o parser_tilt.o 
MISC_OBJS  =  buttons.o leds.o vector_math.o ini_file.o 
FUSE_OBJS  =  fuse_gyro.o fuse_ag.o fuse_log.o fuse_accel.o fuse_magnet.o 
VG_OBJS    =  libshapes.o oglinit.o 
APP_CPP_OBJS= instant_main.o string_util.o 
IPC_OBJS    = picamscan_memory.o sway_memory.o visual_memory.o
THREAD_OBJS = audio_thread.o video_thread.o can_thread.o file_thread.o \
			file_tx_thread.o hmi_thread.o thread_control.o serverthread.o 

# APP CODE
NLP_PROTOCOL_OBJS  		= udp_transponder.o \
	AUDIO_device.o		CAN_protocol.o		GPIO_protocol.o		CAMERA_device.o	 \
	AUDIO_protocol.o	CAN_util.o			IMAGE_device.o		CAMERA_protocol.o \
	AUDIO_util.o		FILE_protocol.o		IMAGE_protocol.o	CAMERA_util.o	 \
	CAN_device.o	package_commands.o		IMAGE_util.o		HMI_protocol.o	\
	client.o		GPIO_device.o			GENERAL_protocol.o	

NLP_ADRENALINE_OBJS  =	 Adrenaline_nlp.o     	Adren_bigmotor_talk.o  		\
	Adren_lcd_talk.o     Adren_pwm_talk.o	 	Adren_analog_talk.o  		\
	Adren_button_talk.o  Adren_picamscan_talk.o Adren_tiltsensor_talk.o

%.o: %.cpp $(DEPS)
	$(CC) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS) 
%.o: %.c $(DEPS) 
	$(CC) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)

instant: $(APP_CPP_OBJS) 									\
		$(addprefix ./nlp_adrenaline/, 	$(NLP_ADRENALINE_OBJS)) \
		$(addprefix ./nlp_protocol/, 	$(NLP_PROTOCOL_OBJS)) 	\
		$(addprefix ./threads/, 		$(THREAD_OBJS)) 		\
		$(addprefix ../core/misc/, 		$(IPC_OBJS)) 		\
		$(addprefix ../core/can/, 		$(CAN_OBJS)) 		\
		$(addprefix ../core/msg/, 		$(MSG_OBJS)) 		\
		$(addprefix ../core/misc/,		$(MISC_OBJS))  		\
		$(addprefix /home/pi/openvg/,	$(VG_OBJS))  		
	g++ -o $@ $^ $(CFLAGS)  $(LIBFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf ../core/fusion/*.o 	
	rm -rf ./adrenaline/*.o 
	rm -rf ./hardware/*.o 	
	rm -rf ./threads/*.o 			
	rm -rf ./nlp_protocol/*.o 				
	rm -rf ./nlp_adrenaline/*.o 					
	rm -rf ./voice/*.o 	
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

#../core/can/%.o:%.c $(DEPS)
#	$(CC)  $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $<  $(OPTIONS) -std=c99 $(CFLAGS)
#../core/misc/%.o:%.c $(DEPS)
#	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $<  $(OPTIONS) $(CFLAGS)
