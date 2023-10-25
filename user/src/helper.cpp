#include "helper.h"

std::ostream& operator<<(std::ostream& os, const QString& str)
{
  return os << str.toStdString();
}

std::ostream& operator<<(std::ostream& os, const QByteArray& str)
{
  return os << str.toStdString();
}

std::ostream& operator<<(std::ostream& os, const QHostAddress& str)
{
  return os << str.toString();
}
