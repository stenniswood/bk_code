RPI = 1
linux_desktop = 2
Mac = 3

PLATFORM  = $(RPI)
#PLATFORM = $(linux_desktop)
#PLATFORM  = $(Mac)

ifeq ($(PLATFORM), $(RPI))
HOME_PATH = /home/pi/     
endif
ifeq ($(PLATFORM), $(linux_desktop))
HOME_PATH = ~/                
endif
ifeq ($(PLATFORM), $(Mac))
HOME_PATH = /Users/stephentenniswood/
endif

CC		   = g++
OPTIONS	   = -Wwrite-strings -fpermissive -Wreturn-type -Wformat

# INCLUDE PATHS:
CORE_INC = can/ fusion/ hardware/ kinetic/ system/ misc/ msg/ wifi/ ipc/ \
	     libs/vgfont/ libs/ilclient/ /adrenaline/include
PCORE_INC = $(addprefix ../core/, $(CORE_INC) )

#SQL_INC  = -I/usr/include/mysql
INC	      = $(PCORE_INC) $(APPS_INC) $(SQL_INC) ./

SELF_INC   = ../../joys_visual/ ./ ../
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
		 	 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg

# LIBRARY FLAGS :
LIBPATHS   = -L/opt/vc/lib -L/usr/lib/arm-linux-gnueabihf -L../core/adrenaline/lib/ -L../core/libs/ilclient/ \
			 -L../core/libs/vgfont/
LIBS 	   = -lilclient -lpthread -lz -lrt -ldl -ljpeg -lshapes -lm 
LIBFLAGS   = $(LIBPATHS) $(LIBS)

CFLAGS     = -g -Wall -DPLATFORM=$(PLATFORM) -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) \
			 -DMac=$(Mac)
LDFLAGS +=   -lEGL -lGLESv2 -lbcm_host -lopenmaxil -lvcos -lvchiq_arm

# OBJECT LISTS : 
# home_screen.o
JOYS_VISUAL_OBJS= joys_main.o test_layouts.o 
#string_util.o test_combo_layouts.o test_game_layouts.o test_graph_layouts.o 

VG_OBJS       = oglinit.o libshapes.o 
TEST_OBJS 	  = test_layouts.o 
ILCLIENT_OBJS = ilclient.o ilcore.o
HARDWARE_OBJS = AUDIO_device.o sinewave.o tone_generator.o
ROBOT_CONTROL_OBJS = robot_control_panel.o robot_app.o robot_diagnostics.o robot_performance.o \
			robot_vision.o
IPC_OBJS   = CAN_memory.o client_memory.o visual_memory.o audio_memory.o ipc_can_rxbuff.o shm_base.o

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

#		$(addprefix /home/pi/openvg/,	$(VG_OBJS))  

visual_hello: $(JOYS_VISUAL_OBJS) \
		$(addprefix /home/pi/openvg/,	$(VG_OBJS))  \
		../core/adrenaline/lib/adrenaline.a 	\
		$(addprefix ./, 					$(TEST_OBJS)) 			\
		$(addprefix ../core/hardware/,		$(HARDWARE_OBJS))		\
		$(addprefix ../core/ipc/,			$(IPC_OBJS))
	g++ -o $@ $^  $(LIBFLAGS)  $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.bak
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

