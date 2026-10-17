/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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

#include "zenbarmodel.h"

#include <algorithm>

#include "ui/view/iconcodes.h"
#include "settings.h"

using namespace kn::appshell;
using namespace kfw;
using namespace kfw::ui;
using namespace kfw::actions;

static const ActionCode COMMAND_SEARCH_CODE("kitnote-command-search");

//! What the rail shows, in groups (an empty code ends a group), and the icon of each: most of
//! these actions have none of their own. It holds what the top row of MuseScore Studio held.
static const std::vector<std::pair<ActionCode, IconCode::Code> > BAR_ACTIONS {
    { "note-input", IconCode::Code::EDIT },
    { "toggle-noteinput", IconCode::Code::NOTE_8TH },
    { "toggle-percussion-panel", IconCode::Code::PERCUSSION },
    { "toggle-palettes", IconCode::Code::MUSIC_NOTES },
    { "inspector", IconCode::Code::SETTINGS_COG },
    { "", IconCode::Code::NONE },
    { "rewind", IconCode::Code::REWIND },
    { "play", IconCode::Code::PLAY },
    { "loop", IconCode::Code::LOOP },
    { "metronome", IconCode::Code::METRONOME },
    { "toggle-mixer", IconCode::Code::MIXER },
    { "", IconCode::Code::NONE },
    { "action://notation/undo", IconCode::Code::UNDO },
    { "action://notation/redo", IconCode::Code::REDO },
};

//! What the command search offers before anything is typed.
static const std::vector<ActionCode> SUGGESTED_COMMANDS {
    "drum-pattern-rock-8", "drum-pattern-shuffle", "drum-pattern-jazz-ride", "drum-pattern-half-bar-fill",
    "drum-rudiment-paradiddle", "sticking-alternate-right", "counting-add", "drum-key-insert",
    "kitnote-step-grid", "kitnote-patterns", "kitnote-practice", "kitnote-song-structure", "measure-repeat-identical", "swing-eighth", "toggle-percussion-panel", "toggle-palettes", "toggle-noteinput",
    "toggle-mixer", "file-export", "kitnote-guide",
};

//! The commands last run from the search, newest first, as their codes joined with ";".
static const kfw::Settings::Key RECENT_COMMANDS_KEY("appshell", "ui/kitnote/recentCommands");
static constexpr int RECENT_COMMANDS_LIMIT = 6;

static QStringList recentCommands()
{
    return QString::fromStdString(kfw::settings()->value(RECENT_COMMANDS_KEY).toString()).split(';', Qt::SkipEmptyParts);
}

ZenBarModel::ZenBarModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void ZenBarModel::load()
{
    actionsRegister()->actionStateChanged().onReceive(this, [this](const ActionCodeList& codes) {
        if (std::find(codes.begin(), codes.end(), "note-input") != codes.end()) {
            // not from inside the notification: showing a toolbar changes action states again
            QMetaObject::invokeMethod(this, [this]() { followNoteInput(); }, Qt::QueuedConnection);
        }

        for (const ActionCode& code : codes) {
            for (const auto& barAction : BAR_ACTIONS) {
                if (barAction.first == code) {
                    emit itemsChanged();
                    return;
                }
            }
        }
    });

    dispatcher()->reg(this, COMMAND_SEARCH_CODE, [this]() {
        emit commandSearchRequested();
    });

    dispatcher()->reg(this, "kitnote-step-grid", [this]() {
        emit stepGridRequested();
    });

    dispatcher()->reg(this, "kitnote-practice", [this]() {
        emit practiceRequested();
    });

    dispatcher()->reg(this, "kitnote-patterns", [this]() {
        emit patternsRequested();
    });

    dispatcher()->reg(this, "kitnote-song-structure", [this]() {
        emit songStructureRequested();
    });

    emit itemsChanged();
}

//! The note-input toolbar (note values, dots, rests, voices) is closed in the "zen" layout. It is wanted
//! exactly while notes are entered, so it opens with note input and closes with it again - unless the
//! user had opened it themselves, in which case it is theirs and stays.
void ZenBarModel::followNoteInput()
{
    const bool inputOn = actionsRegister()->actionState("note-input").checked;
    const bool barOpen = actionsRegister()->actionState("toggle-noteinput").checked;

    if (inputOn && !barOpen) {
        m_openedNoteInputBar = true;
        dispatcher()->dispatch("toggle-noteinput");
    } else if (!inputOn && m_openedNoteInputBar) {
        m_openedNoteInputBar = false;
        if (barOpen) {
            dispatcher()->dispatch("toggle-noteinput");
        }
    }
}

