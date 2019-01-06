#ifndef DT_LOGGING_H
#define DT_LOGGING_H

#include "StringUtils.hpp"

#define logd(...) DoubTech::Utils::Log::d(TAG, __VA_ARGS__)
#define logw(...) DoubTech::Utils::Log::w(TAG, __VA_ARGS__)
#define loge(...) DoubTech::Utils::Log::e(TAG, __VA_ARGS__)

namespace DoubTech {
    namespace Utils {
        class Log {
            public:
                template<typename... Args>
                static void d(const char *tag, Args const&... args) {
                    fprintf(stderr, "%s [DEBUG] %s\n", tag, ThorsAnvil::Socket::buildStringFromParts(args...).c_str());
                }

                template<typename... Args>
                static void w(const char* tag, Args const&... args) {
                    fprintf(stderr, "%s [WARN]  %s\n", tag, ThorsAnvil::Socket::buildStringFromParts(args...).c_str());
                }

                template<typename... Args>
                static void e(const char* tag, Args const&... args) {
                    fprintf(stderr, "%s [ERROR] %s\n", tag, ThorsAnvil::Socket::buildStringFromParts(args...).c_str());
                }
        };
    }
}

#endif