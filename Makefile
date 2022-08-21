CFLAGS = -Wall -O0 -g

test-linked:
	cp test.c linked/test.c && \
	cd linked && \
	gcc test.c edaf35_alloc.c -o test $(CFLAGS) && \
	./test; \
	rm test.c

test-buddy:
	cp test.c buddy/test.c && \
	cd buddy && \
	gcc test.c edaf35_alloc.c -o test $(CFLAGS) && \
	./test; \
	rm test.c

build-linked:
	cp linked/edaf35_alloc.* gawk-3.1.8/ && \
	cd gawk-3.1.8 && \
	make check

build-buddy:
	cp buddy/edaf35_alloc.* gawk-3.1.8/ && \
	cd gawk-3.1.8 && \
	make check