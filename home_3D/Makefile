RPI = 1
linux_desktop = 2
Mac = 3

PLATFORM  = $(RPI)
#PLATFORM = $(linux_desktop)
#PLATFORM = $(Mac)

ifeq ($(PLATFORM), $(RPI))
HARDWARE_PATH = ./hardware/rpi/
endif
ifeq ($(PLATFORM), $(linux_desktop))
HARDWARE_PATH = ./hardware/linux_desktop/
endif
ifeq ($(PLATFORM), $(Mac))
HARDWARE_PATH = ./hardware/mac/
endif

CC         = g++
OPTIONS	   = -std=c99 -g  
CFLAGS = -lwiringPi -lrt -DPLATFORM=$(PLATFORM) -std=gnu++11 \
	     -DRPI=$(RPI) -Dlinux_desktop=$(linux_desktop) -DMac=$(Mac) -DGL_GLEXT_PROTOTYPES=1

#***** INCLUDE PATHS : 
VG_INCLUDE = -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux \
	         -I/opt/vc/include/interface/vcos/pthreads -I/home/pi/openvg
SQL_INC    = /usr/include/mysql /usr/include/
CORE_INC   = ./ ../core/can/ ../core/kinetic/ ../core/misc/ ../core/msg/ ../core/ipc/
LOCAL_INC  = ./sql/ ./street_signs/ ./robot/ ./route/ ./paintings/ ./physics/ \
			./nlp/ ./molecules/ ./atoms/ ./Large/ ./framework/ ./buildings/
LIB_INC	   =  ../abkInstant/threads/ ../core/dbase/ ../abkInstant/nlp_core/
ALL_INCLUSIVE = $(addprefix -I,$(INC)) $(addprefix -I,$(SQL_INC)) $(addprefix -I,$(CORE_INC)) \
			$(addprefix -I, $(LOCAL_INC)) $(addprefix -I,$(LIB_INC)) $(VG_INCLUDE)

# LIBRARY PATHS : -lfreenect 
SQL_LIB = -L/usr/lib/arm-linux-gnueabihf -lmysqlclient -lz -ldl
LDFLAGS = -L/opt/vc/lib/  -lopenmaxil -lbcm_host -lassimp \
          -lvcos -lvchiq_arm -lpthread -lrt -L./vgfont -L../core/libs/ilclient/ -lilclient \
          -lglut -lGL -lGLU  
LIBFLAGS= -L/usr/local/lib -L../core/libs/vgfont/ \
	      -lshapes -lwiringPi -lm $(SQL_LIB)
OPENCV_LIBS = -lopencv_video -lopencv_videoio -lopencv_imgcodecs -lopencv_ts -lopencv_face \
	-lopencv_objdetect -l opencv_ml -lopencv_calib3d -lopencv_imgproc -lopencv_highgui \
	-lopencv_core

# MAIN DIRECTORY OBJECTS:
OBJ     = main.o t_object.o mouse_nav.o cursor_nav.o 
NLP_OBJ = color_protocol.o parameter.o robot_legs_protocol.o \
  create_object_nlp.o positioner_2d.o robot_protocol.o ThreeD_object_protocol.o \
  it.o positioner_3d.o scene_nlp.o top_level_protocol_sim.o \
  nlp_sentence.o robot_arms_protocol.o serverthread.o verbal_xyz.o
ATOMS_OBJ = abstract_wall.o extrusion.o gl_object.o moving_object.o sign.o txt_cylinder.o \
  camera_screen.o face_box.o glSphere.o painting.o stairway.o \
  cylinder.o gl_atom.o ibeam.o paper.o Texture.o wall.o \
  door.o gl_container.o light_switch.o rectangle.o txt_container.o \
  door_handle.o gl_misc.o Map2D.o ring.o touch_depth.o
MOLECULES_OBJ = construction_barricade.o frame.o hinge.o table.o \
  counter.o full_wall.o palette.o table_pole.o \
  bookcase.o drawer.o glComputableWall.o picnic_table.o truss.o \
  brickwall.o  glMolecule.o vehicle.o \
  cabinet.o gl_window.o rafter.o verbal_molecule.o \
  chair.o elevator.o roof.o walking_robot.o
LARGE_OBJ   = road.o terrain.o
PHYSICS_OBJ = physical_world.o
ROBOT_OBJ = robot_arm.o robot_foot.o robot_head.o robot_leg_piece.o \
  robot.o robot_gripper.o robot_interpolator.o robot_moves.o
