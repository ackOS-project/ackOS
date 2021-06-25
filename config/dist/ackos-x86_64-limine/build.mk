LIMINE_BIN := submodules/limine
LIMINE_INSTALL := $(LIMINE_BIN)/limine-install-linux-x86_64

$(BIN_FOLDER)/$(ISONAME): $(BIN_FOLDER)/kernel.elf $(BIN_FOLDER)/ramdisk.tar.gz
	@mkdir -p $(@D)

	@echo [ creating media ] limine-install

	@mkdir -p $(BIN_FOLDER)/isodir/boot/limine
	@mkdir -p $(BIN_FOLDER)/isodir/EFI/BOOT
	@cp $(DIST_FOLDER)/limine.cfg $(BIN_FOLDER)/isodir/boot
	@cp -r $(DIST_FOLDER)/resources $(BIN_FOLDER)/isodir/boot
	@cp $(BIN_FOLDER)/kernel.elf $(BIN_FOLDER)/isodir/boot
	@cp $(BIN_FOLDER)/ramdisk.tar.gz $(BIN_FOLDER)/isodir/boot

	@cp $(LIMINE_BIN)/limine-cd.bin $(LIMINE_BIN)/limine-eltorito-efi.bin $(BIN_FOLDER)/isodir/boot/limine
	@cp $(LIMINE_BIN)/limine.sys $(BIN_FOLDER)/isodir/boot
	@cp $(LIMINE_BIN)/BOOTX64.EFI $(BIN_FOLDER)/isodir/EFI/BOOT

	@xorriso -as mkisofs -b boot/limine/limine-cd.bin \
	        -no-emul-boot -boot-load-size 4 -boot-info-table -part_like_isohybrid \
	        -eltorito-alt-boot -e boot/limine/limine-eltorito-efi.bin \
	        -no-emul-boot $(BIN_FOLDER)/isodir -isohybrid-gpt-basdat -o $@
	@$(LIMINE_INSTALL) $@
