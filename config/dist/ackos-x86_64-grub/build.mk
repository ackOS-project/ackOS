$(BIN_FOLDER)/$(ISONAME): $(BIN_FOLDER)/ackos.elf
	@mkdir -p $(@D)

	@echo [ creating media ] grub-mkrescue

	@mkdir -p $(BIN_FOLDER)/isodir/boot/grub
	@cp $(DIST_FOLDER)/grub.cfg $(BIN_FOLDER)/isodir/boot/grub
	@cp $(BIN_FOLDER)/ackos.elf $(BIN_FOLDER)/isodir/boot

	@sudo grub-mkrescue -o $@ $(BIN_FOLDER)/isodir
