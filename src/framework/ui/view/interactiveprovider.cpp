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
#include "interactiveprovider.h"

#include <memory>

#include <QMetaType>
#include <QMetaProperty>
#include <QDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QGuiApplication>
#include <QWindow>
#include <QWidget>

#include "global/async/async.h"
#include "diagnostics/diagnosticutils.h"

#include "internal/widgetdialogadapter.h"

#include "kfw_framework_config.h"

#include "log.h"

using namespace kfw;
using namespace kfw::ui;
using namespace kfw::async;

InteractiveProvider::InteractiveProvider(const modularity::ContextPtr& iocCtx)
    : QObject(), Contextable(iocCtx)
{
    connect(qApp, &QGuiApplication::focusWindowChanged, this, [this](QWindow* window) {
        raiseWindowInStack(window);
    });

    connect(qApp, &QGuiApplication::focusObjectChanged, this, [this](QObject* obj) {
        auto widget = dynamic_cast<QWidget*>(obj);

        if (widget && widget->isWindow()) {
            raiseWindowInStack(widget);
        }
    });
}

void InteractiveProvider::raiseWindowInStack(QObject* newActiveWindow)
{
    if (!newActiveWindow || m_stack.isEmpty() || m_stack.top().window == newActiveWindow) {
        return;
    }

    for (int i = 0; i < m_stack.size(); ++i) {
        bool found = m_stack[i].window == newActiveWindow;
        if (m_stack[i].window && m_stack[i].window->isWidgetType()) {
            found = newActiveWindow->objectName() == (m_stack[i].window->objectName() + "Window");
        }

        if (found) {
            ObjectInfo info = m_stack.takeAt(i);
            m_stack.push(info);
            notifyAboutCurrentUriChanged();
            return;
        }
    }
}

static std::vector<QColor> getCustomColors()
{
    const int customColorCount = QColorDialog::customCount();
    std::vector<QColor> customColors;
    customColors.reserve(customColorCount);
    for (int i = 0; i < customColorCount; ++i) {
        customColors.push_back(QColorDialog::customColor(i));
    }

    return customColors;
}

static void setCustomColors(const std::vector<QColor>& customColors)
{
    const int customColorCount = std::min(QColorDialog::customCount(), static_cast<int>(customColors.size()));
    for (int i = 0; i < customColorCount; ++i) {
        QColorDialog::setCustomColor(i, customColors[i]);
    }
}

async::Promise<Color> InteractiveProvider::selectColor(const Color& color, const std::string& title, bool allowAlpha)
{
    if (m_isSelectColorOpened) {
        LOGW() << "already opened";
        return async::make_promise<Color>([](auto, auto reject) {
            Ret ret = kfw::make_ret(Ret::Code::UnknownError);
            return reject(ret.code(), ret.text());
        });
    }

    m_isSelectColorOpened = true;

    setCustomColors(config()->colorDialogCustomColors());

    return async::make_promise<Color>([this, color, title, allowAlpha](auto resolve, auto reject) {
        //! FIX https://github.com/musescore/MuseScore/issues/23208
        shortcutsRegister()->setActive(false);

        QColorDialog* dlg = new QColorDialog();
        if (!title.empty()) {
            dlg->setWindowTitle(QString::fromStdString(title));
        }

        QColor currentColor = color.toQColor();

        // If the color is fully transparent, set alpha to opaque, to avoid "Hm, nothing happened" user confusion
        if (currentColor.alpha() == 0) {
            currentColor.setAlpha(255);
        }

        dlg->setCurrentColor(currentColor);
        dlg->setOption(QColorDialog::ShowAlphaChannel, allowAlpha);

        QObject::connect(dlg, &QColorDialog::finished, [this, dlg, resolve, reject](int result) {
            dlg->deleteLater();

            config()->setColorDialogCustomColors(getCustomColors());

            m_isSelectColorOpened = false;
            shortcutsRegister()->setActive(true);

            if (result != QDialog::Accepted) {
                Ret ret = kfw::make_ret(Ret::Code::Cancel);
                (void)reject(ret.code(), ret.text());
                return;
            }

            QColor selectedColor = dlg->selectedColor();
            (void)resolve(Color::fromQColor(selectedColor));
        });

        dlg->open();

        return async::Promise<Color>::dummy_result();
    }, async::PromiseType::AsyncByBody);
}

