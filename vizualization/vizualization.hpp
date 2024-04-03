#ifndef VIZUALIZATION_HPP
#define VIZUALIZATION_HPP

#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <iostream> 
#include <algorithm>

namespace vizualizer {

class Base_block {

    std::string name_;
    std::vector<std::string> body_;
    size_t num_requests_;


    public:
        Base_block(){};
        Base_block(std::string &name) : name_{name}{};

        std::string& get_name()
        {
            return name_;
        }
        void add_name(std::string &name)
        {
            name_ = name;
        }
        void add_body_str(std::string &line)
        {
            body_.push_back(line);

        }
        void increase_counter()
        {
            num_requests_++;
        }

        size_t LOG_bb_size()
        {
            return body_.size();
        }

        void print_bb(std::ofstream &output)
        {
            output << "\t\tnode" << name_ << "[label=<\n";
            output << "\t\t<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n";
            output << "\t\t\t<TR><TD BORDER=\"1\" SIDES=\"TB\">bb: " << name_ << "</TD></TR>\n";
            std::cout << "$$$$ 'print_bb' size: " << body_.size() << std::endl;
            for (auto cur = body_.begin(), end = body_.end(); cur != end; ++cur)
            {
                output << "\t\t\t<TR><TD>" << *cur << "</TD></TR>\n";
            }
            output << "\t\t</TABLE>\n";
            output << "\t\t>];\n";
        }
};

class Function {

    // std::string name_;
    using bb_iter = std::unordered_map<std::string, Base_block>::iterator;
    std::unordered_map<std::string, Base_block> body_;
    std::vector<bb_iter> bb_ordering_;
    int ret_value_;

    const char* function_header = "\t\trankdir=TB;\n\t\tstyle=filled;\n\t\tcolor=lightgrey;\n\t\tnode [style=filled,color=white];\n\t\t";

    public:
        Function(){};
        // Function(std::string name) : name_{name} {};

        void add_bb(Base_block& bb)
        {
            body_[bb.get_name()] = bb;
        }

        void add_to_used_bb(bb_iter bb_iter)
        {
            bb_ordering_.push_back(bb_iter);
        }

        bb_iter find_bb(std::string &name)
        {
            return body_.find(name);
        }

        bb_iter end_body()
        {
            return body_.end();
        }

        void set_ret_value(int &ret_value)
        {
            ret_value_ = ret_value;
        }

        void print_func_header(std::ofstream &output, std::string name)
        {
            output << "\tsubgraph cluster_" << name << " {\n";
            output << function_header;
        }

        void print_func_name(std::ofstream &output, std::string name)
        {
            output << "label = \"func: " << name << "\";" << std::endl;
        }

        void print_func_body(std::ofstream &output)
        {
            for (auto cur = body_.begin(), end = body_.end(); cur != end; ++cur)
            {
                cur->second.print_bb(output);
            }
        }

        void print_func_end(std::ofstream &output)
        {
            output << "\t}" << std::endl;
        }

        void print_bb_links(std::ofstream &output)
        {
            for (auto cur = bb_ordering_.begin(), end = std::prev(bb_ordering_.end()); cur != end; ++cur)
            {
                output << "node" << (*cur)->first << " -> " << "node" << (*std::next(cur))->first << std::endl;
            }
        }

        std::string get_first_bb_name()
        {
            return (*bb_ordering_.begin())->first;
        }
};

class Driver {

    std::ifstream static_file_;
    std::ifstream dynamic_file_;
    std::ofstream vizualization_file_;

    using funcs_iter = std::unordered_map<std::string, Function>::iterator;
    std::unordered_map<std::string, Function> funcs_;

    std::vector<funcs_iter> funcs_ordering_;

    const char* file_header = "digraph {\n\tcompound = true;\n\tnode [shape=plaintext, fontsize=12];";

    public:

        Driver(const std::string &stat_file_name, const std::string &dyn_file_name, const std::string &viz_file_name) 
                : static_file_{stat_file_name}
                , dynamic_file_{dyn_file_name}
                , vizualization_file_{viz_file_name} {}
                
