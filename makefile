OBJS = src/buffer.o src/start.o src/stop.o src/utils.o src/list.o src/info.o src/lexer.o src/cancel.o
HEAD = src/buffer.h src/alog.h

TEST_OBJS = test/start.o test/stop.o test/utils.o test/dir.o test/list.o test/info.o test/lexer.o test/cancel.o
TEST_HEAD = test/tests.h

alog: src/main.c $(OBJS) $(HEAD)
	gcc $(OBJS) src/main.c -o alog

test-runner: test/main.c $(TEST_OBJS) $(TEST_HEAD) $(OBJS) $(HEAD)
	gcc $(TEST_OBJS) $(OBJS) test/main.c -o test-runner

clean:
	rm -f alog test-runner $(OBJS) $(TEST_OBJS)

install-autocomplete:
	cp autocomplete/alog /etc/bash_completion.d/
