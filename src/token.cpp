#include "token.h"
#include "default_logger.h"

namespace x86a{
    Token::Token(uint32_t start, uint32_t end, Token::Type type, uint32_t metadata):
        m_Start(start),
        m_End(end),
        m_TokenType(type),
        m_Metadata(metadata){
    }
    using TokenType = Token::Type;
    bool Token::isLiteralType(Type type)noexcept{
        switch (type) {
        case Type::Imm8:
        case Type::Imm16:
        case Type::Imm32:
            return true;
        default:return false;
        }

    }
    bool Token::isRegisterType(Type type)noexcept{
        switch (type) {
        case Type::Reg8:
        case Type::Reg16:
        case Type::Reg32:
            return true;
        default:return false;
        }
    }

    int Token::getDataWidthFromTokenType(Type type)noexcept{
        switch (type) {
        case Type::Reg8:
        case Type::Imm8:
        case Type::Mem8:
            return 8;
        case Type::Reg16:
        case Type::Imm16:
        case Type::Mem16:
            return 16;
        case Type::Reg32:
        case Type::Imm32:
        case Type::Mem32:
            return 32;
        default:return 0;
        }
    }
    const char* Token::getTokenName(TokenType type){
        using TokenType = Token::Type;
        switch (type) {
            case TokenType::Instruction:return "Instruction";
            case TokenType::Keyword:return "Keyword";
            case TokenType::String:return "String";
            case TokenType::Symbol:return "Symbol";
            case TokenType::Separator:return "Separator";
            case TokenType::Imm8:return "Imm8";
            case TokenType::Imm16:return "Imm16";
            case TokenType::Imm32:return "Imm32";
            case TokenType::Imm64:return "Imm64";
            case TokenType::Mem8:return "Mem8";
            case TokenType::Mem16:return "Mem16";
            case TokenType::Mem32:return "Mem32";
            case TokenType::Reg8:return "Reg8";
            case TokenType::Reg16:return "Reg16";
            case TokenType::Reg32:return "Reg32";
            case TokenType::Index:return "Index";
            case TokenType::Offset:return "Offset";
            case TokenType::Scale:return "scale";
            case TokenType::Eof:return "EOF";
            default:
                x86a::getDefaultLogger()->error("Unsupported token type {}", (int)type);
                std::abort();
        }
    }
    Token::Type Token::getImmediateTypeFromDataWidth(int width)noexcept{
        switch (width) {
            case 8:return Type::Imm8;
            case 16:return Type::Imm16;
            case 32:return Type::Imm32;
        default:
            return Type::Eof;
        }
    }
    Token::Type Token::getRegisterTypeFromDataWidth(int width)noexcept{
        switch (width) {
            case 8:return Type::Reg8;
            case 16:return Type::Reg16;
            case 32:return Type::Reg32;
        default:
            return Type::Eof;
        }
    }
}
