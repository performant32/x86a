#pragma once

#include <pch.h>

namespace x86a{

    class Logger{
    public:
        inline Logger(){}
        inline virtual ~Logger(){}

        template<typename... Args>
        inline void debug(std::string_view fmt, Args&&... args){
            std::cout<<"Debug: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
        }

        template<typename... Args>
        inline void log(std::string_view fmt, Args&&... args){
            std::cout<<"Info: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
        }

        template<typename... Args>
        inline void warn(std::string_view fmt, Args&&... args){
            std::cout<<"Warn: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
        }

        template<typename... Args>
        inline void error(std::string_view fmt, Args&&... args){
            std::cerr<<"Error: " << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
        }

    private:
    };
}
