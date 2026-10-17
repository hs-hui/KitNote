#ifndef KFW_MI_IPROJECTPROVIDER_H
#define KFW_MI_IPROJECTPROVIDER_H

#include "modularity/imoduleinterface.h"

#include "global/io/path.h"

namespace kfw::mi {
class IProjectProvider : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(kfw::mi::IProjectProvider);

public:

    virtual ~IProjectProvider() = default;

    virtual bool isProjectOpened(const io::path_t& path) const = 0;
    virtual bool isAnyProjectOpened() const = 0;
};
}

#endif // KFW_MI_IPROJECTPROVIDER_H
