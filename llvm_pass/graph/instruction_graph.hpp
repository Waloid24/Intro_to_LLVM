#ifndef INSTRUCTION_GRAPH_HPP
#define INSTRUCTION_GRAPH_HPP

#include <iostream>
#include <fstream>
#include <string>

namespace graph {

class FileManager {

    std::string file_name_;
    
    public:

        FileManager(const std::string &file_name) : file_name_{file_name} {}

        void write_file(const std::string &data)
        {
            std::ofstream file(file_name_.data(), std::ios_base::app|std::ios_base::out);
            if (file.is_open())
            {
                file << data;
                file.close();
            }
            else
            {
                std::cerr << "Unable to open file: " << file_name_.data() << std::endl; //TODO: exception here
            }
        }

        ~FileManager() {}
};

FileManager& operator<<(FileManager &file_manager, const std::string &data)
{
    file_manager.write_file(data);
    return file_manager;
}

} /* end of namespace graph */


#endif