build:
	mkdir build
	gcc main.c -O -o build/wish
debug:
	mkdir build
	gcc main.c -g -o build/wish
clean:
	rm -rf build
