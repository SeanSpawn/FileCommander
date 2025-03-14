#pragma once
#include "cfilesystemobject.h"
#include "detail/file_list_hashmap.h"

DISABLE_COMPILER_WARNINGS
#include <QIcon>
RESTORE_COMPILER_WARNINGS

#include <array>
#include <functional>
#include <vector>


struct PanelState {
	FileListHashMap panelContents;
	std::vector<qulonglong/*hash*/>                 selectedItemsHashes;
	qulonglong                                      currentItemHash = 0;
	QString                                         currentFolder;
};

enum PanelPosition {PluginLeftPanel, PluginRightPanel, PluginUnknownPanel};

class CPluginProxy
{
public:
	struct MenuTree {
		inline MenuTree(QString name_, std::function<void()>&& handler_, QIcon icon_ = {}):
			name(std::move(name_)), icon(std::move(icon_)), handler(std::move(handler_))
		{}

		const QString name;
		const QIcon icon;
		const std::function<void()> handler;
		std::vector<MenuTree> children;
	};

	explicit CPluginProxy(std::function<void (std::function<void ()>)> execOnUiThreadImplementation);

	using CreateToolMenuEntryImplementationType = std::function<void(const std::vector<CPluginProxy::MenuTree>&)>;

// Proxy initialization (by core / UI)
	void setToolMenuEntryCreatorImplementation(const CreateToolMenuEntryImplementationType& implementation);

// UI access for plugins; every plugin is only supposed to call this method once
	void createToolMenuEntries(const std::vector<MenuTree>& menuTrees);
	void createToolMenuEntries(const MenuTree& menuTree);

// Events and data updates from the core
	void panelContentsChanged(PanelPosition panel, const QString& folder, const FileListHashMap& contents);

// Events and data updates from UI
	void selectionChanged(PanelPosition panel, const std::vector<qulonglong/*hash*/>& selectedItemsHashes);
	void currentItemChanged(PanelPosition panel, qulonglong currentItemHash);
	void currentPanelChanged(PanelPosition panel);

	[[nodiscard]] PanelPosition currentPanel() const;
	[[nodiscard]] PanelPosition otherPanel() const;

	[[nodiscard]] PanelState& panelState(PanelPosition panel);
	[[nodiscard]] const PanelState& panelState(PanelPosition panel) const;
	[[nodiscard]] QString currentFolderPathForPanel(PanelPosition panel) const;
	[[nodiscard]] QString currentItemPathForPanel(PanelPosition panel) const;
	[[nodiscard]] const CFileSystemObject& currentItemForPanel(PanelPosition panel) const;

	[[nodiscard]] const CFileSystemObject& currentItem() const;
	[[nodiscard]] QString currentItemPath() const;

	void execOnUiThread(const std::function<void()>& code);

private:
	CreateToolMenuEntryImplementationType _createToolMenuEntryImplementation;
	std::array<PanelState, 2> _panelState;
	std::function<void(std::function<void()>)> _execOnUiThreadImplementation;
	PanelPosition                       _currentPanel = PluginUnknownPanel;
};
