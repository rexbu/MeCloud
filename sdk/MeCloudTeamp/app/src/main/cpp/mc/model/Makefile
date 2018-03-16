#Makefile
# bushaofeng. 2011.8.27
# ver=1.0.0

OUTPUT=./output
OUTPUT_INC=$(OUTPUT)/include
OUTPUT_LIB=$(OUTPUT)/lib

ifneq ($(sys), ios)
SUBDIRS = db
endif

SUBDIRS +=async push

ifneq ($(sys), ios)
SUBDIRS+= utp p2p
endif

all::
	mkdir -p $(OUTPUT_INC)
	mkdir -p $(OUTPUT_LIB)
	@for MODEL in $(SUBDIRS);\
		do \
		( cp model.mk $$MODEL/Makefile && cd $$MODEL && make clean sys=$(sys) && make sys=$(sys) target=$$MODEL );\
	done

clean:
	rm -rf $(OUTPUT)
	@for MODEL in $(SUBDIRS);\
		do \
		( cd $$MODEL && make clean sys=$(sys) );\
	done
