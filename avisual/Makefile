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
OPTIONS	   =  -Wwrite-strings -fpermissive -Wreturn-type
# 
# INCLUDE PATHS:
CORE_INC   = ../core/can/  ../core/fusion/ ../core/hardware/ ../core/kinetic 	\
	 		 ../core/misc/ ../core/msg/    ../core/wifi/     ../core/ipc/    	\
	 		 ../core/libs/vgfont/ ../core/adrenaline/graphs/ ../core/libs/ilclient/ \
			 ../core/adrenaline/ ../core/adrenaline/atoms/   ../core/adrenaline/molecules/ \
			 ../core/adrenaline/molecules/calendar/ ../core/adrenaline/system/
APPS_INC   = ./apps/ ./apps/CAN/ ./apps/audio/ ./apps/robot_control ./apps/draw/
GAMES_INC  = ./games/ ./games/blackjack/ ./games/reversi/ ./games/card_core/ ./games/hearts/
INC		= $(CORE_INC) $(APPS_INC) $(GAMES_INC) ./voice/ ./kinetic/ ./
SELF_INC   = ../../avisual/ ./ ../ 
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
		 	 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
#$(info $$INC is [${INC}])

# LIBRARY FLAGS :
LIBFLAGS   = -L../core/libs/ilclient/ -L/opt/vc/lib -L../core/libs/vgfont/  \
			-ljpeg -lshapes -lwiringPi -lm 
CFLAGS = -g -Wall -DPLATFORM=$(PLATFORM) -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac)
#CFLAGS +=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX \
#		-D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
#		-U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize \
#		-pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST \
#		-DUSE_VCHIQ_ARM -Wno-psabi 
#-lilclient -lvgfont 

LDFLAGS += -lilclient -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host \
			-lvcos -lvchiq_arm -lpthread -lrt -L./vgfont 
DEPS   	   = 

# OBJECT LISTS : 
CAN_OBJS   = can_id_list.o 
#CAN_Interface.o can_instance.o can_eid.o can_buff.o can_txbuff.o bk_system_defs.o 
#MSG_OBJS   = packer_lcd.o packer_motor.o packer.o parser_tilt.o 
# ipc_mem_bkinstant.o
MSG_OBJS   =  parser_tilt.o 
MISC_OBJS  = vector_math.o ini_file.o fft.o
HARDWARE_OBJS = AUDIO_device.o sinewave.o tone_generator.o
FUSE_OBJS  = fuse_gyro.o fuse_ag.o fuse_log.o fuse_accel.o fuse_magnet.o 
VG_OBJS    = libshapes.o oglinit.o 
AVISUAL_OBJS= visual_main.o window_layouts.o callbacks.o string_util.o \
			   test_layouts.o test_combo_layouts.o client_list_control.o \
			  test_game_layouts.o test_graph_layouts.o avisual_menu.o  \
			  power.o home_screen.o 
APPS_OBJS =  file_editor_app.o video_app.o video.o 
DRAW_APP_OBJS = draw_app.o draw_canvas.o 
CAN_APP_OBJS = can_app.o gyro_window.o analog_board_app.o filter_select.o msg_value_composer.o
AUDIO_APP_OBJS = audio_app.o audio_app_menu.o 
ROBOT_CONTROL_OBJS = robot_control_panel.o robot_app.o robot_diagnostics.o robot_performance.o \
			robot_vision.o

#ipc_can_rxbuff.o 
IPC_OBJS   = CAN_memory.o client_memory.o visual_memory.o audio_memory.o 
ILCLIENT_OBJS = ilclient.o ilcore.o

REV_GAME_OBJS    	= reversi_ai_player.o reversi_board_memory.o reversi_board.o
HEARTS_GAME_OBJS 	= hearts_game.o hearts_app.o trick.o 
BLACKJACK_GAME_OBJS = blackjack.o blackjack_app.o 
CARDCORE_GAME_OBJS  = card.o card_player.o card_player_chips.o deck.o basic_card_game.o

#  button_array_gui.o Container.o 
ADRENALINE_OBJS   = display.o task_bar.o
ADRENALINE_SYSTEM_OBJS = system_status.o keyboard.o touch_gesture.o keyboard_dev.o \
			display_manager.o mouse_oop.o resource_manager.o
ADRENALINE_ATOMS_OBJS = bitmap.o button.o checkbox.o control.o         \
			directory_listbox.o frame_window.o groupbox.o icon.o       \
			leveler.o line_grouper.o listbox.o marquee.o power_level.o \
			progressbar.o radiobutton.o rectangle.o scroll_bar.o scroll_control.o \
			stick_leg.o tab_control.o table_window.o tabular_listbox.o \
			TextFile.o text_view.o tk_colors.o wave_view.o window.o    \
			horizontal_menu.o vertical_menu.o spinner.o can_msg_view.o wave.o \
			edit_box.o drop_down_box.o listbox_icon.o
