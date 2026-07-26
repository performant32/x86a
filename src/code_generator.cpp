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
            header->e_ehsize = sizeof(Elf32_Ehdr);
        }

        using OffsetType = uint32_t;

        std::unordered_set<std::string_view> shstrings;
        std::unordered_set<std::string_view> strings;
        std::vector<Elf32_Shdr> section_headers;

        section_headers.emplace_back((Elf32_Shdr){0});

        const std::vector<int> symbols;

        for(const auto& [name, section] : sectionsContainer.getSections()){
            shstrings.insert(name);
        }

        shstrings.insert(".symtab");
        shstrings.insert(".strtab");
        shstrings.insert(".shstrtab");
        strings.insert("_start");

        std::vector<char> section_data;
        // str table
        section_data.emplace_back('\0');
        for(const auto& str : strings){
            for(const auto& c : str){
                section_data.emplace_back(c);
            }
            section_data.emplace_back(0);
        }
        OffsetType str_section_at = file_data.size();
        OffsetType str_section_size = section_data.size();
        file_data.insert(file_data.end(), section_data.cbegin(), section_data.cend());

        auto findStrIndex = [&file_data, str_section_at, size=str_section_size](const std::string_view& str){
            for(size_t i = 0; i < size; i++){
                if(strncmp(file_data.data() + str_section_at + i, str.data(), str.size()) == 0){
                    return i;
                }
            }
            return (size_t)0;
        };
        // sh str table
        section_data.clear();
        section_data.emplace_back('\0');
        for(const auto& str : shstrings){
            for(const auto& c : str){
                section_data.emplace_back(c);
            }
            section_data.emplace_back(0);
        }
        OffsetType sh_str_section_at = file_data.size();
        OffsetType sh_str_section_size = section_data.size();
        file_data.insert(file_data.end(), section_data.cbegin(), section_data.cend());

        auto findShStrIndex = [&file_data, sh_str_section_at, size=sh_str_section_size](const std::string_view& str){
            for(size_t i = 0; i < size; i++){
                if(strncmp(file_data.data() + sh_str_section_at + i, str.data(), str.size()) == 0){
                    return i;
                }
            }
            return (size_t)0;
        };

        for(const auto& [name, section] : sectionsContainer.getSections()){
            getDefaultLogger()->debug("Adding section \"{}\"", name);
            const auto& data = section.getData();
            Elf32_Shdr shdr = (Elf32_Shdr){0};
            shdr.sh_size = data.size();
            shdr.sh_name = findShStrIndex(name);
            shdr.sh_addralign = 1;
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

        Elf32_Shdr shstring_section = (Elf32_Shdr){0};
        shstring_section.sh_name = findShStrIndex(".shstrtab");
        shstring_section.sh_offset = sh_str_section_at;
        shstring_section.sh_size = sh_str_section_size;
        shstring_section.sh_type = SHT_STRTAB;
        shstring_section.sh_flags = 0;
        shstring_section.sh_addralign = 1;
        section_headers.emplace_back(shstring_section);

        Elf32_Shdr string_section = (Elf32_Shdr){0};
        string_section.sh_name = findShStrIndex(".strtab");
        string_section.sh_offset = str_section_at;
        string_section.sh_size = str_section_size;
        string_section.sh_type = SHT_STRTAB;
        string_section.sh_flags = 0;
        string_section.sh_addralign = 1;
        section_headers.emplace_back(string_section);
        
        Elf32_Shdr symbol_table = (Elf32_Shdr){0};
        symbol_table.sh_type = SHT_SYMTAB;
        symbol_table.sh_name = findShStrIndex(".symtab");
        symbol_table.sh_entsize = sizeof(Elf32_Sym);
        symbol_table.sh_offset=file_data.size();
        symbol_table.sh_size = sizeof(Elf32_Sym) * 3;
        symbol_table.sh_link = section_headers.size()-1;
        symbol_table.sh_info = 2;//3 symbols
        symbol_table.sh_addralign = 1;//2 symbols
        section_headers.emplace_back(symbol_table);

        section_data.clear();

        Elf32_Sym symbol = (Elf32_Sym){0};
        write(&symbol, sizeof(symbol));

        symbol = (Elf32_Sym){0};
        symbol.st_name = findStrIndex(".text");
        // yes harcoded for now
        symbol.st_shndx = 1;
        symbol.st_value = 0;
        symbol.st_other = STV_DEFAULT;
        symbol.st_info = ELF64_ST_INFO(STB_GLOBAL, STT_SECTION);
        //symbol.st_other
        write(&symbol, sizeof(symbol));

        symbol = (Elf32_Sym){0};
        symbol.st_name = findStrIndex("_start");
        // yes harcoded for now
        symbol.st_shndx = 1;
        symbol.st_other = STV_DEFAULT;
        symbol.st_info = ELF64_ST_INFO(STB_GLOBAL, STT_NOTYPE);
        //symbol.st_other
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
            header->e_shstrndx = section_headers.size()-3;
        }

        file.write(file_data.data(), file_data.size());
        file.close();
        return std::nullopt;
    }
}
