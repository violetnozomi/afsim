// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VehicleGeometry.hpp"

namespace Designer
{

VehicleGeometry::VehicleGeometry()
{
}

VehicleGeometry::~VehicleGeometry()
{
}

size_t VehicleGeometry::AddGeometryObject(GeometryObject* aGeometryObject)
{
   ++mMaxMapIndex;
   mGeometryObjectMap[mMaxMapIndex] = aGeometryObject;
   return mMaxMapIndex;
}

GeometryObject* VehicleGeometry::GetGeometryObjectWithWidget(QTreeWidgetItem* aItem)
{
   if (aItem == nullptr) { return nullptr; }

   for (auto& geometryObj : mGeometryObjectMap)
   {
      GeometryObject* obj = geometryObj.second;
      if (obj != nullptr)
      {
         QTreeWidgetItem* widget = obj->GetTreeWidgetItem();
         if (widget != nullptr)
         {
            if (widget == aItem)
            {
               // Match found
               return obj;
            }
         }
      }
   }

   return nullptr;
}

bool VehicleGeometry::AddMassPropertiesObject(GeometryMassProperties* aMassProperties)
{
   if (mMassProperties == nullptr)
   {
      mMassProperties = aMassProperties;
      return true;
   }

   return false;
}

void VehicleGeometry::RemoveGeometryObject(GeometryObject* aGeometryObject)
{
   for (auto iter = mGeometryObjectMap.begin(); iter != mGeometryObjectMap.end(); ++iter)
   {
      GeometryObject* obj = (*iter).second;
      if (obj != nullptr)
      {
         if (obj == aGeometryObject)
         {
            // Free up memory
            delete obj;

            // Remove this object from the map
            mGeometryObjectMap.erase(iter);

            return;
         }
      }
   }
}

void VehicleGeometry::ResetVehicleGeometry()
{
   mGeometryObjectMap.clear();

   mMassProperties = nullptr;
   mFuselage       = nullptr;
   mBody           = nullptr;
   mWing           = nullptr;
}

bool VehicleGeometry::AddFuselage(GeometryFuselage* aFuselage)
{
   if (mFuselage == nullptr)
   {
      mFuselage = aFuselage;
      return true;
   }

   return false;
}

bool VehicleGeometry::AddBody(GeometryBody* aBody)
{
   if (mBody == nullptr)
   {
      mBody = aBody;
      return true;
   }

   return false;
}

bool VehicleGeometry::AddWing(GeometryWing* aWing)
{
   if (mWing == nullptr)
   {
      mWing = aWing;
      return true;
   }

   return false;
}

bool Designer::VehicleGeometry::AddPropulsionData(GeometryPropulsionData* aPropulsionData)
{
   if (mPropulsionData == nullptr)
   {
      mPropulsionData = aPropulsionData;
      return true;
   }

   return false;
}

GeometrySurface* VehicleGeometry::GetHorizontalStab() const
{
   return GetControlFins("Horizontal Stab");
}

GeometrySurface* VehicleGeometry::GetVerticalTail() const
{
   return GetControlFins("Vertical Tail");
}

GeometrySurface* VehicleGeometry::GetCanard() const
{
   return GetControlFins("Canard");
}

GeometrySurface* VehicleGeometry::GetVentralTail() const
{
   return GetControlFins("Ventral Tail");
}

GeometrySurface* VehicleGeometry::GetVTail() const
{
   return GetControlFins("V-Tail");
}

GeometrySurface* VehicleGeometry::GetInvertedVTail() const
{
   return GetControlFins("Inverted V-Tail");
}

GeometrySurface* VehicleGeometry::GetCanardControlFins() const
{
   return GetControlFins("Canard Control Fin");
}

GeometrySurface* VehicleGeometry::GetMidControlFins() const
{
   return GetControlFins("Mid Control Fin");
}

GeometrySurface* VehicleGeometry::GetTailControlFins() const
{
   return GetControlFins("Tail Control Fin");
}

GeometrySurface* VehicleGeometry::GetControlFins(const std::string& aFinName) const
{
   // Loop through the geometry objects, looking for various control fins
   for (auto& obj : mGeometryObjectMap)
   {
      GeometryObject* object = obj.second;

      if (object->GeometryObjectType() == "GeometrySurface")
      {
         if (object->GetName() == aFinName)
         {
            return dynamic_cast<GeometrySurface*>(object);
         }
      }
   }

   return (GeometrySurface*)nullptr;
}

QStringList VehicleGeometry::Audit()
{
   QStringList fullList;
   
   for (auto& iter : mGeometryObjectMap)
   {
      GeometryObject* obj = iter.second;
      if (obj != nullptr)
      {
         QStringList tempList = obj->Audit();

         std::copy(tempList.begin(), tempList.end(), std::back_inserter(fullList));
      }
   }

   return fullList;
}

bool VehicleGeometry::NameIsUnique(const std::string& aName)
{
   for (auto& object : mGeometryObjectMap)
   {
      GeometryObject* obj = object.second;
      if (aName == obj->GetName())
      {
         // Match found -- name is not unique
         return false;
      }
   }

   return true;
}

} // namespace Designer
