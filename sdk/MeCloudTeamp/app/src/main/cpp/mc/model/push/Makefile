# Makefile
# bushaofeng. 2011.8.27
# ver=1.0.0

BS_PATH = ./../../bs
BASIC_PATH = ./../../basic

include $(BS_PATH)/$(sys).mk

OUTPUT		= ./../output
OUTPUT_INC	= $(OUTPUT)/include
OUTPUT_LIB	= $(OUTPUT)/lib

INCLDIR		= $(BS_PATH) $(BASIC_PATH) /usr/local/include $(OUTPUT_INC)
INCLUDE		= $(addprefix -I, $(INCLDIR))

FPIC            =       -fPIC
WARN_LEVL       =       -Wall
FDEBUG          =       -g
#SHARED			=		-shared

EXTRA_OPT       =       $(FDEBUG)
EXTRA_OPT       +=      $(WARN_LEVL)
EXTRA_OPT       +=      $(FPIC)
EXTRA_OPT		+=		$(SHARED)

OBJDIR = ./
SRCS = $(wildcard ./*.cpp)
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS)) 

lib$(target):$(OBJS)
	    echo $(OBJS)
		ar -r lib$(target).a $(OBJS)
		cp -p *.h $(OUTPUT_INC)
		cp -p *.a $(OUTPUT_LIB)

$(OBJS):%.o: %.cpp
	$(PP) -c $(LDFLAGS) $(EXTRA_OPT) $(INCLUDE) $^ -o $@

clean:
	rm -f *.a */*.o *.o
