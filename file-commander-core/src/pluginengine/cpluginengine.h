#pragma once

#include "cpanel.h"
#include "plugininterface/cfilecommanderplugin.h"
#include "plugininterface/cfilecommanderviewerplugin.h"

#include <memory>
#include <vector>

class CFileCommanderViewerPlugin;
class CPluginWindow;
class QLibrary;

class CPluginEngine final : public PanelContentsChangedListener
{
public:
	CPluginEngine();
	~CPluginEngine() override;

	CPluginEngine& operator=(const CPluginEngine& other) = delete;
	CPluginEngine(const CPluginEngine& other) = delete;

	static CPluginEngine& get();

	void loadPlugins();
	std::vector<QString> activePluginNames();

	// CPanel observers
	void onPanelContentsChanged(Panel p, FileListRefreshCause operation) override;

	void selectionChanged(Panel p, const std::vector<qulonglong>& selectedItemsHashes);
	void currentItemChanged(Panel p, qulonglong currentItemHash);
	void currentPanelChanged(Panel p);

// Operations
	void viewCurrentFile();
	// The window needs a custom deleter because it must be deleted in the same dynamic library where it was allocated
	CFileCommanderViewerPlugin::PluginWindowPointerType createViewerWindowForCurrentFile();

private:
	static PanelPosition pluginPanelEnumFromCorePanelEnum(Panel p);

	CFileCommanderViewerPlugin * viewerForCurrentFile();

private:
	std::vector<std::pair<std::unique_ptr<CFileCommanderPlugin>, std::unique_ptr<QLibrary>>> _plugins;
};
