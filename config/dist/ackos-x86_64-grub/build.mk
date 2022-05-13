$(BIN_FOLDER)/$(ISONAME): $(BIN_FOLDER)/kernel.elf $(BIN_FOLDER)/ramdisk.tar.gz
	@mkdir -p $(@D)

	@echo "Creating GRUB image"

	@mkdir -p $(BIN_FOLDER)/isodir/boot/grub
	@cp $(DIST_FOLDER)/grub.cfg $(BIN_FOLDER)/isodir/boot/grub
	@cp -r $(DIST_FOLDER)/resources $(BIN_FOLDER)/isodir/boot

	@cp $(BIN_FOLDER)/kernel.elf $(BIN_FOLDER)/isodir/boot
	@cp $(BIN_FOLDER)/ramdisk.tar.gz $(BIN_FOLDER)/isodir/boot

	@sudo grub-mkrescue -o $@ $(BIN_FOLDER)/isodir
