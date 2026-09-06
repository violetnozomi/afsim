// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoEntity.hpp : header file

#if !defined(_UTOENTITY_HPP_)
#define _UTOENTITY_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include <map>

#include "UtEntity.hpp"
#include "UtoCallback.hpp"
#include "UtoTypes.hpp"

class UtoEntityDB;
class UtoViewer;

class UTILOSG_EXPORT UtoEntity : public UtoCallback<UtoEntity>, private UtEntity
{
public:
   friend class UtoEntityDB;

   using shape_iterator = UtoShapeMap::iterator;

   UtoEntity();
   ~UtoEntity() override;
   UtoEntity(const UtoEntity&);

   unsigned long ObjectName() const { return m_sObjectName; }

   const std::string& Name() const { return m_sName; }
   void               SetName(const std::string& s);

   const UtoShapeMap& Shapes() const { return m_Shapes; }

   using PositionChangedEvent    = Event1<1, UtoEntity*>;
   using OrientationChangedEvent = Event1<2, UtoEntity*>;
   using PosnOrientChangedEvent  = Event1<3, UtoEntity*>;
   using DestroyedEvent          = Event1<6, UtoEntity*>;

   void SetLocationLLA(double aLatitude, double aLongitude, double aAltitude) override;
   void SetOrientationWCS(double aPsiWCS, double aThetaWCS, double aPhiWCS) override;
   void SetOrientationNED(double aHeadingNED, double aPitchNED, double aRollNED) override;
   void SetLocationWCS(const double aLocationWCS[3]) override;
   void SetLocationWCS(const double aLocationWCS[3], bool aUpdate);

   // UtoEntity inherits from UtEntity privately so the following functions need
   // to be exposed on UtoEntity's public interface.
   using UtEntity::GetLocationLLA;
   using UtEntity::GetLocationWCS;
   using UtEntity::GetOrientationNED;
   using UtEntity::GetOrientationWCS;

   // hey, only use this for UtoMapProjection::Convert, everything else is private to make
   // sure that we call the necessary callbacks!
   UtEntity& BodyCF() const { return *((UtEntity*)this); }

   bool      UnbindShape(const std::string& rName);
   UtoShape* FindShape(const std::string& name) const;
   UtoShape* Bind(const std::string& name, const UtoShape& shape);
   UtoShape* Rebind(const std::string& name, const UtoShape& shape);
   UtoShape* BindShape(const std::string& name, const UtoShape& shape) { return Bind(name, shape); }
   void      Dump(std::ostream& os);

   bool IsVisible(const UtoShape& aShape, UtoViewer* aViewer) const;
   bool IsVisible(const std::string& aShapeName, UtoViewer* aViewer) const;

   bool Show(bool aShow, const std::string& aShapeName, UtoViewer* aViewer);

   void ViewerAdded(UtoViewer* aViewer);
   void ViewerRemoved(UtoViewer* aViewer);

   std::vector<UtoShape*> GetShapes() const;

   unsigned int GetMoveCount() const { return mMoveCount; }
   void         ForceMoveUpdate() const { ++mMoveCount; }
   void         shapeShown();

   UtoEntityDB* GetDB() const { return mDb; }

protected:
private:
   UtoEntity& operator=(const UtoEntity&);
   void       SetDB(UtoEntityDB* db);


   static unsigned long m_nNextId;

   UtoEntityDB* mDb;

   unsigned long m_sObjectName;
   std::string   m_sName;

   UtoShapeMap m_Shapes;

   bool mDeferredPositionUpdate;
   // UtEntity   mLocalCF;
   //  incremented each time the entity moves
   mutable unsigned int mMoveCount;
};

#endif // !defined(_UTOENTITY_HPP_)
