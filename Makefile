.PHONY: all help build test clean reset cleanall

all:
	@echo "No target specified. Use 'make build', 'make test', 'make clean', 'make reset', or 'make cleanall'."
	@echo "Use 'make help' for more information."

help:
	@echo "Available targets:"
	@echo "  build    - Build the project."
	@echo "  test     - Run tests for the project."
	@echo "  clean    - Remove build artifacts."
	@echo "  reset    - Clear test output files (e.g., .dot and .png)."
	@echo "  cleanall - Perform a full clean including test outputs."
	@echo "  help     - Show this help message."

build:
	./scripts/build.sh
	@echo "Build completed successfully."

test:
	./scripts/test.sh
	@echo "Testing completed successfully."

clean:
	rm -rf build
	@echo "Build artifacts removed."

reset:
	rm -rf tests/*.dot
	rm -rf tests/*.png
	@rm -rf tests/*.ssa
	@echo "Test outputs cleared."

cleanall: clean reset
