#include "../include/instruction.h"


unordered_map<addressing_mode, size_t> Binary::instruction_lengths {
        {ADDR_IMM, 2},
	{ADDR_ABSA, 4},
	{ADDR_ABSX, 4},
	{ADDR_ABSY, 4},
	{ADDR_ZERA, 3},
	{ADDR_ZERX, 4},
};

unordered_set<addressing_mode> Binary::allowed_modes {
	ADDR_IMM,
	ADDR_ABSA,
	ADDR_ABSX,
	ADDR_ABSY,
	ADDR_ZERA,
	ADDR_ZERX,
    };


unordered_map<addressing_mode, size_t> Shift::instruction_lengths {
    {ADDR_ACC, 2},
    {ADDR_ABSA, 5},
    {ADDR_ABSX, 6},
    {ADDR_ZERA, 6},
    {ADDR_ZERX, 7},
};

unordered_set<addressing_mode> Shift::allowed_modes {
    ADDR_ACC,
    ADDR_ABSA,
    ADDR_ABSX,
    ADDR_ZERA,
    ADDR_ZERX,
};

