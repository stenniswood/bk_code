cc=g++
OPTIONS	= -std=c99 -lm
INC		= ../core/can/ ../core/dbase/
CFLAGS 	= -lwiringPi -lm
DEPS   	= 
OBJ     =  gpio.o claw_main.o sserial.o roboclaw.o serial_loadcell.o 
DBASE_OBJS = can_sql_logger.o 
#configuration.o leds.o can_id_list.o 

CPPOBJ = 

SQL_INC = -I/usr/include/mysql 
SQL_LIB = -L/usr/lib/arm-linux-gnueabihf -lmysqlclient -lz -ldl 
LIBFLAGS = $(SQL_LIB) -lpthread

%.o: %.cpp $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(SQL_INC) -c -o $@ $< $(CFLAGS)
%.o: %.c $(DEPS)
	$(CC) $(addprefix -I,$(INC)) $(SQL_INC) -c -o $@ $< $(OPTIONS) $(CFLAGS)

#libcan_eid.so: ./core/can_eid.o 
#	gcc -shared -o libcan_eid.so ./core/can_eid.o

claw: $(OBJ) \
	$(addprefix ../core/dbase/, $(DBASE_OBJS))
	g++ -o  $@ $^ $(LIBFLAGS) $(CFLAGS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf ./core/*.o 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
