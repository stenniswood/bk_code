CC=g++
PROG=asynth
OPTIONS	= -std=c++11 -lm -fno-rtti 
INC	= ./visual/ /usr/include/freetype2
SELF_INC= ./
CFLAGS 	= -lm -g
DEPS   	= 

MIDI_OBJ=  midi_synth.o sound_patch.o
OBJ     =  main.o dsp_funcs.o read_pendant.o alsa_prep.o dspwave.o wave.o sound_playback.o resample.o \
		generate.o alsa_record.o beat_detect.o beat_gen.o histo_bin.o

GTK_OBJ   = gtk/peak_pick.o gtk/annotated_graph.o gtk/graph.o gtk/linegraph.o gtk/histogram.o gtk/axis.o gtk/dataseries.o gtk/gtk_main.o

VISUAL_OBJ = visual/gl_graph.o visual/shader_utils.o visual/gl_setup.o
CPPOBJ     = 
LDFLAGS    = -lpthread -lrt -lasound -lcurl -lglut -lGLEW -lGL -lfreetype -L./midi/ -lmidifile -lfftw3

LIBS = `pkg-config --libs gtk+-3.0`
CFLAGS = `pkg-config --cflags gtk+-3.0`

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(addprefix -I,$(SELF_INC)) -c -o $@ $< $(OPTIONS) $(CFLAGS)
	
$(PROG): $(OBJ) $(MIDI_OBJ) $(GTK_OBJ)
	$(CC) -o  $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

clean:
	rm -rf *.o gtk/*.o *.bak 

