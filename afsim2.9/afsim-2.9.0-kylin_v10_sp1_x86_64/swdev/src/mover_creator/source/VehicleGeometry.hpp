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

#ifndef VEHICLE_GEOMETRY_HPP
#define VEHICLE_GEOMETRY_HPP

#include <map>
#include <string>

#include <QTreeWidgetItem>

#include "GeometryBody.hpp"
#include "GeometryFuselage.hpp"
#include "GeometryMassProperties.hpp"
#include "GeometryObject.hpp"
#include "GeometryPropulsionData.hpp"
#include "GeometryWing.hpp"

#include "UtDCM.hpp"
#include "UtMemory.hpp"
#include "UtVec3dX.hpp"

// VehicleGeometry is used to store data about the vehicle geometry for use with the tool's GUI

namespace Designer
{
   class VehicleGeometry
   {
      public:

         VehicleGeometry();
         ~VehicleGeometry();

         // Add a new geometry object to the list
         size_t AddGeometryObject(GeometryObject* aGeometryObject);
         GeometryObject* GetGeometryObjectWithWidget(QTreeWidgetItem* aItem);

         void RemoveGeometryObject(GeometryObject* aGeometryObject);
         void ResetVehicleGeometry();

         const std::map<size_t, GeometryObject*>& GetGeometryObjectMap() { return mGeometryObjectMap; }

         // Add a new geometry mass properties object. Returns true if "add" succeeds.
         // Only a single mass properties object should be added using this function.
         bool AddMassPropertiesObject(GeometryMassProperties* aMassProperties);

         // Returns a pointer to the mass properties object or nullptr if one does not exist.
         GeometryMassProperties* GetMassProperties() const { return mMassProperties; }

         // Add a new geometry fuselage object. Returns true if "add" succeeds.
         // Only a single fuselage object should be added using this function.
         bool AddFuselage(GeometryFuselage* aFuselage);

         // Returns a pointer to the fuselage or nullptr if one does not exist.
         GeometryFuselage* GetFuselage() const { return mFuselage; }

         // Add a new geometry body object. Returns true if "add" succeeds.
         // Only a single body object should be added using this function,
         // which represents the "body" of a weapon.
         bool AddBody(GeometryBody* aBody);

         // Returns a pointer to the body or nullptr if one does not exist.
         GeometryBody* GetBody() const { return mBody; }

         // Add a new geometry wing object. Returns true if "add" succeeds.
         // Only a single wing object should be added using this function.
         bool AddWing(GeometryWing* aWing);

         // Returns a pointer to the fuselage or nullptr if one does not exist.
         GeometryWing* GetWing() const { return mWing; }

         bool AddPropulsionData(GeometryPropulsionData* aPropulsionData);

         GeometryPropulsionData* GetPropulsionData() const { return mPropulsionData; }

         // Returns a pointer to the horizontal stab or nullptr if it does not exist
         GeometrySurface* GetHorizontalStab() const;

         // Returns a pointer to the vertical tail or nullptr if it does not exist
         GeometrySurface* GetVerticalTail() const;

         // Returns a pointer to the canard or nullptr if it does not exist
         GeometrySurface* GetCanard() const;

         // Returns a pointer to the ventral tail or nullptr if it does not exist
         GeometrySurface* GetVentralTail() const;

         // Returns a pointer to the v-tail or nullptr if it does not exist
         GeometrySurface* GetVTail() const;

         // Returns a pointer to the inverted v-tail or nullptr if it does not exist
         GeometrySurface* GetInvertedVTail() const;

         // Returns a pointer to a set of canard control fins or nullptr if one does not exist
         GeometrySurface* GetCanardControlFins() const;

         // Returns a pointer to a set of mid control fins or nullptr if one does not exist
         GeometrySurface* GetMidControlFins() const;

         // Returns a pointer to a set of tail control fins or nullptr if one does not exist
         GeometrySurface* GetTailControlFins() const;

         // This performs an "audit" to check that all geometry objects are fully
         // defined, identifying any issues that exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit();

         // Returns true if no existing geometry object has the specified name
         bool NameIsUnique(const std::string& aName);

      protected:

         GeometrySurface* GetControlFins(const std::string& aFinName) const;

         std::map<size_t, GeometryObject*> mGeometryObjectMap;
         size_t                            mMaxMapIndex       = 0;
         GeometryMassProperties*           mMassProperties    = nullptr;
         GeometryFuselage*                 mFuselage          = nullptr;
         GeometryBody*                     mBody              = nullptr;
         GeometryWing*                     mWing              = nullptr;
         GeometrySurface*                  mCanardControlFins = nullptr;
         GeometrySurface*                  mMidControlFins    = nullptr;
         GeometrySurface*                  mTailControlFins   = nullptr;
         GeometryPropulsionData*           mPropulsionData    = nullptr;
   };
}
#endif // !VEHICLE_GEOMETRY_HPP
