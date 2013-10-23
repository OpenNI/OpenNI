#############################################################################
# Targets
#############################################################################
.PHONY: all clean clean-$(OUTPUT_FILE)

# define the target 'all' (it is first, and so, default)
all: $(OUTPUT_FILE)

# Intermediate directory
$(INT_DIR):
	mkdir -p $(INT_DIR)

# Output directory
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Final output file
$(OUTPUT_FILE): $(SRC_FILES_LIST) | $(OUT_DIR)

clean-$(OUTPUT_FILE):
	rm -rf $(OUTPUT_FILE)
	
clean: clean-$(OUTPUT_FILE)