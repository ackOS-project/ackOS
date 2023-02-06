ifeq ($(DEBUG_BREAKPOINT),)
gdb: $(KERNEL_SYMS)
	@printf "`cat config/debug/gdb.commands`" "$(KERNEL_SYMS)" "" > /tmp/tmp_gdb.commands
	@gdb -x /tmp/tmp_gdb.commands
else
gdb: $(KERNEL_SYMS)
	@printf "`cat config/debug/gdb.commands`" "$(KERNEL_SYMS)" "break $(DEBUG_BREAKPOINT)\ncontinue" > /tmp/tmp_gdb.commands
	@gdb -x /tmp/tmp_gdb.commands
endif
