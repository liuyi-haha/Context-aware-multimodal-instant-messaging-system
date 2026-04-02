//
// Created by 86150 on 2026/3/30.
//

#include "../include/MessageClientAdapter.h"

namespace sys::relation::adapter
{
    QHash<QString, port::MessageClient::LastMessageInfo> adapter::MessageClientAdapter::getLastMessageInfos(
        const QList<QString>& chatSessionIds)
    {
        auto infos = messageApplicationService->getChatSessionLastMessageInfos(
            QSet<QString>(chatSessionIds.begin(), chatSessionIds.end()));
        // 将MessageApplicationService::LastMessageInfo转换为MessageClient::LastMessageInfo
        QHash<QString, port::MessageClient::LastMessageInfo> result;
        for (auto it = infos.begin(); it != infos.end(); ++it)
        {
            const auto& chatSessionId = it.key();
            const auto& info = it.value();
            LastMessageInfo clientInfo;
            clientInfo.text = info.text;
            clientInfo.fileName = info.mediaFileId;
            clientInfo.type = static_cast<MessageType>(info.type);
            clientInfo.sendTime = info.sendTime;
            result.insert(chatSessionId, clientInfo);
        }

        return result;
    }
}
