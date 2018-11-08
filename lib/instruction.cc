#include "../include/instruction.h"
#include "../include/instruction_set.h"


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
	ADDR_INDX,
	ADDR_INDY,
    };


unordered_map<addressing_mode, size_t> Shift::instruction_lengths {
    {ADDR_ACC, 2},
    {ADDR_ABSA, 5},
    {ADDR_ABSX, 6},
    {ADDR_ZERA, 6},
    {ADDR_ZERX, 7},
    {ADDR_INDX, 6},
    {ADDR_INDY, 5},
};

unordered_set<addressing_mode> Shift::allowed_modes {
    ADDR_ACC,
    ADDR_ABSA,
    ADDR_ABSX,
    ADDR_ZERA,
    ADDR_ZERX,
};

unordered_map<addressing_mode, size_t> Auxload::instruction_lengths {
    {ADDR_IMM, 2},
    {ADDR_ABSA, 3},
    {ADDR_ABSX, 4},
    {ADDR_ABSY, 4},
    {ADDR_ZERA, 4},
    {ADDR_ZERX, 4},
    {ADDR_ZERY, 4},
};

unordered_set<addressing_mode> Auxload::allowed_modes {
    ADDR_IMM,
    ADDR_ABSA,
    ADDR_ABSX,
    ADDR_ABSY,
    ADDR_ZERA,
    ADDR_ZERX,
    ADDR_ZERY,
};


unordered_map<addressing_mode, size_t> Noarg::instruction_lengths {
    {ADDR_NONE , 2},
};

unordered_set<addressing_mode> Noarg::allowed_modes {
    ADDR_NONE,
};

unordered_map<addressing_mode, size_t> Branch::instruction_lengths {
    {ADDR_LABEL, 2},
};

unordered_set<addressing_mode> Branch::allowed_modes {
    ADDR_LABEL,
};



unordered_map<addressing_mode, size_t> BIT::instruction_lengths {
    {ADDR_ZERA, 2},
    {ADDR_ABSA, 3},
};

unordered_set<addressing_mode> BIT::allowed_modes {
    ADDR_ZERA,
    ADDR_ABSA,
};



