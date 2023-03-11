#pragma once

#include "Message/Messaging.h"

namespace DYE
{
    class SandboxMessageA : public MessageData
    {

    };

    class  SandboxMessageB : public MessageData
    {

    };

    class SandboxMessageHandler : public MessageHandlerBase<SandboxMessageA>, public MessageHandlerBase<SandboxMessageB>
    {
    public:
        void Handle(SandboxMessageA &messageData) override;
        void Handle(SandboxMessageB &messageData) override;
    };
}