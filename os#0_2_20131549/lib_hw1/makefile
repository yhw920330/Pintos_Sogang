TARGET = testlib
OBJS = main.o bitmap.o hash.o list.o 
CC= gcc

$(TARGET): $(OBJS)
	$(CC) -lm -o $@ $(OBJS)

main.o : list.h bitmap.h hash.h main.c 
	gcc -c main.c
bitmap.o : bitmap.c bitmap.h
	gcc -c bitmap.c
hash.o : hash.c hash.h
	gcc -c hash.c
list.o : list.c list.h 
	gcc -c list.c 

clean:
	rm $(OBJS)$(TARGET)
