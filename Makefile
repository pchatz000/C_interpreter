ipli: bcc.o parser.o tokenizer.o varfun.o
	gcc -o ipli -g $^ 

%.o: %.c $(wildcard *.h)
	gcc -c -g $< $(CFLAGS)

debug: ipli
debug: CFLAGS += -DDEBUG

clean:
	rm *.o
	rm ipli

tests:
	bash runtests.sh

.PHONY: tests clean

