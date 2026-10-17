/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2021 MuseScore Limited and others
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

#include "types/ret.h"
#include "types/string.h"
#include "io/path.h"
#include "io/iodevice.h"
#include "mscio.h"

namespace kfw {
class ZipReader;
}

namespace kn::engraving {
class MscReader
{
public:

    struct Params
    {
        kfw::io::IODevice* device = nullptr;
        kfw::io::path_t filePath;
        kfw::String mainFileName;
        MscIoMode mode = MscIoMode::Zip;
    };

    MscReader() = default;
    MscReader(const Params& params);
    ~MscReader();

    void setParams(const Params& params);
    const Params& params() const;

    kfw::Ret open();
    void close();
    bool isOpened() const;
    bool isContainer() const;

    kfw::ByteArray readStyleFile() const;
    kfw::ByteArray readScoreFile() const;

    std::vector<kfw::String> excerptFileNames() const;
    kfw::ByteArray readExcerptStyleFile(const kfw::String& excerptFileName) const;
    kfw::ByteArray readExcerptFile(const kfw::String& excerptFileName) const;

    kfw::ByteArray readChordListFile() const;
    kfw::ByteArray readThumbnailFile() const;

    std::vector<kfw::String> imageFileNames() const;
    kfw::ByteArray readImageFile(const kfw::String& fileName) const;

    kfw::ByteArray readAudioFile() const;
    kfw::ByteArray readAudioSettingsJsonFile(const kfw::io::path_t& pathPrefix = "") const;
    kfw::ByteArray readViewSettingsJsonFile(const kfw::io::path_t& pathPrefix = "") const;
    kfw::ByteArray readAutomationJsonFile() const;

private:

    struct IReader {
        virtual ~IReader() = default;

        virtual kfw::Ret open(kfw::io::IODevice* device, const kfw::io::path_t& filePath) = 0;
        virtual void close() = 0;
        virtual bool isOpened() const = 0;
        //! NOTE In the case of reading from a directory,
        //! it may happen that we are not reading a container (a directory with a certain structure),
        //! but only one file among others (`.mscx` from MU 3.x)
        virtual bool isContainer() const = 0;
        virtual kfw::StringList fileList() const = 0;
        virtual bool fileExists(const kfw::String& fileName) const = 0;
        virtual kfw::ByteArray fileData(const kfw::String& fileName) const = 0;
    };

    struct ZipFileReader : public IReader
    {
        ~ZipFileReader() override;
        kfw::Ret open(kfw::io::IODevice* device, const kfw::io::path_t& filePath) override;
        void close() override;
        bool isOpened() const override;
        bool isContainer() const override;
        kfw::StringList fileList() const override;
        bool fileExists(const kfw::String& fileName) const override;
        kfw::ByteArray fileData(const kfw::String& fileName) const override;
    private:
        kfw::io::IODevice* m_device = nullptr;
        bool m_selfDeviceOwner = false;
        kfw::ZipReader* m_zip = nullptr;
    };

    struct DirReader : public IReader
    {
        kfw::Ret open(kfw::io::IODevice* device, const kfw::io::path_t& filePath) override;
        void close() override;
        bool isOpened() const override;
        bool isContainer() const override;
        kfw::StringList fileList() const override;
        bool fileExists(const kfw::String& fileName) const override;
        kfw::ByteArray fileData(const kfw::String& fileName) const override;
    private:
        kfw::io::path_t m_rootPath;
    };

    struct XmlFileReader : public IReader
    {
        kfw::Ret open(kfw::io::IODevice* device, const kfw::io::path_t& filePath) override;
        void close() override;
        bool isOpened() const override;
        bool isContainer() const override;
        kfw::StringList fileList() const override;
        bool fileExists(const kfw::String& fileName) const override;
        kfw::ByteArray fileData(const kfw::String& fileName) const override;
    private:
        kfw::io::IODevice* m_device = nullptr;
        bool m_selfDeviceOwner = false;
    };

    IReader* reader() const;
    bool fileExists(const kfw::String& fileName) const;
    kfw::ByteArray fileData(const kfw::String& fileName) const;

    kfw::String mainFileName() const;

    Params m_params;
    mutable IReader* m_reader = nullptr;
};
}
