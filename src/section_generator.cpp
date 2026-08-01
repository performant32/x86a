#include "asm_file.h"
#include "instruction_set.h"
#include "pch.h"
#include "section_generator.h"
#include "default_logger.h"
#include "section_container.h"
#include "token.h"
#include <optional>
#include <ranges>
#include <source_location>

namespace x86a{
    SectionGenerator::SectionGenerator(const InstructionSet& instruction_set): m_InstructionSet(&instruction_set){
        getDefaultLogger()->debug("Symbol generator using instruction set {}", instruction_set.getName());
    }
    bool SectionGenerator::generate(const Tokenizer& tokenizer, SectionContainer& output){
        m_Tokenizer = &tokenizer;

        std::string name = ".text";

        const ASMFile* file = tokenizer.getFile();
        const std::vector<Token>& tokens = tokenizer.getTokens();
        bool generated_error = false;
        m_At = 0;
        for(m_At = 0; m_At < tokens.size() - 1;){
            Token t = tokens[m_At];
            if(t.getType() != Token::Type::Instruction){
                getDefaultLogger()->error("Expected instruction, got \"{}\" type {}", file->getString(t.getStart(), t.getEnd()), (int)t.getType());
                generated_error = true;
                m_At++;
                continue;
            }

            std::string_view mnemonic(file->getString(t.getStart(), t.getEnd()));
            InstructionSet::InstructionIterator possibleInstruction = m_InstructionSet->getInstructionsFromMnemonic(mnemonic);
            auto end = m_InstructionSet->getInstructionsEndIterator();
            if(possibleInstruction == end){
                getDefaultLogger()->error("Expected instruction, got {}", file->getString(t.getStart(), t.getEnd()));
                generated_error = true;
                m_At++;
                continue;
            }
            const Instruction* suitableInstruction = nullptr;
            std::vector<OperandValue> operands;

            size_t previousAt = ++m_At;
            while(possibleInstruction != end){
                const auto& instruction = possibleInstruction->second;
                getDefaultLogger()->debug("Got instruction {}", instruction.getMnemonic());

                operands.clear();
                for(size_t j = 0; j < instruction.getOperands().size(); j++){
                    if(j > 0){
                        Token separator = tokenizer.peek(m_At);
                        if(separator.getType() != Token::Type::Separator){
                            getDefaultLogger()->error("Expected Separator got {}", file->getString(separator.getStart(), separator.getEnd()));
                            break;
                        }
                        m_At++;
                    }
                    Token argument = tokenizer.peek(m_At);
                    Token::Type type = argument.getType();
                    getDefaultLogger()->debug("Argument {} has type {}, value {}", j, Token::getTokenName(argument.getType()), file->getString(argument.getStart(), argument.getEnd()));

                    const auto& operand = instruction.getOperands()[j];
                    AddressingMode mode = operand.mode;
                    switch(mode){
                    case AddressingMode::RM:{
                        std::optional<OperandValue> value = parseOperand();
                        if(!value){
                            generated_error = true;
                            goto end_instruction;
                        }
                        if(value.value().mode == AddressingMode::Immediate)goto end_instruction;
                        operands.emplace_back(std::move(value.value()));
                    }break;
                    case AddressingMode::DirectMemory:{
                        if(type != Token::Type::Index)goto end_instruction;
                        if((Token::Indexer)argument.getMetadata() != Token::Indexer::LeftBracket){
                            getDefaultLogger()->error("Unexpected {}, did you mean '['?", (char)argument.getMetadata());
                            generated_error = true;
                            m_At++;
                            continue;
                        }
                        m_At++;
                        argument = tokenizer.peek(m_At);
                        type = argument.getType();

                        //TODO: Parse symbol as relocatable memory location
                        bool inner_generated_error = false;
                        switch (type) {
                        case Token::Imm32:{
                            operands.emplace_back((OperandValue){.u32=argument.getMetadata(),.mode=AddressingMode::Immediate });
                        }break;
                        case Token::Type::Symbol:{
                            operands.emplace_back((OperandValue){.symbol=file->getString(argument.getStart(), argument.getEnd()), .mode=AddressingMode::DirectMemory, .has_symbol=true});
                        }break;
                        default:{
                            getDefaultLogger()->error("Unexpected {}, did you mean '['?", (char)argument.getMetadata());
                            inner_generated_error = true;
                            generated_error = true;
                            m_At++;
                        }break;
                        }
                        if(inner_generated_error)goto end_instruction;
                        m_At++;
                    }break;
                    case AddressingMode::Immediate:{
                        if(!Token::isLiteralType(type) || Token::getDataWidthFromTokenType(type) > operand.width){
                            goto end_instruction;
                        }
                        OperandValue value = (OperandValue){.mode=AddressingMode::Immediate};
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
                        m_At++;
                    }break;
                    case AddressingMode::Register:{
                        if(!Token::isRegisterType(type) || Token::getDataWidthFromTokenType(type) != operand.width){
                            goto end_instruction;
                        }
                        operands.emplace_back(OperandValue{.register_id=(uint8_t)argument.getMetadata(),.mode=AddressingMode::Register });
                        m_At++;

                    }break;

                    default:
                        getDefaultLogger()->debug("Unsupported operand type for instruction, type {} data {}", (int)argument.getType(), file->getString(argument.getStart(), argument.getEnd()));
                        goto end_instruction;
                    };
                }
                suitableInstruction = &instruction;
                break;

                end_instruction:
                m_At = previousAt;
                ++possibleInstruction;
            }
            if(!suitableInstruction){
                getDefaultLogger()->error("Failed to find suitable instruction for {}", mnemonic);
                generated_error = true;
                m_At++;
                continue;
            }

            Section& section = output.getSection(name);
            // 15 bytes is the max for an instruction
            uint8_t bytes[15];

            int bytesWritten = 0;
            if(auto error = m_InstructionSet->writeInstructionBytes(bytes, &bytesWritten, *suitableInstruction, operands)){
                getDefaultLogger()->error("Failed to generate code for {}", file->getPath().string());
                return false;
            }
            getDefaultLogger()->log("Instruction {} has {} arguments, wrote {} bytes", suitableInstruction->getMnemonic(), operands.size(), bytesWritten);
            std::string values;
            for(size_t i = 0; i < bytesWritten; i++){
                values += std::format("{:>5}: {:X}\n", i, bytes[i]);
            }
            getDefaultLogger()->log("Instruction bytes\n{}", values);
            for(size_t i = 0; i < bytesWritten; i++){
                section.addData(bytes[i]);
            }
            getDefaultLogger()->log("Found suitable instruction {} with {} args", mnemonic, operands.size());
        }
        getDefaultLogger()->log(".text section has {} bytes", output.getSection(".text").getData().size());
        return !generated_error;
    }
    std::optional<OperandValue> SectionGenerator::parseOperand(){
        const ASMFile* file = m_Tokenizer->getFile();
        Token token = m_Tokenizer->peek(m_At);
        OperandValue value = (OperandValue){0};

        bool using_segment = false;
        uint8_t segment_selector=0;

        switch(token.getType()){
            case Token::Type::Index:{
                if(token.getMetadata() != Token::Indexer::LeftBracket){
                    getDefaultLogger()->error("Expected left bracket, got {}" , (char)token.getMetadata());
                    return std::nullopt;
                }
                m_At++;
                std::optional<OperandValue> r = parseRegister();
                if(!r){
                    getDefaultLogger()->error("Error, expected register following '['");
                    return std::nullopt;
                }
                value.mode = AddressingMode::IndirectMemory;
                value.sib.index_rid = r.value().u8;
                token = m_Tokenizer->peek(m_At);
                if(token.getType() != Token::Type::Index && token.getMetadata() == Token::Indexer::RightBracket){
                    getDefaultLogger()->error("Expected left bracket, got {}" , (char)token.getMetadata());
                    return std::nullopt;
                }
                m_At++;
            }break;
            case Token::Type::Symbol:{
                value.symbol = file->getString(token.getStart(), token.getEnd());
                value.has_symbol = true;
                value.mode = AddressingMode::DirectMemory;
                m_At++;
            }break;
            case Token::Type::Imm8:
            case Token::Type::Imm16:
            case Token::Type::Imm32:{
                value.u32 = token.getMetadata();
                value.mode = AddressingMode::Immediate;
                m_At++;
                return value;
            }break;
            case Token::Type::Reg8:
            case Token::Type::Reg16:
            case Token::Type::Reg32:{
                return parseRegister();
            }break;
            default:{
                getDefaultLogger()->error("Unsupported operand type {}, got {}" , (int)token.getType(), file->getString(token.getStart(), token.getEnd()));
                return std::nullopt;
            }
        };
        return value;
    }
    std::optional<OperandValue> SectionGenerator::parseRegister(){
        const ASMFile* file = m_Tokenizer->getFile();
        Token token = m_Tokenizer->peek(m_At);
        switch(token.getType()){
            case Token::Type::Reg8:
            case Token::Type::Reg16:
            case Token::Type::Reg32:{
                OperandValue value = (OperandValue){0};
                value.u32 = token.getMetadata();
                value.mode = AddressingMode::Register;
                m_At++;
                return value;
            }
            default:
                return std::nullopt;
        }
    }
}
