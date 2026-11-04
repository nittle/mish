build:
	mkdir build
	gcc main.c -O -o build/mish
debug:
	mkdir build
	gcc main.c -g -o build/mish
clean:
	rm -rf build
