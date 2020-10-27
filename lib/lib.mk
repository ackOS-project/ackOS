SOURCES += \
		$(wildcard lib/*/*.cpp)

$(BIN)/lib/libc/%.o: lib/libc/%.cpp $(HEADERS)
	@mkdir -p $(BIN)/lib/libc
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)
