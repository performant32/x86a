#include <pch.h>
#include "asm_file.h"
#include "default_logger.h"

ASMFile::ASMFile(const char* path) : m_Path(path){
    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if(!file){
        x86a::getDefaultLogger()->error("Failed to open file {}", path);
        std::abort();
    }

    m_Data.resize(file.tellg());
    file.seekg(std::ios::in);
    file.read(const_cast<char*>(m_Data.data()), m_Data.size());
}
