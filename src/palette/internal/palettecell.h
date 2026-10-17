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
#ifndef KN_PALETTE_PALETTECELL_H
#define KN_PALETTE_PALETTECELL_H

#include "engraving/dom/engravingitem.h"

#include <QAccessibleInterface>

#include "modularity/ioc.h"
#include "ui/iuiactionsregister.h"

namespace kn::engraving {
class XmlReader;
class XmlWriter;
}

namespace kn::palette {
class PaletteCell;
using PaletteCellPtr = std::shared_ptr<PaletteCell>;
using PaletteCellConstPtr = std::shared_ptr<const PaletteCell>;

class AccessiblePaletteCellInterface : public QAccessibleInterface
{
public:
    AccessiblePaletteCellInterface(PaletteCell* cell);

    bool isValid() const override;
    QObject* object() const override;
    QAccessibleInterface* childAt(int x, int y) const override;
    QAccessibleInterface* parent() const override;
    QAccessibleInterface* child(int index) const override;
    int childCount() const override;
    int indexOfChild(const QAccessibleInterface*) const override;
    QString text(QAccessible::Text t) const override;
    void setText(QAccessible::Text t, const QString& text) override;
    QRect rect() const override;
    QAccessible::Role role() const override;
    QAccessible::State state() const override;

private:
    QObject* parentWidget() const;

    PaletteCell* m_cell = nullptr;
};

class PaletteCell : public QObject, public kfw::Contextable
{
    Q_OBJECT
    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionsRegister = { this };

public:
    explicit PaletteCell(const kfw::modularity::ContextPtr& iocCtx, QObject* parent = nullptr);
    PaletteCell(const kfw::modularity::ContextPtr& iocCtx, kn::engraving::ElementPtr e, const QString& _name, qreal _mag = 1.0,
                const QPointF& offset = QPointF(), const QString& tag = "", QObject* parent = nullptr);

    static QAccessibleInterface* accessibleInterface(QObject* object);

    static constexpr const char* mimeDataFormat = "application/musescore/palette/cell";

    const char* translationContext() const;
    QString translatedName() const;

    void retranslate();
    void setElementTranslated(bool translate);

    void write(kn::engraving::XmlWriter& xml, bool pasteMode) const;
    bool read(kn::engraving::XmlReader&, bool pasteMode);
    QByteArray toMimeData() const;

    static PaletteCellPtr fromMimeData(const QByteArray& data, const kfw::modularity::ContextPtr& iocCtx);
    static PaletteCellPtr fromElementMimeData(const QByteArray& data, const kfw::modularity::ContextPtr& iocCtx);

    kn::engraving::ElementPtr element;
    kn::engraving::ElementPtr untranslatedElement;
    QString id;

    QString name; // used for tool tip
    qreal mag { 1.0 };
    double xoffset { 0.0 }; // in spatium units of "gscore"
    double yoffset { 0.0 };
    QString tag;

    bool drawStaff { false };
    bool readOnly { false };
    bool visible { true };
    bool custom { false };
    bool active { false };
    bool focused { false };

private:
    static QString makeId();
};
}

#endif // KN_PALETTE_PALETTECELL_H
