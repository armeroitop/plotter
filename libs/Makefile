.PHONY: all clean cleanall

ROOT_DIR := .

DIRS := $(shell find $(ROOT_DIR) -maxdepth 1 -type d -not -path $(ROOT_DIR))

all:
	$(foreach dir, $(DIRS),$(MAKE) -C $(dir);)

clean:
	$(foreach dir, $(DIRS),$(MAKE) -C $(dir) clean;)

cleanall:
	$(foreach dir, $(DIRS),$(MAKE) -C $(dir) cleanall;)


#$(MAKE) -C tinyPTC
#$(MAKE) -C picoPNG

#$(MAKE) -C picoPNG clean
#$(MAKE) -C tinyPTC clean

#$(MAKE) -C picoPNG cleanall
#$(MAKE) -C tinyPTC cleanall