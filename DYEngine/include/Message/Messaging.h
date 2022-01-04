#pragma once

#include <type_traits>
#include <string>

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
}
