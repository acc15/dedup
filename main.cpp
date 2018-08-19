#include <iostream>
#include <iomanip>
#include <array>
#include <vector>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include "levenshtein_distance.hpp"

namespace fs = boost::filesystem;

enum fs_target {
    FS_DIR,
    FS_FILE,
    FS_ALL
};

struct options {

    size_t max_dist;
    fs::path dir;
    fs_target target;
    std::vector<boost::regex> includes;
    std::vector<boost::regex> excludes;

};

bool filter_matches(const options& o, const fs::path& p) {

    const bool is_dir = fs::is_directory(p);
    const bool target_match = o.target == FS_ALL || (o.target == FS_DIR && is_dir) || (o.target == FS_FILE && !is_dir);
    if (!target_match) {
        return false;
    }

    const bool included = o.includes.empty() ||
            std::any_of(o.includes.begin(), o.includes.end(), [&p](const boost::regex& r) -> bool {
        return boost::regex_match(p.string(), r);
    });
    if (!included) {
        return false;
    }

    const bool excluded = std::any_of(o.excludes.begin(), o.excludes.end(), [&p](const boost::regex& r) -> bool {
        return boost::regex_match(p.string(), r);
    });
    return !excluded;
}


void list_dir(const options& o, std::vector<fs::path>& v) {
    for (fs::recursive_directory_iterator i = fs::recursive_directory_iterator(o.dir); i != fs::recursive_directory_iterator(); i++) {
        const fs::path pp = (*i);
        if (filter_matches(o, pp)) {
            v.push_back(pp);
        }
    }
}

size_t entry_size(const fs::path& p) {

    if (!fs::is_directory(p)) {
        return fs::file_size(p);
    }

    size_t total_size = 0;

    fs::recursive_directory_iterator e;
    for (fs::recursive_directory_iterator i = fs::recursive_directory_iterator(p); i != e; i++) {
        const fs::path pp = (*i);
        if (!fs::is_directory(pp)) {
            total_size += fs::file_size(pp);
        }
    }
    return total_size;
}


options parse_options(int argc, char* argv[]) {

    options o =  {
        3,
        fs::current_path(),
        FS_ALL,
        std::vector<boost::regex>(),
        std::vector<boost::regex>()
    };


    const std::array<std::string, 5> allowed_opts = { "-d", "-m", "-t", "-i", "-e" };

    size_t next_opt = 0;
    for (size_t i = 1; i < static_cast<size_t>(argc); i++) {

        const char* arg = argv[i];
        switch (next_opt) {
            case 1:
                o.dir = fs::path(arg);
                break;

            case 2:
                o.max_dist = static_cast<size_t>( atoi(arg) );
                break;

            case 3:
                if (strcmp(arg, "DIR") == 0) {
                    o.target = FS_DIR;
                } else if (strcmp(arg, "FILE") == 0) {
                    o.target = FS_FILE;
                } else {
                    o.target = FS_ALL;
                }
                break;

            case 4:
                o.includes.push_back(boost::regex(arg));
                break;

            case 5:
                o.excludes.push_back(boost::regex(arg));
                break;

            default:
                //auto e = allowed_opts
                auto it = std::find(allowed_opts.cbegin(), allowed_opts.cend(), std::string(arg));
                if (it != allowed_opts.cend()) {
                    next_opt = static_cast<size_t>(std::distance(allowed_opts.begin(), it)) + 1;
                }
                continue;
        }
        next_opt = 0;
    }
    return o;

}

void print_dupes(const options& opts, const std::vector<fs::path>& paths) {

    size_t dup_count = 0, min_size = 0, max_size = 0;
    for (size_t i = 0; i < paths.size(); i++) {
        for (size_t j = i + 1; j < paths.size(); j++) {

            const fs::path& p1 = paths[i];
            const fs::path& p2 = paths[j];

            const size_t dist = static_cast<size_t>( levenshtein_distance(p1.filename().string(), p2.filename().string()) );
            if (dist > opts.max_dist) {
                continue;
            }

            const size_t s1 = entry_size(p1);
            const size_t s2 = entry_size(p2);

            std::cout << "Dup #" << dup_count + 1 << " (distance " << dist << ")" << std::endl
                      << p1 << " (size " << s1 << ")" << std::endl
                      << p2 << " (size " << s2 << ")" << std::endl << std::endl;

            min_size += std::min(s1, s2);
            max_size += std::max(s1, s2);

            ++dup_count;
        }

    }

    std::cout << "Total dupes: " << dup_count << "; Min dup size: " << min_size << "; Max dup size: " << max_size << std::endl;

}


int main(int argc, char* argv[])
{
    options opts = parse_options(argc, argv);

    std::vector<fs::path> paths;
    list_dir(opts, paths);

    print_dupes(opts, paths);
    return 0;
}
