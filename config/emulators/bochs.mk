bochs: all
	@# hacky way of changing the cd image location
	@printf "`cat config/emulators/bochsrc.bxrc)`" "$(BIN_FOLDER)/$(ISONAME)" > /tmp/bochsrc.bxrc

	@bochs -q -f /tmp/bochsrc.bxrc
