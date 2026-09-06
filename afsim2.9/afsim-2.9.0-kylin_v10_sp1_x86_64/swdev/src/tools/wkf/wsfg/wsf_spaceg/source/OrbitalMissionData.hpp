// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ORBITALMISSIONDATA_HPP
#define ORBITALMISSIONDATA_HPP

#include <map>

#include <QJsonObject>

namespace wsfg
{

namespace spaceg
{

class OrbitalMissionData
{
public:
   OrbitalMissionData() = default;

   QJsonObject GetItemData(int aOrderedIndex) const;
   void        SetItemData(int aOrderedIndex, const QJsonObject& aData, int aParent, int aRow);

   template<typename Callable>
   void ApplyToChildrenOf(int aOrderedIndex, Callable& aOperation) const
   {
      for (const auto& iter : mValues)
      {
         if (iter.second.mParent == aOrderedIndex)
         {
            aOperation(iter.first, iter.second.mParent, iter.second.mRow, iter.second.mData);
         }
      }
   }

private:
   struct Value
   {
      QJsonObject mData;
      int         mParent;
      int         mRow;
   };
   std::map<int, Value> mValues;
};

} // namespace spaceg

} // namespace wsfg

#endif // ORBITALMISSIONDATA_HPP
