build: main.c image_loader.c
	gcc -Wall -Wextra -std=c11 -ggdb main.c image_loader.c -o image_loader

.PHONY: run
run: build
	./image_loader

.PHONY: main.c image_loader.c
main.c image_loader.c: image_loader.h

.PHONY: clean
clean:
	@$(RM) ./image_loader ./output.ppm
