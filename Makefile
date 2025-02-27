# Define folders
SOFTWARE_DIR := Software
GENERATED_DIR := $(SOFTWARE_DIR)/Generated_ArduinoSketches

# Find all sketch directories containing .ino with the same namefiles
SKETCHES := $(shell \
  for f in $(SOFTWARE_DIR)/*/*.ino; do \
    d=$$(basename $$(dirname $$f)); \
    b=$$(basename $$f .ino); \
    if [ "$$d" = "$$b" ]; then echo $$f; fi; \
  done)
SKETCH_NAMES := $(basename $(notdir $(SKETCHES)))
ALL_SKETCH_NAMES := $(basename $(notdir $(wildcard $(SOFTWARE_DIR)/*/*.ino)))

# Colors for pretty printing
YELLOW  := \033[1;33m
GREEN   := \033[1;32m
BLUE    := \033[1;36m
RED     := \033[1;31m
RESET   := \033[0m

# Set default goal
.DEFAULT_GOAL := help

# Recursive function to collect dependencies and fix includes
define find_dependencies
	@grep -h -oE '#include "[^"]+"' $(1) 2>/dev/null | \
		sed 's/#include "\(.*\)"/\1/' | while read header; do \
			rel_path=$$header; \
			base_name=$$(basename $$header); \
			full_path=$$(find $(SOFTWARE_DIR) -type f -name "$$base_name" -not -path "$(GENERATED_DIR)/*"); \
			if [ -n "$$full_path" ]; then \
    			dest_path_header=$(GENERATED_DIR)/$(notdir $(2))/$$base_name; \
    			if [ ! -f "$$dest_path_header" ]; then \
    			    cp $$full_path $$dest_path_header; \
    			    echo "→ generated $$base_name"; \
    			fi; \
				sed -i '' -e "s|#include \"$$rel_path\"|#include \"$$base_name\"|g" $(GENERATED_DIR)/$(notdir $(2))/*; \
				cpp_path=$$(find $(SOFTWARE_DIR) -type f -name "$${base_name%.h}.cpp" -not -path "$(GENERATED_DIR)/*"); \
				dest_path_cpp=$(GENERATED_DIR)/$(notdir $(2))/$${base_name%.h}.cpp; \
				if [ -n "$$cpp_path" ] && [ ! -f "$$dest_path_cpp" ]; then \
					cp $$cpp_path $(GENERATED_DIR)/$(notdir $(2)); \
					echo "→ generated $${base_name%.h}.cpp"; \
				fi; \
				$(MAKE) find-dependencies FILE=$$full_path SKETCH=$(2); \
			fi; \
		done
endef

.PHONY: find-dependencies
find-dependencies:
	$(call find_dependencies,$(FILE),$(SKETCH))

# Function to remove previously generated arduino sketch
define clear_sketch
	@if [ -d "$(GENERATED_DIR)/$(1)" ]; then \
		rm -r "$(GENERATED_DIR)/$(1)"; \
		echo "🗑 Cleared previously generated sketch $(GENERATED_DIR)/$(1)"; \
	fi;
endef

# Rule to generate individual sketch
.PHONY: generate-% 
generate-%:
	$(call clear_sketch,$*)
	@echo "$(YELLOW)🚀 Generating $(notdir $*) ...$(RESET)"
	@mkdir -p $(GENERATED_DIR)/$(notdir $*)
	@cp  $(SOFTWARE_DIR)/$(notdir $*)/*.ino $(GENERATED_DIR)/$(notdir $*)
	@for file in $(SOFTWARE_DIR)/$(notdir $*)/*.ino $(SOFTWARE_DIR)/$(notdir $*)/*.h $(SOFTWARE_DIR)/$(notdir $*)/*.cpp; do \
		$(MAKE) find-dependencies FILE=$$file SKETCH=$*; \
	done
	@echo "$(GREEN)✅ $(notdir $*) generated in $(GENERATED_DIR)/$(notdir $*)$(RESET)"

# Rule to generate all sketches
.PHONY: generate
generate:
	@echo "$(YELLOW)🚀 Generating all sketches ...$(RESET)"
	@for sketch in $(SKETCH_NAMES); do \
		$(MAKE) generate-$(notdir $$sketch); \
	done
	@echo "$(GREEN)✅ All sketches generated!$(RESET)"

# Help command with valid commands
.PHONY: help
help:
	@echo "$(YELLOW)📌 Available commands:$(RESET)"
	@echo "$(GREEN)  make help$(RESET) - Show this help message"
	@echo "$(GREEN)  make generate$(RESET) - generate all sketches"
	@echo "$(GREEN)  make generate-$(BLUE)<ArduinoSketch>$(RESET) - Generate <ArduinoSketch> only"
	@echo "$(YELLOW)📂 Sketches found:$(RESET)"
	@for sketch in $(ALL_SKETCH_NAMES); do \
    	if echo "$(SKETCH_NAMES)" | grep -qw "$$sketch"; then \
     	   echo "$(BLUE)  $$sketch $(RESET)"; \
   		else \
        	echo "$(RED)  $$sketch$(RESET) - $$sketch.ino not inside directory with same name (expected $(SOFTWARE_DIR)/$$sketch/)"; \
    	fi; \
	done

# Rule to clean up generated files
.PHONY: clean
clean:
	@echo "🗑 Cleaning up generated Arduino files ..."
	@rm -rf $(GENERATED_DIR)
	@echo "$(GREEN)✅ Clean complete!$(RESET)"