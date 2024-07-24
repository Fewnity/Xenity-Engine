TARGET = Bullet
OBJS = main.o CTimer.o Render.o vram.o 

PSPPATH = $(shell psp-config --psp-prefix)

CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PSP_FW_VERSION=371


INCDIR = 
LIBS =   -lbulletsoftbody -lbulletdynamics -lbulletcollision -lbulletmath -lz -lpspgum_vfpu -lpspgu -lstdc++ -lpsprtc -lpspctrl -lm -lpspvfpu -lpspsdk -lmmio -lpsppower
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Bullet Physics Test2

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

