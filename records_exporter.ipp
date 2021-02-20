/
// Created by fedor on 11.01.2021.
//

#ifndef JOURNAL_RECORDS_EXPORTER_IPP
#define JOURNAL_RECORDS_EXPORTER_IPP

#include "records_exporter.h"

namespace ssa {

template<class TRecordWrapper>
void RecordsExporter<TRecordWrapper>::ExportFunc()
{
    while(true)
    {
        auto *filledPagePtr = edrBorderingBuffer_.GetFilledElement();
        if (filledPagePtr != nullptr)
        {
            exportThreadFile_ << "Get " << filledPagePtr->event.m_FirstTime << std::endl;

            filledPagePtr->SetStatus(RecordsExportProcessingStructure::STATUS::EMPTY);
            edrBorderingBuffer_.IncrementFilledBorder();

            exportThreadFile_ << "Released" << std::endl;
        }
        else {
            std::this_thread::sleep_for(milliseconds(1));
        }

        if (!init_ && edrBorderingBuffer_.GetFilledElement() == nullptr) {
            return;
        }
    }
}

template<class TRecordWrapper>
RecordsExporter<TRecordWrapper>::~RecordsExporter() {
    if (recordsExportThread_.joinable())
        recordsExportThread_.join();

    exportThreadFile_.close();
    mainThreadFile_.close();
}

template<class TRecordWrapper>
void RecordsExporter<TRecordWrapper>::Insert(const TRecordWrapper& record) {
    auto *emptyPagePtr = edrBorderingBuffer_.GetEmptyElement();
    if (emptyPagePtr == nullptr) {
        //TODO ВЫВОД ДЛЯ ОТЛАДКИ, ПОТОМ УБРАТЬ
        mainThreadFile_ << "Records export buffer is overfilled" << std::endl;
        return;
    }

    mainThreadFile_ << "Insert " << record.m_FirstTime << std::endl;

    emptyPagePtr->event = record;
    emptyPagePtr->SetStatus(RecordsExportProcessingStructure::STATUS::FILLED);
    edrBorderingBuffer_.IncrementEmptyBorder();
    mainThreadFile_ << "Inserted all in buffer" << std::endl;
}

}

#endif //JOURNAL_RECORDS_EXPORTER_IPP