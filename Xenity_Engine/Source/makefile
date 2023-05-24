TARGET = Test
OBJS = main.o
 
CFLAGS =
CXXFLAGS = $(CFLAGS) -std=c++11 -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
 
INCDIR = ../include/
LIBDIR =
LDFLAGS =
LIBS= -lpspgum -lpspgu -lm
 
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Test
#PSP_EBOOT_ICON = ICON0.PNG 
 
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak