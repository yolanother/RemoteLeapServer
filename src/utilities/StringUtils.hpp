// Borrowed from
// https://github.com/Loki-Astari/Examples/blob/master/Version2/Utility.h
// License: MIT

#ifndef THORSANVIL_SOCKET_UTILITY_H
#define THORSANVIL_SOCKET_UTILITY_H

#include <string>
#include <sstream>
#include <utility>
#include <cstddef>

#define throwRuntime(...) throw std::runtime_error (ThorsAnvil::Socket::buildErrorMessage(TAG, "::", __func__, ": ", __VA_ARGS__))
#define throwLogicError(...) throw std::logic_error (ThorsAnvil::Socket::buildErrorMessage(TAG, "::", __func__, ": ", __VA_ARGS__))
#define throwDomainError(...) throw std::domain_error (ThorsAnvil::Socket::buildErrorMessage(TAG, "::", __func__, ": ", __VA_ARGS__))

namespace ThorsAnvil
{
    namespace Socket
    {
        template<typename... Args>
        int print(std::ostream& s, Args&... args)
        {
            using Expander = int[];
            return Expander{ 0, ((s << std::forward<Args>(args)), 0)...}[0];
        }

        template<typename... Args>
        std::string buildStringFromParts(Args const&... args)
        {
            std::stringstream msg;
            print(msg, args...);
            return msg.str();
        }

        template<typename... Args>
        std::string buildErrorMessage(Args const&... args)
        {
            return buildStringFromParts(args...);
        }
    }
}

#endif