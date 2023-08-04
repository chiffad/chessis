#pragma once

#include <QByteArray>
#include <QHostAddress>
#include <QString>
#include <functional>
#include <string>

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

std::ostream& operator<<(std::ostream& os, const QString& str);
std::ostream& operator<<(std::ostream& os, const QByteArray& str);
std::ostream& operator<<(std::ostream& os, const QHostAddress& str);
