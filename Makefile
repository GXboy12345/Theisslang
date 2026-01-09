# Makefile for Theisslang

.PHONY: build clean install test help

# Default target
all: build

# Build the executable
build:
	python3 build.py

# Clean build artifacts
clean:
	rm -rf build dist build_venv *.spec

# Install the executable to /usr/local/bin (requires sudo)
install: build
	sudo cp dist/theisslang /usr/local/bin/theisslang
	sudo chmod +x /usr/local/bin/theisslang

# Test the built executable
test: build
	./dist/theisslang -e "MR. THEISS! Mister Theiss?"
	./dist/theisslang examples/hello_world.theiss

# Show help
help:
	@echo "Theisslang Build System"
	@echo ""
	@echo "Targets:"
	@echo "  build    - Build the standalone executable"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install executable to /usr/local/bin (requires sudo)"
	@echo "  test     - Test the built executable"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make build    # Build the executable"
	@echo "  make install  # Build and install system-wide"
	@echo "  make test     # Build and test"
