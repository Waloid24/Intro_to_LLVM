VIZUALIZATION_DIR := vizualization
PASS_DIR := pass
FILES := ${VIZUALIZATION_DIR}/static.txt ${VIZUALIZATION_DIR}/dynamic.txt

# remove_files:
#     @for file in $(FILES); do\
#         if [ -f $$file ]; then\
#             rm $$file;\
#         fi\
#     done

# .PHONY: remove_files

all:
	@for file in $(FILES); do\
        if [ -f $$file ]; then\
            rm $$file;\
        fi\
    done
	# @if [ -d visualization ]; then \
    #     rm -rf visualization; \
    # fi
    # mkdir visualization

	clang++ ${PASS_DIR}/Pass.cpp -c -fPIC -I`llvm-config --includedir` -o Pass.o
	clang++ Pass.o -fPIC -shared -o libPass.so
	clang -Xclang -load -Xclang ./libPass.so ./tests/factorial.c ${PASS_DIR}/log.c
	# clang -Xclang -load -Xclang ./libPass.so ./tests/factorial.c -emit-llvm -S -o llvm_ir.ll
