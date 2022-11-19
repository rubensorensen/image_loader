build: main.c image_loader.c
	gcc -Wall -Wextra -std=c11 -ggdb $(shell pkg-config --cflags sdl2) main.c image_loader.c window.c -o image_loader $(shell pkg-config --libs sdl2)

.PHONY: run
run: build
	./image_loader

.PHONY: main.c image_loader.c window.c
main.c image_loader.c window.c: image_loader.h window.h

.PHONY: clean
clean:
	@$(RM) ./image_loader
