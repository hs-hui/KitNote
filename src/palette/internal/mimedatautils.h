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
#ifndef KN_PALETTE_MIMEDATAUTILS_H
#define KN_PALETTE_MIMEDATAUTILS_H

#include "io/buffer.h"

#include "global/modularity/ioc.h"

#include "engraving/rw/xmlreader.h"
#include "engraving/rw/xmlwriter.h"

namespace kn::palette {
template<class T>
QByteArray toMimeData(T* t)
{
    kfw::io::Buffer buffer;
    buffer.open(kfw::io::IODevice::WriteOnly);
    engraving::XmlWriter xml(&buffer);
    t->write(xml, true);
    xml.flush();
    buffer.close();
    return buffer.data().toQByteArray();
}

template<class T>
std::shared_ptr<T> fromMimeData(const QByteArray& data, const kfw::AsciiStringView& tagName, const kfw::modularity::ContextPtr& iocCtx)
{
    engraving::XmlReader e(data);
    while (e.readNextStartElement()) {
        const kfw::AsciiStringView tag(e.name());
        if (tag == tagName) {
            std::shared_ptr<T> t(new T(iocCtx));
            if (!t->read(e, true)) {
                return nullptr;
            }
            return t;
        } else {
            return nullptr;
        }
    }
    return nullptr;
}
}

#endif // KN_PALETTE_MIMEDATAUTILS_H
