#pragma once


#include "asm_file.h"
#include "token.h"

namespace x86a {
    class Tokenizer{
    public:
        enum Error{
            None=0,
            UnexpectedSymbol,
            InvalidImmediate
        };
        struct ErrorType{
            std::string_view data;
            uint32_t line;
            uint32_t column;
            Error error;
        };

        std::optional<ErrorType> tokenize(const ASMFile& file);

        const ASMFile* getFile() const noexcept{return m_File;}
        const std::vector<Token>& getTokens()const noexcept{return m_Tokens;}

    private:
        const ASMFile* m_File=nullptr;
        std::vector<Token> m_Tokens;
    };
}
