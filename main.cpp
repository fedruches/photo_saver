#include <boost/program_options.hpp>
#include <iostream>

#include <curl/curl.h>
#include <curlcpp/curl_easy.h>
#include <curlcpp/curl_ios.h>

#include <fstream>
#include <ostream>

namespace bpo = boost::program_options;

#include <cstdio>
#include <cstring>

#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

/* <DESC>
 * Performs an FTP upload and renames the file just after a successful
 * transfer.
 * </DESC>
 */

/* NOTE: if you want this example to work on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
static size_t read_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
    curl_off_t nread;
    /* in real-world cases, this would probably get this data differently
       as this fread() stuff is exactly what the library already would do
       by default internally */
    size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);

    nread = (curl_off_t)retcode;
    return retcode;
}

int main(int argc, char *argv[])
{
    bpo::options_description desc{"Options"};
    desc.add_options()
            ("help,h", "Help screen")
            ("src", bpo::value<std::string>()->default_value("/home/fedor/Pictures/Untitled.png"), "Src")
            ("dst", bpo::value<std::string>()->default_value("picture.png"), "Dst");

    bpo::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 1;
    }

    std::string REMOTE_URL = "sftp://fedor:user@192.168.1.237:22/" + vm["dst"].as<std::string>();

    CURL *curl;
    CURLcode res;
    FILE *hd_src;
    struct stat file_info{};
    curl_off_t fsize;

    struct curl_slist *headerlist = nullptr;

    static const char buf [] = "rename SERVER/picture.png SERVER/renamed-and-fine.png";

    /* get the file size of the local file */
    if(stat(vm["src"].as<std::string>().c_str(), &file_info)) {
        printf("Couldn't open '%s': %s\n", vm["src"].as<std::string>().c_str(), strerror(errno));
        return 1;
    }
    fsize = (curl_off_t)file_info.st_size;

    printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);

    /* get a FILE * of the same file */
    hd_src = fopen(vm["src"].as<std::string>().c_str(), "rb");

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* build a list of commands to pass to libcurl */
        headerlist = curl_slist_append(headerlist, buf);

        /* specify target */
        curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL.c_str());

        /* we want to use our own read function */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

        /* enable uploading */
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        /* pass in that last of FTP commands to run after the transfer */
        //curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

        /* now specify which file to upload */
        curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

        /* Set the size of the file to upload (optional).  If you give a *_LARGE
           option you MUST make sure that the type of the passed-in argument is a
           curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
           make sure that to pass in a type 'long' argument. */
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                         (curl_off_t)fsize);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* Now run off and do what you've been told! */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* clean up the FTP commands list */
        curl_slist_free_all(headerlist);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    fclose(hd_src); /* close the local file */

    curl_global_cleanup();
    return 0;
}