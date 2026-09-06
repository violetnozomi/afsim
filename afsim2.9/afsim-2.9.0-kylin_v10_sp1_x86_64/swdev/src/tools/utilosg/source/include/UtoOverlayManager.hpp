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
// UtoOverlayManager.hpp : header file

#if !defined(_UTOOVERLAYMANAGER_HPP_)
#define _UTOOVERLAYMANAGER_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>

#include "UtoTypes.hpp"

class UtoOverlay;
class UtoWorld;

class UtoOverlayManager
{
public:
   typedef std::list<Utok::NameOverlay>                            OverlayList;
   typedef OverlayList::iterator                                   iterator;
   typedef OverlayList::const_iterator                             const_iterator;
   typedef std::pair<OverlayList::iterator, OverlayList::iterator> IteratorPair;

   UtoOverlayManager();
   ~UtoOverlayManager();

   OverlayList::iterator AddOverlay(const std::string& name, UtoWorld* pWorld);
   OverlayList::iterator InsertOverlay(const std::string& name, OverlayList::iterator it, UtoWorld* pWorld);
   void                  RemoveOverlay(OverlayList::iterator it);
   void                  RemoveAllOverlays();
   OverlayList::iterator FindOverlay(const std::string& name);
   UtoOverlay*           Find(const std::string& name) const
   {
      UtoOverlayManager*    mthis = const_cast<UtoOverlayManager*>(this);
      OverlayList::iterator i     = mthis->FindOverlay(name);
      if (i != mthis->m_Overlays.end())
      {
         return i->overlay;
      }
      return nullptr;
   }
   std::string  FindName(const OverlayList::iterator it);
   void         BringOverlayToFront(OverlayList::iterator it);
   IteratorPair Overlays();

   const_iterator end() const { return m_Overlays.end(); }
   iterator       end() { return m_Overlays.end(); }

   const_iterator begin() const { return m_Overlays.begin(); }
   iterator       begin() { return m_Overlays.begin(); }

protected:
private:
   UtoOverlayManager(const UtoOverlayManager&);
   UtoOverlayManager& operator=(const UtoOverlayManager&);

   OverlayList m_Overlays;
};

#endif // !defined(_UTOOVERLAYMANAGER_HPP_)
