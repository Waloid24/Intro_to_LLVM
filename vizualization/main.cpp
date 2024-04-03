#include "vizualization.hpp"

int main (void)
{
    vizualizer::Driver driver("vizualization/static.txt", "vizualization/dynamic.txt", "vizualization/vizualization.dot");

    driver.process_static_file();
    driver.process_dynamic_file();
    driver.fill_vizualization_file();

}