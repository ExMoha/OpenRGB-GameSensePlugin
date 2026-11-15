/*---------------------------------------------------------*\
| SteelSeriesGameSense.h                                    |
|                                                           |
|   Sends 8-zone keyboard lighting through SteelSeries      |
|   Engine's local GameSense API.                           |
|                                                           |
|   xmo4                                        12 Nov 2026 |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "OpenRGBPluginInterface.h"

#define GAMESENSE_ZONE_COUNT        8

class SteelSeriesGameSense
{
public:
    SteelSeriesGameSense(OpenRGBPluginAPIInterface* api);
    ~SteelSeriesGameSense();

    /*-----------------------------------------------------*\
    | Returns false if Engine is unavailable; the keepalive |
    | thread keeps retrying with the latest colors.         |
    \*-----------------------------------------------------*/
    bool        SendColors(const std::vector<RGBColor>& colors);

    /*-----------------------------------------------------*\
    | Hand lighting back to GG                              |
    \*-----------------------------------------------------*/
    void        Stop();

    /*-----------------------------------------------------*\
    | coreProps.json is in the shared ProgramData folder,   |
    | so only a 127.0.0.1 address is ever accepted from it. |
    \*-----------------------------------------------------*/
    static bool ParseLoopbackAddress(const std::string& core_props, int& port);
    static std::string ReadCoreProps();

private:
    bool        Register();
    bool        Post(const char* uri, const std::string& body);
    bool        SendLastColors();
    void        KeepaliveThreadFunction();

    OpenRGBPluginAPIInterface*                          api;

    std::mutex                                          mutex;
    std::vector<RGBColor>                               last_colors;
    std::chrono::time_point<std::chrono::steady_clock>  retry_after;
    int                                                 port;
    unsigned int                                        event_value;
    bool                                                registered;
    bool                                                active;

    std::atomic<bool>                                   keepalive_thread_run;
    std::thread                                         keepalive_thread;
};
