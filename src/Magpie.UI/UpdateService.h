#pragma once
#include "WinRTUtils.h"

namespace winrt::Magpie::UI {

enum class UpdateStatus {
	Pending,
	Checking,
	NoUpdate,
	ErrorWhileChecking,
	Available,
	Downloading,
	ErrorWhileDownloading,
	Installing
};

class UpdateService {
public:
	static UpdateService& Get() noexcept {
		static UpdateService instance;
		return instance;
	}

	UpdateService(const UpdateService&) = delete;
	UpdateService(UpdateService&&) = delete;

	UpdateStatus Status() const noexcept {
		return _status;
	}

	event_token StatusChanged(delegate<UpdateStatus> const& handler) {
		return _statusChangedEvent.add(handler);
	}

	WinRTUtils::EventRevoker StatusChanged(auto_revoke_t, delegate<UpdateStatus> const& handler) {
		event_token token = StatusChanged(handler);
		return WinRTUtils::EventRevoker([this, token]() {
			StatusChanged(token);
		});
	}

	void StatusChanged(event_token const& token) {
		_statusChangedEvent.remove(token);
	}

	fire_and_forget CheckForUpdatesAsync();

	fire_and_forget DownloadAndInstall();

	double DownloadProgress() const noexcept {
		assert(_status == UpdateStatus::Downloading);
		return _downloadProgress;
	}

	event_token DownloadProgressChanged(delegate<double> const& handler) {
		return _downloadProgressChangedEvent.add(handler);
	}

	WinRTUtils::EventRevoker DownloadProgressChanged(auto_revoke_t, delegate<double> const& handler) {
		event_token token = DownloadProgressChanged(handler);
		return WinRTUtils::EventRevoker([this, token]() {
			DownloadProgressChanged(token);
		});
	}

	void DownloadProgressChanged(event_token const& token) {
		_downloadProgressChangedEvent.remove(token);
	}

	void LeavingAboutPage();

	void ClosingMainWindow();

	void Cancel();

	const std::wstring& Tag() const noexcept {
		assert(_status >= UpdateStatus::Available);
		return _tag;
	}

	const std::wstring& BinaryUrl() const noexcept {
		assert(_status >= UpdateStatus::Available);
		return _binaryUrl;
	}

private:
	UpdateService() = default;

	void _Status(UpdateStatus value);

	event<delegate<UpdateStatus>> _statusChangedEvent;
	event<delegate<double>> _downloadProgressChangedEvent;

	std::wstring _tag;
	std::wstring _binaryUrl;
	std::wstring _binaryHash;
	UpdateStatus _status = UpdateStatus::Pending;
	double _downloadProgress = 0;
};

}
