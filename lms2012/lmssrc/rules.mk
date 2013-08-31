# Shared Makefile rules for all applications.

LMS_TO_RBF = ../adk/cmp
LMS_TO_RBF_DEPS = ../adk/lmsasm/bytecodes.c ../adk/lmsasm/bytecodes.h
XBM_TO_RGF = ../adk/bmp
BMP_TO_XBM = convert
TASM = ../adk/tasm
TASM_DEPS = ../adk/lmststasm/bytecodes.h ../adk/lmststasm/lmstypes.h

INSTALL_TO ?= AM1808 X86

INSTALL_DIR_AM1808 = ../../lms2012/Linux_AM1808/$(APP_PATH)/$(APP)
INSTALL_DIR_X86 = ../../lms2012/Linux_X86/$(APP_PATH)/$(APP)
INSTALL_DIR_source = ../../$(APP)/source

# Make is not friend with spaces, so we have to be creative to handle them.
space = $(subst +,\ ,$1)

ifneq ($(ICON),)
GRAPHICS += icon
icon_SOURCE = $(ICON)
endif

GENERATED := $(PROGRAMS:%=$(APP)/%.rbf) \
	$(PROGRAMS_TASM:%=$(APP)/%.rbf) \
	$(GRAPHICS:%=$(APP)/%.rgf) \
	$(GRAPHICS_XBM:%=$(APP)/%.xbm) \
	$(SOUNDS:%=$(APP)/%.rsf)

ifneq ($(ARCHIVE),y)
INSTALLED := $(foreach to,$(INSTALL_TO), \
	$(PROGRAMS:%=$(INSTALL_DIR_$(to))/%.rbf) \
	$(PROGRAMS_TASM:%=$(INSTALL_DIR_$(to))/%.rbf) \
	$(GRAPHICS:%=$(INSTALL_DIR_$(to))/%.rgf) \
	$(GRAPHICS_XBM:%=$(INSTALL_DIR_$(to))/%.xbm) \
	$(SOUNDS:%=$(INSTALL_DIR_$(to))/%.rsf))
else
INSTALLED := $(foreach to,$(INSTALL_TO),$(INSTALL_DIR_$(to)).raf)
endif

install: $(call space,$(INSTALLED))

ifneq ($(ARCHIVE),y)
all: $(call space,$(GENERATED))
else
all: $(call space,$(APP).raf)
$(call space,$(APP).raf): $(call space,$(GENERATED))
	tar czf "$@" "$(basename $@)"
$(call space,$(INSTALLED)): $(call space,$(APP).raf) .dirstamp_install
	cp "$<" "$@"
endif

define PROGRAM_rule
$$(call space,$$(APP)/$1).rbf: $$(call space,$1).lms .dirstamp $(LMS_TO_RBF_DEPS)
	$$(LMS_TO_RBF) "$$(basename $$<)" "$$@"
$$(call space,$$(foreach to,$$(INSTALL_TO),$$(INSTALL_DIR_$$(to))/$1.rbf)): $$(call space,$$(APP)/$1).rbf .dirstamp_install
	cp "$$<" "$$@"
endef
$(foreach program,$(PROGRAMS),\
	$(eval $(call PROGRAM_rule,$(program))))

define PROGRAM_TASM_rule
$$(call space,$$(APP)/$1).rbf: $$(call space,$1).c .dirstamp $(TASM_DEPS)
	$$(TASM) "$$(basename $$<)" "$$(basename $$@)"
$$(call space,$$(foreach to,$$(INSTALL_TO),$$(INSTALL_DIR_$$(to))/$1.rbf)): $$(call space,$$(APP)/$1).rbf .dirstamp_install
	cp "$$<" "$$@"
endef
$(foreach program,$(PROGRAMS_TASM),\
	$(eval $(call PROGRAM_TASM_rule,$(program))))

define GRAPHIC_rule
$$(call space,$$(APP)/$1).rgf: $$(call space,$2).bmp .dirstamp | ../.convert-check
	$$(BMP_TO_XBM) "$$<" bitmap.xbm
	$$(XBM_TO_RGF) bitmap "$$(basename $$@)"
$$(call space,$$(foreach to,$$(INSTALL_TO),$$(INSTALL_DIR_$$(to))/$1.rgf)): $$(call space,$$(APP)/$1).rgf .dirstamp_install
	cp "$$<" "$$@"
endef
$(foreach graphic,$(GRAPHICS),\
	$(eval $(call GRAPHIC_rule,$(graphic),\
	$(if $($(graphic)_SOURCE),$($(graphic)_SOURCE),$(graphic)))))

define GRAPHIC_XBM_rule
$$(call space,$$(APP)/$1).xbm: $$(call space,$2).bmp .dirstamp | ../.convert-check
	$$(BMP_TO_XBM) "$$<" "$$@"
$$(call space,$$(foreach to,$$(INSTALL_TO),$$(INSTALL_DIR_$$(to))/$1.xbm)): $$(call space,$$(APP)/$1).xbm .dirstamp_install
	cp "$$<" "$$@"
endef
$(foreach graphic,$(GRAPHICS_XBM),\
	$(eval $(call GRAPHIC_XBM_rule,$(graphic),\
	$(if $($(graphic)_SOURCE),$($(graphic)_SOURCE),$(graphic)))))

define SOUND_rule
$$(call space,$$(APP)/$1).rsf: $$(call space,$1).rsf .dirstamp
	cp "$$<" "$$@"
$$(call space,$$(foreach to,$$(INSTALL_TO),$$(INSTALL_DIR_$$(to))/$1.rsf)): $$(call space,$$(APP)/$1).rsf .dirstamp_install
	cp "$$<" "$$@"
endef
$(foreach sound,$(SOUNDS),\
	$(eval $(call SOUND_rule,$(sound))))

.dirstamp:
	mkdir -p $(call space,$(APP))
	touch $@
.dirstamp_install:
ifneq ($(ARCHIVE),y)
	mkdir -p $(call space,$(foreach to,$(INSTALL_TO),$(INSTALL_DIR_$(to))))
else
	mkdir -p $(call space,$(dir $(foreach to,$(INSTALL_TO),$(INSTALL_DIR_$(to)))))
endif
	touch $@

../.convert-check:
	@if ! which $(BMP_TO_XBM) > /dev/null; then \
		echo "##################" >&2; \
		echo "# Can not find $(BMP_TO_XBM), please install imagemagick package." >&2; \
		echo "##################" >&2; \
		echo >&2; \
	else \
		touch $@; \
	fi

$(LMS_TO_RBF_DEPS): ../adk/lmsasm/%: ../../lms2012/source/%
	cp $< $@
$(TASM_DEPS): ../adk/lmststasm/%: ../../lms2012/source/%
	cp $< $@

clean:
	rm -rf $(call space,$(APP))
	rm -f $(call space,$(APP)).raf
	rm -f .dirstamp .dirstamp_install $(LMS_TO_RBF_DEPS) $(TASM_DEPS)
	rm -f bitmap.xbm

uninstall:
	rm -f $(call space,$(INSTALLED))

.PHONY: all clean
