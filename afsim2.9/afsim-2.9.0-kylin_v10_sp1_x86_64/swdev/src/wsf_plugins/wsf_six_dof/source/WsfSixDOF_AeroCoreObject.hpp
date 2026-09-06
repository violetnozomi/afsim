// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSIXDOFAEROCOREOBJECT_HPP
#define WSFSIXDOFAEROCOREOBJECT_HPP

#include "wsf_six_dof_export.h"

#include <list>

#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

// The AeroCoreObject provides the main aerodynamics for an object.
// It includes various aero parameters and stability derivatives. It may
// include one or more aero modes, which provide configuration-dependent aero.
// Modes also provide a means to remove any aero effects, such as while a subobject
// is internally carried. A different mode can then be used when 'free'.

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT AeroCoreObject
{
public:
   AeroCoreObject()  = default;
   ~AeroCoreObject() = default;

   AeroCoreObject& operator=(const AeroCoreObject& other) = delete;

   virtual bool Initialize();

   // SetModeName is used to change the aero mode
   void        SetModeName(const std::string& aName);
   std::string GetModeName() const;

   double GetRefArea_sqft() const;

   // Lift
   double CL_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);

   // Drag
   double Cd_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);

   // Side force
   double CY_AlphaBetaMach(double aMach, double aAlpha_rad, double aBeta_rad);

   // The CalculateAeroCLArea function is called to calculate the lift coefficient (CL)
   // including reference area effects but not including dynamic pressure effects.
   double CalculateAeroCLArea(double aMach, double aAlpha_rad);

   // The CalculateAeroCdArea function is called to calculate the drag coefficient (Cd)
   // including reference area effects but not including dynamic pressure effects.
   double CalculateAeroCdArea(double aMach, double aAlpha_rad);

protected:
   AeroCoreObject(const AeroCoreObject& aSrc) = default;

   std::string mModeName = "DEFAULT";

   double mRefArea_sqft = 0.0;

   // Force coefficients
   UtCloneablePtr<UtTable::Table> mCL_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCd_AlphaBetaMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mCY_AlphaBetaMachTablePtr{nullptr};
};
} // namespace six_dof
} // namespace wsf

#endif
