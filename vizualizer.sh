#!/bin/bash

echo "Creating vizualizer..."
clang++ vizualization/main.cpp -o build/vizualizer
echo "Done!"

./build/vizualizer

dot -Tpng vizualization/vizualization.dot -o graph.png
