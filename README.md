# OpenRGB SteelSeries GameSense Plugin

Keeps OpenRGB's Direct mode colors applied on SteelSeries 8-zone keyboards (Apex 3 and Apex 3 TKL)
while SteelSeries GG is running. This plugin is mainly for people who need GG open for other
features like Sonar.

The plugin attaches to OpenRGB's existing Apex controller and sends its Direct mode colors to GG
through the local GameSense API.

## Download

Get `OpenRGBGameSensePlugin.dll` from the Releases page. Each release states the
OpenRGB plugin API version it was built for. OpenRGB only loads plugins built for its own API
version, so pick the release that matches your OpenRGB version.

## Install

1. Download the DLL (above) or build it (below).
2. Copy `OpenRGBGameSensePlugin.dll` to `%APPDATA%\OpenRGB\plugins\`.
3. Restart the OpenRGB GUI if it was open before.

Plugins are only loaded by the GUI, not by the background service, so the GUI needs to stay running
in the tray. Make sure OpenRGB is enabled under Windows startup apps (Task Manager → Startup apps).

The plugin does nothing unless SteelSeries GG is installed and
`%PROGRAMDATA%\SteelSeries\SteelSeries Engine 3\coreProps.json` exists. If GG isn't running yet, it
retries registration every 5 seconds and applies the last colors once Engine becomes available.

When the plugin is unloaded, GG takes control of the lighting again.

## Build (Windows)

Requires Qt 6 (MSVC 2022 x64), Visual Studio 2022 Build Tools, and jom.

```
build.cmd
```

The DLL is written to `release\OpenRGBGameSensePlugin.dll`. Set `QTDIR` or `VCVARS` first if Qt or
the Build Tools are installed somewhere other than the default paths.

`OpenRGB/` is a submodule used only for headers. When an OpenRGB release changes
`OPENRGB_PLUGIN_API_VERSION`, update the submodule to that release, update the number in
`GameSensePlugin.json`, fix whatever the API changed, and rebuild.

## Tests

```
tests\run-tests.bat
```

Builds and runs a self-check of the `coreProps.json` address parser against the plugin's real
source. It checks that only `127.0.0.1:<port>` addresses with a valid port are accepted.

## Security

The GameSense port is read from `coreProps.json` in the shared ProgramData folder, which can be
modified by other local users.

Only `127.0.0.1:<port>` addresses are accepted, so the plugin never sends GameSense requests outside
the local machine.

## License

GPL-2.0-or-later. See `LICENSE`.
