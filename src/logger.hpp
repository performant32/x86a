#pragma once

#include <pch.h>

namespace x86a{

    class Logger{
    public:
        const char* RED = "\033[31m";
        const char* GREEN = "\033[32m";
        const char* BLUE = "\033[34m";
        const char* YELLOW = "\033[33m";
        const char* RESET = "\033[0m";

        inline Logger(){}
        inline virtual ~Logger(){}

        template<typename... Args>
        inline void debug(std::string_view fmt, Args&&... args){
            std::cout<<GREEN << "Debug: " << std::vformat(fmt, std::make_format_args(args...)) << RESET << std::endl;
        }

        template<typename... Args>
        inline void log(std::string_view fmt, Args&&... args){
            std::cout<<BLUE<<"Info: " << std::vformat(fmt, std::make_format_args(args...)) << RESET << std::endl;
        }

        template<typename... Args>
        inline void warn(std::string_view fmt, Args&&... args){
            std::cout<<YELLOW<<"Warn: " << std::vformat(fmt, std::make_format_args(args...)) << RESET << std::endl;
        }

        template<typename... Args>
        inline void error(std::string_view fmt, Args&&... args){
            std::cerr<<RED<<"Error: " << std::vformat(fmt, std::make_format_args(args...)) << RESET <<std::endl;
        }

    private:
    };
}
