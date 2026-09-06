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
// Entity.cpp : implementation file

#include "UtoEntity.hpp"

#include <sstream>

#include "UtMath.hpp"
#include "UtoEntity.hpp"
#include "UtoEntityDB.hpp"
#include "UtoPriv.hpp"
#include "UtoShape.hpp"
#include "UtoShapeImp.hpp"
#include "UtoViewer.hpp"
#ifdef _WIN32
#pragma warning(disable : 4355)
#endif

unsigned long UtoEntity::m_nNextId = 0;

UtoEntity::UtoEntity()
   : m_sName("")
   , mDeferredPositionUpdate(false)
{
   mMoveCount     = 0;
   double zero[3] = {0.0, 0.0, 0.0};
   UtEntity::SetLocationWCS(zero); // we attempt to replicate old behavior
   UtEntity::SetOrientationWCS(UtMath::cPI, UtMath::cPI, UtMath::cPI);
   m_sObjectName = m_nNextId++;
}

UtoEntity::UtoEntity(const UtoEntity& rhs)
   : UtEntity(rhs)
   , m_sName(rhs.m_sName)
   , m_Shapes()
   , mDeferredPositionUpdate(false)
{
   // copy all the shapes.
   UtoShapeMap::const_iterator it, end(rhs.m_Shapes.end());
   for (it = rhs.m_Shapes.begin(); it != end; ++it)
   {
      Bind(it->first, *it->second);
   }
   m_sObjectName = m_nNextId++;
}

UtoEntity::~UtoEntity()
{
   // Invoke the callbacks for the interested observers
   sendEvent(DestroyedEvent(), this);
}

/* UTO Function Block **********************************************************

Function: SetName

Description:
   Sets the display name

*******************************************************************************/

void UtoEntity::SetName(const std::string& s)
{
   m_sName = s;
}

void UtoEntity::SetLocationLLA(double aLatitude, double aLongitude, double aAltitude)
{
   UtEntity::SetLocationLLA(aLatitude, aLongitude, aAltitude);
   UtEntity::SetOrientationNED(0, 0, 0);
   // mLocalCF.SetLocationWCS(aLocationWCS);
   mDeferredPositionUpdate = false;
   sendEvent(PositionChangedEvent(), this);
   ++mMoveCount;
}

void UtoEntity::SetLocationWCS(const double aLocationWCS[3])
{
   UtEntity::SetLocationWCS(aLocationWCS);
   UtEntity::SetOrientationNED(0, 0, 0);
   // mLocalCF.SetLocationWCS(aLocationWCS);
   mDeferredPositionUpdate = false;
   sendEvent(PositionChangedEvent(), this);
   ++mMoveCount;
}

void UtoEntity::SetLocationWCS(const double aLocationWCS[3], bool aUpdate)
{
   UtEntity::SetLocationWCS(aLocationWCS);
   UtEntity::SetOrientationNED(0, 0, 0);
   // mLocalCF.SetLocationWCS(aLocationWCS);
   if (aUpdate)
   {
      mDeferredPositionUpdate = false;
      sendEvent(PositionChangedEvent(), this);
   }
   else
   {
      mDeferredPositionUpdate = true;
   }
   ++mMoveCount;
}

void UtoEntity::SetOrientationWCS(double aPsiWCS, double aThetaWCS, double aPhiWCS)
{
   UtEntity::SetOrientationWCS(aPsiWCS, aThetaWCS, aPhiWCS);
   if (mDeferredPositionUpdate)
   {
      mDeferredPositionUpdate = false;
      sendEvent(PosnOrientChangedEvent(), this);
   }
   else
   {
      sendEvent(OrientationChangedEvent(), this);
   }
   ++mMoveCount;
}


void UtoEntity::SetOrientationNED(double aHeadingNED, double aPitchNED, double aRollNED)
{
   UtEntity::SetOrientationNED(aHeadingNED, aPitchNED, aRollNED);
   if (mDeferredPositionUpdate)
   {
      mDeferredPositionUpdate = false;
      sendEvent(PosnOrientChangedEvent(), this);
   }
   else
   {
      sendEvent(OrientationChangedEvent(), this);
   }
   ++mMoveCount;
}

