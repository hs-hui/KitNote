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
#include "memfilesystem.h"

#include <cstring>

#include <QFile>
#include <QFileInfo>

#include "global/containers.h"
#include "global/stringutils.h"

#include "log.h"

using namespace kfw::io;

//! NOTE Very simple implementation without directory support

static bool isRC(const kfw::io::path_t& path)
{
    return kfw::strings::startsWith(path.toStdString(), ":/");
}

kfw::Ret MemFileSystem::exists(const kfw::io::path_t& path) const
{
    if (isRC(path)) {
        return QFile::exists(path.toQString());
    } else {
        return kfw::contains(m_files, path);
    }
}

kfw::Ret MemFileSystem::remove(const kfw::io::path_t& path, bool onlyIfEmpty)
{
    UNUSED(onlyIfEmpty);
    if (isRC(path)) {
        UNREACHABLE;
        return kfw::Ret();
    } else {
        kfw::remove(m_files, path);
        return kfw::make_ok();
    }
}

kfw::Ret MemFileSystem::clear(const kfw::io::path_t& path)
{
    if (isRC(path)) {
        UNREACHABLE;
        return kfw::Ret();
    } else {
        kfw::remove(m_files, path);
        return kfw::make_ok();
    }
}

kfw::Ret MemFileSystem::copy(const kfw::io::path_t& src, const kfw::io::path_t& dst, bool replace)
{
    if (isRC(src) || isRC(dst)) {
        NOT_SUPPORTED;
        return kfw::Ret();
    }

    if (replace) {
        m_files[dst] = m_files.at(src);
    } else {
        if (!kfw::contains(m_files, dst)) {
            m_files[dst] = m_files.at(src);
        }
    }
    return kfw::make_ok();
}

kfw::Ret MemFileSystem::move(const kfw::io::path_t& src, const kfw::io::path_t& dst, bool replace)
{
    if (isRC(src) || isRC(dst)) {
        NOT_SUPPORTED;
        return kfw::Ret();
    }

    if (replace) {
        m_files[dst] = m_files.at(src);
        kfw::remove(m_files, src);
    } else {
        if (!kfw::contains(m_files, dst)) {
            m_files[dst] = m_files.at(src);
            kfw::remove(m_files, src);
        }
    }
    return kfw::make_ok();
}

kfw::Ret MemFileSystem::makePath(const kfw::io::path_t& /*path*/) const
{
    NOT_IMPLEMENTED;
    return kfw::make_ret(kfw::Ret::Code::NotImplemented);
}

kfw::Ret MemFileSystem::makeLink(const kfw::io::path_t& /*targetPath*/, const kfw::io::path_t& /*linkPath*/) const
{
    NOT_IMPLEMENTED;
    return kfw::make_ret(kfw::Ret::Code::NotImplemented);
}

kfw::io::EntryType MemFileSystem::entryType(const kfw::io::path_t& /*path*/) const
{
    NOT_IMPLEMENTED;
    return kfw::io::EntryType::File;
}

kfw::RetVal<uint64_t> MemFileSystem::fileSize(const kfw::io::path_t& path) const
{
    kfw::RetVal<uint64_t> rv;
    rv.ret = exists(path);
    if (!rv.ret) {
        return rv;
    }

    if (isRC(path)) {
        QFileInfo fi(path.toQString());
        return kfw::RetVal<uint64_t>::make_ok(fi.size());
    } else {
        return kfw::RetVal<uint64_t>::make_ok(m_files.at(path).size());
    }
}

kfw::RetVal<kfw::io::paths_t> MemFileSystem::scanFiles(const kfw::io::path_t& /*rootDir*/, const std::vector<std::string>& /*filters*/,
                                                         kfw::io::ScanMode /*mode*/) const
{
    NOT_IMPLEMENTED;
    return kfw::make_ret(kfw::Ret::Code::NotImplemented);
}

void MemFileSystem::setAttribute(const kfw::io::path_t& /*path*/, Attribute /*attribute*/) const
{
    NOT_IMPLEMENTED;
}

