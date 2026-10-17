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

#include <list>
#include <string>

#include "types/string.h"
#include "types/bytearray.h"
#include "io/path.h"

#include "modularity/ioc.h"
#include "global/icryptographichash.h"

namespace kn::engraving {
class Image;
class Score;

//---------------------------------------------------------
//   ImageStoreItem
//---------------------------------------------------------

class ImageStoreItem
{
    kfw::GlobalInject<kfw::ICryptographicHash> cryptographicHash;

public:
    ImageStoreItem(const std::string& p);
    void dereference(Image*);
    void reference(Image*);

    kfw::ByteArray& buffer() { return m_buffer; }
    const kfw::ByteArray& buffer() const { return m_buffer; }
    bool loaded() const { return !m_buffer.empty(); }
    bool isUsed(Score*) const;
    bool isUsed() const { return !m_references.empty(); }
    std::string hashName() const;
    const kfw::ByteArray& hash() const { return m_hash; }
    const std::string& type() const { return m_type; }
    void set(const kfw::ByteArray& b, const kfw::ByteArray& h) { m_buffer = b; m_hash = h; }

private:

    std::list<Image*> m_references;
    std::string m_type; // image type (file extension)
    kfw::ByteArray m_buffer;
    kfw::ByteArray m_hash; // 16 byte md4 hash of _buffer
};

//---------------------------------------------------------
//   ImageStore
//---------------------------------------------------------

class ImageStore
{
    kfw::GlobalInject<kfw::ICryptographicHash> cryptographicHash;

public:
    ImageStore() = default;
    ImageStore(const ImageStore&) = delete;
    ImageStore& operator=(const ImageStore&) = delete;
    ~ImageStore();

    ImageStoreItem* getImage(std::string name) const;
    ImageStoreItem* add(const std::string& name, const kfw::ByteArray&);
    void clearUnused();

    typedef std::vector<ImageStoreItem*> ItemList;
    typedef ItemList::iterator iterator;
    typedef ItemList::const_iterator const_iterator;

    iterator begin() { return m_items.begin(); }
    const_iterator begin() const { return m_items.begin(); }
    iterator end() { return m_items.end(); }
    const_iterator end() const { return m_items.end(); }

private:

    ItemList m_items;
};

extern ImageStore imageStore;       // this is the global imageStore
} // namespace kn::engraving
