#pragma once
#include <pch.h>

alignas(1) struct Registers{
    union{
        union{
            struct{
                uint8_t al;
                uint8_t ah;
            };
            uint16_t ax;
        };
        uint32_t eax;
    };
    union{
        union{
            struct{
                uint8_t bl;
                uint8_t bh;
            };
            uint16_t bx;
        };
        uint32_t ebx;
    };
    union{
        union{
            struct{
                uint8_t cl;
                uint8_t ch;
            };
            uint16_t cx;
        };
        uint32_t ecx;
    };
    union{
        union{
            struct{
                uint8_t dl;
                uint8_t dh;
            };
            uint16_t dx;
        };
        uint32_t edx;
    };
    union{
        union{
            struct{
                uint8_t spl;
                uint8_t sph;
            };
            uint16_t sp;
        };
        uint32_t esp;
    };
    union{
        union{
            struct{
                uint8_t bpl;
                uint8_t bph;
            };
            uint16_t bp;
        };
        uint32_t ebp;
    };
    union{
        union{
            struct{
                uint8_t sil;
                uint8_t sih;
            };
            uint16_t si;
        };
        uint32_t esi;
    };
    union{
        union{
            struct{
                uint8_t dil;
                uint8_t dih;
            };
            uint16_t di;
        };
        uint32_t edi;
    };
    uint16_t cs;
    uint16_t ss;
    uint16_t ds;
    uint16_t es;
};
