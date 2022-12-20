CFLAGS = -Werror -Wall -O0 -g -lm

test-linked:
	cp test.c linked/test.c && \
	cd linked && \
	gcc test.c -o test $(CFLAGS) && \
	./test

test-buddy:
	cp test.c buddy/test.c && \
	cd buddy && \
	gcc test.c -o test $(CFLAGS) && \
	./test

build-linked:
	cp linked/malloc.* gawk-3.1.8/ && \
	cd gawk-3.1.8 && \
	make check

build-buddy:
	cp buddy/malloc.* gawk-3.1.8/ && \
	cd gawk-3.1.8 && \
	make check