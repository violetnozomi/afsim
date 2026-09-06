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
// UtoOverlayManager.cpp : implementation file

#include "UtoOverlayManager.hpp"

#include <algorithm>

#include "UtoOverlay.hpp"

// {secret}
struct overlay_finder
{
   overlay_finder(const std::string& name)
      : m_sName(name)
   {
   }
   bool operator()(Utok::NameOverlay& no)
   {
      if (no.name == m_sName)
         return true;
      return false;
   }

   std::string m_sName;
};

/////////////////////////////////////////////////////////////////////////////

UtoOverlayManager::UtoOverlayManager() {}

UtoOverlayManager::~UtoOverlayManager()
{
   RemoveAllOverlays();
}

UtoOverlayManager::OverlayList::iterator UtoOverlayManager::AddOverlay(const std::string& name, UtoWorld* pWorld)
{
   return InsertOverlay(name, m_Overlays.end(), pWorld);
}

UtoOverlayManager::OverlayList::iterator UtoOverlayManager::InsertOverlay(const std::string&    name,
                                                                          OverlayList::iterator before,
                                                                          UtoWorld*             pWorld)
{
   OverlayList::iterator it = std::find_if(m_Overlays.begin(), m_Overlays.end(), overlay_finder(name));
   if (it == m_Overlays.end())
   {
      UtoOverlay* p = new UtoOverlay(pWorld, name);
      it            = m_Overlays.insert(before, Utok::NameOverlay(name, p));
   }
   else
      it = m_Overlays.end();

   return it;
}

void UtoOverlayManager::RemoveOverlay(OverlayList::iterator remove)
{
   delete (remove->overlay);
   m_Overlays.erase(remove);
}

void UtoOverlayManager::RemoveAllOverlays()
{
   OverlayList::iterator it, end(m_Overlays.end());
   for (it = m_Overlays.begin(); it != end; ++it)
   {
      delete (it->overlay);
   }
   m_Overlays.clear();
}

UtoOverlayManager::OverlayList::iterator UtoOverlayManager::FindOverlay(const std::string& name)
{
   OverlayList::iterator it = std::find_if(m_Overlays.begin(), m_Overlays.end(), overlay_finder(name));
   return it;
}

UtoOverlayManager::IteratorPair UtoOverlayManager::Overlays()
{
   IteratorPair pr = std::make_pair(m_Overlays.begin(), m_Overlays.end());
   return pr;
}

std::string UtoOverlayManager::FindName(const OverlayList::iterator it)
{
   return it->name;
}

void UtoOverlayManager::BringOverlayToFront(OverlayList::iterator it)
{
   Utok::NameOverlay temp;
   temp.name    = it->name;
   temp.overlay = it->overlay;
   m_Overlays.erase(it);
   m_Overlays.push_back(temp);
}
