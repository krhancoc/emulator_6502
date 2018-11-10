#ifndef __TYPES_H_
#define __TYPES_H_

enum class Reg { X, Y, A, P, S, Null};

enum class Mode {
    ABSOLUTE, ZERO_PAGE, INDIRECT,
    ABSOLUTE_INDEXED, ZERO_PAGE_INDEXED, INDEXED_INDIRECT,
    INDIRECT_INDEXED, IMMEDIATE, REGISTER
};


#endif