bool MemFileSystem::setPermissionsAllowedForAll(const kfw::io::path_t& /*path*/) const
{
    NOT_IMPLEMENTED;
    return false;
}

kfw::RetVal<kfw::ByteArray> MemFileSystem::readFile(const kfw::io::path_t& path) const
{
    kfw::RetVal<kfw::ByteArray> rv;
    rv.ret = readFile(path, rv.val);
    return rv;
}

kfw::Ret MemFileSystem::readFile(const kfw::io::path_t& path, kfw::ByteArray& data) const
{
    kfw::Ret ret = exists(path);
    if (!ret) {
        return ret;
    }

    if (isRC(path)) {
        QFile file(path.toQString());
        file.open(QIODevice::ReadOnly);
        qint64 size = file.size();
        data.resize(static_cast<size_t>(size));
        file.read(reinterpret_cast<char*>(data.data()), size);
    } else {
        data = m_files.at(path);
    }

    return kfw::make_ok();
}

kfw::Ret MemFileSystem::writeFile(const kfw::io::path_t& path, const kfw::ByteArray& data)
{
    if (isRC(path)) {
        UNREACHABLE;
        return kfw::Ret();
    }
    m_files[path] = data;
    return kfw::make_ok();
}

kfw::RetVal<StreamId> MemFileSystem::openStream(const kfw::io::path_t& path, kfw::io::OpenMode mode)
{
    kfw::RetVal<StreamId> result;

    if (isRC(path)) {
        UNREACHABLE;
        result.ret = kfw::Ret();
        return result;
    }

    if (mode == kfw::io::OpenMode::WriteOnly) {
        m_files[path] = kfw::ByteArray();
    } else {
        // Append: ensure entry exists, if not, create empty
        m_files.emplace(path, kfw::ByteArray());
    }

    size_t initialPos = m_files[path].size(); // 0 for WriteOnly (just cleared), existing size for Append

    StreamId id = m_nextStreamId++;
    m_openStates.emplace(id, OpenFileState { path, initialPos });

    result.ret = kfw::make_ok();
    result.val = id;
    return result;
}

kfw::Ret MemFileSystem::writeToStream(StreamId fileId, const kfw::ByteArray& data, uint64_t offset)
{
    auto it = m_openStates.find(fileId);
    if (it == m_openStates.end()) {
        return kfw::make_ret(kfw::Ret::Code::UnknownError);
    }

    OpenFileState& state = it->second;
    kfw::ByteArray& buf = m_files[state.path];

    size_t writePos = (offset == STREAM_POS_CURRENT) ? state.pos : static_cast<size_t>(offset);

    size_t end = writePos + data.size();
    if (end > buf.size()) {
        buf.resize(end);
    }
    std::memcpy(buf.data() + writePos, data.constData(), data.size());

    state.pos = end;
    return kfw::make_ok();
}

kfw::Ret MemFileSystem::closeStream(StreamId fileId)
{
    auto it = m_openStates.find(fileId);
    if (it == m_openStates.end()) {
        return kfw::make_ret(kfw::Ret::Code::UnknownError);
    }

    m_openStates.erase(it);

    return kfw::make_ok();
}

kfw::io::path_t MemFileSystem::canonicalFilePath(const kfw::io::path_t& path) const
{
    return path;
}

kfw::io::path_t MemFileSystem::absolutePath(const kfw::io::path_t& path) const
{
    return path;
}

kfw::io::path_t MemFileSystem::absoluteFilePath(const kfw::io::path_t& path) const
{
    return path;
}

kfw::DateTime MemFileSystem::birthTime(const kfw::io::path_t& /*path*/) const
{
    NOT_IMPLEMENTED;
    return kfw::DateTime();
}

kfw::DateTime MemFileSystem::lastModified(const kfw::io::path_t& /*path*/) const
{
    NOT_IMPLEMENTED;
    return kfw::DateTime();
}

kfw::Ret MemFileSystem::isWritable(const kfw::io::path_t& /*path*/) const
{
    return kfw::make_ok();
}
