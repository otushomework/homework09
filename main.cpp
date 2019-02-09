#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <range/v3/all.hpp>


using IpAddressStr = std::vector<std::string>;
using IpAddressInt = unsigned int;
using Filter = std::map<int, int>;
using FilterRes = std::map<int, bool>;

auto split(const std::string &str, char d)
{
    IpAddressStr r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

auto getOctet(const IpAddressInt &ip, const int pos)
{
    return (ip >> (8*pos)) & 0xff;
}

//$ range < range.tsv
int main(int, char const *[])
{
    try
    {
        std::vector<IpAddressInt> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            auto v = split(line, '\t');
            auto ip_val = split(v.at(0), '.');
            IpAddressInt converted = (((std::stoi(ip_val.at(0))) * 256 + std::stoi(ip_val.at(1))) * 256 + std::stoi(ip_val.at(2))) * 256 + std::stoi(ip_val.at(3));

            ip_pool.push_back(converted);
        }

        // reverse lexicographical sort
        ranges::sort( ip_pool, std::greater<IpAddressInt>() );

        auto printTransformed = [](const IpAddressInt &ip)
        {
            std::string buffer;
            for (int i = 3; i >= 0; --i)
            {
                if (i != 3) buffer.append(".");
                buffer.append(std::to_string(getOctet(ip, i)));
            }
            std::cout << buffer << std::endl;
        };

        auto printRange = [&]( bool (*filter)(IpAddressInt) )
        {
            auto res = ip_pool | ranges::view::remove_if( filter );

            ranges::for_each(res, printTransformed);
        };

        printRange([](IpAddressInt) -> bool { return false; });
        printRange([](IpAddressInt ip) -> bool { return getOctet(ip, 3) != 1; });
        printRange([](IpAddressInt ip) -> bool { return getOctet(ip, 3) != 46 || getOctet(ip, 2) != 70; });
        printRange([](IpAddressInt ip) -> bool {
            for (int i = 3; i >= 0; --i)
            {
                if (getOctet(ip, i) == 46) return false;
            }
            return true;
        });
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
