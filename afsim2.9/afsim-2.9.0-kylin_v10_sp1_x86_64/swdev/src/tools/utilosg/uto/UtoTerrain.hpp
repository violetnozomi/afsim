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

// UtoTerrain.hpp: interface for the UtoTerrain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UTOTERRAIN_HPP_)
#define _UTOTERRAIN_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "utilosg_export.h"

#include "UtoTypes.hpp"

class UtoTerrainImp;

namespace osgUtil
{
class IntersectionVisitor;
};

class UTILOSG_EXPORT UtoTerrain
{
public:
   // constructor / deconstructor
   UtoTerrain();
   virtual ~UtoTerrain();

   virtual UtoTerrainImp*       Impl()       = 0;
   virtual const UtoTerrainImp* Impl() const = 0;

   virtual std::string Name()                           = 0;
   virtual void        SetName(const std::string& Name) = 0;

   virtual UtoTerrain* Clone() const = 0;

   // sets the color blending value
   virtual void SetColor(const UtoColor& rColor, unsigned int nClrMask = UtoColor::RGBA) = 0;
   // returns the current color blending value
   virtual const UtoColor& Color() const = 0;

   // returns the height of the terrain
   virtual void Height(double* aPosition) const                          = 0;
   virtual void Height(double* aPosition, UtoOrientation& rOrient) const = 0;

   // returns the terrain intersection point
   virtual bool Intersect(const double aPos[3],
                          const double aVec[3],
                          double       aIPoint[3],
                          double       aINorm[3],
                          double       aMagnitude = 100000.0) const
   {
      return false;
   }
   virtual bool Intersect(osgUtil::IntersectionVisitor* aIntersectionPtr) const { return false; }
   virtual void SetShaders(const std::string& aUnlitVShader,
                           const std::string& aUnlitFShader,
                           const std::string& aLitVShader,
                           const std::string& aLitFShader)
   {
   }
   virtual void SetTexture(const int aUnit, const std::string& aFilename) {}
   // enables / disables the color blending
   virtual void SetDisableColorBlending(bool bDisable) = 0;
   // obtains the state of color blending
   virtual bool IsColorBlendingDisabled() const = 0;

   virtual void SetUniform(const std::string& aName, const float aValue);
   virtual void SetUniform(const std::string& aName, const float aValue[4]);
   virtual void SetUniformMatrix(const std::string& aName, const double aValue[][4]);
   virtual void SetUniformInverseMatrix(const std::string& aName, const double aValue[][4]);

   virtual void SetPolygonOffset(const float aFactor, const float aUnits){};
   virtual void SetLightingSwitchStates(const int aUnlitValue, const int aLitValue) {}
   virtual bool Lighting() { return false; }
   virtual void EnableLighting(const bool aState) {}
   void         PostLoad();
   void         SetProjection(const std::string& aProjection, const double& aOffset = 0.0);

   virtual void Dump(std::ostream&) = 0;

private:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoTerrain(const UtoTerrain&);
#endif
   UtoTerrain& operator=(const UtoTerrain&);
};

#endif // !defined(_UTOTERRAIN_HPP_)
