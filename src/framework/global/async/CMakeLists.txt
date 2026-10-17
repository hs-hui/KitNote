include(../thirdparty/kors_async/async/async.cmake)

target_sources(kfw_global PRIVATE
    ${KORS_ASYNC_SRC}
    async.h
    asyncable.h
    changednotify.h
    channel.h
    notification.h
    notifylist.h
    processevents.h
    promise.h
)
