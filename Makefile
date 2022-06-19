test-linked:
	cd linked-list && gcc -g test.c -o test && ./test

build-linked:
	cp linked-list/alloc.c gawk-3.1.8/alloc.c && cd gawk-3.1.8 && make check