bool InteractiveProvider::isSelectColorOpened() const
{
    return m_isSelectColorOpened;
}

RetVal<Val> InteractiveProvider::openSync(const UriQuery& q)
{
#ifndef KFW_MODULE_UI_SYNCINTERACTIVE_SUPPORTED
    NOT_SUPPORTED;
    std::abort();
    {
        RetVal<Val> rv;
        rv.ret = kfw::make_ret(Ret::Code::NotSupported);
        return rv;
    }
#endif

    // KitNote: the answer and the loop are shared with the handlers, not referred to on this stack. The
    // loop also ends when the application is told to quit (QCoreApplication::exit ends every loop) while
    // the dialog is still open; this function has returned by the time the dialog is closed (as the
    // application shuts down), and its handlers used to write into this stack frame then - a crash on exit
    // with the "save changes?" question up.
    struct SyncState {
        RetVal<Val> rv;
        QEventLoop* loop = nullptr;
    };
    auto state = std::make_shared<SyncState>();
    QEventLoop loop;
    state->loop = &loop;

    Promise<Val>::Resolve resolve;
    Promise<Val>::Reject reject;
    Promise<Val> promise = async::make_promise<Val>([&resolve, &reject](auto res, auto rej) {
        resolve = res;
        reject = rej;
        return Promise<Val>::Result::unchecked();
    }, PromiseType::AsyncByBody);

    promise.onResolve(this, [state](const Val& val) {
        state->rv = RetVal<Val>::make_ok(val);
        if (state->loop) {
            state->loop->quit();
        }
    });

    promise.onReject(this, [state](int code, const std::string& err) {
        LOGE() << code << " " << err;
        state->rv.ret = make_ret(code, err);
        if (state->loop) {
            state->loop->quit();
        }
    });

    auto func = openFunc(q);
    func(resolve, reject);

    ContainerMeta openMeta = uriRegister()->meta(q.uri());
    if (openMeta.type == ContainerType::PrimaryPage) {
        LOGW() << "Primary pages should not open in synchronous mode, please fix this.";
        state->loop = nullptr;
        return state->rv;
    }

    loop.exec();
    state->loop = nullptr;

    return state->rv;
}

Promise<Val> InteractiveProvider::openAsync(const UriQuery& q)
{
    return async::make_promise<Val>(openFunc(q), PromiseType::AsyncByBody);
}

async::Promise<Val> InteractiveProvider::openAsync(const Uri& uri, const QVariantMap& params)
{
    return async::make_promise<Val>(openFunc(UriQuery(uri), params), PromiseType::AsyncByBody);
}

Promise<Val>::BodyResolveReject InteractiveProvider::openFunc(const UriQuery& q)
{
    QVariantMap params;
    const UriQuery::Params& p = q.params();
    for (auto it = p.cbegin(); it != p.cend(); ++it) {
        params[QString::fromStdString(it->first)] = it->second.toQVariant();
    }

    return openFunc(q, params);
}

Promise<Val>::BodyResolveReject InteractiveProvider::openFunc(const UriQuery& q, const QVariantMap& params)
{
    auto func = [this, q, params](Promise<Val>::Resolve resolve, Promise<Val>::Reject reject) {
        IF_ASSERT_FAILED(!m_openingObject.objectId.isValid()) {
            LOGE() << "The opening of the previous object has not been completed"
                   << ", objectId: " << m_openingObject.objectId.toString()
                   << ", query: " << m_openingObject.query.toString();
        }

        m_openingObject = { q, resolve, reject, QVariant(), nullptr };

        RetVal<OpenData> openedRet;

        notifyAboutCurrentUriWillBeChanged();

        ContainerMeta openMeta = uriRegister()->meta(q.uri());
        switch (openMeta.type) {
        case ContainerType::QWidgetDialog:
            openedRet = openWidgetDialog(q.uri(), params);
            break;
        case ContainerType::PrimaryPage:
        case ContainerType::QmlDialog:
            openedRet = openQml(q.uri(), params);
            break;
        case ContainerType::Undefined:
            openedRet.ret = make_ret(Ret::Code::UnknownError);
            break;
        }

        if (!openedRet.ret) {
            LOGE() << "failed open err: " << openedRet.ret.toString() << ", uri: " << q.toString();
            return reject(openedRet.ret.code(), openedRet.ret.text());
        }

        return Promise<Val>::Result::unchecked();
    };

    return func;
}

