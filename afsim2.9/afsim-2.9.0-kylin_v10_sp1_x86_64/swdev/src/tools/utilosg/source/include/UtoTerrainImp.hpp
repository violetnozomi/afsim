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

// UtoTerrainImp.hpp: interface for the UtoTerrainImp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UTOTERRAINIMP_HPP_)
#define _UTOTERRAINIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UtoMapProjection.hpp"
#include "UtoTypes.hpp"

class UtoViewer;
namespace osg
{
class Node;
}

class UtoTerrainImp;

namespace Utok
{
struct Properties
{
   virtual ~Properties() {}
   virtual bool Dispatch(UtoTerrainImp* ptr, const std::string& attribute, const std::string& buf) = 0;
};

template<class T>
struct Properties_T : public Properties
{
   typedef bool (T::*ReadAttrFnPtr)(const std::string& attribute, const std::string& buf);
   typedef std::map<std::string, ReadAttrFnPtr> AttributeMap;
   ~Properties_T() override {}
   bool Dispatch(UtoTerrainImp* ptr, const std::string& attribute, const std::string& buf) override
   {
      // Find the corresponding function and make the call
      typename AttributeMap::iterator it = Attributes.find(attribute);
      if (it != Attributes.end())
      {
         ReadAttrFnPtr fn = it->second;
         return (static_cast<T*>(ptr)->*fn)(attribute, buf);
      }
      return false;
   }
   void Set(const char* name, ReadAttrFnPtr fn) { Attributes[name] = fn; }
   bool IsEmpty() const { return Attributes.empty(); }

private:
   AttributeMap Attributes;
};
} // namespace Utok

// calculate the height of terrain for a particular node
// if the orientation is null, the orientation will not be calculated
bool HeightOfTerrain(osg::Node* pRoot, double* rPosn, UtoOrientation* pOrient = nullptr);

bool IntersectTerrain(osg::Node*    pRoot,
                      const double* rPosition,
                      const double* rVector,
                      double*       rIntersectPoint,
                      double*       rIntersectNormal,
                      double        aMagnitude = 100000.0);

class UtoTerrainImp
{
public:
   // constructor / deconstructor
   UtoTerrainImp();
   virtual ~UtoTerrainImp();

   std::string Name() const { return m_sName; }
   void        SetName(const std::string& sName) { m_sName = sName; }

   virtual void Dump(std::ostream& o) = 0;

   virtual osg::Node* AddToWorld(UtoViewer& Viewer) = 0;

   virtual void BlendWithFog(bool bBlend) {}

   virtual void RemoveFromWorld(UtoViewer& Viewer) {}

   UtoMapProjection Projection() const { return m_pProjection; }

   static int ReadColor3(const std::string& in, UtoColor& clr);
   static int ReadColor4(const std::string& in, UtoColor& clr);
   static int ReadLatLon(const std::string& in, double& value);

   virtual bool PreLoad();
   virtual bool PostLoad();
   virtual bool ReadAttribute(const std::string& attr, const std::string& buf);

   // sets the color blending value
   virtual void SetColor(const UtoColor& rColor, unsigned int nClrMask);
   // returns the current color blending value
   virtual const UtoColor& Color() const { return m_oBlendColor; }

   // returns the height of the terrain
   virtual void Height(double* rPosn, UtoOrientation* pOrient = nullptr) { return; }

   // returns the terrain intersection point
   virtual bool Intersect(const double* rPosition, const double* rVector, double* rIntersectionPoint, double* rIntersectNormal) const
   {
      return false;
   }

   // determines the current state of color blending
   bool IsColorBlendingDisabled() const;
   // sets the color blending values
   virtual void DisableColorBlending(bool bDisable);

   virtual void SetUniform(const std::string& name, const float value)                    = 0;
   virtual void SetUniform(const std::string& name, const float value[4])                 = 0;
   virtual void SetUniformMatrix(const std::string& name, const double value[][4])        = 0;
   virtual void SetUniformInverseMatrix(const std::string& name, const double value[][4]) = 0;

   void SetUniform(osg::Node* node, const std::string& name, const float value);
   void SetUniform(osg::Node* node, const std::string& name, const float value[4]);
   void SetUniformMatrix(osg::Node* node, const std::string& name, const double value[][4]);
   void SetUniformInverseMatrix(osg::Node* node, const std::string& name, const double value[][4]);
   void SetProjection(const std::string& aProjection, const double& aOffset = 0.0);

protected:
   typedef Utok::Properties Properties;

   UtoTerrainImp(const UtoTerrainImp&);

   UtoColor m_oBlendColor;

   void SetProperties(Properties* p) { m_pProperties = p; }
   void SetProjection(UtoMapProjection p) { m_pProjection = p; }

private:
   UtoTerrainImp& operator=(const UtoTerrainImp&);

   typedef UtoMapProjection* (UtoTerrainImp::*MakeProjectionFn)();

   struct EQRParams
   {
      double lat, lon;
   };

   void InitGenericProperties();

   bool ReadProjection(const std::string& attr, const std::string& buf);
   bool ReadEarthModel(const std::string& attr, const std::string& buf);
   bool ReadAngleUnits(const std::string& attr, const std::string& buf);
   bool ReadDistUnits(const std::string& attr, const std::string& buf);
   bool ReadLCCParam(const std::string& attr, const std::string& buf);
   bool ReadTMParam(const std::string& attr, const std::string& buf);
   bool ReadEQRParam(const std::string& attr, const std::string& buf);
   bool ReadColorParam(const std::string& attr, const std::string& buf);
   bool ReadDisableColorBlending(const std::string& attr, const std::string& buf);

   UtoMapProjection* MakeCME_EQRProjection();
   UtoMapProjection* MakeCME_WCSProjection();

   std::string                              m_sName;
   static Utok::Properties_T<UtoTerrainImp> m_GenericProperties;
   Utok::Properties*                        m_pProperties;

   bool m_bDisableColorBlending;

   EQRParams m_EQRParams;

   MakeProjectionFn MakeProjection;
   UtoMapProjection m_pProjection;
};

inline bool UtoTerrainImp::IsColorBlendingDisabled() const
{
   return m_bDisableColorBlending;
}

inline void UtoTerrainImp::DisableColorBlending(bool bDisable)
{
   m_bDisableColorBlending = bDisable;
}

#endif // !defined(_UTOTERAINIMP_HPP_)
