test-linked:
	cd linked-list && gcc -g test.c -o test && ./test

test-buddy:
	cd buddy && gcc -g test.c -o test && ./test

build-linked:
	cp linked-list/alloc.* gawk-3.1.8/ && cd gawk-3.1.8 && make check

build-buddy:
	cp buddy/alloc.* gawk-3.1.8/ && cd gawk-3.1.8 && make check