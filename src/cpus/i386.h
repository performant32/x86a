#pragma once

#include "bitfields.h"
#include "instruction_set.h"
#include "macro_helper.h"
namespace x86a {
    class I386: public InstructionSet{
    public:
        //https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf
        //32 bit
        enum class EffectiveAddress{
            //used for |
            None=0,
            //e.g. [eax]
            EAXPtr  =0b000,
            ECXPtr  =0b001,
            EDXPtr  =0b010,
            EBXPtr  =0b011,
            ESIPtr  =0b110,
            EDIPtr  =0b111,

            //Registers
            EAX=        BITS(0b11, 3, 2),
            AX =        BITS(0b11, 3, 2),
            AL =        BITS(0b11, 3, 2),

            ECX=        BITS(0b11, 3, 2) | 0b1,
            CX =        BITS(0b11, 3, 2) | 0b1,
            CL =        BITS(0b11, 3, 2) | 0b1,

            EBX=        BITS(0b11, 3, 2) | 0b11,
            BX =        BITS(0b11, 3, 2) | 0b11,
            BL =        BITS(0b11, 3, 2) | 0b11,
            //[--][--]
            SIB         =0b100,
            Disp32      =0b101,
            
        };
        I386();

        std::string_view getEffectiveAddressName(EffectiveAddress address)const noexcept;

        std::span<const char* const, std::dynamic_extent> getKeywords()const noexcept;

        uint8_t createModRMByte(EffectiveAddress address, uint8_t register_opcode)const noexcept;
        std::optional<std::string> writeInstructionBytes(uint8_t* output, int* bytesWritten, const Instruction& instruction, const std::vector<OperandValue>& arguments)const override;
        const char* getName() const noexcept override {return "I386";}
    };
}
