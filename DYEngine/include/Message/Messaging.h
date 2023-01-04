#pragma once

#include "Util/Macro.h"

#include <type_traits>
#include <string>
#include <vector>
#include <algorithm>

namespace DYE
{
    struct MessageData
    {
        /// Return the debug name of the MessageData
        virtual std::string GetName() const = 0;

        virtual ~MessageData() = default;
    };

    template<typename TMessageData>
    class MessageHandlerBase
    {
        static_assert(std::is_base_of<MessageData, TMessageData>::value, "TMessageData must inherit from MessageData.");
    public:
        /// Handle the given message data
        virtual void Handle(TMessageData& messageData) = 0;
    };

    template<typename TMessageData>
    class MessageDispatcherBase
    {
        static_assert(std::is_base_of<MessageData, TMessageData>::value, "TMessageData must inherit from MessageData.");
    public:
        /// Register message event handler, we pass in a raw pointer because the handler itself is responsible for registering & unregistering.
        void Register(MessageHandlerBase<TMessageData>* handler)
        {
#ifdef DYE_DEBUG
            auto registeredHandlerItr = std::find(std::begin(m_MessageHandlers), std::end(m_MessageHandlers), handler);

            if (registeredHandlerItr != std::end(m_MessageHandlers))
            {
                DYE_LOG_WARN("Register a handler that has already been registered.");
            }
#endif
            m_MessageHandlers.push_back(handler);
        }

        /// Unregister an message handler, we pass in a raw pointer because the handler itself is responsible for registering & unregistering.
        void Unregister(MessageHandlerBase<TMessageData>* handler)
        {
            auto registeredHandlerItr = std::find(std::begin(m_MessageHandlers), std::end(m_MessageHandlers), handler);

            if (registeredHandlerItr != std::end(m_MessageHandlers))
            {
                m_MessageHandlers.erase(registeredHandlerItr);
            }
            else
            {
                DYE_LOG_WARN("Try to unregister a handler that was not registered in the list.");
            }
        }

    protected:
        void broadcast(TMessageData& messageData)
        {
            for (MessageHandlerBase<TMessageData>* pHandler : m_MessageHandlers)
            {
                pHandler->Handle(messageData);
            }
        }

    private:
        std::vector<MessageHandlerBase<TMessageData>*> m_MessageHandlers {};
    };
}
