/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
 * Modified for KitNote, 2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "global/iinteractive.h"

namespace kfw {
class WebInteractive : public kfw::IInteractive
{
public:
    WebInteractive(std::shared_ptr<kfw::IInteractive> origin);

    ButtonData buttonData(Button b) const override;

    // question
    Result questionSync(const std::string& contentTitle, const Text& text, const ButtonDatas& buttons, int defBtn = int(Button::NoButton),
                        const Options& options = {}, const std::string& dialogTitle = {}) override;

    kfw::async::Promise<Result> question(const std::string& contentTitle, const Text& text, const ButtonDatas& buttons,
                                          int defBtn = int(Button::NoButton), const Options& options = {},
                                          const std::string& dialogTitle = {}) override;

    // info
    Result infoSync(const std::string& contentTitle, const Text& text, const ButtonDatas& buttons, int defBtn = int(Button::NoButton),
                    const Options& options = {}, const std::string& dialogTitle = {}) override;

    kfw::async::Promise<Result> info(const std::string& contentTitle, const Text& text, const ButtonDatas& buttons = {},
                                      int defBtn = int(Button::NoButton), const Options& options = {},
                                      const std::string& dialogTitle = {}) override;

    // warning
    Result warningSync(const std::string& contentTitle, const Text& text, const ButtonDatas& buttons, int defBtn = int(Button::NoButton),
                       const Options& options = {}, const std::string& dialogTitle = {}) override;

    kfw::async::Promise<Result> warning(const std::string& contentTitle, const Text& text, const ButtonDatas& buttons = {},
                                         int defBtn = int(Button::NoButton), const Options& options = {},
                                         const std::string& dialogTitle = {}) override;

    // error
    Result errorSync(const std::string& contentTitle, const Text& text, const ButtonDatas& buttons, int defBtn = int(Button::NoButton),
                     const Options& options = { WithIcon }, const std::string& dialogTitle = {}) override;

    kfw::async::Promise<Result> error(const std::string& contentTitle, const Text& text, const ButtonDatas& buttons = {},
                                       int defBtn = int(Button::NoButton), const Options& options = { WithIcon },
                                       const std::string& dialogTitle = {}) override;

    // progress
    void showProgress(const std::string& title, kfw::Progress progress) override;

    // files
    kfw::async::Promise<kfw::io::path_t> selectOpeningFile(const std::string& title, const kfw::io::path_t& dir,
                                                             const std::vector<std::string>& filter) override;
    kfw::io::path_t selectOpeningFileSync(const std::string& title, const kfw::io::path_t& dir, const std::vector<std::string>& filter,
                                           const int options = 0) override;
    kfw::io::paths_t selectOpeningFilesSync(const std::string& title, const kfw::io::path_t& dir, const std::vector<std::string>& filter,
                                             const int options = 0) override;

    kfw::io::path_t selectSavingFileSync(const std::string& title, const kfw::io::path_t& path, const std::vector<std::string>& filter,
                                          bool confirmOverwrite = true) override;

    // dirs
    kfw::io::path_t selectDirectory(const std::string& title, const kfw::io::path_t& dir) override;
    kfw::io::paths_t selectMultipleDirectories(const std::string& title, const kfw::io::path_t& dir,
                                                const kfw::io::paths_t& selectedDirectories) override;

    // color
    kfw::async::Promise<kfw::Color> selectColor(const kfw::Color& color = kfw::Color::WHITE, const std::string& title = {},
                                                  bool allowAlpha = false) override;
    bool isSelectColorOpened() const override;

    // custom
    kfw::RetVal<kfw::Val> openSync(const kfw::UriQuery& uri) override;
    kfw::async::Promise<kfw::Val> open(const kfw::UriQuery& uri) override;
    kfw::RetVal<bool> isOpened(const kfw::UriQuery& uri) const override;
    kfw::RetVal<bool> isOpened(const kfw::Uri& uri) const override;
    kfw::async::Channel<kfw::Uri> opened() const override;

    void raise(const kfw::UriQuery& uri) override;

    void close(const kfw::UriQuery& uri) override;
    void close(const kfw::Uri& uri) override;
    void closeAllDialogs() override;

    kfw::ValCh<kfw::Uri> currentUri() const override;
    kfw::RetVal<bool> isCurrentUriDialog() const override;
    std::vector<kfw::Uri> stack() const override;

    kfw::Ret openUrl(const std::string& url) const override;
    kfw::Ret openUrl(const QUrl& url) const override;

    kfw::Ret isAppExists(const std::string& appIdentifier) const override;
    kfw::Ret canOpenApp(const kfw::UriQuery& uri) const override;
    kfw::async::Promise<kfw::Ret> openApp(const kfw::UriQuery& uri) const override;

    kfw::Ret revealInFileBrowser(const kfw::io::path_t& filePath) const override;

private:

    std::shared_ptr<kfw::IInteractive> m_origin = nullptr;
};
}
