/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include <QObject>
#include <qqmlintegration.h>

#include "global/utils.h"

#include "async/asyncable.h"

#include "engraving/dom/engravingitem.h"

namespace kn::notation {
class PercussionPanelPadModel : public QObject, public kfw::async::Asyncable
{
    Q_OBJECT

    Q_PROPERTY(QString padName READ padName NOTIFY padNameChanged)

    Q_PROPERTY(QString keyboardShortcut READ keyboardShortcut NOTIFY keyboardShortcutChanged)
    Q_PROPERTY(QString midiNote READ midiNote NOTIFY midiNoteChanged)

    //! KitNote: the kind of drum, for the colour of the pad:
    //! 0 other, 1 bass drum, 2 snare, 3 hi-hat, 4 cymbal, 5 tom
    Q_PROPERTY(int drumFamily READ drumFamily NOTIFY midiNoteChanged)

    Q_PROPERTY(QVariant notationPreviewItem READ notationPreviewItemVariant NOTIFY notationPreviewItemChanged)

    Q_PROPERTY(QList<QVariantMap> contextMenuItems READ contextMenuItems CONSTANT)

    QML_ELEMENT

public:
    explicit PercussionPanelPadModel(QObject* parent = nullptr);

    QString padName() const { return m_padName; }
    void setPadName(const QString& padName);

    QString keyboardShortcut() const { return m_keyboardShortcut; }
    void setKeyboardShortcut(const QString& keyboardShortcut);

    int pitch() const { return m_pitch; }
    void setPitch(int pitch);

    QString midiNote() const { return QString::fromStdString(kfw::pitchToString(m_pitch)); }

    int drumFamily() const
    {
        switch (m_pitch) {      // General MIDI percussion
        case 35: case 36:
            return 1;
        case 37: case 38: case 39: case 40:
            return 2;
        case 42: case 44: case 46:
            return 3;
        case 49: case 51: case 52: case 53: case 55: case 57: case 59:
            return 4;
        case 41: case 43: case 45: case 47: case 48: case 50:
            return 5;
        default:
            return 0;
        }
    }

    void setNotationPreviewItem(kn::engraving::ElementPtr item);
    kn::engraving::ElementPtr notationPreviewItem() const { return m_notationPreviewItem; }

    const QVariant notationPreviewItemVariant() const;

    QList<QVariantMap> contextMenuItems() const;
    Q_INVOKABLE void handleMenuItem(const QString& itemId);

    Q_INVOKABLE void triggerPad(const Qt::KeyboardModifiers& modifiers = Qt::KeyboardModifier::NoModifier);

    enum class PadAction {
        TRIGGER_STANDARD,
        TRIGGER_ADD,
        TRIGGER_INSERT,
        DUPLICATE,
        DELETE,
        DEFINE_SHORTCUT,
    };

    kfw::async::Channel<PadAction> padActionTriggered() const { return m_padActionTriggered; }

signals:
    void padNameChanged();

    void keyboardShortcutChanged();
    void midiNoteChanged();

    void notationPreviewItemChanged();

private:
    QString m_padName;

    QString m_keyboardShortcut;
    int m_pitch = -1;

    kn::engraving::ElementPtr m_notationPreviewItem;

    kfw::async::Channel<PadAction> m_padActionTriggered;
};
}
