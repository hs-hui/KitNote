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

#include "types/string.h"
#include "types/ret.h"
#include "io/path.h"
#include "io/iodevice.h"
#include "mscio.h"

namespace kfw {
class ZipWriter;
class TextStream;
}

namespace kn::engraving {
class MscWriter
{
public:

    struct Params
    {
        kfw::io::IODevice* device = nullptr;
        kfw::io::path_t filePath;
        kfw::String mainFileName;
        MscIoMode mode = MscIoMode::Zip;
    };

    MscWriter() = default;
    MscWriter(const Params& params);
    ~MscWriter();

    void setParams(const Params& params);
    const Params& params() const;

    kfw::Ret open();
    void close();
    bool isOpened() const;
    bool hasError() const;

    void writeStyleFile(const kfw::ByteArray& data);
    void writeScoreFile(const kfw::ByteArray& data);
    void addExcerptStyleFile(const kfw::String& excerptFileName, const kfw::ByteArray& data);
    void addExcerptFile(const kfw::String& excerptFileName, const kfw::ByteArray& data);
    void writeChordListFile(const kfw::ByteArray& data);
    void writeThumbnailFile(const kfw::ByteArray& data);
    void addImageFile(const kfw::String& fileName, const kfw::ByteArray& data);
    void writeAudioFile(const kfw::ByteArray& data);
    void writeAudioSettingsJsonFile(const kfw::ByteArray& data, const kfw::io::path_t& pathPrefix = "");
    void writeViewSettingsJsonFile(const kfw::ByteArray& data, const kfw::io::path_t& pathPrefix = "");
    void writeAutomationJsonFile(const kfw::ByteArray& data);

private:

    struct IWriter {
        virtual ~IWriter() = default;

        virtual kfw::Ret open(kfw::io::IODevice* device, const kfw::io::path_t& filePath) = 0;
        virtual void close() = 0;
        virtual bool isOpened() const = 0;
        virtual bool hasError() const = 0;
        virtual bool addFileData(const kfw::String& fileName, const kfw::ByteArray& data) = 0;
    };

    struct ZipFileWriter : public IWriter
    {
        ~ZipFileWriter() override;
        kfw::Ret open(kfw::io::IODevice* device, const kfw::io::path_t& filePath) override;
        void close() override;
        bool isOpened() const override;
        bool hasError() const override;
        bool addFileData(const kfw::String& fileName, const kfw::ByteArray& data) override;

    private:
        kfw::io::IODevice* m_device = nullptr;
        kfw::ZipWriter* m_zip = nullptr;
    };

    struct DirWriter : public IWriter
    {
        kfw::Ret open(kfw::io::IODevice* device, const kfw::io::path_t& filePath) override;
        void close() override;
        bool isOpened() const override;
        bool hasError() const override;
        bool addFileData(const kfw::String& fileName, const kfw::ByteArray& data) override;
    private:
        kfw::io::path_t m_rootPath;
        bool m_hasError = false;
    };

    struct XmlFileWriter : public IWriter
    {
        ~XmlFileWriter() override;
        kfw::Ret open(kfw::io::IODevice* device, const kfw::io::path_t& filePath) override;
        void close() override;
        bool isOpened() const override;
        bool hasError() const override;
        bool addFileData(const kfw::String& fileName, const kfw::ByteArray& data) override;
    private:
        kfw::io::IODevice* m_device = nullptr;
        kfw::TextStream* m_stream = nullptr;
    };

    struct Meta {
        std::vector<kfw::String> files;
        bool isWritten = false;

        bool contains(const kfw::String& file) const;
        void addFile(const kfw::String& file);
    };

    IWriter* writer() const;

    bool addFileData(const kfw::String& fileName, const kfw::ByteArray& data);

    void writeMeta();
    void writeContainer(const std::vector<kfw::String>& paths);

    kfw::String mainFileName() const;

    Params m_params;
    mutable IWriter* m_writer = nullptr;
    Meta m_meta;
    bool m_hadError = false;
};
}
