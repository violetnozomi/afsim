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
// UtoShape.hpp : header file

#if !defined(_UTOSHAPE_HPP_)
#define _UTOSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoTypes.hpp"

class UtoShapeImp;
class UtoViewer;
class UtoShapeFrame;
namespace osg
{
class Program;
class Group;
} // namespace osg
class UTILOSG_EXPORT UtoShape
{
public:
   enum Flags
   {
      AUTO_SCALE      = 0x0008,
      BILLBOARD       = 0x0010,
      HORZ_STABILIZED = 0x0020
   };
   enum ReferenceFrame
   {
      // The entity's frame.  (front, right, down)
      cFRAME_ENTITY,
      // Horizon-stabilized entity frame, (front, right, down)
      cFRAME_ENTITY_YAW_ONLY,
      // (north, east, down) at entity's location
      cFRAME_NED_AT_ENTITY,
      // ECEF_x,y,z at entity's position, not rotated with entity
      cFRAME_WORLD_AT_ENTITY,
      // Frame at world origin (center of earth or flat-map origin)
      cFRAME_WORLD,
      // A derived UtoShapeFrame type has been supplied
      cFRAME_CUSTOM
   };
   // destructor
   virtual ~UtoShape();

   const std::string& GetName() const;
   void               SetName(const std::string& aName);

   // sets / gets the color for the shape
   const UtoColor& Color() const;
   void            SetColor(const UtoColor& clr, unsigned int nClrMask = UtoColor::RGBA, unsigned int nReserved = 0);

   // sets / gets the scale factor for the shape
   double       Scaling() const;
   virtual void SetScaling(double dFactor);

   double StaticScale() const;

   double FinalScale() const;

   // sets / gets the shape position
   const UtoPosition& Position() const;
   void               SetPosition(const UtoPosition& rPos);

   // sets / gets the shape orientation
   const UtoOrientation& Orientation() const;
   void                  SetOrientation(const UtoOrientation& rOrient);

   // sets / gets the culling active flag
   bool CullingActive();
   void SetCullingActive(bool aCullingActive);

   // sets / gets the shape flags
   void SetShapeFlags(long mask, long state);
   bool HasFlag(long mask) const;
   long Flags() const;

   void                  SetReferenceFrame(const ReferenceFrame& aFrame);
   const ReferenceFrame& GetReferenceFrame() const;

   UtoShapeFrame* FindFrame(const UtoViewer& aViewer) const;

   // clones the specified shape
   virtual UtoShape* Clone() const = 0;

   // dumps the specified shape to the stream
   virtual void Dump(std::ostream& rOut);

   UtoShapeImp*       Impl() { return m_pImpRoot; }
   const UtoShapeImp* Impl() const { return m_pImpRoot; }

   void               SetAuxData(const std::string& a_AuxData);
   const std::string& AuxData() const;

   void SetUniformf(const char* aName, float aValue);
   void SetUniform3f(const char* aName, float aValueX, float aValueY, float aValueZ);
   void SetUniform3i(const char* aName, int aValueX, int aValueY, int aValueZ);
   void SetUniform4f(const char* aName, float aValueX, float aValueY, float aValueZ, float aValueW);
   void SetUniform4i(const char* aName, int aValueX, int aValueY, int aValueZ, int aValueW);

   void SetUniformSampler(const char*          aName,
                          const unsigned char* aSampler1D,
                          const unsigned int   aComponents,
                          const unsigned int   aLength);

   void SetUniformSampler(const char*     aName,
                          const char*     aFilename,
                          unsigned char*& aData,
                          unsigned int&   aComponents,
                          unsigned int&   aSize);

   void SetProgram(osg::Program* aProgramPtr, bool aEnable);

   bool BindToViewer(UtoViewer& aViewer);
   bool BindToViewer(UtoViewer& aViewer, osg::Group* aParentNode);
   void UnbindViewer(UtoViewer& aViewer);

   // UtoSymbol* FindSymbol(UtoViewer& aViewer) const;

   void OverrideRenderBin(int aBin, bool aNested);

   unsigned int GetViewerId();
   void         SetViewerId(unsigned int aViewerId);

   static void SetVespaCoordinateSystem(bool aState);

   void SetNodeMask(unsigned int aMask, bool aState);
   bool AllowUpdates();
   void SetAllowUpdates(bool aState);

   osg::Group*       GetTree();
   const osg::Group* GetTree() const;

   bool Show(bool aShow, UtoViewer& aViewer);
   bool IsVisible(UtoViewer* aViewer) const;

   UtoEntity* Owner() const;

   void UnbindViewers();

protected:
   // constructor
   UtoShape(UtoShapeImp*);

   void SetImpl(UtoShapeImp*);
   void Swap(UtoShape&);

private:
   UtoShape();
   UtoShape(const UtoShape&);
   UtoShape& operator=(const UtoShape&);


   UtoShapeImp* m_pImpRoot;
};

#endif // !defined(_UTOSHAPE_HPP_)