ROUTE_OBJ = multi_path_route.o route.o route_feet.o routing.o
SQL_OBJ   = scene.o sql_load.o
BUILDINGS_OBJ = apartment.o basket_ball_court.o floor.o room2.o sports_bench.o \
    dwelling2.o floor_plan.o room_of_tables.o dwelling.o dwelling_level.o \
    quarter_629.o basketball_arena.o hampton_complex.o 
FRAMEWORK_OBJ = asset_importer.o imageloader.o key_press.o sequencer_memory.o \
    ConFloat.o init_samples.o object_primitive_creator.o tk_colors.o \
    DecisionTree.o instant_ipc.o proc_client_cmds.o stats.o verbal_object.o


CONTROL_OBJS = serial.o serial_synchronous.o roboclaw.o x_eyes.o neck_thread.o
VISION_OBJS  = findEyeCenter.o findEyeCorner.o face_detect.o face_recog.o face_summary.o \
			   optical_flow.o misc_detect.o  image_proc_misc.o helpers.o frame_grabber.o
DBASE_OBJS    = viki_logger.o vision_logger.o
MISC_OBJS     = super_string.o
IPC_OBJS      = vision_memory.o simulator_memory.o client_memory.o
KINETIC_OBJS  = vector.o

%.o: %.cpp $(DEPS)
	$(CC) $(ALL_INCLUSIVE) -c -o $@ $< -g $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(ALL_INCLUSIVE) -c -o $@ $< $(OPTIONS) $(CFLAGS)


#libs/libframework.a: $(addprefix framework/, $(FRAMEWORK_OBJ))
#	ar rcs $@ $^
#libs/libatoms.a: $(addprefix atoms/, $(ATOMS_OBJ))
#	ar rcs $@ $^
#libs/libbuildings.a: $(addprefix buildings/, $(BUILDINGS_OBJ))
#	ar rcs $@ $^
#libs/libmolecules.a: $(addprefix molecules/, $(MOLECULES_OBJ))
#	ar rcs $@ $^
#libs/liblarge.a: $(addprefix Large/, $(LARGE_OBJ))
#	ar rcs $@ $^
#libs/libnlp.a: $(addprefix nlp/, $(NLP_OBJ))
#	ar rcs $@ $^
#libs/libphysics.a: $(addprefix physics/, $(PHYSICS_OBJ))
#	ar rcs $@ $^
#libs/librobot.a: $(addprefix robot/, $(ROBOT_OBJ))
#	ar rcs $@ $^
#libs/libroute.a: $(addprefix route/, $(ROUTE_OBJ))
#	ar rcs $@ $^
#libs/libsql.a: $(addprefix sql/, $(SQL_OBJ))
#	ar rcs $@ $^
#BK_LIBS = -L./libs -lframework -latoms -lnlp -lbuildings -lmolecules -llarge -lphysics -lrobot -lroute -lsql 


home3D: $(OBJ) $(addprefix ../core/kinetic/,$(KINETIC_OBJS)) \
	$(addprefix ./nlp/,    $(NLP_OBJ)) \
	$(addprefix ./physics/,$(PHYSICS_OBJ)) \
	$(addprefix ./atoms/,  $(ATOMS_OBJ)) \
	$(addprefix ./molecules/,$(MOLECULES_OBJ)) \
	$(addprefix ./robot/,  $(ROBOT_OBJ)) \
	$(addprefix ./route/,  $(ROUTE_OBJ)) \
	$(addprefix ./Large/,  $(LARGE_OBJ)) \
	$(addprefix ./framework/,$(FRAMEWORK_OBJ)) \
	$(addprefix ./buildings/,$(BUILDINGS_OBJ)) \
	$(addprefix ./sql/,$(SQL_OBJ)) \
    $(addprefix ../core/dbase/,$(DBASE_OBJS)) \
	$(addprefix ../core/misc/,$(MISC_OBJS)) \
	$(addprefix ../core/ipc/,$(IPC_OBJS)) \
	$(addprefix ../abkInstant/threads/, $(INSTANT_OBJS))
	g++ -o $@ $^ $(CFLAGS) $(LIBFLAGS) $(LDFLAGS) $(OPENCV_LIBS)

//all: home3D libs/libframework.a libs/libatoms.a libs/libbuildings.a libs/libmolecules.a \
//	libs/liblarge.a libs/libnlp.a libs/libphysics.a libs/librobot.a libs/libroute.a libs/libroute.a libs/libsql.a

clean:
	rm ./*/*.o
	rm ../core/dbase/*.o
	rm ../core/misc/*.o
	rm ../core/ipc/*.o
	rm ../core/kinetic/*.o
	rm -rf *.o $(PRG).elf *.eps *.bak
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)


