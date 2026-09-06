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
// UtoShapeImp.hpp : header file

#if !defined(_UTOSHAPEIMP_HPP_)
#define _UTOSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/MatrixTransform>
#include <osg/ref_ptr>

#include "UtEntityPart.hpp"
#include "UtoPriv.hpp"
#include "UtoRenderInfo.hpp"
#include "UtoShape.hpp"
#include "UtoTypes.hpp"
#include "UtoUpdateCallback.hpp"

// class UtoSymbol;
class UtoShapeFrame;
class UtoShape;
class UtoViewer;
class UtoViewerProjection;
class UtoEntity;
class UtoShapeFrameTransform;
namespace osg
{
class Program;
}


struct UtoViewerShapeData : public osg::Referenced
{
   UtoViewerShapeData(UtoShapeImp* s)
      : shape(s)
   {
   }

   UtoShapeImp* shape;
};


class UtoShapeImp
{
public:
   friend class UtoShape;
   friend class UtoShapeFrameTransform;

   enum Flags // internal shape flags
   {
      INTERNAL_MASK   = 0xFF000000,
      PUBLIC_MASK     = 0x00FFFFFF,
      SHARED_STATESET = 0x01000000,
      NO_CALLBACK     = 0x02000000,
      NO_PROPERTIES   = 0x04000000,
      NO_ALIGNMENT    = 0x08000000
   };

   UtoShapeImp();
   UtoShapeImp(const UtoShapeImp& rhs);
   virtual ~UtoShapeImp();
   UtoShape*          GetPublicPtr() const { return m_PublicPtr; }
   const std::string& Name() const { return m_Name; }
   void               SetName(const std::string& aName) { m_Name = aName; }

   virtual const UtoColor& Color() const { return m_oBlendColor; }
   virtual void            SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved = 0);

   double       Scaling() const { return m_Scaling; }
   virtual void SetScaling(double fScaling);

   double StaticScale() const { return m_StaticScale; }

   double FinalScale() const;

   const UtoPosition& Position() const { return m_Position; }
   virtual bool       SetPosition(const UtoPosition& Position);

   const UtoOrientation& Orientation() const { return m_Orientation; }
   virtual bool          SetOrientation(const UtoOrientation& Orientation);

   bool SetPositionOrientation(const UtoPosition& Position, const UtoOrientation& Orientation);

   virtual void SetShapeFlags(long mask, long state);

   bool CullingActive();
   void SetCullingActive(bool aCullingActive);

   osg::Group*       GetTree() { return m_RootNode.get(); }
   const osg::Group* GetTree() const { return m_RootNode.get(); }

   // bool Load(std::istream& is);
   virtual bool PreLoad();
   virtual bool PostLoad();
   virtual bool ReadAttribute(const std::string& attr, const std::string& buf);

   bool HasFlag(long mask) const { return ((m_lFlags & mask) == mask); }
   long Flags() const { return m_lFlags; }

   UtoEntity*   Owner() const { return m_Owner; }
   virtual void SetOwner(UtoEntity* owner);

   void UpdateTransform();
   void TransformChanged();

   void               SetAuxData(const std::string& a_AuxData) { m_AuxData = a_AuxData; }
   const std::string& AuxData() const { return m_AuxData; }

   void OverrideRenderBin(int aBin, bool aNested);

   unsigned int GetViewerId() { return m_ViewerId; }
   // 0 for all viewers, otherwise the ID of the sole viewer that should show this shape
   void SetViewerId(unsigned int viewerId) { m_ViewerId = viewerId; }

   static void SetVespaCoordinateSystem(bool aState) { s_VespaCoordinateSystem = aState; }

   void SetNodeMask(unsigned int aMask, bool aState);
   void SetAllowUpdates(bool aState);
   bool AllowUpdates() { return mAllowUpdates; }

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

   bool BindToViewer(UtoViewer& aViewer, osg::Group* aParentNode = nullptr);
   void UnbindViewer(UtoViewer& aViewer);

   // UtoSymbol* FindSymbol(UtoViewer& aViewer) const;
   UtoShapeFrame* FindFrame(const UtoViewer& aViewer) const;
   bool           Show(bool aShow, UtoViewer& aViewer);
   bool           IsVisible(UtoViewer* aViewer) const;

   // virtual void UpdateTransform(UtoShapeFrameTransform* aTransform);
   const UtoShape::ReferenceFrame& GetReferenceFrame() const;
   void                            SetReferenceFrame(const UtoShape::ReferenceFrame& aFrame);
   void                            SetReferenceFrame(UtoShapeFrame* aFrame);

protected:
   void FrameChanged();

   bool UseStaticScale() const { return m_UseStaticScale; }
   void SetUseStaticScale(bool useStaticScale);

   // don't add nodes directly to m_RootNode as that will break billboarding
   void AddShapeChildNode(osg::Node* node);
   void RemoveShapeChildNode(osg::Node* node);

   osg::RefMatrix* GetOrCreateStaticRotation();

   UtoShape*                m_PublicPtr;
   osg::ref_ptr<osg::Group> m_RootNode;
   UtoShape::ReferenceFrame mFrame;
   UtoEntity*               m_Owner;
   // this holds any rotations or scaling loaded from the resource definition
   // file
   osg::ref_ptr<osg::RefMatrix> m_StaticRotation;
   double                       m_StaticScale;
   bool                         m_UseStaticScale;
   UtoPosition                  m_StaticOffset;

   // defines color / transparency for the shape
   UtoColor m_oBlendColor;

   UtoPosition    m_Position;
   UtoOrientation m_Orientation;

private:
   UtoShapeImp& operator=(const UtoShapeImp&);

   void SetupAutoScaleCallback();
   void RemoveAutoScaleCallback();
   // void UpdateBillboard();
   void CopyStateSet();

   unsigned int AllocateSamplerIndex(const char* aName);

   void UnbindViewers();

   int        m_Id;
   static int s_Id;

   double      m_Scaling;
   std::string m_Name;
   std::string m_AuxData;

   long m_lFlags;
   // UtoShape::ReferenceFrame   mFrame;
   //  0 for all viewers, otherwise the ID of the sole viewer that should show this shape
   unsigned int m_ViewerId;
   unsigned int mMoveCount;

   static bool s_VespaCoordinateSystem;

   double                              m_Zoom;
   bool                                mAllowUpdates;
   std::map<std::string, unsigned int> mSamplerToIndex;

   unsigned int mSamplerIndex;

protected:
   UtoShapeFrame*              mFrameTemplate;
   std::vector<UtoShapeFrame*> mFrames;
};

#endif // !defined(_UTOSHAPEIMP_HPP_)
