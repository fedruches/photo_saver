//
// Created by fedor on 06.01.2021.
//

#include "curl.h"
#include <utility>

namespace curl_wrapper {

Curl::Curl(std::string remoteUrl, FILE *sourceFileHandler) : remoteUrl_{std::move(remoteUrl)},
                                                             sourceFileHandler_{sourceFileHandler} {
    curl_global_init(CURL_GLOBAL_ALL);
    curl_ = curl_easy_init();

    if (curl_ == nullptr)
        throw std::runtime_error("libCURL cant be initialized");
}

Curl::~Curl() {
    curl_global_cleanup();
    fclose(sourceFileHandler_);
    std::filesystem::remove(tmpDirListFileName_);

    curl_global_cleanup();
}

void Curl::PrepareToFileTransfer(std::string sourceFileName) {
    auto fSize = OpenSourceFile(std::move(sourceFileName));

    printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fSize);

    curl_easy_setopt(curl_, CURLOPT_URL, remoteUrl_.c_str());

    /* we want to use our own read function */
    curl_easy_setopt(curl_, CURLOPT_READFUNCTION, read_callback);

    /* enable uploading */
    curl_easy_setopt(curl_, CURLOPT_UPLOAD, 1L);

    /* now specify which file to upload */
    curl_easy_setopt(curl_, CURLOPT_READDATA, sourceFileHandler_);

    /* Set the size of the file to upload (optional).  If you give a *_LARGE
       option you MUST make sure that the type of the passed-in argument is a
       curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
       make sure that to pass in a type 'long' argument. */
    curl_easy_setopt(curl_, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t) fSize);

    curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
}

curl_off_t Curl::OpenSourceFile(std::string sourceFileName) {
    struct stat file_info{};

    if (stat(sourceFileName.c_str(), &file_info))
        std::cerr << "Couldn't open " << sourceFileName << std::endl;

    return static_cast<curl_off_t>(file_info.st_size);
}

void Curl::Perform() {
    auto res = curl_easy_perform(curl_);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " <<
        curl_easy_strerror(res) << std::endl;

        throw std::runtime_error("curl_easy_perform() failed: ");
    }
}

std::string Curl::GetFileListing() {
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYSTATUS, 0);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)&result_);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
    //curl_easy_setopt(curl_, CURLOPT_DIRLISTONLY, 1);

    return result_;
}

std::string Curl::GetResult() {
    return result_;
}

void Curl::ChangeUrl(const std::string &url) {
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
}

void Curl::ClearResult() {
    result_.clear();
}

}