CFLAGS = -Werror -Wall -O0 -g -lm

test-linked:
	cp test.c linked/test.c && \
	cd linked && \
	gcc test.c -o test $(CFLAGS) && \
	./test

test-buddy:
	cp test.c buddy/test.c && \
	cp test_freed.c buddy/test_freed.c && \
	cd buddy && \
	gcc test.c -o test $(CFLAGS) && \
	gcc test_freed.c -o test_freed $(CFLAGS) && \
	./test ; \
	./test_freed

build-linked:
	cp linked/malloc.* gawk-3.1.8/ && \
	cd gawk-3.1.8 && \
	make check

build-buddy:
	cp buddy/malloc.* gawk-3.1.8/ && \
	cd gawk-3.1.8 && \
	make check