# Makefile
# bushaofeng. 2011.8.27
# ver=1.0.0

OUTPUT		= ./output
OUTPUT_INC	= $(OUTPUT)/include
OUTPUT_LIB	= $(OUTPUT)/lib

BS_PATH 	= ./../bs

include $(BS_PATH)/$(sys).mk

INCLDIR		= $(BS_PATH) /usr/local/include
INCLUDE		= $(addprefix -I, $(INCLDIR))

FPIC        =       -fPIC
WARN_LEVL   =       -Wall
FDEBUG      =       -g
SHARED		=		-shared

EXTRA_OPT   =       $(FDEBUG)
EXTRA_OPT   +=      $(WARN_LEVL)
EXTRA_OPT   +=      $(FPIC)
EXTRA_OPT	+=		$(SHARED)

OBJDIR = ./
SRCS = $(wildcard ./*.cpp)
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS)) 

TARGET=libbasic
$(TARGET):$(OBJS)
	mkdir -p $(OUTPUT_INC)
	mkdir -p $(OUTPUT_LIB)
	echo $(OBJS)
	ar -r libbasic.a $(OBJS)
	cp -p *.h $(OUTPUT_INC)
	cp -p *.a $(OUTPUT_LIB)

$(OBJS):%.o: %.cpp 
	$(CC) -c $(LDFLAGS) $(EXTRA_OPT) $(INCLUDE) $^ -o $@

clean:
	rm -rf $(OUTPUT) *.a */*.o *.o
