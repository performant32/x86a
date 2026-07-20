#include <pch.h>
#include "asm_file.h"
#include "default_logger.h"
#include "tokenizer.h"

using namespace x86a;
int main(int argc, char** argv){
    if(argc < 2){
        getDefaultLogger()->error("Error, invalid path");
        return -1;
    }
    ASMFile file(argv[1]);
    Tokenizer tokenizer;
    if(auto result = tokenizer.tokenize(file)){
        Tokenizer::ErrorType error = result.value();
        getDefaultLogger()->error("Error: {}:{}, {}", error.line, error.column);
        return -1;
    }
    for(const auto& token : tokenizer.getTokens()){
        getDefaultLogger()->debug("Token type {}", Token::getTokenName(token.getTokenType()));
    }

    //getDefaultLogger()->warn("Hello world {}", 5);
}
