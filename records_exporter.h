//
// Created by fedor on 11.01.2021.
//

#ifndef _JOURNAL_RECORDS_EXPORT_RECORDS_EXPORTER_H_
#define _JOURNAL_RECORDS_EXPORT_RECORDS_EXPORTER_H_

#include <array>
#include <fstream>

#include "cef_converter.h"
#include "journal/bordering_buffer.h"
#include "journal/buffer_handling_structures.hpp"

namespace ssa
{

template<class TRecordWrapper>
class RecordsExporter
{
public:
    enum class INSERT_STATUS : uint8_t
    {
        ADDED = 1,
        OVERFILLED = 2
    };

    explicit RecordsExporter(uint16_t bufferSize,
                             std::atomic_bool &init) :
            bufferSize_{bufferSize},
            init_{init},
            edrBorderingBuffer_{bufferSize_},
            recordsExportThread_{&RecordsExporter<TRecordWrapper>::ExportFunc, this}
            {}

    RecordsExporter() = default;

    ~RecordsExporter();

    // TODO подумать над возвращаемым значением
    void Insert(const TRecordWrapper& record);

private:
    void ExportFunc();

private:
    uint16_t bufferSize_{0};

    std::atomic_bool &init_;

    size_t bufferBorder_{0};

    BorderingBuffer<TRecordWrapper, RecordsExportProcessingStructure> edrBorderingBuffer_{};

    std::thread recordsExportThread_{};

    //TODO для отладки
    std::ofstream mainThreadFile_{};

    std::ofstream exportThreadFile_{};
};

}

#include "records_exporter.ipp"

#endif //_JOURNAL_RECORDS_EXPORT_RECORDS_EXPORTER_H_