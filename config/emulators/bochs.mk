bochs: all
	@# hacky way of changing the cd image location
	@printf "`cat config/emulators/bochsrc.bxrc`" "$(OS_IMAGE)" "$(VM_LOGFILE)" > /tmp/bochsrc.bxrc

	@bochs -q -f /tmp/bochsrc.bxrc