RetVal<bool> InteractiveProvider::isOpened(const Uri& uri) const
{
    for (const ObjectInfo& objectInfo: allOpenObjects()) {
        if (objectInfo.query.uri() == uri) {
            return RetVal<bool>::make_ok(true);
        }
    }

    return RetVal<bool>::make_ok(false);
}

RetVal<bool> InteractiveProvider::isOpened(const UriQuery& uri) const
{
    for (const ObjectInfo& objectInfo: allOpenObjects()) {
        if (objectInfo.query == uri) {
            return RetVal<bool>::make_ok(true);
        }
    }

    return RetVal<bool>::make_ok(false);
}

async::Channel<Uri> InteractiveProvider::opened() const
{
    return m_opened;
}

void InteractiveProvider::raise(const UriQuery& uri)
{
    for (const ObjectInfo& objectInfo: allOpenObjects()) {
        if (objectInfo.query != uri) {
            continue;
        }

        ContainerMeta openMeta = uriRegister()->meta(objectInfo.query.uri());
        switch (openMeta.type) {
        case ContainerType::QWidgetDialog: {
            if (auto window = dynamic_cast<QWidget*>(objectInfo.window)) {
                window->raise();
                window->activateWindow();
            }
        } break;
        case ContainerType::QmlDialog:
            raiseQml(objectInfo.objectId);
            break;
        case ContainerType::PrimaryPage:
        case ContainerType::Undefined:
            break;
        }
    }
}

Promise<Ret> InteractiveProvider::close(const Uri& uri)
{
    std::vector<ObjectInfo> objs = collectOpenObjects([&uri](const ObjectInfo& obj) {
        return obj.query.uri() == uri;
    });

    return closeObjects(objs);
}

Promise<Ret> InteractiveProvider::close(const UriQuery& uri)
{
    std::vector<ObjectInfo> objs = collectOpenObjects([&uri](const ObjectInfo& obj) {
        return obj.query == uri;
    });

    return closeObjects(objs);
}

Ret InteractiveProvider::closeSync(const UriQuery& uri)
{
    std::vector<ObjectInfo> objs = collectOpenObjects([&uri](const ObjectInfo& obj) {
        return obj.query == uri;
    });

    return closeObjectsSync(objs);
}

Ret InteractiveProvider::closeAllDialogsSync()
{
    std::vector<ObjectInfo> objs = collectOpenObjects([this](const ObjectInfo& obj) {
        if (kfw::diagnostics::isDiagnosticsUri(obj.query.uri())) {
            return false;
        }
        ContainerMeta meta = uriRegister()->meta(obj.query.uri());
        return meta.type == ContainerType::QWidgetDialog || meta.type == ContainerType::QmlDialog;
    });

    return closeObjectsSync(objs);
}

Promise<Ret> InteractiveProvider::closeObjects(const std::vector<ObjectInfo>& objs)
{
    return async::make_promise<Ret>([this, objs](auto resolve, auto) {
        if (objs.empty()) {
            return resolve(make_ok());
        }

        auto count = std::make_shared<size_t>(objs.size());
        auto ret = std::make_shared<Ret>(make_ok());

        for (const ObjectInfo& obj : objs) {
            const QString objectId = obj.objectId.toString();
            bool ok = m_onClosedFuncs.try_emplace(objectId, [this, objectId, count, ret, resolve](const Ret& funcRet) {
                if (!funcRet) {
                    *ret = funcRet;
                }

                if (--(*count) == 0) {
                    (void)resolve(*ret);
                }

                kfw::remove(m_onClosedFuncs, objectId);
            }).second;

            IF_ASSERT_FAILED(ok) {
                if (--(*count) == 0) {
                    (void)resolve(*ret);
                }
                continue;
            }

            ContainerMeta meta = uriRegister()->meta(obj.query.uri());

            switch (meta.type) {
                case ContainerType::QWidgetDialog: {
                    if (auto window = dynamic_cast<QWidget*>(obj.window)) {
                        ok = window->close();
                    } else {
                        ok = false;
                    }
                } break;
                case ContainerType::QmlDialog:
                    closeQml(obj.objectId);
                    break;
                case ContainerType::PrimaryPage:
                case ContainerType::Undefined:
                    break;
            }

            if (!ok) {
                if (auto it = m_onClosedFuncs.find(objectId); it != m_onClosedFuncs.end()) {
                    it->second(make_ret(Ret::Code::UnknownError));
                }
            }
        }

        return Promise<Ret>::dummy_result();
    });
}

