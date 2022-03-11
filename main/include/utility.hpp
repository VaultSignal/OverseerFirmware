#ifndef VAULT_SIGNAL_UTILITY_HPP
#define VAULT_SIGNAL_UTILITY_HPP
#include <string>

namespace VaultSignal
{
    class utility
    {
    public:
        /**
         * @brief Convert a boolean to its string representation.
         *
         * @param val Value to be converted.
         * @return std::string "true" for true, "false" for false.
         */
        static const char *booleanToString(bool val);
    };
}; // VaultSignal

#endif // VAULT_SIGNAL_UTILITY_HPP
