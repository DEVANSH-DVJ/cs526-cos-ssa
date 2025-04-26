.PHONY: all help build clean cleanall test

all:
	@echo "No target specified. Use 'make build', 'make test', 'make clean', or 'make cleanall'."
	@echo "Use 'make help' for more information."

help:
	@echo "Available targets:"
	@echo "  build    - Build the project."
	@echo "  test     - Test the project."
	@echo "  clean    - Clean the build artifacts."
	@echo "  cleanall - Clean all artifacts including test results."
	@echo "  help     - Show this help message."

build:
	./scripts/build.sh
	@echo "Build complete."

clean:
	rm -rf build
	@echo "Clean complete."

cleanall: clean
	rm -f tests/*.dot
	rm -f tests/*.png
	@echo "Clean complete."

test:
	./scripts/test.sh
	@echo "Test complete."