QVariantList ZenBarModel::items() const
{
    QVariantList result;

    for (const auto& [code, icon] : BAR_ACTIONS) {
        if (code.empty()) {
            QVariantMap divider;
            divider["divider"] = true;
            result << divider;
            continue;
        }

        const UiAction& action = actionsRegister()->action(code);
        if (!action.isValid()) {
            continue;
        }

        const UiActionState state = actionsRegister()->actionState(code);

        QVariantMap item;
        item["divider"] = false;
        item["code"] = QString::fromStdString(code);
        item["icon"] = static_cast<int>(icon);
        item["title"] = action.title.qTranslatedWithoutMnemonic();
        item["shortcut"] = QString::fromStdString(shortcutsRegister()->shortcut(code).sequencesAsString());
        item["enabled"] = state.enabled;
        item["checked"] = state.checked;
        result << item;
    }

    return result;
}

void ZenBarModel::dispatch(const QString& actionCode)
{
    dispatcher()->dispatch(actionCode.toStdString());
}

void ZenBarModel::run(const QString& actionCode)
{
    QStringList recent = recentCommands();
    recent.removeAll(actionCode);
    recent.prepend(actionCode);
    while (recent.size() > RECENT_COMMANDS_LIMIT) {
        recent.removeLast();
    }
    kfw::settings()->setSharedValue(RECENT_COMMANDS_KEY, kfw::Val(recent.join(';').toStdString()));

    dispatch(actionCode);
}

QVariantMap ZenBarModel::commandItem(const UiAction& action) const
{
    const QString title = action.title.qTranslatedWithoutMnemonic();
    const QString description = action.description.qTranslated();

    QVariantMap item;
    item["code"] = QString::fromStdString(action.code);
    item["title"] = title;
    item["description"] = description == title ? QString() : description;
    item["shortcut"] = QString::fromStdString(shortcutsRegister()->shortcut(action.code).sequencesAsString());
    item["recent"] = false;
    return item;
}

QVariantList ZenBarModel::search(const QString& text) const
{
    static constexpr int LIMIT = 60;

    QVariantList result;
    const QString needle = text.trimmed();

    if (needle.isEmpty()) {
        // what was run last, then the usual drum commands
        std::vector<ActionCode> codes;
        for (const QString& code : recentCommands()) {
            codes.push_back(code.toStdString());
        }
        const size_t recentCount = codes.size();
        for (const ActionCode& code : SUGGESTED_COMMANDS) {
            if (std::find(codes.begin(), codes.end(), code) == codes.end()) {
                codes.push_back(code);
            }
        }

        for (size_t i = 0; i < codes.size(); ++i) {
            const UiAction& action = actionsRegister()->action(codes[i]);
            if (action.isValid()) {
                QVariantMap item = commandItem(action);
                item["recent"] = i < recentCount;
                result << item;
            }
        }
        return result;
    }

    // names that begin with the text first, then names that contain it, then descriptions
    QVariantList begins, contains, described;

    for (const UiAction& action : actionsRegister()->actionList()) {
        if (action.code == COMMAND_SEARCH_CODE || action.scCtx == "disabled") {
            continue;
        }

        const QString title = action.title.qTranslatedWithoutMnemonic();
        if (title.isEmpty()) {
            continue;
        }

        if (title.startsWith(needle, Qt::CaseInsensitive)) {
            begins << commandItem(action);
        } else if (title.contains(needle, Qt::CaseInsensitive)) {
            contains << commandItem(action);
        } else if (action.description.qTranslated().contains(needle, Qt::CaseInsensitive)
                   // the English name too, whatever the language of the interface ("paradiddle", "swing")
                   || action.title.raw().str.toQString().remove(QLatin1Char('&')).contains(needle, Qt::CaseInsensitive)
                   || QString::fromStdString(action.code).contains(needle, Qt::CaseInsensitive)) {
            described << commandItem(action);
        }
    }

    auto byTitle = [](const QVariant& a, const QVariant& b) {
        return a.toMap().value("title").toString().localeAwareCompare(b.toMap().value("title").toString()) < 0;
    };

    for (QVariantList* list : { &begins, &contains, &described }) {
        std::sort(list->begin(), list->end(), byTitle);
        for (const QVariant& item : *list) {
            if (result.size() >= LIMIT) {
                break;
            }
            result << item;
        }
    }

    return result;
}

