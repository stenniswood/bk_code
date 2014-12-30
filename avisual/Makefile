CC		   = g++
OPTIONS	   = -fpermissive -Wwrite-strings 
INC		   = ../core/can/  ../core/fusion/ ../core/hardware/ ../core/kinetic 			\
			 ../core/misc/ ../core/msg/    ../core/wifi/     ../core/libs/ilclient/  	\
			 ../core/libs/vgfont/  ./voice/ ./kinetic/ ./ ../core/adrenaline/graphs/ 	\
			 ./games/ ./games/blackjack/ ./games/reversi/ ./games/card_core/ ./games/hearts/ \
			 ../core/adrenaline/ ../core/adrenaline/atoms/ ../core/adrenaline/molecules/

SELF_INC   = ../../avisual/ ./ ../
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
		 	 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
LIBFLAGS   = -L../core/libs/ilclient/ -L/opt/vc/lib -L../core/libs/vgfont/  \
			-ljpeg -lshapes -lwiringPi -lm 

#CFLAGS +=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX \
#		-D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
#		-U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize \
#		-pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST \
#		-DUSE_VCHIQ_ARM -Wno-psabi 

LDFLAGS += -lilclient -L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host \
			-lvcos -lvchiq_arm -lpthread -lrt -L./vgfont 

#-lilclient -lvgfont 
DEPS   	   = 

# CORE CODE:
CAN_OBJS   = can_id_list.o 
#CAN_Interface.o can_instance.o can_eid.o can_buff.o can_txbuff.o bk_system_defs.o 
MSG_OBJS   = packer_lcd.o packer_motor.o packer.o parser_tilt.o 
MISC_OBJS  = vector_math.o ini_file.o visual_memory.o audio_memory.o fft.o
HARDWARE_OBJS  = AUDIO_device.o sinewave.o tone_generator.o
FUSE_OBJS  = fuse_gyro.o fuse_ag.o fuse_log.o fuse_accel.o fuse_magnet.o 
VG_OBJS    = libshapes.o oglinit.o 
APP_CPP_OBJS= visual_main.o window_layouts.o callbacks.o string_util.o \
			  ipc_mem_bkinstant.o audio_app.o test_layouts.o test_combo_layouts.o \
			  test_game_layouts.o test_graph_layouts.o avisual_menu.o draw_app.o  \
			  power.o audio_app_menu.o 
IPC_OBJS   = 
ILCLIENT_OBJS = ilclient.o ilcore.o

REV_GAME_OBJS    	= reversi_ai_player.o reversi_board_memory.o reversi_board.o
HEARTS_GAME_OBJS 	= hearts_game.o trick.o 
BLACKJACK_GAME_OBJS = blackjack.o 
CARDCORE_GAME_OBJS  = card.o card_player.o deck.o basic_card_game.o

#  button_array_gui.o Container.o 
ADRENALINE_OBJS = display.o display_manager.o mouse.o resource_manager.o task_bar.o system_status.o 
ADRENALINE_ATOMS_OBJS = bitmap.o button.o checkbox.o control.o         \
			directory_listbox.o frame_window.o groupbox.o icon.o       \
			leveler.o line_grouper.o listbox.o marquee.o power_level.o \
			progressbar.o radiobutton.o rectangle.o scroll_bar.o scroll_control.o \
			stick_leg.o tab_control.o table_window.o tabular_listbox.o \
			TextFile.o text_view.o tk_colors.o wave_view.o window.o    \
			horizontal_menu.o vertical_menu.o spinner.o can_msg_view.o 
ADRENALINE_MOLECULES_OBJS = adren_network_view.o file_browser.o  	  \
			frequency_view.o image_gallery.o list_status.o side_bar.o \
			stereo_power.o stick_figure.o system_bar.o can_app.o 
ADRENALINE_GRAPH_OBJS =  Graphbase.o dataset.o line_graph.o histogram.o \
			 bar_graph.o bar_graph_stacked.o scatter_plot.o
KINETIC_OBJS = 

# APP CODE
VOICE_OBJS 	= 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

avisual: $(APP_CPP_OBJS) 					\
		$(addprefix ./voice/, 				$(VOICE_OBJS)) 			\
		$(addprefix ./kinetic/, 			$(KINETIC_OBJS)) 		\
		$(addprefix ./games/reversi/, 		$(REV_GAME_OBJS)) 		\
		$(addprefix ./games/hearts/, 		$(HEARTS_GAME_OBJS)) 	\
		$(addprefix ./games/, 				$(GAME_OBJS)) 			\
		$(addprefix ./games/blackjack/, 	$(BLACKJACK_GAME_OBJS)) \
		$(addprefix ./games/card_core/, 	$(CARDCORE_GAME_OBJS)) 	\
		$(addprefix ../core/misc/,			$(MISC_OBJS))  			\
		$(addprefix /home/pi/openvg/,		$(VG_OBJS))  			\
		$(addprefix ../core/hardware/,			$(HARDWARE_OBJS))	\
		$(addprefix ../core/adrenaline/,	$(ADRENALINE_OBJS)) 	\
		$(addprefix ../core/adrenaline/atoms/,	   $(ADRENALINE_ATOMS_OBJS)) 	 \
		$(addprefix ../core/adrenaline/molecules/, $(ADRENALINE_MOLECULES_OBJS)) \
		$(addprefix ../core/adrenaline/graphs/,	   $(ADRENALINE_GRAPH_OBJS))	 \
		$(addprefix ../core/can/,			$(CAN_OBJS))
	g++ -o $@ $^  $(LIBFLAGS)  $(LDFLAGS)


clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf ../core/fusion/*.o 	
	rm -rf ../core/adrenaline/*.o 
	rm -rf ../core/adrenaline/atoms/*.o 	
	rm -rf ../core/adrenaline/molecules/*.o
	rm -rf ../core/adrenaline/graphs/*.o 
	rm -rf ./hardware/*.o 	
	rm -rf ./wifi/*.o 		
	rm -rf ./voice/*.o 	
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

