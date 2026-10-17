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
#ifndef KN_PALETTE_CUSTOMIZEKITDIALOG_H
#define KN_PALETTE_CUSTOMIZEKITDIALOG_H

#include "ui_customizekitdialog.h"

#include "modularity/ioc.h"
#include "global/iinteractive.h"
#include "context/iglobalcontext.h"
#include "notation/inotationconfiguration.h"
#include "engraving/iengravingfontsprovider.h"
#include "engraving/rendering/isinglerenderer.h"
#include "ui/iuiconfiguration.h"

#include "engraving/dom/drumset.h"

namespace kn::palette {
//---------------------------------------------------------
//   CustomizeKitDialog
//---------------------------------------------------------

class CustomizeKitDialog : public QDialog, private Ui::CustomizeKitDialog, public kfw::Contextable
{
    Q_OBJECT

public:
    inline static kfw::GlobalInject<engraving::IEngravingFontsProvider> engravingFonts;
    inline static kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<engraving::rendering::ISingleRenderer> engravingRenderer = { this };

public:
    CustomizeKitDialog(QWidget* parent = nullptr);

private slots:
    void bboxClicked(QAbstractButton* button);
    void itemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void nameChanged(const QString&);
    void valueChanged();
    void load();
    void save();
    void customGboxToggled(bool);
    void customQuarterChanged(int);

    void defineShortcut();

private:
    void initDrumsetAndKey();

    void apply();
    void cancel();

    QTreeWidgetItem* loadPitchesList(); // Returns a tree item to select, if applicable...

    void updateExample();

    void fillCustomNoteheadsDataFromComboboxes(int pitch);
    void setCustomNoteheadsGUIEnabled(bool enabled);

    void setEnabledPitchControls(bool enable);
    void fillNoteheadsComboboxes(bool customGroup, int pitch);

    void notifyAboutNoteInputStateChanged();

    notation::INotationPtr m_notation;
    notation::InstrumentKey m_instrumentKey;

    engraving::Drumset m_originDrumset;
    engraving::Drumset m_editedDrumset;
};
}

#endif // KN_PALETTE_CUSTOMIZEKITDIALOG_H
