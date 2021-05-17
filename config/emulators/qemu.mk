qemu: all
	@qemu-system-x86_64 $(QEMU_FLAGS) -enable-kvm -cdrom $(BIN_FOLDER)/$(ISONAME)

qemu-debug: all strip-symbols
	@sudo qemu-system-x86_64 $(QEMU_FLAGS) -cdrom $(BIN_FOLDER)/$(ISONAME) -s -S -d int -no-reboot -monitor telnet:127.0.0.1:55555,server,nowait;
