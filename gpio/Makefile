cc=g++
CFLAGS=-I. -lwiringPi
DEPS = 
OBJ = gpio_test.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
gpio_test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
