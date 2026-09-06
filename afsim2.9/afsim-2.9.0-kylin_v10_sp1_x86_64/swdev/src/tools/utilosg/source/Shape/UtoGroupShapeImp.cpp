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
// UtoGroupShapeImp.cpp : implementation file

#include "UtoGroupShapeImp.hpp"

#include <algorithm>

#include "UtoShapeFrame.hpp"

#ifdef _WIN32
#pragma warning(disable : 4355)
#endif

struct SetFlag
{
   SetFlag(long mask, long flag)
      : m_Mask(mask)
      , m_Flag(flag)
   {
   }
   void operator()(UtoShapeMap::value_type& obj) { obj.second->Impl()->SetShapeFlags(m_Mask, m_Flag); }
   long m_Mask, m_Flag;
};

UtoGroupShapeImp::UtoGroupShapeImp()
   : m_Shapes()
   , m_RecursiveEnabled(false)
{
}

UtoGroupShapeImp::UtoGroupShapeImp(const UtoGroupShapeImp& rhs)
   : UtoShapeImp(rhs)
   , m_Shapes()
   , m_RecursiveEnabled(rhs.m_RecursiveEnabled)
{
   // copy all the shapes.
   UtoShapeMap::const_iterator it, end(rhs.m_Shapes.end());
   for (it = rhs.m_Shapes.begin(); it != end; ++it)
   {
      Bind(it->first, *it->second);
   }
}

UtoGroupShapeImp::~UtoGroupShapeImp() {}

bool UtoGroupShapeImp::EnableRecursiveProperties(bool enable)
{
   bool temp          = m_RecursiveEnabled;
   m_RecursiveEnabled = enable;

   // turn off/on the NO_PROPERTIES flag on contained shapes.
   long flag = enable ? NO_PROPERTIES : 0;
   std::for_each(m_Shapes.begin(), m_Shapes.end(), SetFlag(NO_PROPERTIES, flag));

   return temp;
}

void UtoGroupShapeImp::SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved)
{
   // call the base class
   UtoShapeImp::SetColor(Color, nClrMask, nReserved);

   if (m_RecursiveEnabled)
   {
      for (UtoShapeMap::iterator cur = m_Shapes.begin(); cur != m_Shapes.end(); ++cur)
      {
         cur->second->SetColor(Color, nClrMask, nReserved);
      }
   }
}

void UtoGroupShapeImp::SetOwner(UtoEntity* pOwner)
{
   // pass this information to the parent
   UtoShapeImp::SetOwner(pOwner);
   // update the owner for each shape
   for (UtoShapeMap::iterator cur = m_Shapes.begin(); cur != m_Shapes.end(); ++cur)
   {
      cur->second->Impl()->SetOwner(pOwner);
   }
}

bool UtoGroupShapeImp::UnbindShape(const std::string& rName)
{
   UtoShapeMap::iterator i = m_Shapes.find(rName);
   if (i != m_Shapes.end())
   {
      OnShapeUnbound(*i->second);

      m_Shapes.erase(i);
      return true;
   }
   return false;
}

UtoShape* UtoGroupShapeImp::Bind(const std::string& name, const UtoShape& shape)
{
   if (FindShape(name) == nullptr)
   {
      UtoShape* p = shape.Clone();
      p->Impl()->SetName(name);

      std::pair<UtoShapeMap::iterator, bool> ib = m_Shapes.insert(std::make_pair(name, p));
      if (!ib.second)
      {
         // DEBUG(("Failed to insert shape %s during bind.", name.c_str()));
         delete p;
         return nullptr;
      }
      else
      {
         OnShapeBound(*p);
         return p;
      }
   }
   else
   {
      return nullptr;
   }
}

UtoShape* UtoGroupShapeImp::FindShape(const std::string& name) const
{
   UtoShapeMap::const_iterator i = m_Shapes.find(name);
   if (i != m_Shapes.end())
   {
      return i->second;
   }
   return nullptr;
}

void UtoGroupShapeImp::OnShapeUnbound(UtoShape& shape)
{
   for (size_t i = 0; i < mFrames.size(); ++i)
   {
      shape.UnbindViewer(*mFrames[i]->GetViewer());
   }

   GetTree()->removeChild(shape.Impl()->GetTree());
   shape.Impl()->SetOwner(nullptr);
   delete &shape;
}

void UtoGroupShapeImp::OnShapeBound(UtoShape& shape)
{
   shape.Impl()->SetOwner(m_Owner);
   for (size_t i = 0; i < mFrames.size(); ++i)
   {
      shape.BindToViewer(*mFrames[i]->GetViewer(), mFrames[i]->GetNode());
   }
}
