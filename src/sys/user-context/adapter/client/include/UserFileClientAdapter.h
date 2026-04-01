#pragma once
#include "dependencyinjector.h"
#include "sys/file-context/application/service/include/FileApplicationService.h"
#include "sys/user-context/port/client/include/FileClient.h"

namespace sys::user::adapter
{
    class FileClientAdapter final : public port::FileClient
    {
    public:
        explicit FileClientAdapter(FileApplicationService* fileApplicationService = QInjection::Inject)
            : fileApplicationService(fileApplicationService)
        {
        }

        void uploadAvatar(const QSharedPointer<domain::User>& user) override;

    private:
        FileApplicationService* fileApplicationService;
    };
}
