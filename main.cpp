#include <boost/program_options.hpp>
#include <iostream>

#include <curl/curl.h>
#include <curlcpp/curl_easy.h>
#include <curlcpp/curl_ios.h>

#include "curl_wrapper/curl.h"

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

int main(int argc, char *argv[])
{
    try {
        bpo::options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("src",
                 bpo::value<std::string>()->default_value("/home/fedor/Pictures/mac-rebisz-stalker-004-1920.jpg"),
                 "src")
                ("dst", bpo::value<std::string>()->default_value("picture_mac.jpg"), "dst");

        bpo::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 1;
        }

    std::string remoteUrl = "sftp://user:pass@ip:22/~/"/* + vm["dst"].as<std::string>()*/;

        /* get a FILE * of the same file */
        auto hdSrc = fopen(vm["src"].as<std::string>().c_str(), "rb");

        curl_wrapper::Curl curl(std::move(remoteUrl), hdSrc);
        //curl.PrepareToFileTransfer(vm["src"].as<std::string>());

        curl.GetFileListing();

        struct curl_slist *headerlist = nullptr;

        static const char buf[] = "rename SERVER/picture.png SERVER/renamed-and-fine.png";

        headerlist = curl_slist_append(headerlist, buf);

        /* Now run off and do what you've been told! */
        curl.Perform();

        auto str = curl.GetResult();

        /* clean up the FTP commands list */
        curl_slist_free_all(headerlist);

        std::cout << str << std::endl;

        return 0;
    }
    catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception" << std::endl;
    }

}