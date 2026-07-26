#include "default_logger.h"
#include "pch.h"
#include <elf.h>
#include <unordered_map>
#include "code_generator.h"

namespace x86a{
    CodeGenerator::CodeGenerator(const InstructionSet& instruction_set):
        m_InstructionSet(&instruction_set){
    }
    std::optional<std::string> CodeGenerator::generate(const SectionContainer& sectionsContainer, std::filesystem::path output){
        const auto& path = sectionsContainer.getFile()->getPath();
        std::ofstream file(output, std::ios::out | std::ios::binary);
        if(!file){
            return std::format("Failed to open file ", output.string());
        }
        std::vector<char> file_data;
        file_data.resize(sizeof(Elf32_Ehdr));
        auto write = [&file_data](void* data, size_t n){
            size_t at = file_data.size();
            file_data.resize(at + n);
            memcpy(file_data.data() + at, data, n);
        };

        {
            Elf32_Ehdr* header=((Elf32_Ehdr*)file_data.data());
            *header = (Elf32_Ehdr){0};
            header->e_ident[EI_MAG0] = ELFMAG0;
            header->e_ident[EI_MAG1] = ELFMAG1;
            header->e_ident[EI_MAG2] = ELFMAG2;
            header->e_ident[EI_MAG3] = ELFMAG3;

            header->e_ident[EI_CLASS] = ELFCLASS32;
            header->e_ident[EI_DATA] = ELFDATA2LSB;
            header->e_ident[EI_VERSION] = EV_CURRENT;
            header->e_ident[EI_OSABI] = ELFOSABI_SYSV;
            header->e_ident[EI_ABIVERSION] = 0;
            header->e_ident[EI_PAD] = 0;

            header->e_type = ET_REL;
            header->e_machine = EM_386;
            header->e_version = EV_CURRENT;
        }

        using OffsetType = uint32_t;

        std::unordered_set<std::string_view> strings;
        std::vector<Elf32_Shdr> section_headers;

        section_headers.emplace_back((Elf32_Shdr){0});

        const std::vector<int> symbols;

        for(const auto& [name, section] : sectionsContainer.getSections()){
            std::vector<char> data;

            for(const auto& instruction : section.getInstructions()){
                // 15 bytes is the max for an instruction
                uint8_t bytes[15];

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
            }

            auto it = strings.insert(name);
            int str_at = 1 + std::distance(strings.begin(), it.first);

            Elf32_Shdr shdr = (Elf32_Shdr){0};
            shdr.sh_size = data.size();
            shdr.sh_name = str_at;
            shdr.sh_offset = file_data.size();
            shdr.sh_type = SHT_PROGBITS;

            if(name == ".text"){
                shdr.sh_flags = SHF_ALLOC | SHF_EXECINSTR;
                shdr.sh_entsize = data.size();
            }else if(name == ".data"){
                shdr.sh_flags = SHF_WRITE | SHF_ALLOC;
            }
            section_headers.emplace_back(shdr);
            file_data.insert(file_data.end(), data.cbegin(), data.cend());
        }
        strings.insert(".symtab");
        strings.insert(".strtab");
        strings.insert("_start");

        std::vector<char> section_data;
        section_data.emplace_back('\0');
        for(const auto& str : strings){
            for(const auto& c : str){
                section_data.emplace_back(c);
            }
            section_data.emplace_back(0);
        }
        getDefaultLogger()->log("Section data size {}", section_data.size());

        auto findSection = [&strings](const std::string& str){
            return 1 + std::distance(strings.begin(), strings.find(str));
        };
        Elf32_Shdr string_section = (Elf32_Shdr){0};
        string_section.sh_name = findSection(".strtab");
        string_section.sh_offset = file_data.size();
        //string_section.sh_entsize = sizeof(Elf32_Shdr);
        string_section.sh_size = section_data.size();
        string_section.sh_flags = SHF_ALLOC;
        string_section.sh_type = SHT_STRTAB;
        string_section.sh_flags = 0;
        //string_section.sh_info = strings.size();
        //string_section.sh_link = section_headers.size()+1;
        getDefaultLogger()->warn("Section is {}", section_headers.size());
        section_headers.emplace_back(string_section);
        file_data.insert(file_data.end(), section_data.cbegin(), section_data.cend());

        
        Elf32_Shdr symbol_table = (Elf32_Shdr){0};
        symbol_table.sh_type = SHT_SYMTAB;
        symbol_table.sh_flags = SHF_ALLOC;
        symbol_table.sh_name = findSection(".symtab");
        symbol_table.sh_entsize = sizeof(Elf32_Sym);
        symbol_table.sh_offset=file_data.size();
        symbol_table.sh_size = sizeof(Elf32_Sym) * 2;
        symbol_table.sh_link = section_headers.size()-1;
        symbol_table.sh_info = 2;
        section_headers.emplace_back(symbol_table);

        section_data.clear();

        Elf32_Sym symbol = (Elf32_Sym){0};
        write(&symbol, sizeof(symbol));

        symbol = (Elf32_Sym){0};
        symbol.st_name = findSection("_start");
        // yes harcoded for now
        symbol.st_shndx = 1;
        symbol.st_value = 0;
        symbol.st_info = STT_FUNC;
        write(&symbol, sizeof(symbol));
        //file_data.insert(file_data.end(), section_data.cbegin(), section_data.cend());

        OffsetType sh_offset = file_data.size();

        for(size_t i = 0; i < section_headers.size(); i++){
            uint32_t at = file_data.size();
            file_data.resize(at + sizeof(Elf32_Shdr));
            Elf32_Shdr* shdr = (Elf32_Shdr*)(file_data.data()+at);
            getDefaultLogger()->debug("Offset is {}", section_headers[i].sh_offset);
            memcpy((char*)shdr, (char*)&section_headers[i], sizeof(Elf32_Shdr));
        }
        getDefaultLogger()->log("sections {}", section_headers.size());

        {
            Elf32_Ehdr* header=((Elf32_Ehdr*)file_data.data());
            header->e_shoff = sh_offset;
            header->e_shentsize = sizeof(Elf32_Shdr);
            header->e_shnum = section_headers.size();
            header->e_shstrndx = section_headers.size()-1;
        }

        file.write(file_data.data(), file_data.size());
        file.close();
        return std::nullopt;
    }
}
