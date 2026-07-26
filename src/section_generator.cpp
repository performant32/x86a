#include "asm_file.h"
#include "instruction_set.h"
#include "pch.h"
#include "section_generator.h"
#include "default_logger.h"
#include "section_container.h"
#include "token.h"
#include <ranges>
#include <source_location>

namespace x86a{
    SectionGenerator::SectionGenerator(const InstructionSet& instruction_set): m_InstructionSet(&instruction_set){
        getDefaultLogger()->debug("Symbol generator using instruction set {}", instruction_set.getName());
    }
    bool SectionGenerator::generate(const Tokenizer& tokenizer, SectionContainer& output){
        std::string name = ".text";

        const ASMFile* file = tokenizer.getFile();
        const std::vector<Token>& tokens = tokenizer.getTokens();
        bool generated_error = false;
        for(size_t i = 0; i < tokens.size() - 1;){
            Token t = tokens[i];
            if(t.getType() != Token::Type::Instruction){
                getDefaultLogger()->error("Expected instruction, got \"{}\" type {}", file->getString(t.getStart(), t.getEnd()), (int)t.getType());
                generated_error = true;
                i++;
                continue;
            }

            getDefaultLogger()->debug("Searching for instruction {}", file->getString(t.getStart(), t.getEnd()));
            std::string_view mnemonic(file->getString(t.getStart(), t.getEnd()));
            InstructionSet::InstructionIterator possibleInstruction = m_InstructionSet->getInstructionsFromMnemonic(mnemonic);
            auto end = m_InstructionSet->getInstructionsEndIterator();
            if(possibleInstruction == end){
                getDefaultLogger()->error("Expected instruction, got {}", file->getString(t.getStart(), t.getEnd()));
                generated_error = true;
                i++;
                continue;
            }
            const Instruction* suitableInstruction = nullptr;
            std::vector<OperandValue> operands;

            while(possibleInstruction != end){
                const auto& instruction = possibleInstruction->second;
                getDefaultLogger()->debug("Got instruction {}", instruction.getMnemonic());
                size_t at = i+1;

                operands.clear();
                for(size_t j = 0; j < instruction.getOperands().size(); j++){
                    if(j > 0){
                        Token separator = tokenizer.peek(at);
                        if(separator.getType() != Token::Type::Separator){
                            getDefaultLogger()->error("Expected Separator got {}", file->getString(separator.getStart(), separator.getEnd()));
                            break;
                        }
                        at++;
                    }
                    Token argument = tokenizer.peek(at);
                    Token::Type type = argument.getType();
                    getDefaultLogger()->debug("Argument {} has type {}, value {}", j, Token::getTokenName(argument.getType()), file->getString(argument.getStart(), argument.getEnd()));
                    const auto& operand = instruction.getOperands()[j];
                    switch(operand.mode){
                    //case AddressingMode::Symbol:
                    case AddressingMode::DirectMemory:{
                        if(type != Token::Type::Index)goto end_instruction;
                        if((Token::Indexer)argument.getMetadata() != Token::Indexer::LeftBracket){
                            getDefaultLogger()->error("Unexpected {}, did you mean '['?", (char)argument.getMetadata());
                            generated_error = true;
                            i++;
                            continue;
                        }
                        i++;
                        argument = tokenizer.peek(at);
                        type = argument.getType();

                        //TODO: Parse symbol as relocatable memory location
                        bool inner_generated_error = false;
                        switch (type) {
                        default:{
                            getDefaultLogger()->error("Unexpected {}, did you mean '['?", (char)argument.getMetadata());
                            inner_generated_error = true;
                            generated_error = true;
                            i++;
                        }break;
                        }
                        if(inner_generated_error)goto end_instruction;
                    }break;
                    case AddressingMode::Immediate:{
                        if(!Token::isLiteralType(type) || Token::getDataWidthFromTokenType(type) != operand.width){
                            goto end_instruction;
                        }
                        OperandValue value;
                        switch(type){
                        case Token::Type::Imm8:
                            value.u8 = (uint16_t)argument.getMetadata();
                            break;
                        case Token::Type::Imm16:
                            value.u16 = (uint16_t)argument.getMetadata();
                            break;
                        case Token::Type::Imm32:
                            value.u32 = (uint32_t)argument.getMetadata();
                            break;
                        default:
                            // should prob NEVER get called unless but
                            getDefaultLogger()->error("Unsupported token type {}:{}", std::source_location::current().line(), std::source_location::current().line());
                            std::abort();
                        }
                        operands.emplace_back(value);
                    }break;
                    case AddressingMode::Register:{
                        if(!Token::isRegisterType(type) || Token::getDataWidthFromTokenType(type) != operand.width){
                            getDefaultLogger()->error("Expected register, got {}, ", file->getString(argument.getStart(), argument.getEnd()));
                            goto end_instruction;
                        }
                        operands.emplace_back(OperandValue{.register_id=(uint8_t)argument.getMetadata()});
                    }break;

                    default:
                        goto end_instruction;
                    };
                    at++;
                }
                i = at;
                suitableInstruction = &instruction;
                break;

                end_instruction:
                ++possibleInstruction;
            }
            if(!suitableInstruction){
                getDefaultLogger()->error("Failed to find suitable instruction for {}", mnemonic);
                generated_error = true;
                i++;
                continue;
            }

            Section& section = output.getSection(name);
            // 15 bytes is the max for an instruction
            uint8_t bytes[15];

            /*
            int bytesWritten = 0;
            if(auto error = m_InstructionSet->writeInstructionBytes(bytes, &bytesWritten, instruction.getInstruction(), instruction.getOperandValues())){
                return std::format("Failed to generate code for {}", path.string());
            }
            getDefaultLogger()->log("Instruction {} has {} arguments, wrote {} bytes", instruction.getMnemonic(), instruction.getOperandValues().size(), bytesWritten);
            std::string values;
            for(size_t i = 0; i < bytesWritten; i++){
                values += std::format("{:>5}: {:X}\n", i, bytes[i]);
            }
            getDefaultLogger()->log("Instruction bytes\n{}", values);
            data.reserve(data.size()+bytesWritten);
            for(size_t i = 0; i < bytesWritten; i++){
                data.emplace_back(bytes[i]);
            }
            */

            uint8_t b[] = {
                0xb8, 0x1, 0x00, 0x00, 0x00,
                0xbb, 0x29, 0x00, 0x00, 0x00,
                0xcd, 0x80,                   
            };
            for(size_t i = 0; i < sizeof(b); i++){
                section.addData(b[i]);
            }

            getDefaultLogger()->log("Found suitable instruction {} with {} args", mnemonic, operands.size());
        }
        return !generated_error;
    }
}