Ret InteractiveProvider::closeObjectsSync(const std::vector<ObjectInfo>& objs)
{
    if (objs.empty()) {
        return make_ok();
    }

    // KitNote: shared with the handler, not on this stack (see openSync: the loop may end before it is called)
    struct SyncState {
        Ret ret = make_ok();
        QEventLoop* loop = nullptr;
        bool done = false;
    };
    auto state = std::make_shared<SyncState>();
    QEventLoop loop;
    state->loop = &loop;

    closeObjects(objs).onResolve(this, [state](const Ret& closeRet) {
        state->ret = closeRet;
        state->done = true;
        if (state->loop) {
            state->loop->quit();
        }
    });

    if (!state->done) {
        loop.exec();
    }
    state->loop = nullptr;

    return state->ret;
}

void InteractiveProvider::fillData(QmlLaunchData* data, const Uri& uri, const QVariantMap& params) const
{
    ContainerMeta meta = uriRegister()->meta(uri);
    data->setValue("module", meta.qmlModule);
    data->setValue("path", meta.qmlPath);
    data->setValue("type", meta.type);
    data->setValue("uri", QString::fromStdString(uri.toString()));
    data->setValue("params", params);
    data->setValue("modal", params.value("modal", ""));
}

void InteractiveProvider::fillData(QObject* object, const QVariantMap& params) const
{
    const QMetaObject* metaObject = object->metaObject();
    for (int i = 0; i < metaObject->propertyCount(); i++) {
        QMetaProperty metaProperty = metaObject->property(i);
        if (params.contains(metaProperty.name())) {
            object->setProperty(metaProperty.name(), params[metaProperty.name()]);
        }
    }
}

ValCh<Uri> InteractiveProvider::currentUri() const
{
    ValCh<Uri> v;
    if (!m_stack.empty()) {
        v.val = m_stack.last().query.uri();
    }
    v.ch = m_currentUriChanged;
    return v;
}

RetVal<bool> InteractiveProvider::isCurrentUriDialog() const
{
    if (m_stack.empty()) {
        return RetVal<bool>::make_ok(false);
    }

    const ObjectInfo& last = m_stack.last();
    if (!last.window) {
        return RetVal<bool>::make_ok(false);
    }

    return RetVal<bool>::make_ok(last.window != mainWindow()->qWindow());
}

async::Notification InteractiveProvider::currentUriAboutToBeChanged() const
{
    return m_currentUriAboutToBeChanged;
}

std::vector<Uri> InteractiveProvider::stack() const
{
    std::vector<Uri> uris;
    for (const ObjectInfo& info : m_stack) {
        uris.push_back(info.query.uri());
    }
    return uris;
}

QWindow* InteractiveProvider::topWindow() const
{
    QWindow* mainWin = mainWindow()->qWindow();

    if (m_stack.empty()) {
        LOGE() << "stack is empty";
        return mainWin;
    }

    const ObjectInfo& last = m_stack.last();
    if (!last.window) {
        return mainWin;
    }

    if (last.window == mainWin) {
        return mainWin;
    }

    // TODO/HACK: last.window doesn't seem to have a parent when the top window is a widget....
    if (!last.window->parent() && !topWindowIsWidget()) {
        ASSERT_X("Window must have a parent!");
    }

    if (!last.window->isWidgetType()) {
        QWindow* qwindow = qobject_cast<QWindow*>(last.window);
        IF_ASSERT_FAILED(qwindow) {
            return mainWin;
        }
        return qwindow;
    }

    // QWidget
    QWidget* qwidget = qobject_cast<QWidget*>(last.window);
    QWindow* qwindow = qwidget->windowHandle();
    IF_ASSERT_FAILED(qwindow) {
        return mainWin;
    }
    return qwindow;
}

bool InteractiveProvider::topWindowIsWidget() const
{
    if (m_stack.empty()) {
        return false;
    }

    const ObjectInfo& last = m_stack.last();
    if (!last.window) {
        return false;
    }

    return last.window->isWidgetType();
}

