#pragma once

#include <QString>

#include <string_view>

namespace utils
{
inline QString strviewToQString(std::string_view str)
{
    return QString::fromStdString(std::string(str));
}
} // namespace utils
