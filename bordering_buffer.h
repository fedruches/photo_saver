//
// Created by f.malashin on 16.09.2020.
//

#pragma once

#ifndef _BORDERING_BUFFER_H_INCLUDED_
#define _BORDERING_BUFFER_H_INCLUDED_

#include "structures.h"

#include <iostream>
#include <atomic>
#include <array>
#include <mutex>

namespace ssa
{

template<typename Element, typename HandlingStructure>
class BorderingBuffer
{
public:
    BorderingBuffer() : emptyBorder_{0}, filledBorder_{0} {}

    explicit BorderingBuffer(uint16_t bufferSize) : bufferSize_{bufferSize},
                                                    emptyBorder_{0},
                                                    filledBorder_{0},
                                                    cyclePageBuffer_{bufferSize_}
                                                    {}

    void IncrementEmptyBorder();

    void IncrementFilledBorder();

    HandlingStructure* GetEmptyElement();

    HandlingStructure* GetFilledElement();

    bool IsBordersEqual();

    void Clear();

private:
    std::size_t bufferSize_{0};

    size_t emptyBorder_{0};

    size_t filledBorder_{0};

    std::vector<HandlingStructure> cyclePageBuffer_{};
};

}

#include "bordering_buffer.ipp"

#endif //_BORDERING_BUFFER_H_INCLUDED_