QString InteractiveProvider::objectId(const QVariant& val) const
{
    static int count(0);

    ++count;

    QString objectId;
    if (val.canConvert<QObject*>()) {
        QObject* obj = val.value<QObject*>();
        IF_ASSERT_FAILED(obj) {
            return QString();
        }

        objectId = QString(obj->metaObject()->className()) + "_" + QString::number(count);
    } else {
        objectId = "unknown_" + QString::number(count);
    }
    return "object://" + objectId;
}

Ret InteractiveProvider::toRet(const QVariant& jsr) const
{
    QVariantMap jsobj = jsr.toMap();
    IF_ASSERT_FAILED(jsobj.contains("errcode")) {
        return make_ret(Ret::Code::UnknownError);
    }

    Ret ret;
    ret.setCode(jsobj.value("errcode").toInt());
    return ret;
}

RetVal<Val> InteractiveProvider::toRetVal(const QVariant& jsrv) const
{
    RetVal<Val> rv;
    QVariantMap jsobj = jsrv.toMap();

    IF_ASSERT_FAILED(jsobj.contains("errcode")) {
        rv.ret = make_ret(Ret::Code::UnknownError);
        return rv;
    }

    int errcode = jsobj.value("errcode").toInt();
    QVariant val = jsobj.value("value");

    rv.ret.setCode(errcode);
    rv.val = Val::fromQVariant(val);

    return rv;
}

RetVal<bool> InteractiveProvider::isOpened(const QString& objectId) const
{
    for (const ObjectInfo& objectInfo: allOpenObjects()) {
        if (objectInfo.objectId == objectId) {
            return RetVal<bool>::make_ok(true);
        }
    }

    return RetVal<bool>::make_ok(false);
}

RetVal<InteractiveProvider::OpenData> InteractiveProvider::openWidgetDialog(const Uri& uri, const QVariantMap& params)
{
    RetVal<OpenData> result;

    ContainerMeta meta = uriRegister()->meta(uri);
    int widgetMetaTypeId = meta.widgetMetaTypeId;

    static int count(0);
    QString objectId = QString("%1_%2").arg(widgetMetaTypeId).arg(++count);

    QMetaType metaType = QMetaType(widgetMetaTypeId);
    QDialog* dialog = static_cast<QDialog*>(metaType.create());

    if (!dialog) {
        result.ret = make_ret(Ret::Code::UnknownError);
        return result;
    }

    fillData(dialog, params);

    //! NOTE Will be deleted with the dialog
    WidgetDialogAdapter* adapter = new WidgetDialogAdapter(dialog);
    adapter->onShow([this, objectId, dialog]() {
        if (isOpened(objectId).val) {
            LOGE() << "Dialog is already opened: " << objectId << ", ignoring this show event";
            return;
        }

        async::Async::call(this, [this, objectId, dialog]() {
            QQmlEngine::setObjectOwnership(dialog, QQmlEngine::CppOwnership);
            QQmlEngine::setObjectOwnership(dialog->windowHandle(), QQmlEngine::CppOwnership);
            onOpen(ContainerType::QWidgetDialog, objectId, dialog->window());
        });
    })
    .onHide([this, objectId, dialog]() {
        if (!isOpened(objectId).val) {
            LOGE() << "Dialog is not opened: " << objectId << ", ignoring this hide event";
            return;
        }

        QDialog::DialogCode dialogCode = static_cast<QDialog::DialogCode>(dialog->result());
        Ret::Code errorCode = dialogCode == QDialog::Accepted ? Ret::Code::Ok : Ret::Code::Cancel;

        QVariantMap ret;
        ret["errcode"] = static_cast<int>(errorCode);

        onClose(objectId, ret);

        dialog->deleteLater();
    });

    bool modal = params.value("modal", "true") == "true";
    dialog->setWindowModality(modal ? Qt::ApplicationModal : Qt::NonModal);
    dialog->show();
    dialog->activateWindow();     // give keyboard focus to aid blind users

    result.ret = make_ret(Ret::Code::Ok);
    result.val.objectId = objectId;

    return result;
}

RetVal<InteractiveProvider::OpenData> InteractiveProvider::openQml(const Uri& uri, const QVariantMap& params)
{
    QmlLaunchData data;
    fillData(&data, uri, params);

    emit fireOpen(&data);

    RetVal<OpenData> result;
    result.ret = toRet(data.value("ret"));
    result.val.objectId = data.value("objectId").toString();

    return result;
}

void InteractiveProvider::closeQml(const QVariant& objectId)
{
    emit fireClose(objectId);
}

