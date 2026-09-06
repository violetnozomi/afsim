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
// ResourceCallback.h : header file

#ifndef RESOURCE_CALLBACK_H
#define RESOURCE_CALLBACK_H

#include <iostream>

/**
\brief This class describes the callback functions to be overloaded
*/

class UtoResourceCallback
{
public:
   virtual ~UtoResourceCallback() {}

   /**
   \brief Resource Count to load.

   The NumResourceToLoad function will be called when the loader knows
   how many resources are going to be loaded.
   */
   virtual void NumResourcesToLoad(const int& count) const {}
   /**
   \brief Next resource to load.

   The OnLoadBegin function will be called before each shape is loaded.
    */
   virtual void OnLoadBegin(const std::string& name, const std::string& type, const int& position) const {}
   /**
   \brief Resource finished loading.

   The OnLoadFinish function will be called after a shape has been loaded and
   the user should return a bool on whether or not to cancel the resource
   loading process.  true continues the loading; false cancels loading.
   */
   virtual bool OnLoadFinish(const std::string& sName) const { return true; }
};

#endif
