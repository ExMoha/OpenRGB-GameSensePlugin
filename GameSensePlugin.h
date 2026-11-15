/*---------------------------------------------------------*\
| GameSensePlugin.h                                         |
|                                                           |
|   OpenRGB plugin that mirrors OpenRGB's own SteelSeries   |
|   8-zone keyboard device to SteelSeries GG's GameSense    |
|   API, so its colors stick while GG is running.           |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <mutex>
#include <vector>
#include <QObject>
#include "OpenRGBPluginInterface.h"
#include "SteelSeriesGameSense.h"

class GameSensePlugin : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID FILE "GameSensePlugin.json")
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    OpenRGBPluginInfo   GetPluginInfo() override;
    unsigned int        GetPluginAPIVersion() override;

    void                Load(OpenRGBPluginAPIInterface* plugin_api_ptr) override;
    QWidget*            GetWidget() override;
    QMenu*              GetTrayMenu() override;
    void                Unload() override;

    void                OnProfileAboutToLoad() override;
    void                OnProfileLoad(nlohmann::json profile_data) override;
    nlohmann::json      OnProfileSave() override;
    unsigned char*      OnSDKCommand(unsigned int pkt_id, unsigned char* pkt_data, unsigned int* pkt_size) override;

    void                ProfileManagerUpdated(unsigned int update_reason) override;
    void                ResourceManagerUpdated(unsigned int update_reason) override;
    void                SettingsManagerUpdated(unsigned int update_reason) override;

private:
    void                AttachControllers();
    static void         ControllerUpdated(void* this_ptr, unsigned int update_reason, void* controller_ptr);

    OpenRGBPluginAPIInterface*              api         = nullptr;
    SteelSeriesGameSense*                   gamesense   = nullptr;

    std::mutex                              attached_mutex;
    std::vector<RGBControllerInterface*>    attached;
};
