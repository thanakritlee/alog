OBJS = src/buffer.o
HEAD = src/buffer.h

alog: src/main.c $(OBJS) $(HEAD)
	gcc $(OBJS) src/main.c -o alog

clean:
	rm alog $(OBJS)
