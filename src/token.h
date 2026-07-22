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
            Mem32,
            Mem48,
            Reg8,
            Reg16,
            Reg32
        };

        static bool isLiteralType(Type type)noexcept;
        static bool isRegisterType(Type type)noexcept;

        /// @brief converts Immedate, registers, and memory types to their respective widths, (e.g. Imm8 -> 8, Imm16 - > 16, Mem16 -> 16)
        /// Memory types specify the size of the data they point to NOT the size of the pointer
        /// @return returns 0 if not suitable
        static int getDataWidthFromTokenType(Type type)noexcept;

        /// @brief converts and int(e.g. 16) to a Imm16 and other immediate types
        /// @return returns Type::Eof if not suitable
        static Type getImmediateTypeFromDataWidth(int width)noexcept;

        /// @brief converts and int(e.g. 16) to a Reg16 and other register types
        /// @return returns Type::Eof if not suitable
        static Type getRegisterTypeFromDataWidth(int width)noexcept;

        Token(uint32_t start, uint32_t end, Token::Type type);
        static const char* getTokenName(Token::Type type);

        uint32_t getStart()const noexcept{return m_Start;}
        uint32_t getEnd()const noexcept{return m_End;}
        Token::Type getType()const noexcept{return m_TokenType;}
    private:
        uint32_t m_Start;
        uint32_t m_End;
        Token::Type m_TokenType;
    };
}
