// Self-check for SteelSeriesGameSense::ParseLoopbackAddress (the coreProps.json trust boundary).
// Build: testsun-tests.bat
#include <cassert>
#include <cstdio>
#include "SteelSeriesGameSense.h"

int main()
{
    int         port = 0;

    assert(SteelSeriesGameSense::ParseLoopbackAddress(R"({"encryptedAddress":"127.0.0.1:54723","address":"127.0.0.1:54705"})", port));
    assert(port == 54705);

    const char* rejected[] =
    {
        R"({"address":"192.168.1.5:54705"})",       // non-loopback host
        R"({"address":"127.0.0.1.evil.com:80"})",   // prefix trick
        R"({"address":"127.0.0.1:"})",              // empty port
        R"({"address":"127.0.0.1:0"})",             // port out of range
        R"({"address":"127.0.0.1:65536"})",
        R"({"address":"127.0.0.1:123456"})",
        R"({"address":"127.0.0.1:80/x"})",          // path injection
        R"({"address":"127.0.0.1:-1"})",
        R"({"address":5})",                         // wrong type
        R"({})",
        R"(not json)",
        "",
    };

    for(const char* props : rejected)
    {
        assert(!SteelSeriesGameSense::ParseLoopbackAddress(props, port));
    }

    std::puts("gamesense_parse_test: all passed");
    return 0;
}
