CC=gcc
CFLAGS= -D framesize=$(X) -D varmemsize=$(Y)

mysh: shell.c interpreter.c shellmemory.c back_store.c
	$(CC) $(CFLAGS) -c shell.c interpreter.c shellmemory.c kernel.c pcb.c ready_queue.c back_store.c -lpthread
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o kernel.o pcb.o ready_queue.o back_store.o

clean: 
	rm mysh; rm *.o