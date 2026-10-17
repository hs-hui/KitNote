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
#pragma once

#include <QObject>
#include <qqmlintegration.h>

#include "async/asyncable.h"
#include "modularity/ioc.h"
#include "global/iinteractive.h"
#include "actions/iactionsdispatcher.h"
#include "context/iglobalcontext.h"

#include "playback/iplaybackcontroller.h"
#include "playback/kitnoterecording.h"

namespace kn::playback {
//! KitNote: File > Play along with a recording (RecordingDialog.qml) - the recording of the score, and where
//! measure 1 begins in it (kitnoterecording.h). Every change is a step of the score's undo history.
class RecordingModel : public QObject, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(bool attached READ attached NOTIFY changed)
    Q_PROPERTY(QString fileName READ fileName NOTIFY changed)
    Q_PROPERTY(QString filePath READ filePath NOTIFY changed)
    Q_PROPERTY(double offset READ offset NOTIFY changed)
    Q_PROPERTY(bool finding READ finding NOTIFY changed)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(QString status READ status NOTIFY changed)
    Q_PROPERTY(bool warning READ warning NOTIFY changed)

    QML_ELEMENT

    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<context::IGlobalContext> context = { this };
    kfw::ContextInject<IPlaybackController> controller = { this };

public:
    explicit RecordingModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();

    bool attached() const;
    QString fileName() const;
    QString filePath() const;
    double offset() const;
    bool finding() const;
    bool playing() const;
    QString status() const;
    bool warning() const;

    Q_INVOKABLE void chooseFile();
    //! a path, or the file: URL of a file dropped on the dialog
    Q_INVOKABLE void setFile(const QString& pathOrUrl);
    Q_INVOKABLE void setOffset(double seconds);
    Q_INVOKABLE void findOffset();
    Q_INVOKABLE void remove();
    Q_INVOKABLE void togglePlay();

signals:
    void changed();
    void playingChanged();

private:
    ScoreRecording recording() const;
    QString scorePath() const;
    void apply(const ScoreRecording& recording);
    void onOffsetFound(const QString& path, std::optional<double> offset);
    bool followsTiming() const;
    void removeRecording(bool timingToo);

    bool m_finding = false;
    QString m_message;          // what finding the offset came to
    bool m_messageIsWarning = false;
};
}
