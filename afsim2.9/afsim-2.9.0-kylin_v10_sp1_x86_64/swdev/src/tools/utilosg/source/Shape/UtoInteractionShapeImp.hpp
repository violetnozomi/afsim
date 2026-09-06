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
// UtoInteractionShapeImp.h : header file

#if !defined(_UtoInteractionSHAPEIMP_H_)
#define _UtoInteractionSHAPEIMP_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoAttrPolyLineShapeImp.hpp"
#include "UtoMapProjection.hpp"
#include "UtoTypes.hpp"

class UtoInteractionShapeImp;

class UtoInteractionShapeImp : public UtoAttrPolyLineShapeImp
{
public:
   UtoInteractionShapeImp();
   UtoInteractionShapeImp(const UtoInteractionShapeImp&);
   ~UtoInteractionShapeImp() override;

   void         Clear();
   void         SetTexturePattern(unsigned char aPattern[], const int aSize);
   void         Update(osg::NodeVisitor* nv, osg::Drawable* drawable) override;
   virtual void Update(UtoRenderInfo* info, osg::NodeVisitor* nv);

   int AddInboundSegment(UtoEntity* aSrc, int aOffset, const UtoColor& aColor);
   int AddOutboundSegment(UtoEntity* aSrc, int aOffset, const UtoColor& aColor);

   class Interaction
   {
   public:
      Interaction(UtoEntity* aSrc, int aDir, int aOffset, const UtoColor& aColor)
         : mTarget(aSrc)
         , mDirection(aDir)
         , mOffset(aOffset)
         , mColor(aColor)
      {
      }
      UtoEntity* mTarget;
      int        mDirection;
      int        mOffset;
      UtoColor   mColor;
   };

   void ArchSegments(bool aState);

   void SetSceneProjection(const UtoMapProjection& aProjection); // 0 -> round, 1 -> Flat
   void SetFillFactor(double aFactor) { mFillFactor = aFactor; }

private:
   int  Insert(int pos, const UtoPosition pts[], const UtoColor col[], const float texco[], int num);
   int  Remove(int pos);
   void OnRemoveEntityCB(UtoEntity* entity);

   osg::ref_ptr<osg::FloatArray> m_TexCoord;
   unsigned char*                mTexture;

   UtoInteractionShapeImp& operator=(const UtoInteractionShapeImp&);

   osg::ref_ptr<UtoUpdateCallback<UtoInteractionShapeImp>> m_Callback;

   std::vector<Interaction> mInteractionList;

   double           mFillFactor;
   bool             mArchSegments;
   UtoMapProjection mMapProjection;
};

#endif // !defined(_CMEINTERACTIONSHAPEIMP_H_)