void InteractiveProvider::raiseQml(const QVariant& objectId)
{
    emit fireRaise(objectId);
}

void InteractiveProvider::onOpen(const QVariant& type, const QVariant& objectId, QObject* window)
{
    ContainerType::Type containerType = type.value<ContainerType::Type>();

    IF_ASSERT_FAILED(containerType != ContainerType::Undefined) {
        containerType = ContainerType::QmlDialog;
    }

    m_openingObject.objectId = objectId;
    m_openingObject.window = window;
    if (!m_openingObject.window) {
        m_openingObject.window = (containerType == ContainerType::PrimaryPage) ? mainWindow()->qWindow() : qApp->focusWindow();
    }

    if (m_openingObject.query.param("floating").toBool()) {
        m_floatingObjects.push_back(m_openingObject);
        m_openingObject = ObjectInfo(); // clear
        return;
    }

    if (ContainerType::PrimaryPage == containerType) {
        // Replace bottom item of the stack, because that always reflects the current PrimaryPage
        if (m_stack.empty()) {
            m_stack.push(m_openingObject);
        } else {
            m_stack[0] = m_openingObject;
        }
    } else if (ContainerType::QmlDialog == containerType) {
        m_stack.push(m_openingObject);
    } else if (ContainerType::QWidgetDialog == containerType) {
        m_stack.push(m_openingObject);
    } else {
        IF_ASSERT_FAILED_X(false, "unknown page type") {
            m_stack.push(m_openingObject);
        }
    }

    notifyAboutCurrentUriChanged();

    Uri uri = m_openingObject.query.uri();
    m_openingObject = ObjectInfo();     // clear

    Async::call(this, [this, uri]() {
        m_opened.send(uri);
    });
}

void InteractiveProvider::onClose(const QString& objectId, const QVariant& jsrv)
{
    RetVal<Val> rv = toRetVal(jsrv);

    ObjectInfo obj;

    bool inStack = false;
    for (int i = 0; i < m_stack.size(); ++i) {
        if (m_stack.at(i).objectId == objectId) {
            obj = m_stack.at(i);
            inStack = true;
            m_stack.remove(i);
            break;
        }
    }

    if (!inStack) {
        for (size_t i = 0; i < m_floatingObjects.size(); ++i) {
            if (m_floatingObjects.at(i).objectId == objectId) {
                obj = m_floatingObjects.at(i);
                m_floatingObjects.erase(m_floatingObjects.begin() + i);
                break;
            }
        }
    }

    DO_ASSERT(obj.objectId.isValid());

    if (rv.ret) {
        (void)obj.resolve(rv.val);
    } else {
        (void)obj.reject(rv.ret.code(), rv.ret.text());
    }

    if (inStack) {
        notifyAboutCurrentUriChanged();
    }

    Async::call(this, [this, objectId, rv]() {
        auto onClosedIt = m_onClosedFuncs.find(objectId);
        if (onClosedIt != m_onClosedFuncs.end()) {
            onClosedIt->second(rv.ret);
        }
    });
}

std::vector<InteractiveProvider::ObjectInfo> InteractiveProvider::allOpenObjects() const
{
    std::vector<ObjectInfo> result;

    result.insert(result.end(), m_stack.cbegin(), m_stack.cend());
    result.insert(result.end(), m_floatingObjects.cbegin(), m_floatingObjects.cend());

    return result;
}

std::vector<InteractiveProvider::ObjectInfo> InteractiveProvider::collectOpenObjects(std::function<bool(const ObjectInfo&)> accepted) const
{
    std::vector<ObjectInfo> result;
    for (const ObjectInfo& obj : allOpenObjects()) {
        if (accepted(obj)) {
            result.push_back(obj);
        }
    }

    return result;
}

void InteractiveProvider::notifyAboutCurrentUriChanged()
{
    m_currentUriChanged.send(currentUri().val);
}

void InteractiveProvider::notifyAboutCurrentUriWillBeChanged()
{
    m_currentUriAboutToBeChanged.notify();
}

// === QmlLaunchData ===
QmlLaunchData::QmlLaunchData(QObject* parent)
    : QObject(parent)
{
}

QVariant QmlLaunchData::value(const QString& key) const
{
    return m_data.value(key);
}

void QmlLaunchData::setValue(const QString& key, const QVariant& val)
{
    m_data[key] = val;
}

QVariant QmlLaunchData::data() const
{
    return m_data;
}
