#include <boost/program_options.hpp>
#include <iostream>

#include <curl/curl.h>
#include "curl_wrapper/curl.h"

#include <fstream>
#include <ostream>

namespace bpo = boost::program_options;

#include <cstdio>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


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

        /* get a FILE * of the same file */
        auto hdSrc1 = fopen(vm["src"].as<std::string>().c_str(), "rb");
        auto hdSrc2 = fopen(vm["src"].as<std::string>().c_str(), "rb");

        struct curl_slist *headerlist = nullptr;
        static const char buf[] = "rename SERVER/picture.png SERVER/renamed-and-fine.png";

        headerlist = curl_slist_append(headerlist, buf);

        std::string remoteUrl = "sftp://user:pass@x.x.x.x:22/~/"/* + vm["dst"].as<std::string>()*/;
        {
            curl_wrapper::Curl curl(remoteUrl, hdSrc1);
            //curl.PrepareToFileTransfer(vm["src"].as<std::string>());

            curl.ChangeUrl(remoteUrl);
            curl.GetFileListing();

            /* Now run off and do what you've been told! */
            curl.Perform();

            auto str = curl.GetResult();

            std::cout << str << std::endl;

            std::string remoteDirName;
            std::cout << "Enter a directory name what you want to move" << std::endl;
            std::cin >> remoteDirName;

            remoteDirName = remoteUrl + remoteDirName;
            curl.ChangeUrl(remoteDirName);

            curl.GetFileListing();
            curl.ClearResult();
            curl.Perform();
            auto str2 = curl.GetResult();
            std::cout << str2 << std::endl;
        }

        curl_slist_free_all(headerlist);

        return 0;
    }
    catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    catch(...) {
        std::cout << "Unknown exception" << std::endl;
    }

}