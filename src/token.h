#pragma once

#include <pch.h>

namespace x86a{
    class Token{
    public:
        enum Type{
            Eof,
            Instruction,
            Symbol,
            String,
            Separator,
            Imm8,
            Imm16,
            Imm32,
            Imm64,
            Mem8,
            Mem16,
            Mem48,
            Reg8,
            Reg16
        };

        Token(uint32_t start, uint32_t end, Token::Type type);
        static const char* getTokenName(Token::Type type);

        uint32_t getStart()const noexcept{return m_Start;}
        uint32_t getEnd()const noexcept{return m_End;}
        Token::Type getTokenType()const noexcept{return m_TokenType;}
    private:
        uint32_t m_Start;
        uint32_t m_End;
        Token::Type m_TokenType;
    };
}
