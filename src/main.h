#pragma once

#include "pch.h"

namespace x86a {
    enum class ExitCode{
        None=0,
        TokenizingFailed,
        SymbolGenerationFailed,
        InvalidFlag
    };
    class Main{
    public:
        ExitCode run(std::vector<std::string_view>&& args);
    private:
        std::string_view peek(size_t at)const noexcept;
        std::vector<std::string_view> m_Args;
    };
}
// Yes this is unnecessary but i dont feel like casting to ints all the time
template <>
struct std::formatter<x86a::ExitCode>{
    //Handles {} and cases like {:x}
    constexpr auto parse(std::format_parse_context& ctx){
        auto it = ctx.begin();
        if(it != ctx.end() && *it != '}'){
            throw std::format_error("Sorry, bad specifier for error code");
        }
        return it;
    }
    template<typename FormatContext>
    auto format(const x86a::ExitCode& code, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}", (int) code);
    }
};
