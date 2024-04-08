#ifndef VIZUALIZATION_NEW_HPP
#define VIZUALIZATION_NEW_HPP

#include <unordered_map>
#include <string>
#include <fstream>
#include <list>
#include <iostream> 
#include <algorithm>
#include <vector>
#include <stack>

namespace vizualizer {

class Line final {

    std::string body_;

    std::vector<std::string> dst_;
    std::vector<int> arg_;
    std::vector<size_t> num_;
    std::vector<std::string> bb_name_;

    size_t rep_counter = 0;

    public:

        // Line(){}
        Line(std::string line, size_t num = 0) : body_{line}, num_{num}{
            rep_counter++;
        };

        void print(std::ofstream &output, size_t index = 0)
        {
            output << "\t\t\t<TR><TD port=\"" << num_[index] << "\">" << body_ << "</TD></TR>\n";
        }
        size_t get_line_num(size_t index=0)
        {
            return num_[index];
        }
        void add_dst(std::string dst)
        {
            dst_.push_back(dst);
        }
        void add_arg(int arg)
        {
            arg_.push_back(arg);
        }
        std::string get_dst(size_t index = 0)
        {
            return dst_[index];
        }
        int get_arg(size_t index = 0)
        {
            return arg_[index];
        }
        void add_bb_name(std::string bb_name)
        {
            bb_name_.push_back(bb_name);
        }
        std::string get_bb_name(size_t index = 0)
        {
            return bb_name_[index];
        }
        void increase_rep_counter()
        {
            ++rep_counter;
        }
        size_t get_rep_counter()
        {
            return rep_counter;
        }
};

class Base_block final {

    std::string name_;
    std::list<Line> body_;
    size_t calls_num_ = 0;
    size_t lines_num_ = 0;
    std::vector<std::list<Line>::iterator> call_lines_;
    std::vector<std::list<Line>::iterator> ret_lines_;

    public:
        Base_block(std::string &name) : name_{name} {}

        void print(std::ofstream &output)
        {
            output << "\t\tnode" << name_ << "[label=<\n";
            output << "\t\t<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n";
            output << "\t\t\t<TR><TD BORDER=\"1\" SIDES=\"TB\">bb: " << name_ << "</TD></TR>\n";
            for (auto line : body_)
            {
                line.print(output);
            }
            output << "\t\t</TABLE>\n";
            if (calls_num_ < 2)
                output << "\t\t>, shape=plaintext, style=filled, fillcolor=\"#8c96c6\"];\n";
            else if (calls_num_ >= 2 && calls_num_ < 5)
                output << "\t\t>, shape=plaintext, style=filled, fillcolor=\"#8c6bb1\"];\n";
            else
                output << "\t\t>, shape=plaintext, style=filled, fillcolor=\"#6e016b\", fontcolor=\"#ffffff\"];\n";
        }
        void add_element(Line &line)
        {
            body_.push_back(line);
            ++lines_num_;
        }
        std::list<Line>::iterator begin_element()
        {
            return body_.begin();
        }
        std::list<Line>::iterator end_element()
        {
            return body_.end();
        }
        std::string get_name() const
        {
            return name_;
        }
        size_t get_num_line() const
        {
            return lines_num_;
        }
        void increase_counter()
        {
            ++calls_num_;
        }
        size_t ret_size()
        {
            return ret_lines_.size();
        }
        std::vector<std::list<Line>::iterator>::iterator ret_lines_begin()
        {
            return ret_lines_.begin();
        }
        std::vector<std::list<Line>::iterator>::iterator ret_lines_end()
        {
            return ret_lines_.end();
        }
        void add_ret_line(std::list<Line>::iterator iter)
        {
            ret_lines_.push_back(iter);
        }
        size_t call_size()
        {
            return call_lines_.size();
        }
        std::vector<std::list<Line>::iterator>::iterator call_lines_begin()
        {
            return call_lines_.begin();
        }
        std::vector<std::list<Line>::iterator>::iterator call_lines_end()
        {
            return call_lines_.end();
        }
        void add_call_line(std::list<Line>::iterator iter)
        {
            call_lines_.push_back(iter);
        }

};

class Function final {

    std::string name_;
    std::list<Base_block> body_;
    using body_iter_ = std::list<Base_block>::iterator;
    std::unordered_map<std::string, std::list<Base_block>::iterator> body_map_;
    using body_map_iter = std::unordered_map<std::string, body_iter_>::iterator;
    std::list<body_map_iter> bb_ordering_;

    public:

        Function (std::string &name) : name_{name} {}

        void print(std::ofstream &output)
        {
            output << "\tsubgraph cluster_" << name_ << " {\n";
            output << "\t\trankdir=TB;\n\t\tstyle=filled;\n\t\tcolor=lightgrey;\n\t\tnode [style=filled,color=white];\n\t\t";
            output << "label = \"func: " << name_ << "\";" << std::endl;
            for (auto bb : body_)
            {
                bb.print(output);
            }
            output << "\t}" << std::endl;
        }

