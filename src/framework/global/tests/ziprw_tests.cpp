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
#include <gtest/gtest.h>

#include "io/file.h"

#include "global/serialization/zipwriter.h"
#include "global/serialization/zipreader.h"

using namespace kfw;

class Zip_RW_Tests : public ::testing::Test
{
public:
};

TEST_F(Zip_RW_Tests, Write_And_Read)
{
    //! [GIVEN] A zip file
    io::IODevice* device = new io::File("test.zip");
    ZipWriter writer(device);

    //! [WHEN] Writing data to the zip file
    writer.addFile("file1.txt", "Hello World!");
    writer.addFile("folder/file2.txt", "Hello World 2!");

    writer.close();

    //! [THEN] The data can be read back
    ZipReader reader(device);
    auto data = reader.fileData("file1.txt");
    EXPECT_EQ(data, "Hello World!");

    auto data2 = reader.fileData("folder/file2.txt");
    EXPECT_EQ(data2, "Hello World 2!");

    reader.close();
}
