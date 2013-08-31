SUBDIRS = Bluetooth Brick+Datalog Brick+Info Brick+Program IR+Control \
          Motor+Control Port+View Sleep Volume WiFi ui c_ui
SUBDIRS_DISABLED = TEST SelfTest tst Debug Demo Test

# Make is not friend with spaces, so we have to be creative to handle them.
space = $(subst +,\ ,$1)

install: $(SUBDIRS:%=%.install)

%.install:
	$(MAKE) -C $(call space,$*) install

all: $(SUBDIRS:%=%.all)

%.all:
	$(MAKE) -C $(call space,$*) all

uninstall: $(SUBDIRS:%=%.uninstall) $(SUBDIRS_DISABLED:%=%.uninstall)

%.uninstall:
	$(MAKE) -C $(call space,$*) uninstall

clean: $(SUBDIRS:%=%.clean) $(SUBDIRS_DISABLED:%=%.clean)
	rm -f .convert-check

%.clean:
	$(MAKE) -C $(call space,$*) clean

.PHONY: install all uninstall clean
