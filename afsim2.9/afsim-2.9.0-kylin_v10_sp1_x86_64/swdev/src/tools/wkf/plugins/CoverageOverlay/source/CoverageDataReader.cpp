// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "CoverageDataReader.hpp"

#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include "UtCast.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtUnits.hpp"

namespace CoverageOverlay
{

CoverageDataReader::CoverageDataReader(const QString& aFileName)
{
   QFile inFile{aFileName};
   if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      std::stringstream msg{};
      msg << "Unable to open file " << aFileName.toStdString();
      throw std::runtime_error{msg.str()};
   }
   QTextStream inStream{&inFile};

   mData = ut::make_unique<CoverageData>();

   ReadHeader(inStream);
   for (int i = 0; i < mNumComponents; ++i)
   {
      ReadBlock(inStream);
   }
   mData->SetFileName(aFileName.toStdString());
   mValid = true;
}

std::unique_ptr<CoverageData> CoverageDataReader::GetLoadedData()
{
   if (mValid)
   {
      mValid = false;
      return std::move(mData);
   }
   return nullptr;
}

void CoverageDataReader::ReadHeader(QTextStream& aStream)
{
   // Read line with coverage name
   QString line{};
   if (aStream.readLineInto(&line))
   {
      mData->SetCoverageName(line.toStdString());
   }
   else
   {
      throw std::runtime_error{"Error reading coverage name from file."};
   }

   // Read line with grid name, and ignore it
   if (!aStream.readLineInto(&line))
   {
      throw std::runtime_error{"Error reading grid name from file."};
   }

   // Read line with number of components
   if (aStream.readLineInto(&line))
   {
      bool ok;
      mNumComponents = line.toInt(&ok);
      if (ok)
      {
         if (mNumComponents <= 0)
         {
            throw std::runtime_error{"Number of components must be positive."};
         }
      }
      else
      {
         throw std::runtime_error{"Expected integer for number of components."};
      }
   }
   else
   {
      throw std::runtime_error{"Error reading component count."};
   }

   // Read line with number of measures
   if (aStream.readLineInto(&line))
   {
      bool ok;
      int  num = line.toInt(&ok);
      if (ok)
      {
         if (num > 0)
         {
            mData->SetNumFields(ut::cast_to_size_t(num));
         }
         else
         {
            throw std::runtime_error{"Number of fields must be positive."};
         }
      }
      else
      {
         throw std::runtime_error{"Expected integer for number of fields."};
      }
   }
   else
   {
      throw std::runtime_error{"Error reading number of fields."};
   }

   ReadMeasureNames(aStream);
   ReadMeasureTypes(aStream);
   ReadMeasureSubtypes(aStream);
}

void CoverageDataReader::ReadMeasureNames(QTextStream& aStream)
{
   QString line;

   if (aStream.readLineInto(&line))
   {
      auto tokens = line.split(QString{","});
      if (ut::cast_to_size_t(tokens.size()) != mData->GetNumFields())
      {
         throw std::runtime_error{"Number of field names must match number of fields."};
      }

      std::vector<std::string> fields{};
      for (int i = 0; i < tokens.size(); ++i)
      {
         fields.emplace_back(tokens.at(i).trimmed().toStdString());
      }
      mData->SetFieldNames(std::move(fields));
   }
   else
   {
      throw std::runtime_error{"Error reading field names."};
   }
}

void CoverageDataReader::ReadMeasureTypes(QTextStream& aStream)
{
   QString line;

   if (aStream.readLineInto(&line))
   {
      auto tokens = line.split(QString{","});
      if (ut::cast_to_size_t(tokens.size()) != mData->GetNumFields())
      {
         throw std::runtime_error{"Number of field types must match number of fields."};
      }

      std::vector<std::string> types{};
      for (int i = 0; i < tokens.size(); ++i)
      {
         types.emplace_back(tokens.at(i).trimmed().toStdString());
      }
      mData->SetFieldTypes(std::move(types));
   }
   else
   {
      throw std::runtime_error{"Error reading field types."};
   }
}

