LIMINE_OBJ_DIR := $(OBJ_DIR)/limine
LIMINE_DIR := submodules/limine
LIMINE_DEPLOY := $(LIMINE_DIR)/limine-deploy

OS_IMAGE = $(BIN_DIR)/$(DIST).iso

$(LIMINE_DEPLOY):
	@echo Building limine deploy tool
	@make -C $(LIMINE_DIR) limine-deploy

$(OS_IMAGE): $(KERNEL_BIN) $(LIMINE_DEPLOY) config/dist/$(DIST)/limine.cfg
	@mkdir -p $(@D)

	@echo "Creating LIMINE image"

	@mkdir -p $(LIMINE_OBJ_DIR)
	@cp config/dist/$(DIST)/limine.cfg $(LIMINE_OBJ_DIR)
	@cp -r config/dist/$(DIST)/resources $(LIMINE_OBJ_DIR)
	@cp $(KERNEL_BIN) $(LIMINE_OBJ_DIR)

	@cp $(LIMINE_DIR)/limine.sys $(LIMINE_DIR)/limine-cd.bin $(LIMINE_DIR)/limine-cd-efi.bin $(LIMINE_OBJ_DIR)

	@xorriso -as mkisofs -b limine-cd.bin \
	        -no-emul-boot -boot-load-size 4 -boot-info-table \
	        --efi-boot limine-cd-efi.bin \
			-efi-boot-part --efi-boot-image --protective-msdos-label \
	        $(LIMINE_OBJ_DIR) -o $@
	@$(LIMINE_DEPLOY) $@
