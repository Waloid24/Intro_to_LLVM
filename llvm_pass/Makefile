VIZUALIZATION_DIR := vizualization
PASS_DIR := pass
PASS_SERVICE_FILES := ${VIZUALIZATION_DIR}/static.txt ${VIZUALIZATION_DIR}/dynamic.txt
BUILD_DIR := build
EXEC_FILES := programm 

.PHONY: remove_pass_files remove_viz_files remove_build clean remove_exec_files

all: remove_pass_files

	@if [ ! -d "$(BUILD_DIR)" ]; then \
        	echo "Directory $(BUILD_DIR) does not exist."; \
		mkdir -p ${BUILD_DIR}; \
	fi

	@echo "Creating shared library..."
	@clang++ ${PASS_DIR}/Pass.cpp -c -fPIC -I`llvm-config --includedir` -o ${BUILD_DIR}/Pass.o
	@clang++ ${BUILD_DIR}/Pass.o -fPIC -shared -o ${BUILD_DIR}/libPass.so
	@echo "Building your programm..."
	@clang -Xclang -load -Xclang ./${BUILD_DIR}/libPass.so ./tests/factorial.c ${PASS_DIR}/log.c -o programm
	@echo "Done!"

clean: remove_viz_files remove_pass_files remove_build remove_exec_files

remove_exec_files:
	@for file in $(EXEC_FILES); do \
		if [ -f $$file ]; then \
			rm $$file; \
		fi \
	done

remove_build:
	@if [ -d "$(BUILD_DIR)" ]; then \
        echo "Directory $(BUILD_DIR) exists. Deleting it..."; \
        rm -rf "$(BUILD_DIR)"; \
    fi

remove_pass_files:
	@for file in $(PASS_SERVICE_FILES); do \
		if [ -f $$file ]; then \
			rm $$file; \
		fi \
	done

remove_viz_files:
	@if [ -f ${VIZUALIZATION_DIR}/vizualization.dot ]; then \
		rm ${VIZUALIZATION_DIR}/vizualization.dot; \
	fi \
	


