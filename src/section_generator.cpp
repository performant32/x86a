#include "asm_file.h"
#include "instruction_set.h"
#include "pch.h"
#include "section_generator.h"
#include "default_logger.h"
#include "section_container.h"
#include <ranges>
#include <source_location>

namespace x86a{
    SectionGenerator::SectionGenerator(const InstructionSet& instruction_set): m_InstructionSet(&instruction_set){
        getDefaultLogger()->debug("Symbol generator using instruction set {}", instruction_set.getName());
    }
    bool SectionGenerator::generate(const Tokenizer& tokenizer, SectionContainer& output){
        std::string section = "text";

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
                    case AddressingMode::Immediate:{
                        if(!Token::isLiteralType(type) || Token::getDataWidthFromTokenType(type) != operand.width){
                            getDefaultLogger()->error("Here {}", Token::getTokenName(type));
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
                            value.u32 = (uint16_t)argument.getMetadata();
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

            getDefaultLogger()->log("Found suitable instruction {} with {} args", mnemonic, operands.size());
            output.getSection(section).addInstruction(*suitableInstruction, std::move(operands));
        }
        return !generated_error;
    }
}
