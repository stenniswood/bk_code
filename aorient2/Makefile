cc		  = g++
OPTIONS	  = -std=gnu99 -lm 
INC		  = ../core/can/ ../core/msg/ ../core/misc/ ../core/fusion/ ../core/ipc/ ./
SELF_INC  = ../../orient/ ./
CFLAGS 	  = -lwiringPi -lm -lrt -lstdc++
DEPS   	  = 
OBJ        = MahonyAHRS.o triangle.o segway_main.o board_list.o help.o sway_config.o app_tilt.o 
CAN_OBJS   = CAN_Interface.o can_instance.o can_eid.o can_rxbuff.o can_txbuff.o   \
	         bk_system_defs.o can_id_list.o system_msgs_callback.o 
MSG_OBJS   = packer_lcd.o packer.o parser_tilt.o packer_motor.o
MISC_OBJS  = leds.o vector_math.o timer.o preferences.o
FUSE_OBJS  = fuse_ag.o fuse_accel.o fuse_gyro.o fuse_magnet.o fuse_log.o catagorization.o 
IPC_OBJS   = sway_memory.o
CPPOBJ     = 

CFLAGS  +=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi 
LDFLAGS +=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont

##include ./Makefile.include

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(INCLUDES) -c -o $@ $< $(CFLAGS) -Wno-deprecated-declarations

%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(INCLUDES) -g -c -o $@ $< $(OPTIONS) $(CFLAGS) -fgnu89-inline -Wno-deprecated-declarations

#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o 

sway: $(OBJ)  $(addprefix   ../core/can/, $(CAN_OBJS)) 	 \
		$(addprefix ../core/misc/,$(MISC_OBJS))  	 \
		$(addprefix ../core/ipc/,$(IPC_OBJS))          \
		$(addprefix ../core/fusion/,$(FUSE_OBJS))  	 \
		$(addprefix ../core/msg/, $(MSG_OBJS)) 
	gcc -o   $@ $^ $(CFLAGS) $(LDFLAGS) 

%.a: $(OBJS)
	$(AR) r $@ $^

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)