        void add_element(Base_block &bb)
        {
            body_.push_back(bb);
            body_map_[bb.get_name()] = std::prev(body_.end());
        }
        body_iter_ begin_element()
        {
            return body_.begin();
        }
        body_iter_ end_element()
        {
            return body_.end();
        }
        std::string get_name() const
        {
            return name_;
        }
        body_map_iter find_bb(std::string &name)
        {
            return body_map_.find(name);
        }
        body_map_iter body_map_begin() 
        {
            return body_map_.begin();
        }
        body_map_iter body_map_end() 
        {
            return body_map_.end();
        }
        void add_to_ordering(body_map_iter iter)
        {
            bb_ordering_.push_back(iter);
        }
        void create_links(std::ofstream &output)
        {
            std::unordered_map<std::string, Line> line_set;
            std::string str;
            for (auto cur = bb_ordering_.begin(), end = std::prev(bb_ordering_.end()); cur != end; ++cur)
            {
                str = "node" + (*cur)->first + " -> " + "node" + (*std::next(cur))->first;
                auto iter = line_set.find(str);
                if (iter == line_set.end())
                    line_set.emplace(str, str);
                else
                    iter->second.increase_rep_counter();
            }
            for (auto cur = line_set.begin(), end = line_set.end(); cur != end; ++cur)
            {
                size_t rep_counter = cur->second.get_rep_counter();
                if (cur->second.get_rep_counter() != 1)
                    output << cur->first << "[label = \"" << rep_counter << "\"]" << std::endl;
                else
                    output << cur->first << std::endl;
            }
        }
        std::list<body_map_iter>::iterator bb_ord_begin()
        {
            return bb_ordering_.begin();
        }
        std::list<body_map_iter>::iterator bb_ord_end()
        {
            return bb_ordering_.end();
        }

};

class Module final {


    std::list<Function> body_;
    using body_iter_ = std::list<Function>::iterator;
    std::unordered_map<std::string, body_iter_> body_map_;
    using body_map_iter = std::unordered_map<std::string, body_iter_>::iterator;
    std::list<body_map_iter> func_ordering_;

    public:
        void add_element(Function &func)
        {
            body_.push_back(func);
            body_map_[func.get_name()] = std::prev(body_.end());
        }
        std::list<Function>::iterator begin_element()
        {
            return body_.begin();
        }
        std::list<Function>::iterator end_element()
        {
            return body_.end();
        }
        body_map_iter find_func(std::string &name)
        {
            return body_map_.find(name);
        }
        body_map_iter body_map_begin() 
        {
            return body_map_.begin();
        }
        body_map_iter body_map_end() 
        {
            return body_map_.end();
        }
        void add_to_ordering(body_map_iter iter)
        {
            func_ordering_.push_back(iter);
        }
        std::list<body_map_iter>::iterator func_ordering_begin()
        {
            return func_ordering_.begin();
        }
        void create_links(std::ofstream &output)
        {
            for (auto func = body_.begin(), end = body_.end(); func != end; ++func)
            {
                func->create_links(output);

                auto bb_ord_begin = func->begin_element();
                auto bb_ord_end = func->end_element();
                for (auto bb = bb_ord_begin, end = bb_ord_end; bb != end; ++bb)
                {
                    if (bb->ret_size() != 0)
                    {
                        auto ret_line = bb->ret_lines_begin();
                        auto end = bb->ret_lines_end();

                        size_t index = 0;
                        for (; ret_line != end; ++ret_line)
                        {
                            output << "\tnode" << bb->get_name() << ":" << (*ret_line)->get_line_num(index) << " -> " << "node" << (*ret_line)->get_bb_name(index) <<  ":" 
                                << std::stoi((*ret_line)->get_dst(index))+1 << "[label = \"args: " 
                                << (*ret_line)->get_arg(index) << "\"]" << std::endl;
                            ++index;
                        }
                    }
                    else if (bb->call_size() != 0)
                    {
                        auto call_line = bb->call_lines_begin();
                        auto end_line = bb->call_lines_end();
                        for (; call_line != end_line; ++call_line)
                        {
                            output << "\tnode" << bb->get_name() << ":" << (*call_line)->get_line_num() << " -> " << "node" 
                                << (*call_line)->get_dst() << "[label = \"args: " 
                                << (*call_line)->get_arg() << "\"]" << std::endl;
                        }
                    }
                }
            }
        }
};

class Driver final {

    std::ifstream static_file_;
    std::ifstream dynamic_file_;
    std::ofstream vizualization_file_;
    Module body_;

    public:
        Driver(const std::string &stat_file_name, const std::string &dyn_file_name, const std::string &viz_file_name) 
                : static_file_{stat_file_name}
                , dynamic_file_{dyn_file_name}
                , vizualization_file_{viz_file_name} {}