ADRENALINE_MOLECULES_OBJS = adren_network_view.o file_browser.o  	  \
			frequency_view.o image_gallery.o list_status.o side_bar.o \
			stereo_power.o stick_figure.o system_bar.o can_window.o   \
			application.o file_editor.o label_edit_box.o
ADRENALINE_SYSTEM_UI_OBJS = calendar.o calendar_summary.o

ADRENALINE_GRAPH_OBJS =  Graphbase.o dataset.o line_graph.o histogram.o \
			 bar_graph.o bar_graph_stacked.o scatter_plot.o 
#KINETIC_OBJS = 

# APP CODE
VOICE_OBJS 	= 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

avisual: $(AVISUAL_OBJS) 					\
		$(addprefix ./voice/, 				$(VOICE_OBJS)) 			\
		$(addprefix ./kinetic/, 			$(KINETIC_OBJS)) 		\
		$(addprefix ./games/reversi/, 		$(REV_GAME_OBJS)) 		\
		$(addprefix ./games/hearts/, 		$(HEARTS_GAME_OBJS)) 	\
		$(addprefix ./games/, 				$(GAME_OBJS)) 			\
		$(addprefix ./games/blackjack/, 	$(BLACKJACK_GAME_OBJS)) \
		$(addprefix ./games/card_core/, 	$(CARDCORE_GAME_OBJS)) 	\
		$(addprefix ../core/misc/,			$(MISC_OBJS))  			\
		$(addprefix /home/pi/openvg/,		$(VG_OBJS))  			\
		$(addprefix ../core/hardware/,		$(HARDWARE_OBJS))		\
		$(addprefix ../core/adrenaline/,	$(ADRENALINE_OBJS)) 	\
		$(addprefix ../core/adrenaline/system/,	   $(ADRENALINE_SYSTEM_OBJS)) 	 \
		$(addprefix ../core/adrenaline/atoms/,	   $(ADRENALINE_ATOMS_OBJS)) 	 \
		$(addprefix ../core/adrenaline/molecules/, $(ADRENALINE_MOLECULES_OBJS)) \
		$(addprefix ../core/adrenaline/molecules/calendar/, $(ADRENALINE_SYSTEM_UI_OBJS)) \
		$(addprefix ../core/adrenaline/graphs/,	   $(ADRENALINE_GRAPH_OBJS))	 \
		$(addprefix ../core/msg/,			$(MSG_OBJS))  			\
		$(addprefix ../core/can/,			$(CAN_OBJS))			\
		$(addprefix ../core/ipc/,			$(IPC_OBJS))			\
		$(addprefix ./apps/,				$(APPS_OBJS))			\
		$(addprefix ./apps/audio/,			$(AUDIO_APP_OBJS))		\
		$(addprefix ./apps/CAN/,			$(CAN_APP_OBJS))		\
		$(addprefix ./apps/draw/,			$(DRAW_APP_OBJS))		\
		$(addprefix ./apps/robot_control/,	$(ROBOT_CONTROL_OBJS))
	g++ -o $@ $^  $(LIBFLAGS)  $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.bak 
	rm -rf ../core/*/*.o 
	rm -rf ../core/adrenaline/*/*.o ../core/adrenaline/*/*/*.o
	rm -rf ./hardware/*.o
	rm -rf ./apps/*.o
	rm -rf ./wifi/*.o 		
	rm -rf ./voice/*.o 	
	rm -rf ./apps/*/*.o
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

clean_core:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o 
	rm -rf ../core/ipc/*.o 	
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf ../core/fusion/*.o 	
	rm -rf ../core/adrenaline/*.o 
	rm -rf ../core/adrenaline/atoms/*.o 	
	rm -rf ../core/adrenaline/molecules/*.o
	rm -rf ../core/adrenaline/graphs/*.o

# Leave core modules intact
clean_local:
	rm -rf *.o $(PRG).elf *.eps  *.bak
	rm -rf ./hardware/*.o
	rm -rf ./apps/*.o ./apps/CAN/*.o ./apps/audio/*.o ./apps/robot_control/*.o
	rm -rf ./wifi/*.o
	rm -rf ./voice/*.o
	rm -rf ./apps/*.o ./apps/CAN/*.o ./apps/audio/*.o ./apps/robot_control/*.o


