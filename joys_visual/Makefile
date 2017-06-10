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
CORE_INC = can/ fusion/ hardware/ kinetic/ mysql/ misc/ msg/ wifi/ ipc/ \
	     libs/vgfont/ libs/ilclient/ /adrenaline/include
PCORE_INC = $(addprefix ../core/, $(CORE_INC) )

#SQL_INC  = -I/usr/include/mysql
INC	      = $(PCORE_INC) $(APPS_INC) $(SQL_INC) ./

SELF_INC   = ../../joys_visual/ ./ ../
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
		 	 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg

# LIBRARY FLAGS :
SQL_LIB    = -L/usr/lib/arm-linux-gnueabihf -lmysqlclient -lpthread -lz -lm -lrt -ldl
LIBFLAGS   = -L../core/adrenaline/lib/ -L../core/libs/ilclient/ -L/opt/vc/lib -L../core/libs/vgfont/  \
			-ljpeg -lshapes -lwiringPi -lm $(SQL_LIB)
SQL_CFLAGS = -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing -g
CFLAGS     = -g -Wall -DPLATFORM=$(PLATFORM) -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) \
			 -DMac=$(Mac) $(SQL_CFLAGS)
LDFLAGS += -lilclient -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host \
			-lvcos -lvchiq_arm -lpthread -lrt -L./vgfont 

# OBJECT LISTS : 

# 
HARDWARE_OBJS= AUDIO_device.o sinewave.o tone_generator.o
FUSE_OBJS   = fuse_gyro.o fuse_ag.o fuse_log.o fuse_accel.o fuse_magnet.o 
VG_OBJS     = libshapes.o oglinit.o 
JOYS_VISUAL_OBJS= joys_main.o test_layouts.o window_layouts.o string_util.o \
			   home_screen.o
TEST_OBJS = test_layouts.o test_combo_layouts.o test_game_layouts.o test_graph_layouts.o 

GALLERY_APP_OBJS = gallery_app.o
FILE_EDIT_OBJS = file_editor_app.o 
FINDER_OBJS    = finder_app.o
VIDEO_APP_OBJS = video_app.o video.o 
DRAW_APP_OBJS  = draw_app2.o draw_canvas.o 
CAN_APP_OBJS   = can_app.o gyro_window.o analog_board_app.o filter_select.o msg_value_composer.o
AUDIO_APP_OBJS = audio_app.o audio_app_menu.o 
ROBOT_CONTROL_OBJS = robot_control_panel.o robot_app.o robot_diagnostics.o robot_performance.o \
			robot_vision.o
DBASE_OBJS    = viki_logger.o vision_logger.o machine_defs.o can_sql_logger.o sql_common.o \
			calendar_entry.o sql_users.o
#

#ipc_can_rxbuff.o 
IPC_OBJS   = CAN_memory.o client_memory.o visual_memory.o audio_memory.o ipc_can_rxbuff.o shm_base.o
ILCLIENT_OBJS = ilclient.o ilcore.o

REV_GAME_OBJS    	= reversi_ai_player.o reversi_board_memory.o reversi_board.o \
					  reversi_app.o
HEARTS_GAME_OBJS 	= hearts_game.o hearts_app.o trick.o 
BLACKJACK_GAME_OBJS = blackjack.o blackjack_app.o 
CARDCORE_GAME_OBJS  = card.o card_player.o card_player_chips.o deck.o basic_card_game.o

KINETIC_OBJS = balance.o vector_file.o

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

jvisual: $(JOYS_VISUAL_OBJS) ../core/adrenaline/lib/adrenaline.a \
		$(addprefix /home/pi/openvg/,		$(VG_OBJS))  			\
		$(addprefix ./, 					$(TEST_OBJS)) 			\
		$(addprefix ../core/misc/,			$(MISC_OBJS))  			\
		$(addprefix ../core/hardware/,		$(HARDWARE_OBJS))		\
		$(addprefix ../core/fusion/,		$(FUSE_OBJS))  			\
		$(addprefix ../core/msg/,			$(MSG_OBJS))  			\
		$(addprefix ../core/can/,			$(CAN_OBJS))			\
		$(addprefix ../core/ipc/,			$(IPC_OBJS))			\
	    $(addprefix ../core/mysql/, 		$(DBASE_OBJS))
	g++ -o $@ $^  $(LIBFLAGS)  $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.bak
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

