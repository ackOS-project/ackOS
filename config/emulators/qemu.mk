QEMU_FLAGS += \
			-chardev stdio,id=char0,signal=on \
			-chardev file,id=char1,path=$(VM_LOGFILE),signal=off \
			-serial chardev:char0 \
			-serial chardev:char1 \
			-m $(VM_MEMORY)M

qemu: all
	@qemu-system-x86_64 $(QEMU_FLAGS) -enable-kvm -cdrom $(BIN_FOLDER)/$(ISONAME)

qemu-no-kvm: all
	@qemu-system-x86_64 $(QEMU_FLAGS) -cdrom $(BIN_FOLDER)/$(ISONAME)

qemu-debug: all strip-symbols
	@qemu-system-x86_64 $(QEMU_FLAGS) -cdrom $(BIN_FOLDER)/$(ISONAME) -s -S -d int -no-reboot -monitor telnet:127.0.0.1:55555,server,nowait;
