//
// Created by fedor on 06.01.2021.
//

#ifndef PHOTO_SAVER_CURL_H
#define PHOTO_SAVER_CURL_H

#include <iostream>
#include <memory>
#include <cstring>
#include <sstream>
#include <filesystem>
#include <fstream>

#include <curl/curl.h>
#include <sys/stat.h>

namespace curl_wrapper {

class Curl {
public:
    struct MemoryStruct {
        char *memory;
        size_t size;
    };

public:
    explicit Curl(std::string remoteUrl, FILE *sourceFileHandler);

    ~Curl();

    std::string GetFileListing();

    void PrepareToFileTransfer(std::string sourceFileName);

    static curl_off_t OpenSourceFile(std::string sourceFileName);

    void Perform();

    std::string GetDirList();

    std::string GetResult();

    MemoryStruct memoryStruct_;

private:
    CURL *curl_;

    // Remote URL, for example "sftp://user:pass@x.x.x.x/"
    std::string remoteUrl_;

    FILE *sourceFileHandler_;

    std::string tmpDirListFileName_ = "dir_list.tmp";
    FILE *tmpFileForDirList_;

    std::string result_;
};

}

static size_t write_callback(void* data, size_t size, size_t nmemb, void* ptr)
{
    size_t realsize = size * nmemb;
    auto *mem = (curl_wrapper::Curl::MemoryStruct *)ptr;

    char *newPtr = static_cast<char *>(realloc(mem->memory, mem->size + realsize + 1));
    if(newPtr == nullptr) {
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = newPtr;
    memcpy(&(mem->memory[mem->size]), data, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

static size_t read_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);

    auto nread = (curl_off_t)retcode;
    return retcode;
}

#endif //PHOTO_SAVER_CURL_H
