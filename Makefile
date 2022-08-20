CFLAGS = -Wall -O0 -g

test-linked:
	cd linked && \
	gcc test.c edaf35_alloc.c -o test $(CFLAGS) && \
	./test

test-buddy:
	cd buddy && \
	gcc test.c edaf35_alloc.c -o test $(CFLAGS) && \
	./test

build-linked:
	cp linked/edaf35_alloc.* gawk-3.1.8/ && \
	cd gawk-3.1.8 && \
	make check

build-buddy:
	cp buddy/edaf35_alloc.* gawk-3.1.8/ && \
	cd gawk-3.1.8 && \
	make check