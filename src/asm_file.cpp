#include <pch.h>
#include "asm_file.h"
#include "default_logger.h"

std::optional<std::string> ASMFile::open(std::string_view path){
    m_Path = path;
    std::ifstream file(m_Path, std::ios::in | std::ios::binary | std::ios::ate);
    if(!file){
        return "file DNE or has invalid permissions";
    }

    m_Data.resize(file.tellg());
    file.seekg(std::ios::beg);
    file.read(const_cast<char*>(m_Data.data()), m_Data.size());
    size_t bytes_read = file.tellg();
    if(bytes_read != m_Data.size()){
        x86a::getDefaultLogger()->error("Failed to read all contents of {}", m_Path.c_str());
        return "Failed to read contents";
    }
    return std::nullopt;
}