bool UtoEntity::UnbindShape(const std::string& rName)
{
   UtoShape* shapePtr = FindShape(rName);
   if (shapePtr)
   {
      shapePtr->UnbindViewers();
      shapePtr->Impl()->SetOwner(nullptr);
      delete shapePtr;
      m_Shapes.erase(rName);
      return true;
   }
   return false;
}

UtoShape* UtoEntity::FindShape(const std::string& name) const
{
   UtoShapeMap::const_iterator i = m_Shapes.find(name);
   if (i != m_Shapes.end())
   {
      return i->second;
   }
   return nullptr;
}

UtoShape* UtoEntity::Bind(const std::string& name, const UtoShape& shape)
{
   if (FindShape(name) == nullptr)
   {
      UtoShape* p = shape.Clone();

      std::pair<UtoShapeMap::iterator, bool> ib = m_Shapes.insert(std::make_pair(name, p));
      if (!ib.second)
      {
         // DEBUG(("Failed to insert shape %s during bind.", name.c_str()));
         delete p;
         return nullptr;
      }
      else
      {
         std::string fname = Name() + "_" + name;
         //         p->Impl()->SetName(fname);
         p->Impl()->SetName(name);
         p->Impl()->GetTree()->setName(name);
         p->Impl()->GetTree()->setUserData(new UtoViewerShapeData(p->Impl()));
         p->Impl()->SetOwner(this);
         for (size_t i = 0; i < mDb->Viewers().size(); ++i)
         {
            UtoViewer& v = *mDb->Viewers()[i];
            p->BindToViewer(v);
         }
         return p;
      }
   }
   else
   {
      return nullptr;
   }
}

UtoShape* UtoEntity::Rebind(const std::string& name, const UtoShape& shape)
{
   UnbindShape(name);
   return BindShape(name, shape);
}

void UtoEntity::Dump(std::ostream& os)
{
   os << "  Object Name: " << ObjectName() << std::endl;
   os << "  Name:      " << Name() << std::endl;
   double lla[3];
   GetLocationLLA(lla[0], lla[1], lla[2]);
   os << "  Position(LLA):   " << lla[0] << " " << lla[1] << " " << lla[2] << std::endl;
   double hpr[3];
   GetOrientationNED(hpr[0], hpr[1], hpr[2]);
   os << "  Orientation(HPR-NED): " << hpr[0] << " " << hpr[1] << " " << hpr[2] << std::endl;

   os << "Shape Dump" << std::endl;

   if (m_Shapes.empty())
   {
      os << "  None" << std::endl;
   }
   else
   {
      for (UtoShapeMap::iterator pr = m_Shapes.begin(); pr != m_Shapes.end(); ++pr)
      {
         os << "  Name: " << pr->first << std::endl;
         pr->second->Dump(os);
         os << std::endl;
      }
   }
}

void UtoEntity::ViewerAdded(UtoViewer* aViewerPtr)
{
   for (UtoShapeMap::iterator i = m_Shapes.begin(); i != m_Shapes.end(); ++i)
   {
      i->second->BindToViewer(*aViewerPtr);
   }
}

void UtoEntity::ViewerRemoved(UtoViewer* aViewerPtr)
{
   for (UtoShapeMap::iterator i = m_Shapes.begin(); i != m_Shapes.end(); ++i)
   {
      i->second->UnbindViewer(*aViewerPtr);
   }
}

bool UtoEntity::IsVisible(const UtoShape& aShape, UtoViewer* aViewer) const
{
   return aShape.IsVisible(aViewer);
}

bool UtoEntity::IsVisible(const std::string& aShapeName, UtoViewer* aViewer) const
{
   UtoShape* shape = FindShape(aShapeName);
   return shape && shape->IsVisible(aViewer);
}

bool UtoEntity::Show(bool aShow, const std::string& aShapeName, UtoViewer* aViewer)
{
   UtoShape* s = FindShape(aShapeName);
   return s && s->Show(aShow, *aViewer);
}

std::vector<UtoShape*> UtoEntity::GetShapes() const
{
   std::vector<UtoShape*> shapes;
   for (UtoShapeMap::const_iterator i = m_Shapes.begin(); i != m_Shapes.end(); ++i)
   {
      shapes.push_back(i->second);
   }
   return shapes;
}

void UtoEntity::shapeShown()
{
   // the orientation needs to be updated when a shape changes 'AllowUpdates()' to true
   ++mMoveCount;
}

void UtoEntity::SetDB(UtoEntityDB* db)
{
   mDb = db;
}
