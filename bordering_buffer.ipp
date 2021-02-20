// Created by f.malashin on 16.09.2020.
//

#pragma once

#ifndef _BORDERING_BUFFER_IPP_INCLUDED_
#define _BORDERING_BUFFER_IPP_INCLUDED_

#include "bordering_buffer.h"

namespace ssa
{

template<typename Element, typename HandlingStructure>
HandlingStructure *BorderingBuffer<Element, HandlingStructure>::GetEmptyElement() {
    if (cyclePageBuffer_[emptyBorder_].GetStatus() == static_cast<int>(HandlingStructure::STATUS::EMPTY))
        return &cyclePageBuffer_[emptyBorder_];

    return nullptr;
}

template<typename Element, typename HandlingStructure>
HandlingStructure *BorderingBuffer<Element, HandlingStructure>::GetFilledElement() {
    if (cyclePageBuffer_[filledBorder_].GetStatus() == static_cast<int>(HandlingStructure::STATUS::FILLED))
        return &cyclePageBuffer_[filledBorder_];

    return nullptr;
}

template<typename Element, typename HandlingStructure>
void BorderingBuffer<Element, HandlingStructure>::IncrementEmptyBorder() {
    emptyBorder_ = (emptyBorder_ + 1) % bufferSize_;
}

template<typename Element, typename HandlingStructure>
void BorderingBuffer<Element, HandlingStructure>::IncrementFilledBorder() {
    filledBorder_ = (filledBorder_ + 1) % bufferSize_;
}

template<typename Element, typename HandlingStructure>
bool BorderingBuffer<Element, HandlingStructure>::IsBordersEqual() {
    return emptyBorder_ == filledBorder_;
}

template<typename Element, typename HandlingStructure>
void BorderingBuffer<Element, HandlingStructure>::Clear() {
    for (auto &x : cyclePageBuffer_)
        x.Clear();

    emptyBorder_ = 0;
    filledBorder_ = 0;
}

}

#endif //_BORDERING_BUFFER_IPP_INCLUDED_