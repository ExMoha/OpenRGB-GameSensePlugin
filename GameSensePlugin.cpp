/*---------------------------------------------------------*\
| GameSensePlugin.cpp                                       |
|                                                           |
|   OpenRGB plugin that mirrors OpenRGB's own SteelSeries   |
|   8-zone keyboard device to SteelSeries GG's GameSense    |
|   API, so its colors stick while GG is running.           |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include <algorithm>
#include "GameSensePlugin.h"
#include "LogManager.h"
#include "ResourceManagerCallback.h"

/*---------------------------------------------------------*\
| Description OpenRGB's Apex 3 driver gives its devices;    |
| the 8-zone variants are the ones with 8 LEDs.             |
\*---------------------------------------------------------*/
#define APEX3_DESCRIPTION   "SteelSeries Apex 3 Device"

static bool IsEightZoneApex(RGBControllerInterface* controller)
{
    return(controller->GetDescription() == APEX3_DESCRIPTION && controller->GetLEDCount() == GAMESENSE_ZONE_COUNT);
}

OpenRGBPluginInfo GameSensePlugin::GetPluginInfo()
{
    OpenRGBPluginInfo info;

    info.Name           = "SteelSeries GameSense";
    info.Description    = "Keeps SteelSeries 8-zone keyboard colors while SteelSeries GG is running";
    info.Version        = "1.0";
    info.Commit         = "";
    info.URL            = "https://gitlab.com/xmo4/OpenRGB-GameSensePlugin";

    /*-----------------------------------------------------*\
    | No UI: an out-of-range location tells OpenRGB not to  |
    | create a tab for it.                                  |
    \*-----------------------------------------------------*/
    info.Location       = OPENRGB_PLUGIN_LOCATION_SETTINGS + 1;
    info.Label          = "SteelSeries GameSense";
    info.ProtocolVersion = 0;

    return(info);
}

unsigned int GameSensePlugin::GetPluginAPIVersion()
{
    return(OPENRGB_PLUGIN_API_VERSION);
}

void GameSensePlugin::Load(OpenRGBPluginAPIInterface* plugin_api_ptr)
{
    api = plugin_api_ptr;

    /*-----------------------------------------------------*\
    | coreProps.json only exists once GG has been installed |
    | and run; without it there is nothing to drive.        |
    \*-----------------------------------------------------*/
    if(SteelSeriesGameSense::ReadCoreProps().empty())
    {
        api->LogEntry(__FILE__, __LINE__, LL_INFO, "[GameSense Plugin] SteelSeries GG not found, staying idle");
        return;
    }

    gamesense = new SteelSeriesGameSense(api);

    AttachControllers();
}

void GameSensePlugin::Unload()
{
    {
        std::lock_guard<std::mutex> lock(attached_mutex);

        /*-------------------------------------------------*\
        | Only touch controllers that still exist           |
        \*-------------------------------------------------*/
        std::vector<RGBControllerInterface*> current = api->GetRGBControllers();

        for(RGBControllerInterface* controller : attached)
        {
            if(std::find(current.begin(), current.end(), controller) != current.end())
            {
                controller->UnregisterUpdateCallback(this);
            }
        }

        attached.clear();
    }

    /*-----------------------------------------------------*\
    | Deleting the client stops our GameSense game, so GG   |
    | goes back to its own lighting.                        |
    | Safe: UnregisterUpdateCallback above takes the same   |
    | mutex SignalUpdate holds while calling callbacks, so  |
    | no callback is still running here.                    |
    \*-----------------------------------------------------*/
    delete gamesense;
    gamesense = nullptr;
}

void GameSensePlugin::AttachControllers()
{
    std::lock_guard<std::mutex> lock(attached_mutex);

    std::vector<RGBControllerInterface*> current = api->GetRGBControllers();

    /*-----------------------------------------------------*\
    | Forget controllers that were removed (rescan), without|
    | calling into them as they may already be freed.       |
    \*-----------------------------------------------------*/
    attached.erase(std::remove_if(attached.begin(), attached.end(), [&](RGBControllerInterface* controller)
    {
        return(std::find(current.begin(), current.end(), controller) == current.end());
    }), attached.end());

    for(RGBControllerInterface* controller : current)
    {
        if(IsEightZoneApex(controller) && std::find(attached.begin(), attached.end(), controller) == attached.end())
        {
            controller->RegisterUpdateCallback(ControllerUpdated, this);
            attached.push_back(controller);

            api->LogEntry(__FILE__, __LINE__, LL_INFO, "[GameSense Plugin] Mirroring %s to GameSense", controller->GetName().c_str());

            /*---------------------------------------------*\
            | A profile may have been applied before we     |
            | attached; push the current state now. All     |
            | black means OpenRGB has no state yet, so      |
            | leave GG's lighting alone in that case.       |
            \*---------------------------------------------*/
            bool has_color = false;

            for(unsigned int led_idx = 0; led_idx < GAMESENSE_ZONE_COUNT; led_idx++)
            {
                has_color = has_color || (controller->GetColor(led_idx) != 0);
            }

            if(has_color)
            {
                ControllerUpdated(this, RGBCONTROLLER_UPDATE_REASON_UPDATELEDS, controller);
            }
        }
    }
}

void GameSensePlugin::ControllerUpdated(void* this_ptr, unsigned int update_reason, void* controller_ptr)
{
    if(update_reason != RGBCONTROLLER_UPDATE_REASON_UPDATELEDS && update_reason != RGBCONTROLLER_UPDATE_REASON_UPDATEMODE)
    {
        return;
    }

    GameSensePlugin*        plugin      = (GameSensePlugin*)this_ptr;
    RGBControllerInterface* controller  = (RGBControllerInterface*)controller_ptr;
    int                     active_mode = controller->GetActiveMode();

    /*-----------------------------------------------------*\
    | Only Direct is mirrored; hardware effects can't be    |
    | expressed through GameSense, so hand control back.    |
    | Brightness needs nothing here: the keyboard's own     |
    | brightness command still applies under GG.            |
    \*-----------------------------------------------------*/
    if(active_mode < 0 || controller->GetModeName((unsigned int)active_mode) != "Direct")
    {
        plugin->gamesense->Stop();
        return;
    }

    std::vector<RGBColor> colors(GAMESENSE_ZONE_COUNT);

    for(unsigned int led_idx = 0; led_idx < GAMESENSE_ZONE_COUNT; led_idx++)
    {
        colors[led_idx] = controller->GetColor(led_idx);
    }

    plugin->gamesense->SendColors(colors);
}

void GameSensePlugin::ResourceManagerUpdated(unsigned int update_reason)
{
    if(gamesense != nullptr && update_reason == RESOURCEMANAGER_UPDATE_REASON_DEVICE_LIST_UPDATED)
    {
        AttachControllers();
    }
}

/*---------------------------------------------------------*\
| Unused plugin hooks                                       |
\*---------------------------------------------------------*/
QWidget*        GameSensePlugin::GetWidget()                                    { return(nullptr); }
QMenu*          GameSensePlugin::GetTrayMenu()                                  { return(nullptr); }
void            GameSensePlugin::OnProfileAboutToLoad()                         {}
void            GameSensePlugin::OnProfileLoad(nlohmann::json)                  {}
nlohmann::json  GameSensePlugin::OnProfileSave()                                { return(nlohmann::json()); }
unsigned char*  GameSensePlugin::OnSDKCommand(unsigned int, unsigned char*, unsigned int*) { return(nullptr); }
void            GameSensePlugin::ProfileManagerUpdated(unsigned int)            {}
void            GameSensePlugin::SettingsManagerUpdated(unsigned int)           {}
