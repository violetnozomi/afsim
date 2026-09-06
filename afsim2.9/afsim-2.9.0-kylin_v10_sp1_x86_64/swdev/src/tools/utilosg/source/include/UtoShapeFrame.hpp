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
#ifndef UTOSHAPEFRAME_HPP
#define UTOSHAPEFRAME_HPP
#include <osg/MatrixTransform>

#include "UtoShape.hpp"
class UtoViewer;
class UtoShapeImp;
namespace osg
{
class AutoTransform;
}
// interface for a shape's reference frame, which binds the shape to the viewer
class UtoShapeFrame
{
public:
   UtoShapeFrame(UtoShapeImp* aShapePtr, UtoViewer* aViewerPtr);
   virtual ~UtoShapeFrame() {}
   virtual UtoShapeFrame* Clone(UtoShapeImp* aShape, UtoViewer* aViewer) const = 0;
   virtual bool           IsVisible() const                                    = 0;
   virtual void           SetVisible(bool aIsVisible)                          = 0;
   virtual osg::Group*    GetNode()                                            = 0;
   virtual void           FrameChanged() {}
   virtual UtoViewer*     GetViewer() const { return mViewerPtr; }
   virtual UtoShapeImp*   GetShape() const { return mShapePtr; }
   virtual void           Unref() { delete this; }
   void                   SetParentNode(osg::Group* aParentNode);
   osg::Group*            GetParentNode() const;
   virtual void           SetName(const std::string& aName) = 0;

protected:
   osg::Group*  mParentNode;
   UtoShapeImp* mShapePtr;
   UtoViewer*   mViewerPtr;
};

// Default reference frame implementation, uses UtoShape::ReferenceFrame
// to determine which transformation to use
class UtoShapeFrameTransform : public osg::MatrixTransform, public UtoShapeFrame
{
public:
   const char* className() const override { return "UtoShapeFrameTransform"; }

   UtoShapeFrameTransform(UtoShapeImp* aShapePtr, UtoViewer* aViewerPtr);
   ~UtoShapeFrameTransform() override;
   UtoShapeFrame* Clone(UtoShapeImp* aShape, UtoViewer* aViewer) const override;
   void           accept(osg::NodeVisitor& nv) override;

   UtoViewer* GetViewer() const override { return mViewerPtr; }

   bool        IsVisible() const override;
   void        SetVisible(bool aIsVisible) override;
   osg::Group* GetNode() override;
   void        FrameChanged() override
   {
      setMatrix(osg::Matrix::identity());
      UpdateTransform();
   }

   void         UpdateTransform_World(osg::Matrix& m);
   void         UpdateTransformEntityYawOnly(osg::Matrix& m);
   void         UpdateTransformNED_AtEntity(osg::Matrix& m);
   void         UpdateTransform_Entity(osg::Matrix& m);
   void         UpdateTransform_WorldAtEntityPosition(osg::Matrix& m);
   void         ApplyAutoScale(osg::Matrix& m);
   virtual void UpdateTransform();
   void         Unref() override;
   void         SetName(const std::string& aName) override { setName(aName); }

protected:
   void                FlagsChanged();
   unsigned int        mMoveCount;
   osg::AutoTransform* mBillboardNode;

private:
};

#endif
