/**
 * @file NrmPlanScenarioBinding.cpp
 * @brief Implements one-file resource-plan scenario binding resolution.
 */

#include "NrmPlanScenarioBinding.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace
{
const QString cDIRECTIVE = QString::fromLatin1("# NRM_SCENARIO");

QString Unquote(QString aValue)
{
   aValue = aValue.trimmed();
   if (aValue.size() >= 2 && aValue.front() == QLatin1Char('"') &&
       aValue.back() == QLatin1Char('"'))
   {
      aValue = aValue.mid(1, aValue.size() - 2).trimmed();
   }
   return aValue;
}

QString ResolveCandidate(const QString& aValue, const QString& aSourceRoot)
{
   const QString value = Unquote(aValue);
   if (value.isEmpty()) return QString();
   const QFileInfo candidate(QDir::isAbsolutePath(value)
                                ? value
                                : QDir(aSourceRoot).filePath(value));
   return candidate.exists() && candidate.isFile()
             ? candidate.canonicalFilePath()
             : QString();
}

QString ReadEmbeddedBinding(const QString& aPlanPath,
                            const QString& aSourceRoot)
{
   QFile plan(aPlanPath);
   if (!plan.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
   while (!plan.atEnd())
   {
      const QString line = QString::fromUtf8(plan.readLine()).trimmed();
      if (line.startsWith(cDIRECTIVE))
      {
         return ResolveCandidate(line.mid(cDIRECTIVE.size()), aSourceRoot);
      }
   }
   return QString();
}
}

QString WkNrm::ResolvePlanScenarioPath(const QString& aPlanPath,
                                       const QString& aSourceRoot)
{
   const QString embedded = ReadEmbeddedBinding(aPlanPath, aSourceRoot);
   if (!embedded.isEmpty()) return embedded;

   QFile binding(aPlanPath + QString::fromLatin1(".scenario"));
   if (!binding.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
   return ResolveCandidate(QString::fromUtf8(binding.readLine()), aSourceRoot);
}
