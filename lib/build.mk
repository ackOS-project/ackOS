LIB_DIR := lib
LIB_SYSROOT_HEADERS :=
LIB_ARCHIVE_DIR := $(OBJ_DIR)/$(LIB_DIR)
LIB_ARCHIVES :=

LIBACK_DIR = liback
LIBC_DIR = libc

LIBS += LIBACK
LIBS += LIBC

CFLAGS += -I $(LIB_DIR) -I $(LIB_DIR)/libc

define LIBRARY_TEMPLATE =
$(1)_ARCHIVE := $(LIB_ARCHIVE_DIR)/$($(1)_DIR).a

$(1)_SOURCES += \
		$$(call rwildcard, $(LIB_DIR)/$($(1)_DIR), *.c)

$(1)_HEADERS += \
		$$(call rwildcard, $(LIB_DIR)/$($(1)_DIR),*.h)

$(1)_OBJECTS := $$(patsubst %.c, $(OBJ_DIR)/%.c.o, $$($(1)_SOURCES))

LIB_ARCHIVES += $$($(1)_ARCHIVE)
LIB_SYSROOT_HEADERS += $$(patsubst $(LIB_DIR)/%.h, $$(SYSROOT_DIR)/usr/lib/include/%.h, $$($(1)_HEADERS))

$$($(1)_ARCHIVE): $$($(1)_OBJECTS)
	@mkdir -p $$(@D)
	@echo "Creating $($(1)_DIR).a" 
	$(V)$(AR) -rsc $$@ $$^
	@mkdir -p $(SYSROOT_DIR)/usr/lib/static
	$(V)cp $$@ $(SYSROOT_DIR)/usr/lib/static

$$(OBJ_DIR)/$$(LIB_DIR)/$($(1)_DIR)/%.c.o: $$(OBJ_DIR)/$$(LIB_DIR)/%.c $($(1)_HEADERS)
	@mkdir -p $$(@D)
	@echo "Building $$<"
	$(V)$(CC) -c $$< -o $$@ $(CFLAGS)

endef

$(foreach lib, $(LIBS), $(eval $(call LIBRARY_TEMPLATE,$(lib))))

$(SYSROOT_DIR)/usr/lib/include/%.h: $(LIB_DIR)/%.h
	@$(CREATE_DIRS)
	$(V)cp $< $@

copy-headers-to-sysroot: $(LIB_SYSROOT_HEADERS)

LIB_TARGETS := $(LIB_ARCHIVES)
