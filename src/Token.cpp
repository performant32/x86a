#include "token.h"
#include "default_logger.h"

namespace x86a{
    Token::Token(uint32_t start, uint32_t end, TokenType type):
        m_Start(start),
        m_End(end),
        m_TokenType(type){
    }
    using TokenType = Token::TokenType;
    const char* Token::getTokenName(TokenType type){
        using TokenType = Token::TokenType;
        switch (type) {
            case TokenType::Instruction:
                return "Instruction";
            case TokenType::String:
                return "String";
            case TokenType::Imm8:
                return "Imm8";
            case TokenType::Imm16:
                return "Imm16";
            case TokenType::Mem8:
                return "Mem8";
            case TokenType::Mem16:
                return "Mem16";
            case TokenType::Eof:
                return "EOF";
            default:
                x86a::getDefaultLogger()->error("Unsupported token type {}", (int)type);
                std::abort();
        }
    }
}
