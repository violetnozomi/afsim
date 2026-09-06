// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UtQtInitializeResources_HPP
#define UtQtInitializeResources_HPP

// This is required to initialize the resources when statically linking utilqt
inline void initUtQtResources()
{
   Q_INIT_RESOURCE(utilqt);
}

#endif
