all:
	clang++ Pass.cpp -c -fPIC -I`llvm-config --includedir` -o Pass.o
	clang++ Pass.o -fPIC -shared -o libPass.so
	clang -Xclang -load -Xclang ./libPass.so ./tests/factorial.c log.c
	# asfd clang -Xclang -load -Xclang ./libPass.so ./tests/factorial.c -emit-llvm -S -o llvm_ir.ll
