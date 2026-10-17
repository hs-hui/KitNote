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

#include <optional>

#include "msgpack_forward.h"
#include "../types/bytearray.h"
#include "../types/string.h"
#include "../types/number.h"
#include "../types/flags.h"
#include "../types/retval.h"
#include "../types/uri.h"
#include "../types/bytearray.h"
#include "../io/path.h"

void pack_custom(std::vector<uint8_t>& data, const size_t& value);
bool unpack_custom(kfw::msgpack::Cursor& cursor, size_t& value);

template<typename T>
void pack_custom(std::vector<uint8_t>& data, const std::optional<T>& value);
template<typename T>
bool unpack_custom(kfw::msgpack::Cursor& cursor, std::optional<T>& value);

void pack_custom(std::vector<uint8_t>& data, const kfw::String& value);
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::String& value);

template<typename T>
void pack_custom(std::vector<uint8_t>& data, const kfw::number_t<T>& value);
template<typename T>
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::number_t<T>& value);

template<typename T>
void pack_custom(std::vector<uint8_t>& data, const kfw::Flags<T>& value);
template<typename T>
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::Flags<T>& value);

// Ret[Val]
void pack_custom(std::vector<uint8_t>& data, const kfw::Ret& value);
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::Ret& value);
template<typename T>
void pack_custom(std::vector<uint8_t>& data, const kfw::RetVal<T>& value);
template<typename T>
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::RetVal<T>& value);
template<typename T1, typename T2>
void pack_custom(std::vector<uint8_t>& data, const kfw::RetVal2<T1, T2>& value);
template<typename T1, typename T2>
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::RetVal2<T1, T2>& value);

// Uri
void pack_custom(std::vector<uint8_t>& data, const kfw::Uri& value);
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::Uri& value);

// ByteArray
void pack_custom(std::vector<uint8_t>& data, const kfw::ByteArray& value);
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::ByteArray& value);

// path_t
void pack_custom(std::vector<uint8_t>& data, const kfw::io::path_t& value);
bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::io::path_t& value);

#include "../thirdparty/kors_msgpack/msgpack/msgpack.h"

// std standart
inline void pack_custom(std::vector<uint8_t>& data, const size_t& value)
{
    kfw::msgpack::Packer::pack(data, static_cast<uint64_t>(value));
}

inline bool unpack_custom(kfw::msgpack::Cursor& cursor, size_t& value)
{
    uint64_t v = 0;
    bool ok = kfw::msgpack::UnPacker::unpack(cursor, v);
    value = static_cast<size_t>(v);
    return ok;
}

template<typename T>
inline void pack_custom(std::vector<uint8_t>& data, const std::optional<T>& value)
{
    bool has_value = value.has_value();
    kfw::msgpack::Packer::pack(data, has_value);
    if (has_value) {
        kfw::msgpack::Packer::pack(data, value.value());
    }
}

template<typename T>
inline bool unpack_custom(kfw::msgpack::Cursor& cursor, std::optional<T>& value)
{
    bool has_value = false;
    bool ok = kfw::msgpack::UnPacker::unpack(cursor, has_value);
    if (!ok) {
        return false;
    }

    if (!has_value) {
        value = std::nullopt;
        return true;
    }

    T val = {};
    ok = kfw::msgpack::UnPacker::unpack(cursor, val);
    if (!ok) {
        return false;
    }

    value = std::make_optional<T>(val);
    return true;
}

// standard types
inline void pack_custom(std::vector<uint8_t>& data, const kfw::String& value)
{
    kfw::msgpack::Packer::pack(data, value.toStdString());
}

inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::String& value)
{
    std::string str;
    bool ok = kfw::msgpack::UnPacker::unpack(cursor, str);
    value = kfw::String::fromStdString(str);
    return ok;
}

template<typename T>
inline void pack_custom(std::vector<uint8_t>& data, const kfw::number_t<T>& value)
{
    kfw::msgpack::Packer::pack(data, value.raw());
}

