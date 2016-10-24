CC	   = g++
OPTIONS	   = -fpermissive -Wwrite-strings 
INC	  = ./lib/  

SELF_INC   = ./ ../
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
		 	 -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg


#CFLAGS +=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX \
#		-D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
#		-U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize \
#		-pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST \
#		-DUSE_VCHIQ_ARM -Wno-psabi 

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT 
else
	LIBS = -lglut
endif
LIBS +=  -lpthread -lm -lglut -lGL -L/usr/local/lib/ -L./lib -lfreenect -L/usr/local/lib/ \
	-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_calib3d \
	-lopencv_ml -lopencv_objdetect -lopencv_ts -lopencv_imgcodecs
DEPS   = 

# CORE CODE:
BKVIEW_OBJS  = bkview.o line_segmenter.o rectangular.o 

# APP CODE
VOICE_OBJS 	= 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 
%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) $(VG_INCLUDE) $(addprefix -I,$(INC)) -c -o $@ $< $(OPTIONS) 

bkview: $(BKVIEW_OBJS)
	g++ -o $@ $^  $(LIBS)  $(LDFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

