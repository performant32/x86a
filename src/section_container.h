#pragma once

#include "asm_file.h"
#include "instruction_set.h"

namespace x86a{


    class Section{
    public:
    public:
        Section(std::string_view name);

        void addData(char data)noexcept;

        const std::vector<char>& getData()const noexcept{return m_Data;}
        const std::string& getName()const noexcept{return m_Name;}
    private:
        std::vector<char> m_Data;
        std::string m_Name;
    };

    class SectionContainer{
    public:
        struct StringHash {
            using is_transparent = void;
            [[nodiscard]] size_t operator()(const char *txt) const {
                //The FNV-1a Hash Algorithm
                constexpr std::size_t fnv_prime = 1099511628211ULL;
                constexpr std::size_t fnv_offset_basis = 14695981039346656037ULL;
                
                std::size_t hash = fnv_offset_basis;
                for (size_t i = 0; i < strlen(txt); i++) {
                    char c = txt[i];
                    if(c >= 'a' && c <= 'z')c -= 'a' - 'A';
                    hash ^= static_cast<std::size_t>(c);
                    hash *= fnv_prime;
                }
                return hash;
            }
            [[nodiscard]] size_t operator()(std::string_view txt) const {
                //The FNV-1a Hash Algorithm
                constexpr std::size_t fnv_prime = 1099511628211ULL;
                constexpr std::size_t fnv_offset_basis = 14695981039346656037ULL;
                
                std::size_t hash = fnv_offset_basis;
                for(char c : txt){
                    if(c >= 'a' && c <= 'z')c -= 'a' - 'A';
                    hash ^= static_cast<std::size_t>(c);
                    hash *= fnv_prime;
                }
                return hash;
            }
            [[nodiscard]] size_t operator()(const std::string &txt) const {
                //The FNV-1a Hash Algorithm
                constexpr std::size_t fnv_prime = 1099511628211ULL;
                constexpr std::size_t fnv_offset_basis = 14695981039346656037ULL;
                
                std::size_t hash = fnv_offset_basis;
                for(char c : txt){
                    if(c >= 'a' && c <= 'z')c -= 'a' - 'A';
                    hash ^= static_cast<std::size_t>(c);
                    hash *= fnv_prime;
                }
                return hash;
            }
        };
        using SectionMap = std::unordered_map<std::string, Section,StringHash, std::equal_to<>>;
        SectionContainer(const ASMFile* file);
        Section& getSection(std::string_view name);

        const ASMFile* getFile() const noexcept{return m_File;}
        const SectionMap& getSections() const noexcept{return m_Sections;}
    private:
        const ASMFile* m_File = nullptr;
        SectionMap m_Sections;
    }; 
}