template<typename T>
inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::number_t<T>& value)
{
    T val = {};
    bool ok = kfw::msgpack::UnPacker::unpack(cursor, val);
    value = kfw::number_t<T>(val);
    return ok;
}

template<typename T>
inline void pack_custom(std::vector<uint8_t>& data, const kfw::Flags<T>& value)
{
    typename kfw::Flags<T>::Int val = value;
    kfw::msgpack::Packer::pack(data, val);
}

template<typename T>
inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::Flags<T>& value)
{
    typename kfw::Flags<T>::Int val = 0;
    bool ok = kfw::msgpack::UnPacker::unpack(cursor, val);
    value = kfw::Flags<T>(val);
    return ok;
}

// Ret[Val]
inline void pack_custom(std::vector<uint8_t>& data, const kfw::Ret& value)
{
    kfw::msgpack::Packer::pack(data, value.code(), value.text());
}

inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::Ret& value)
{
    int code = 0;
    std::string text;
    bool ok = kfw::msgpack::UnPacker::unpack(cursor, code, text);
    value = kfw::Ret(code, text);
    return ok;
}

template<typename T>
inline void pack_custom(std::vector<uint8_t>& data, const kfw::RetVal<T>& value)
{
    kfw::msgpack::Packer::pack(data, value.ret, value.val);
}

template<typename T>
inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::RetVal<T>& value)
{
    return kfw::msgpack::UnPacker::unpack(cursor, value.ret, value.val);
}

template<typename T1, typename T2>
inline void pack_custom(std::vector<uint8_t>& data, const kfw::RetVal2<T1, T2>& value)
{
    kfw::msgpack::Packer::pack(data, value.ret, value.val1, value.val2);
}

template<typename T1, typename T2>
inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::RetVal2<T1, T2>& value)
{
    return kfw::msgpack::UnPacker::unpack(cursor, value.ret, value.val1, value.val2);
}

// Uri
inline void pack_custom(std::vector<uint8_t>& data, const kfw::Uri& value)
{
    kfw::msgpack::Packer::pack(data, value.toString());
}

inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::Uri& value)
{
    std::string str;
    bool ok = kfw::msgpack::UnPacker::unpack(cursor, str);
    value = kfw::Uri(str);
    return ok;
}

// ByteArray
inline void pack_custom(std::vector<uint8_t>& data, const kfw::ByteArray& value)
{
    kfw::msgpack::Packer::pack(data, value.constVData());
}

inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::ByteArray& value)
{
    return kfw::msgpack::UnPacker::unpack(cursor, value.vdata());
}

// path_t
inline void pack_custom(std::vector<uint8_t>& data, const kfw::io::path_t& value)
{
    kfw::msgpack::Packer::pack(data, value.toStdString());
}

inline bool unpack_custom(kfw::msgpack::Cursor& cursor, kfw::io::path_t& value)
{
    std::string str;
    bool ok = kfw::msgpack::UnPacker::unpack(cursor, str);
    value = kfw::io::path_t(str);
    return ok;
}

// pack / unpack
namespace kfw::msgpack {
struct Options {
    size_t rezerveSize = 100;
};

template<class ... Types>
static inline void pack(std::vector<uint8_t>& data, const Types&... args)
{
    Packer::pack(data, args ...);
}

template<class ... Types>
static inline ByteArray pack(const Options& opt, const Types&... args)
{
    ByteArray ba;
    std::vector<uint8_t>& vdata = ba.vdata();
    vdata.clear();
    vdata.reserve(opt.rezerveSize);
    Packer::pack(vdata, args ...);
    vdata.emplace_back(0);
    return ba;
}

template<class ... Types>
static inline ByteArray pack(const Types&... args)
{
    return pack(Options {}, args ...);
}

template<class ... Types>
static inline bool unpack(kfw::msgpack::Cursor& cursor, Types&... args)
{
    return UnPacker::unpack(cursor, args ...);
}

template<class ... Types>
static inline bool unpack(const ByteArray& data, Types&... args)
{
    return UnPacker::unpack(data.constVData(), args ...);
}
}
