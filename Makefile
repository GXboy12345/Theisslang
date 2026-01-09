# Makefile for Theisslang

.PHONY: build build-native clean install test test-native help

# Default target
all: build

# Build the Python-based executable
build:
	python3 build.py

# Build the native C executable
build-native:
	@echo "Building native C executable..."
	gcc -O2 -Wall -Wextra -o theisslang-native theisslang.c
	@echo "Native executable built: ./theisslang-native"

# Clean build artifacts
clean:
	rm -rf build dist build_venv *.spec theisslang-native

# Install the executable to /usr/local/bin (requires sudo)
install: build
	sudo cp dist/theisslang /usr/local/bin/theisslang
	sudo chmod +x /usr/local/bin/theisslang

# Install the native executable to /usr/local/bin (requires sudo)
install-native: build-native
	sudo cp theisslang-native /usr/local/bin/theisslang
	sudo chmod +x /usr/local/bin/theisslang

# Test the Python-based built executable
test: build
	./dist/theisslang -e "MR. THEISS! Mister Theiss?"
	./dist/theisslang examples/hello_world.theiss

# Test the native C executable
test-native: build-native
	./theisslang-native -e "MR. THEISS! Mister Theiss?"
	./theisslang-native examples/hello_world.theiss

# Show help
help:
	@echo "Theisslang Build System"
	@echo ""
	@echo "Targets:"
	@echo "  build         - Build the Python-based standalone executable"
	@echo "  build-native  - Build the native C executable (no Python dependency)"
	@echo "  clean         - Remove all build artifacts"
	@echo "  install       - Install Python executable to /usr/local/bin (requires sudo)"
	@echo "  install-native- Install native executable to /usr/local/bin (requires sudo)"
	@echo "  test          - Test the Python-based executable"
	@echo "  test-native   - Test the native C executable"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make build-native    # Build fast native executable"
	@echo "  make install-native  # Build and install native version system-wide"
	@echo "  make test-native     # Build and test native version"
