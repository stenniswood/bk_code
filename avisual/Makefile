CC		   = g++
OPTIONS	   = -fpermissive -Wwrite-strings  
INC		   = ../core/can/ ../core/msg/ ../core/fusion/ ../core/misc/ \
			 ../core/adrenaline/ ../core/adrenaline/graphs/ ./wifi/ ./hardware/  \
			 ./voice/ ./kinetic/ ./
SELF_INC   = ../../avisual/ ./ ../
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
			 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
LIBFLAGS   = -L/opt/vc/lib -lGLESv2 -lEGL -ljpeg -lshapes -lwiringPi -lm -lshapes
CFLAGS 	   = 
DEPS   	   = 

# CORE CODE:
CAN_OBJS   = CAN_Interface.o can_instance.o can_eid.o can_buff.o can_txbuff.o \
	         bk_system_defs.o can_id_list.o 
MSG_OBJS   = packer_lcd.o packer_motor.o packer.o parser_tilt.o 
MISC_OBJS  = vector_math.o ini_file.o visual_memory.o
FUSE_OBJS  = fuse_gyro.o fuse_ag.o fuse_log.o fuse_accel.o fuse_magnet.o 
VG_OBJS    = libshapes.o oglinit.o 
APP_CPP_OBJS= visual_main.o window_layouts.o callbacks.o string_util.o \
			  ipc_mem_bkinstant.o test_layouts.o
IPC_OBJS   = 

#  button_array_gui.o Container.o
ADRENALINE_OBJS = 
ADRENALINE_CPP_OBJS = control.o  \
			leveler.o button.o checkbox.o progressbar.o radiobutton.o  	\
			listbox.o tabular_listbox.o directory_listbox.o file_browser.o  \
			text_view.o icon.o scroll_bar.o scroll_control.o window.o  	\
			side_bar.o tab_control.o table_window.o frame_window.o     	\
			display_manager.o display.o mouse.o image_gallery.o power_level.o \
			stereo_power.o

ADRENALINE_GRAPH_OBJS =  Graphbase.o dataset.o line_graph.o histogram.o 		\
			 bar_graph.o bar_graph_stacked.o scatter_plot.o
KINETIC_OBJS = 

# APP CODE
VOICE_OBJS 	= 

%.o: %.cpp $(DEPS)
	$(CC) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS) 
%.o: %.c $(DEPS) 
	$(CC) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)

avisual: $(APP_CPP_OBJS) 					\
		$(addprefix ./voice/, 				$(VOICE_OBJS)) 			\
		$(addprefix ./kinetic/, 			$(KINETIC_OBJS)) 		\
		$(addprefix ../core/misc/,			$(MISC_OBJS))  			\
		$(addprefix /home/pi/openvg/,		$(VG_OBJS))  			\
		$(addprefix ../core/adrenaline/,	$(ADRENALINE_OBJS)) 	\
		$(addprefix ../core/adrenaline/,	$(ADRENALINE_CPP_OBJS)) \
		$(addprefix ../core/adrenaline/graphs/,	$(ADRENALINE_GRAPH_OBJS))
	g++ -o $@ $^ $(CFLAGS)  $(LIBFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ../core/can/*.o 
	rm -rf ../core/msg/*.o 
	rm -rf ../core/misc/*.o 
	rm -rf ../core/fusion/*.o 	
	rm -rf ../core/adrenaline/*.o 
	rm -rf ../core/adrenaline/graphs/*.o 
	rm -rf ./hardware/*.o 	
	rm -rf ./wifi/*.o 		
	rm -rf ./voice/*.o 	
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

