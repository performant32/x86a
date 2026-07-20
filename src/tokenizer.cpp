#include <pch.h>
#include "tokenizer.h"
#include "token.h"

namespace x86a {
    std::optional<Tokenizer::ErrorType> Tokenizer::tokenize(const ASMFile& file){
        const std::vector<char>& data = file.getData();
        for(size_t i = 0; i < data.size(); i++){
            m_Tokens.emplace_back(Token{static_cast<uint32_t>(i), static_cast<uint32_t>(i + 1), Token::TokenType::Instruction});
        }
        m_Tokens.emplace_back(Token{0,0,Token::TokenType::Eof});
        return std::nullopt;
    }
}
