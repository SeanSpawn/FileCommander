#pragma once

#include "fileoperations/coperationperformer.h"

DISABLE_COMPILER_WARNINGS
#include <QTimer>
#include <QWidget>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class CCopyMoveDialog;
}

class CMainWindow;

class CCopyMoveDialog final : public QWidget, protected CFileOperationObserver
{
	Q_OBJECT

public:
	CCopyMoveDialog(QWidget* parent, Operation, std::vector<CFileSystemObject>&& source, QString destination, CMainWindow * mainWindow);
	~CCopyMoveDialog() override;

// Callbacks
	void onProgressChanged(float totalPercentage, size_t numFilesProcessed, size_t totalNumFiles, float filePercentage, uint64_t speed /* B/s*/, uint32_t secondsRemaining) override;
	void onProcessHalted(HaltReason, const CFileSystemObject& source, const CFileSystemObject& dest, const QString& errorMessage) override; // User decision required (file exists, file is read-only etc.)
	void onProcessFinished(const QString& message) override; // Done or canceled
	void onCurrentFileChanged(const QString& file) override; // Starting to process a new file

signals:
	void closed();

protected:
	void closeEvent(QCloseEvent * e) override;

private: // slots
	// True if cancelled, false if the user chose to continue
	bool cancelPressed();
	void pauseResume();
	void switchToBackground();

private:
	void cancel();

private:
	Ui::CCopyMoveDialog * ui;
	std::unique_ptr<COperationPerformer> _performer;
	CMainWindow         * _mainWindow;
	Operation             _op;
	QTimer                _eventsProcessTimer;
	const QString         _titleTemplate;
	const QString         _labelTemplate;
};
