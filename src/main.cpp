#include "pch.h"
#include "main.h"
#include "asm_file.h"
#include "default_logger.h"
#include "section_container.h"
#include "instruction_set.h"
#include "cpus/i386.h"
#include "section_generator.h"
#include "code_generator.h"
#include "tokenizer.h"

using namespace x86a;
int main(int argc, char** argv){
    std::vector<std::string_view> args;
    for(size_t i = 1; i < argc; i++){
        args.emplace_back(argv[i]);
    }

    x86a::Main main;
    return (int)main.run(std::move(args));

}
namespace x86a{
    std::string_view Main::peek(size_t at)const noexcept{
        if(at >= m_Args.size())return "";
        return m_Args[at];
    }

    ExitCode Main::run( std::vector<std::string_view>&& args){
        m_Args = std::move(args);
        std::vector<std::string_view> sources;
        std::string_view output_path=".";
        for(size_t i ={0u}; i < m_Args.size(); i++){
            const auto& arg = m_Args[i];
            if(arg == "-o"){
                if(peek(i+1) == ""){
                    getDefaultLogger()->error("Error, flag \"-o\": Expected output file path or directory");
                    return ExitCode::InvalidFlag;
                }
                output_path = m_Args[++i];
                continue;
            }
            sources.emplace_back(arg);
        }
        I386 instruction_set;

        bool tokenizer_stage_failed = false;
        std::vector<ASMFile> files;
        std::vector<Tokenizer> tokenizers;
        for(const auto& string : sources){
            files.emplace_back();
            ASMFile& file = files.at(files.size() - 1);
            if(auto msg = file.open(string)){
                getDefaultLogger()->error("Failed to open file \"{}\", error: {}", file.getPath().string(), msg.value());
                tokenizer_stage_failed = true;
                continue;
            }
            getDefaultLogger()->debug("Tokenizing {0}", file.getPath().string());
            Tokenizer tokenizer;
            if(auto result = tokenizer.tokenize(&instruction_set, file)){
                Tokenizer::ErrorType error = result.value();
                getDefaultLogger()->error("{}:\tError: {}:{}, {}", file.getPath().string(), error.line, error.column, error.data);
                tokenizer_stage_failed = true;
            }
            for(const auto& token : tokenizer.getTokens()){
                std::string_view str{file.getData().data() + token.getStart(), token.getEnd() - token.getStart()};
                getDefaultLogger()->debug("Token type {}, data {}", Token::getTokenName(token.getType()), str);
            }
            tokenizers.emplace_back(std::move(tokenizer));
        }
        if(tokenizer_stage_failed){
            getDefaultLogger()->error("Exiting with code {}", ExitCode::TokenizingFailed);
            return ExitCode::TokenizingFailed;
        }

        bool symbol_generation_failed = false;
        SectionGenerator section_generator{instruction_set};
        std::vector<SectionContainer> sections;
        for(const auto& tokenizer : tokenizers){
            getDefaultLogger()->log("Generating symbol table for {0}\n\n", tokenizer.getFile()->getPath().string());
            SectionContainer section(tokenizer.getFile());
            if(!section_generator.generate(tokenizer, section)){
                getDefaultLogger()->error("Failed building {}", tokenizer.getFile()->getPath().string());
                symbol_generation_failed = true;
            }
            sections.emplace_back(std::move(section));
        }

        if(symbol_generation_failed){
            getDefaultLogger()->error("Exiting with code {}", ExitCode::SymbolGenerationFailed);
            return ExitCode::SymbolGenerationFailed;
        }

        bool code_generation_failed = false;
        CodeGenerator code_generator(instruction_set);
        // TODO: pass flags to generator
        for(const auto& section_map : sections){
            auto path = section_map.getFile()->getPath();
            auto file_output = path.replace_extension(".o");
            if(auto result = code_generator.generate(section_map, file_output)){
                getDefaultLogger()->error("Code generation failed: {}", result.value());
                code_generation_failed = true;
                std::filesystem::remove(path);
            }

        }
        if(code_generation_failed == true){
            getDefaultLogger()->error("Exiting with code {}", ExitCode::SymbolGenerationFailed);
            return ExitCode::SymbolGenerationFailed;
        }
        getDefaultLogger()->log("Building elf files with output path {}", output_path);
        return ExitCode::None;
    }
}
