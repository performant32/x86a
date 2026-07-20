#pragma once

#include <pch.h>

namespace x86a{
    class Token{
    public:
        enum TokenType{
            Eof,
            Instruction,
            String,
            Imm8,
            Imm16,
            Mem8,
            Mem16,
            Mem48,
            Reg8,
            Reg16
        };

        Token(uint32_t start, uint32_t end, TokenType type);
        static const char* getTokenName(TokenType type);

        uint32_t getStart()const noexcept{return m_Start;}
        uint32_t getEnd()const noexcept{return m_End;}
        TokenType getTokenType()const noexcept{return m_TokenType;}
    private:
        uint32_t m_Start;
        uint32_t m_End;
        TokenType m_TokenType;
    };
}