        void process_static_file()
        {
            std::string line;
            std::string func_name;
            Base_block bb;

            while (std::getline(static_file_, line))
            {
                auto first_token = get_first_token(line.begin(), line.end());
                if (first_token == "function")
                {
                    func_name = line.substr(9);
                    auto insert_res = funcs_.emplace(func_name, Function{});
                    funcs_ordering_.push_back(insert_res.first);
                }
                else if (first_token == "bb")
                {
                    Base_block bb = process_bb(line);
                    funcs_[func_name].add_bb(bb);
                }
                else
                {
                    std::cerr << "Undefined behaviour" << std::endl;
                }
            }
        }

        void process_dynamic_file()
        {
            std::string line;

            while (std::getline(dynamic_file_, line))
            {
                int ret_value = 0;
                size_t offset = 0;
                
                bool is_ret = false;

                auto first_token = get_first_token(line.begin(), line.end());

                if (first_token == "ret")
                {
                    is_ret = true;
                    first_token = get_first_token(line.begin()+sizeof("ret"), line.end());
                    size_t ret_value_len = first_token.length();
                    ret_value = std::stoi(first_token);
                    offset = sizeof("ret")+ret_value_len+1;
                    first_token = get_first_token(line.begin()+offset, line.end());
                }

                auto funcs_iter = funcs_.find(first_token);
                funcs_ordering_.push_back(funcs_iter); // вставили в список функций

                offset += first_token.length() + 1;

                funcs_iter->second.set_ret_value(ret_value);
                
                auto bb_name = get_first_token(line.begin()+offset, line.end());
                auto bb_iter = funcs_iter->second.find_bb(bb_name);

                if (bb_iter != funcs_iter->second.end_body() && !is_ret)
                {
                    bb_iter->second.increase_counter();
                    funcs_iter->second.add_to_used_bb(funcs_iter->second.find_bb(bb_name)); // вставили в список базовых блоков
                }
            }
        }

        void fill_vizualization_file()
        {
            vizualization_file_ << file_header << std::endl;

            for (auto cur = funcs_.begin(), end = funcs_.end(); cur != end; ++cur)
            {
                cur->second.print_func_header(vizualization_file_, cur->first);
                cur->second.print_func_name(vizualization_file_, cur->first);
                cur->second.print_func_body(vizualization_file_);
                cur->second.print_func_end(vizualization_file_);
            }

            create_links(vizualization_file_);

            vizualization_file_ << "}" << std::endl;
        }

        ~Driver() {
            static_file_.close();
            dynamic_file_.close();
            vizualization_file_.close();
        }
    
    private:

        void create_link_functions(std::ofstream &output)
        {
            for (auto cur = funcs_ordering_.begin(), end = std::prev(funcs_ordering_.end()); cur != end; ++cur)
            {
                output << "node" << (*cur)->second.get_first_bb_name() << " -> " << "node" << (*std::next(cur))->second.get_first_bb_name() << std::endl;
            }
        }

        void create_links(std::ofstream &output)
        {
            for (auto cur = funcs_.begin(), end = funcs_.end(); cur != end; ++cur)
            {
                cur->second.print_bb_links(output);
            }
            create_link_functions(output);
        }

        Base_block process_bb(std::string &line)
        {
            std::string bb_name = line.substr(3);
            Base_block bb(bb_name);

            while (std::getline(static_file_, line))
            {
                line = line.substr(line.find_first_not_of(" \t"));
                auto first_token = get_first_token(line.begin(), line.end());
                if (first_token != "bb" && first_token != "function")
                {
                    // if (first_token != "ret")
                        bb.add_body_str(line);
                }
                else
                {
                    std::streampos pos = static_file_.tellg();
                    if (pos != -1) 
                    {
                        static_file_.seekg((size_t)pos - line.length() - sizeof('\n'));
                    } else {
                        std::cerr << "Не удалось определить смещение в файле!" << std::endl;
                    }
                    break;
                }
            }

            std::cout << "result bb size = " << bb.LOG_bb_size() << std::endl;
            return bb;
        }

        std::string get_first_token(std::string::iterator start, std::string::iterator end)
        {
            auto spacePos = std::find(start, end, ' ');
            return std::string(start, spacePos);
        }

};

} /* namespace vizualizer */


#endif