void CoverageDataReader::ReadMeasureSubtypes(QTextStream& aStream)
{
   QString line;

   if (aStream.readLineInto(&line))
   {
      auto tokens = line.split(QString{","});
      if (ut::cast_to_size_t(tokens.size()) != mData->GetNumFields())
      {
         throw std::runtime_error{"Number of field subtypes must match number of fields."};
      }

      std::vector<std::string> subtypes{};
      std::vector<int>         unitTypes{};
      for (int i = 0; i < tokens.size(); ++i)
      {
         // Search for [*] in the string
         int opening = tokens.at(i).indexOf("[");
         if (opening == -1)
         {
            // No unit type text, so save results directly
            subtypes.emplace_back(tokens.at(i).trimmed().toStdString());
            unitTypes.push_back(UtUnits::_cUNIT_TYPE_COUNT);
         }
         else
         {
            int closing = tokens.at(i).indexOf("]", opening);
            if (closing == -1)
            {
               throw std::runtime_error{"Incorrect unit specification on field subtype"};
            }
            else
            {
               // Detect if that is a valid unit
               auto unitString = tokens.at(i).mid(opening + 1, closing - opening - 1);
               // Set the unit type if it exists, or to the default value if not
               int type = 0;
               int end  = UtUnits::_cUNIT_TYPE_COUNT;
               for (; type < end; ++type)
               {
                  if (UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(type))->IsUnitValid(unitString.toStdString()))
                  {
                     break;
                  }
               }
               if (type == end)
               {
                  throw std::runtime_error{"Unknown units"};
               }

               QString modified{tokens.at(i)};
               subtypes.emplace_back(
                  modified.replace(opening, closing - opening + 1, QChar{' '}).simplified().toStdString());
               unitTypes.push_back(type);
            }
         }
      }
      mData->SetFieldSubtypes(std::move(subtypes));
      mData->SetFieldUnitTypes(std::move(unitTypes));
   }
   else
   {
      throw std::runtime_error{"Error reading field types."};
   }
}

void CoverageDataReader::ReadBlock(QTextStream& aStream)
{
   QString line{};
   size_t  pointCountLat;
   size_t  pointCountLon;

   // Read line with component name and point count
   if (aStream.readLineInto(&line))
   {
      auto tokens = line.split(QString{","});
      if (tokens.size() != 3)
      {
         throw std::runtime_error{"Block information formatting error."};
      }

      bool ok;
      pointCountLat = ut::cast_to_size_t(tokens.at(1).toInt(&ok));
      if (!ok)
      {
         throw std::runtime_error{"Expected an integer for the number of latitudes in a block."};
      }

      pointCountLon = ut::cast_to_size_t(tokens.at(2).toInt(&ok));
      if (!ok)
      {
         throw std::runtime_error{"Expected an integer for the number of longitudes in a block."};
      }

      if (pointCountLat < 2 || pointCountLon < 2)
      {
         throw std::runtime_error{"Coverage overlays require at least a 2x2 grid of points."};
      }
   }
   else
   {
      throw std::runtime_error{"Error reading block name and size."};
   }

   // Read in the point records
   std::vector<Record> records{};
   auto                pointCount = pointCountLat * pointCountLon;
   for (size_t i = 0; i < pointCount; ++i)
   {
      if (!aStream.readLineInto(&line))
      {
         throw std::runtime_error{"Error reading point record."};
      }
      auto tokens = line.split(QString{","});
      if (ut::cast_to_size_t(tokens.size()) != mData->GetNumFields() + 5)
      {
         // Each line should have one entry per field, plus the ID, lat. Index, lon. Index, Lat, Lon and Alt
         throw std::runtime_error{"Unexpected number of values for a point record"};
      }

      Record rec{};
      bool   ok;
      rec.mLatIndex = ut::cast_to_size_t(tokens.at(0).toInt(&ok));
      if (!ok || rec.mLatIndex >= pointCountLat)
      {
         throw std::runtime_error{"Unable to interpret latitude index for a point record."};
      }
      rec.mLonIndex = ut::cast_to_size_t(tokens.at(1).toInt(&ok));
      if (!ok || rec.mLonIndex >= pointCountLon)
      {
         throw std::runtime_error{"Unable to interpret longitude index for a point record."};
      }
      for (int field = 2; field < tokens.size(); ++field)
      {
         rec.mFields.push_back(tokens.at(field).toFloat(&ok));
         if (!ok)
         {
            throw std::runtime_error{"Unable to convert value from file."};
         }
      }
      records.emplace_back(std::move(rec));
   }

   ProcessRecords(pointCountLat, pointCountLon, std::move(records));
}

void CoverageDataReader::ProcessRecords(std::size_t aNumLats, std::size_t aNumLons, std::vector<Record>&& aRecords)
{
   CoverageData::Component comp{aNumLats, aNumLons};
   for (auto& record : aRecords)
   {
      comp.SetValues(record.mLatIndex, record.mLonIndex, record.mFields);
   }
   mData->AddComponent(std::move(comp));
}

} // namespace CoverageOverlay
