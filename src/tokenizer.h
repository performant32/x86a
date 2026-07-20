#pragma once


#include "asm_file.h"
#include "token.h"

namespace x86a {
    class Tokenizer{
    public:
        enum Error{
            None=0,
            BadInstruction
        };
        struct ErrorType{
            Token token;
            uint32_t line;
            uint32_t column;
            Error error;
        };

        std::optional<ErrorType> tokenize(const ASMFile& file);
        const std::vector<Token>& getTokens()const noexcept{return m_Tokens;}
    private:
        std::vector<Token> m_Tokens;
    };
}
