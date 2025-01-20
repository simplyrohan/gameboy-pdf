.PHONY: build

build:
ifndef ROM
	$(error ROM is not set; set it with `make ROM=path/to/rom.gb")
endif
	@emcc pdfboy.c gnuboy/*.c -o build/gnuboy.js -sWASM=0 -sSINGLE_FILE=1 -sENVIRONMENT=shell -sEXPORTED_FUNCTIONS=[_main,_loop] -sFORCE_FILESYSTEM=1
	# @cat glue.js >> build/gnuboy.js
	@cat glue.js build/gnuboy.js >> build/tmp
	@mv build/tmp build/gnuboy.js
	@./embed_rom.py $(ROM) build/gnuboy.js

	@python generate.py