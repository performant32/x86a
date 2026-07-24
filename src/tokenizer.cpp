#include <pch.h>
#include "tokenizer.h"
#include "default_logger.h"
#include "token.h"

namespace x86a {
    Token Tokenizer::peek(size_t at)const noexcept{
        if(at >= m_Tokens.size())return Token{0,0,Token::Type::Eof};
        return m_Tokens[at];
    } // TODO: handle preprocessor directives
    std::optional<Tokenizer::ErrorType> Tokenizer::tokenize(const InstructionSet* instruction_set, const ASMFile& file){
        m_File = &file;
        const std::vector<char>& data = file.getData();
        uint32_t line = 1;
        uint32_t last_line_at = (uint32_t)-1;
        uint32_t column = 1;

        auto peek = [&data](int at){
            if(at < data.size())return data[at];
            return '\0';
        };
        auto is_valid_digit = [](char digit, int base){
            if(base == 2)return digit >= '0' && digit <= '1';
            if(base == 8)return digit >= '0' && digit <= '8';
            if(base == 16){
                if((digit >= 'a' && digit <= 'f') || (digit >= 'A' && digit <= 'F'))
                    return true;
            }
            return digit >= '0' && digit <= '9';
        };

        auto extract_number = [](char digit, int base){
            if(digit >= '0' && digit <= '9')return digit - '0';
            if((digit >= 'a' && digit <= 'f'))return (digit - 'a') + 10;
            else return (digit - 'A') + 10;

        };

        size_t i;
        for(i = 0; i < data.size();){
            last_line_at = i - last_line_at;
            char c = data[i];
            if(c == ' '){i++;continue;}
            if(c == '\r'){
                i++;
                if((c = data[i]) == '\n'){
                    line++;
                    column=1;
                    last_line_at = i;
                    i++;
                    continue;
                }
            }
            if(c == '\n'){
                line++;
                column=1;
                last_line_at = i;
                i++;
                continue;
            }
            if(c == ','){
                m_Tokens.emplace_back(Token{(uint32_t)i, (uint32_t)i+1, Token::Type::Separator});
                i++;
                continue;
            }
            if(std::isalpha(c)){
                //symbol or instruction
                size_t str_start = i;
                while(i < data.size() && std::isalpha(data[i]))
                    i++;
                size_t str_end = i;
                std::string_view str{data.data() + str_start, str_end - str_start};
                bool is_instruction = instruction_set->getInstructionsFromMnemonic(str) != instruction_set->getInstructionsEndIterator();
                uint32_t metadata = 0;
                Token::Type type = Token::Type::Instruction;

                if(!is_instruction){
                    if(auto reg = instruction_set->getRegister(str)){
                        type = Token::getRegisterTypeFromDataWidth(reg->width);
                        instruction_set->getInstructionsFromMnemonic(str);
                        auto it = instruction_set->getRegisterId(str);

                        if(!it){
                            getDefaultLogger()->error("FATAL, could not get register id");
                            return ErrorType("FATAL, could not get register id", line, column, Error::UnsupportedRegister);
                        }
                        metadata=(uint8_t)it.value();
                    }
                    else
                    type = Token::Type::String;
                }
                Token token(str_start, str_end, type, metadata);
                m_Tokens.emplace_back(token);
                continue;
            }
            if(std::isdigit(c)){
                int base = 10;
                if(c == '0'){
                    switch(peek(i+1)){
                    case 'B':
                    case 'b':
                        i+=2;
                        break;
                    case 'X':
                    case 'x':
                        base = 16;
                        i+=2;
                        break;
                    }
                }
                uint64_t number=0;
                size_t num_start = i;
                while(i < data.size() && is_valid_digit(data[i], base)){
                    number*=base;
                    number+=extract_number(data[i], base);
                    i++;
                }
                size_t num_end = i;

                std::string_view str{data.data() + num_start, num_end - num_start};
                //getDefaultLogger()->debug("Number {} real/decimal number {}", str, number);

                Token::Type type = Token::Type::Imm8;
                if(number > 0xFFFFFFFF)type = Token::Type::Imm64;
                else if(number > 0xFFFF)type = Token::Type::Imm32;
                else if(number > 0xFF)type = Token::Type::Imm16;
                Token token(num_start, num_end, type, number);
                m_Tokens.emplace_back(token);
                continue;
            }
            return Tokenizer::ErrorType{std::string_view(&data[i], sizeof(char)), line, column, Tokenizer::Error::UnexpectedSymbol};
        }
        m_Tokens.emplace_back(Token{static_cast<uint32_t>(i),static_cast<uint32_t>(i),Token::Type::Eof});
        return std::nullopt;
    }
}
