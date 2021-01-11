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
    explicit Curl(std::string remoteUrl, FILE *sourceFileHandler);

    ~Curl();

    std::string GetFileListing();

    void PrepareToFileTransfer(std::string sourceFileName);

    static curl_off_t OpenSourceFile(std::string sourceFileName);

    void Perform();

    std::string GetDirList();

    std::string GetResult();

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
    size_t fullSize = size * nmemb;
    auto *mem = (std::string *)ptr;
    *mem += std::string(static_cast<char *>(data));

    return fullSize;
}

static size_t read_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);

    auto nread = (curl_off_t)retcode;
    return retcode;
}

#endif //PHOTO_SAVER_CURL_H
