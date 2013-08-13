ifeq ($(ARCH),X86)
CROSS_COMPILE =
else ifeq ($(ARCH),AM1808)
CROSS_COMPILE = arm-none-linux-gnueabi-
else
$(error unknown ARCH)
endif

BASE = ../..

ifneq ($(CONF),Linuxmod)

OBJS = $(SOURCES:%.c=%.o)
DEPS = $(OBJS:%.o=%.d)

INCLUDES = -I$(BASE)/lms2012/source \
	   -I$(BASE)/c_com/source \
	   -I$(BASE)/c_input/source \
	   -I$(BASE)/c_memory/source \
	   -I$(BASE)/c_output/source \
	   -I$(BASE)/c_sound/source \
	   -I$(BASE)/c_ui/source

ifeq ($(ARCH),X86)
DBUS_CFLAGS := $(shell pkg-config dbus-1 --cflags)
CFLAGS = -DLinux_X86 $(INCLUDES) $(DBUS_CFLAGS) -O0 -g3 -Wall -fPIC
LDFLAGS = -L$(BASE)/lms2012/Linux_$(ARCH)/sys/lib
else
DEVKIT = $(BASE)/../extra/linux-devkit/arm-none-linux-gnueabi
INCLUDES += -I$(DEVKIT)/usr/include/dbus-1.0
INCLUDES += -I$(DEVKIT)/usr/lib/dbus-1.0/include
INCLUDES += -I$(DEVKIT)/usr/include
CFLAGS = -DPCASM $(INCLUDES) -O0 -Wall -fPIC
LDFLAGS = -L$(BASE)/lms2012/Linux_$(ARCH)/sys/lib -L$(DEVKIT)/usr/lib
endif

ifeq ($(CONF),Linuxlib)
LDFLAGS += -shared
INSTALL_DIR = sys/lib
else
INSTALL_DIR = sys
VPATH = sys/lib
endif

INSTALL_TARGET = $(BASE)/lms2012/Linux_$(ARCH)/$(INSTALL_DIR)/$(TARGET)

all: install

%.o: ../source/%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c -MMD -MP -o $@ $<

$(TARGET): $(OBJS) $(LIBS)
	$(CROSS_COMPILE)gcc $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

install: $(INSTALL_TARGET)
$(INSTALL_TARGET): $(TARGET)
	mkdir -p $(dir $@) && cp $< $@

$(SUBDIRS:%=-l%): FORCE
	$(MAKE) -C ../../$(@:-l%=%)/Linuxlib_$(ARCH) install

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

uninstall:
	rm -f $(INSTALL_TARGET)

.PHONY: all install clean uninstall FORCE
FORCE:

-include $(DEPS)

endif