        void process_static_file()
        {
            std::string line;
            std::string name;
            size_t num_line;
            
            while (std::getline(static_file_, line))
            {
                auto first_token = get_first_token(line.begin(), line.end());
                if (first_token == "function")
                {
                    name = line.substr(sizeof("function"));
                    Function func(name);
                    body_.add_element(func);
                }
                else if (first_token == "bb")
                {
                    num_line = 0;
                    name = line.substr(sizeof("bb"));
                    Base_block bb(name);
                    std::prev(body_.end_element())->add_element(bb);
                }
                else
                {
                    Line str(line, num_line);
                    std::prev(std::prev(body_.end_element())->end_element())->add_element(str);
                    ++num_line;
                }
            }
        }

        void process_dynamic_file()
        {
            int ret_value = 0;
            std::string::iterator iter;
            std::string line;
            std::string prev_func_name;
            auto func_ordering_iter = body_.func_ordering_begin();

            std::stack<std::pair<std::string, std::string>> call_ordering;
            call_ordering.emplace("main", "0");

            while (std::getline(dynamic_file_, line))
            {
                auto first_token = get_first_token(line.begin(), line.end());
                iter = line.begin();

                if (first_token == "ret")
                {
                    std::advance(iter, sizeof("ret"));
                    first_token = get_first_token(iter, line.end());
                    std::advance(iter, first_token.length() + sizeof(' '));
                    std::string cur_bb_name = get_first_token(iter, line.end());
                    std::advance(iter, cur_bb_name.length()+sizeof(' '));
                    std::string str_num_line = get_first_token(iter, line.end());
                    int num_line = std::stoi(str_num_line);
                    std::advance(iter, str_num_line.length()+sizeof(' '));
                    int ret_value = std::stoi(get_first_token(iter, line.end()));

                    if (prev_func_name != first_token)
                        body_.add_to_ordering(body_.find_func(first_token));

                    auto bb = body_.find_func(first_token)->second->find_bb(cur_bb_name);
                    auto bb_begin = bb->second->begin_element();        

                    std::advance(bb_begin, num_line);

                    auto ordering_top = call_ordering.top();

                    bb_begin->add_bb_name(ordering_top.first);
                    bb_begin->add_dst(ordering_top.second);
                    bb_begin->add_arg(ret_value);
                    bb->second->add_ret_line(bb_begin);

                    prev_func_name = first_token;
                    call_ordering.pop();
                }
                else if (first_token == "call")
                {
                    std::advance(iter, sizeof("call"));
                    first_token = get_first_token(iter, line.end());

                    std::advance(iter, first_token.length() + sizeof(" ->"));
                    std::string callee_func = get_first_token(iter, line.end());

                    auto callee_func_iter = body_.find_func(callee_func);
                    if (callee_func_iter != body_.body_map_end())
                    {
                        if (prev_func_name != first_token)
                            body_.add_to_ordering(body_.find_func(first_token));

                        std::advance(iter, callee_func.length()+sizeof(' '));
                        std::string cur_bb_name = get_first_token(iter, line.end());
                        std::advance(iter, cur_bb_name.length()+sizeof(' '));
                        std::string str_num_line = get_first_token(iter, line.end());
                        int num_line = std::stoi(str_num_line);
                        std::advance(iter, str_num_line.length()+sizeof(' '));
                        int arg = std::stoi(get_first_token(iter, line.end()));

                        auto bb = body_.find_func(first_token)->second->find_bb(cur_bb_name);
                        auto bb_begin = bb->second->begin_element(); 
                        
                        std::advance(bb_begin, num_line);
                        bb_begin->add_dst(callee_func_iter->second->begin_element()->get_name());
                        bb_begin->add_arg(arg);
                        bb->second->add_call_line(bb_begin);

                        call_ordering.emplace(cur_bb_name, str_num_line);
                        prev_func_name = first_token;
                    }
                    
                }
                else
                {
                    auto funcs_iter = body_.find_func(first_token);
                    if (funcs_iter != body_.body_map_end())
                    {
                        if (prev_func_name != first_token)
                            body_.add_to_ordering(funcs_iter);
                        
                        std::advance(iter, first_token.length() + 1);

                        auto cur_bb_name = get_first_token(iter, line.end());
                        auto bb_iter = funcs_iter->second->find_bb(cur_bb_name);

                        if (bb_iter != funcs_iter->second->body_map_end())
                        {
                            bb_iter->second->increase_counter();
                            funcs_iter->second->add_to_ordering(funcs_iter->second->find_bb(cur_bb_name));
                        }
                        prev_func_name = first_token;
                    }
                    
                }
            }
        }

        void fill_vizualization_file()
        {
            vizualization_file_ << "digraph {\n\tcompound = true;\n\tnode [shape=plaintext, fontsize=12];" << std::endl;

            for (auto cur = body_.begin_element(), end = body_.end_element(); cur != end; ++cur)
            {
                cur->print(vizualization_file_);
            }

            body_.create_links(vizualization_file_);

            vizualization_file_ << "}" << std::endl;
        }

        std::string get_first_token(std::string::iterator start, std::string::iterator end)
        {
            auto spacePos = std::find(start, end, ' ');
            return std::string(start, spacePos);
        }

        ~Driver() {
            static_file_.close();
            dynamic_file_.close();
            vizualization_file_.close();
        }

};

}

#endif