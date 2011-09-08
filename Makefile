.DELETE_ON_ERROR: yes
.DEFAULT_GOAL:=test
.PHONY: all test clean

OUT:=out

BIN:=$(OUT)/concordance

test: all
	(cat $(MAKEFILE_LIST) $(wildcard *.c)) | $(BIN)
	cat README | $(BIN)

all: $(BIN)

$(OUT)/%: %.c
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(OUT)
