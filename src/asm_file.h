#pragma once

#include "pch.h"

class ASMFile{
public:
    std::optional<std::string> open(std::string_view path);

    inline std::string_view getString(size_t begin, size_t end)const noexcept{
        return std::string_view(m_Data.data() + begin, end - begin);
    }
    const std::filesystem::path& getPath()const noexcept{return m_Path;}
    const std::vector<char>& getData()const noexcept{return m_Data;}
private:
    std::filesystem::path m_Path;
    std::vector<char> m_Data;
};
