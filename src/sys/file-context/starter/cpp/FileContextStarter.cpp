#include "../include/FileContextStarter.h"

#include "dependencypool.h"
#include "sys/file-context/application/service/include/FileApplicationService.h"

namespace sys::file::starter
{

    void FileContextStarter::bootstrap()
    {
        QInjection::addSingleton(new FileApplicationService);
    }

}
