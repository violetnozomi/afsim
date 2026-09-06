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

#include "UtoShapeFrame.hpp"

#include <osg/AutoTransform>

#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtoEntity.hpp"
#include "UtoShapeImp.hpp"
#include "UtoViewer.hpp"

UtoShapeFrameTransform::UtoShapeFrameTransform(UtoShapeImp* aShapePtr, UtoViewer* aViewerPtr)
   : UtoShapeFrame(aShapePtr, aViewerPtr)
{
   mBillboardNode = nullptr;
   osg::MatrixTransform::ref();
   mMoveCount = 0xffffffff;
   setNumChildrenRequiringUpdateTraversal(1);
   FlagsChanged();
}

void UtoShapeFrameTransform::accept(osg::NodeVisitor& nv)
{
   if (mShapePtr && nv.getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
   {
      UtoEntity*   ent = mShapePtr->Owner();
      unsigned int mc  = mShapePtr->mMoveCount + (ent ? ent->GetMoveCount() : 0);
      if (mc != mMoveCount || mShapePtr->HasFlag(UtoShape::AUTO_SCALE))
      {
         mMoveCount = mc;
         UpdateTransform();
         if (ent)
         {
         }
      }
   }
   osg::MatrixTransform::accept(nv);
}

void UtoShapeFrameTransform::UpdateTransform()
{
   FlagsChanged();

   osg::Matrix m;

   for (int i = 0; i < 3; ++i)
      m(3, i) = mShapePtr->m_StaticOffset[i];

   if (mShapePtr->m_StaticRotation.get())
   {
      m.postMult(*mShapePtr->m_StaticRotation.get());
   }

   if (!mBillboardNode)
   {
      double scale = mShapePtr->m_UseStaticScale ? mShapePtr->m_StaticScale : 1.0;
      scale *= mShapePtr->m_Scaling;

      if (scale != 1.0)
      {
         m.postMultScale(osg::Vec3(scale, scale, scale));
      }
   }

   if (mShapePtr->m_Orientation.roll || mShapePtr->m_Orientation.pitch || mShapePtr->m_Orientation.yaw)
   {
      m.postMult(osg::Matrix::rotate(DtoR(mShapePtr->m_Orientation.roll),
                                     osg::Vec3(1, 0, 0),
                                     DtoR(mShapePtr->m_Orientation.pitch),
                                     osg::Vec3(0, 1, 0),
                                     DtoR(mShapePtr->m_Orientation.yaw),
                                     osg::Vec3(0, 0, 1)));
      // This is the old orientation matrix
      // m.postMult(osg::Matrix::rotate(
      //   DtoR(mShapePtr->m_Orientation.roll), osg::Vec3(0, 1, 0),
      //   DtoR(mShapePtr->m_Orientation.pitch), osg::Vec3(1, 0, 0),
      //   -DtoR(mShapePtr->m_Orientation.yaw), osg::Vec3(0, 0, 1)));
   }

   if (mShapePtr->HasFlag(UtoShape::AUTO_SCALE))
   {
      ApplyAutoScale(m);
   }

   for (int i = 0; i < 3; ++i)
      m(3, i) += mShapePtr->m_Position[i];

   switch (mShapePtr->GetReferenceFrame())
   {
   case UtoShape::cFRAME_ENTITY:
      UpdateTransform_Entity(m);
      break;
   case UtoShape::cFRAME_ENTITY_YAW_ONLY:
      UpdateTransformEntityYawOnly(m);
      break;
   case UtoShape::cFRAME_NED_AT_ENTITY:
      UpdateTransformNED_AtEntity(m);
      break;
   case UtoShape::cFRAME_WORLD_AT_ENTITY:
      UpdateTransform_WorldAtEntityPosition(m);
      break;
   case UtoShape::cFRAME_WORLD:
      UpdateTransform_World(m);
      break;
   default:
      break;
   }

   setMatrix(m);
}

void UtoShapeFrameTransform::UpdateTransform_World(osg::Matrix& m)
{
   // nothing to do
}

void UtoShapeFrameTransform::UpdateTransformEntityYawOnly(osg::Matrix& m)
{
   osg::Matrix ent;
   UtEntity&   e = mShapePtr->Owner()->BodyCF();
   mViewerPtr->Projection()->GetNED_Frame(e, ent);
   double heading, pitch, roll;
   e.GetOrientationNED(heading, pitch, roll);
   ent.preMult(osg::Matrix::rotate(heading, osg::Vec3(0, 0, 1)));
   m.postMult(ent);
   _inverseDirty = true;
}

void UtoShapeFrameTransform::UpdateTransformNED_AtEntity(osg::Matrix& m)
{
   osg::Matrix ent;
   mViewerPtr->Projection()->GetNED_Frame(mShapePtr->Owner()->BodyCF(), ent);
   m.postMult(ent);
}

void UtoShapeFrameTransform::UpdateTransform_Entity(osg::Matrix& m)
{
   UtoEntity* e = mShapePtr->Owner();
   // if no entity, use world coordinate frame
   if (!e)
   {
      return UpdateTransform_World(m);
   }

   osg::Matrix ent;
   mViewerPtr->Projection()->GetECS_Frame(e->BodyCF(), ent);
   m.postMult(ent);
}

void UtoShapeFrameTransform::UpdateTransform_WorldAtEntityPosition(osg::Matrix& m)
{
   UtEntity&   e = mShapePtr->Owner()->BodyCF();
   UtoPosition p = mViewerPtr->Projection()->CalcPoint(e);
   m(3, 0) += p[0];
   m(3, 1) += p[1];
   m(3, 2) += p[2];
}

UtoShapeFrameTransform::~UtoShapeFrameTransform() {}

UtoShapeFrame* UtoShapeFrameTransform::Clone(UtoShapeImp* aShape, UtoViewer* aViewer) const
{
   return new UtoShapeFrameTransform(aShape, aViewer);
}

bool UtoShapeFrameTransform::IsVisible() const
{
   osg::Group* parentPtr = GetParentNode();
   for (size_t i = 0; i < getNumParents(); ++i)
   {
      if (getParent(i) == parentPtr)
         return true;
   }
   return false;
}

void UtoShapeFrameTransform::SetVisible(bool aIsVisible)
{
   bool vis = IsVisible();
   if (aIsVisible != vis)
   {
      if (aIsVisible)
      {
         GetParentNode()->addChild(this);
         mViewerPtr->ShapeAdded(mShapePtr->GetPublicPtr());
      }
      else
      {
         GetParentNode()->removeChild(this);
         mViewerPtr->ShapeRemoved(mShapePtr->GetPublicPtr());
      }
   }
}

void UtoShapeFrameTransform::Unref()
{
   mShapePtr  = nullptr;
   mViewerPtr = nullptr;
   osg::MatrixTransform::unref();
}

void UtoShapeFrameTransform::FlagsChanged()
{
   if (mShapePtr->HasFlag(UtoShape::BILLBOARD) != (mBillboardNode != nullptr))
   {
      if (mBillboardNode)
      {
         for (unsigned int i = 0; i < mBillboardNode->getNumChildren(); ++i)
         {
            addChild(mBillboardNode->getChild(i));
         }
         mBillboardNode->removeChildren(0, mBillboardNode->getNumChildren());
         removeChild(mBillboardNode);
         mBillboardNode = nullptr;
      }
      else
      {
         mBillboardNode = new osg::AutoTransform;
         mBillboardNode->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_CAMERA);
         mBillboardNode->setAutoScaleToScreen(false);

         for (unsigned int i = 0; i < getNumChildren(); ++i)
         {
            mBillboardNode->addChild(getChild(i));
         }
         removeChildren(0, getNumChildren());
         addChild(mBillboardNode);
      }
   }
}

void UtoShapeFrameTransform::ApplyAutoScale(osg::Matrix& m)
{
   if (mViewerPtr->ActiveCamera() == UtoViewer::PERSPECTIVE)
   {
      // Reset the matrix just in case it was an orthographic camera
      // last frame.
      UtoCameraPerspective* cam = mViewerPtr->GetPerspective();
      UtoPosition           eye = cam->Position();
      UtoPosition           epos;
      if (mShapePtr)
      {
         if (nullptr != mShapePtr->Owner())
         {
            UtoOrientation EntityOrientation;
            epos = mViewerPtr->Projection()->CalcPointOrient(mShapePtr->Owner()->BodyCF(), EntityOrientation);
         }
      }
      // vector shape position
      const osg::Vec3 vspos(mShapePtr->m_Position);
      // vector eye (camera) position
      const osg::Vec3 veye(eye);
      // vector entity position
      const osg::Vec3 vepos(epos);
      const double    height = mViewerPtr->DisplaySize().cy;
      // vertical OpenGL Units to screen height scaling
      const double scale = 2 * tan(cam->FOV() * M_PI / 180 / 2) / height;

      // eye to shape distance, OpenGL units
      const double shape_depth = (veye - (vepos + vspos)).length();
      // depth * scaling to GL Unit
      const double zoom = shape_depth * scale;
      mShapePtr->m_Zoom = zoom;
#if 0
      { // RAII block
         auto out = ut::log::info() << "Perspective distance to shape:";
         out.AddNote() << "GL Height: " << 2 * shape_depth*tan(cam->FOV()*M_PI / 180 / 2);
         out.AddNote() << "Window height: " << height;
         out.AddNote() << "Scale: " << scale << ;
         out.AddNote() << "Perspective zoom: " << zoom;
   }
#endif
      if (mBillboardNode)
      {
         // Size scales position from entity doesn't.
         mBillboardNode->setScale(zoom * mShapePtr->m_Scaling);
      }
      else
      {
         // Size scales position from entity doesn't.
         m.postMultScale(osg::Vec3(zoom, zoom, zoom));
         m.postMult(osg::Matrix::translate(vspos));
      }
   }
   else
   {
      double zoom       = mViewerPtr->GetOrtho()->Zoom();
      mShapePtr->m_Zoom = zoom;
      if (mBillboardNode)
      {
         // Size scales position from entity doesn't.
         mBillboardNode->setScale(zoom * mShapePtr->m_Scaling);
      }
      else
      {
         // Size scales position from entity also scales.
         // JEM Why here and no where else?  I am changing this to conform to the other modes.
         // I have left this note and the commented multipliers if we find some reason to change this back.
         osg::Matrix translate;
         m.postMultScale(osg::Vec3(zoom, zoom, zoom));
         m.postMult(osg::Matrix::translate(mShapePtr->m_Position));
      }
   }
}

osg::Group* UtoShapeFrameTransform::GetNode()
{
   if (mBillboardNode)
      return mBillboardNode;
   return this;
}


//
// void UtoShapeFrameTransform::ApplyAutoScale()
//{
//   if (mViewerPtr->ActiveCamera() == UtoViewer::PERSPECTIVE)
//   {
//      // Reset the matrix just in case it was an orthographic camera
//      // last frame.
//      UtoCameraPerspective *cam = mViewerPtr->GetPerspective();
//      UtoPosition eye = cam->Position();
//      UtoPosition epos;
//      if (mShapePtr)
//      {
//         UtoOrientation EntityOrientation;
//         epos = mViewerPtr->Projection()->CalcPointOrient(mShapePtr->Owner()->BodyCF(), EntityOrientation);
//      }
//      // vector shape position
//      const osg::Vec3 vspos(m_Position);
//      // vector eye (camera) position
//      const osg::Vec3 veye(eye);
//      // vector entity position
//      const osg::Vec3 vepos(epos);
//      const double height = mViewerPtr->GetWindow()->Size().cy;
//      // vertical OpenGL Units to screen height scaling
//      const double scale = 2 * tan(cam->FOV()*M_PI / 180 / 2) / height;
//
//      // eye to shape distance, OpenGL units
//      const double shape_depth = (veye - (vepos + vspos)).length();
//      // depth * scaling to GL Unit
//      const double zoom = shape_depth * scale;
//      m_Zoom = zoom;
//      if (HasFlag(UtoShape::BILLBOARD))
//      {
//         // Size scales position from entity doesn't.
//         static_cast<osg::AutoTransform*>(m_RootNode.get())->
//            setScale(zoom*m_Scaling);
//      }
//      else
//      {
//         // Size scales position from entity doesn't.
//         static_cast<osg::MatrixTransform*>(m_RootNode.get())->
//            setMatrix(preTranslate * osg::Matrix::scale(zoom, zoom, zoom)
//            * osg::Matrix::translate(vspos) * postTranslate);
//      }
//
//      return;
//   }
//   else
//   {
//      double zoom = mViewerPtr->GetOrtho()->Zoom();
//      m_Zoom = zoom;
//      if (HasFlag(UtoShape::BILLBOARD))
//      {
//         // Size scales position from entity doesn't.
//         static_cast<osg::AutoTransform*>
//            (m_RootNode.get())->setScale(zoom*m_Scaling);
//      }
//      else
//      {
//         // Size scales position from entity also scales.
//         // JEM Why here and no where else?  I am changing this to conform to the other modes.
//         // I have left this note and the commented multipliers if we find some reason to change this back.
//         double zoomLevel = m_Scaling * zoom;
//         osg::Matrix translate;
//         translate.makeTranslate(m_Position);
//         static_cast<osg::MatrixTransform*>(m_RootNode.get())->setMatrix(
//            preTranslate * osg::Matrix::scale(zoom, zoom, zoom) * translate *
//            postTranslate);
//      }
//   }
//}
//
// void UtoShapeImp::TransformChanged()
//{
//   if (HasFlag(UtoShape::BILLBOARD))
//   {
//      osg::AutoTransform* atrans =
//         static_cast<osg::AutoTransform*>(m_RootNode.get());
//      atrans->setScale(m_Scaling);
//      atrans->setPosition(m_Position);
//      return;
//   }
//   // pre shape translate not resource specified translate
//   osg::Matrix preTranslate;
//   preTranslate.makeTranslate(m_StaticOffset);
//
//   // multiply by the static rotation (and scaling) matrix if there is one
//   // This will cause the direction of the translation to move about the
//   // origin.
//   if (osg::RefMatrix* rotation = m_StaticRotation.get())
//      preTranslate *= *rotation;
//
//   if (m_UseStaticScale && m_StaticScale != 1.0)
//   {
//      preTranslate *= osg::Matrix::scale(m_StaticScale, m_StaticScale,
//                                         m_StaticScale);
//   }
//
//   //Set the resource defined scaling for the shape
//   if (m_Scaling != 1.0)
//   {
//      preTranslate *= osg::Matrix::scale(m_Scaling, m_Scaling, m_Scaling);
//   }
//
//   if (m_Orientation.roll || m_Orientation.pitch || m_Orientation.yaw)
//   {
//      preTranslate *= osg::Matrix::rotate(
//         DtoR(m_Orientation.roll), osg::Vec3(0, 1, 0),
//         DtoR(m_Orientation.pitch), osg::Vec3(1, 0, 0),
//         -DtoR(m_Orientation.yaw), osg::Vec3(0, 0, 1));
//   }
//
//   // Translation depends on the status of autoscaling, so it is calculated
//   // later an inserted in this place in the order of multiplications.
//
//   osg::Matrix postTranslate;
//   // access the entity owner to get the entity orientation.
//   //bool DoYaw = !HasFlag(UtoShape::NO_YAW),
//   //   DoPitch = !HasFlag(UtoShape::NO_PITCH),
//   //   DoRoll = !HasFlag(UtoShape::NO_ROLL);
//
//   //if ((DoYaw || DoPitch || DoRoll) && m_Owner)
//   //{
//   //   /*   UtoOrientation EntityOrientation = aTransformedOrientation;
//   //      UtoOrientation LocalOrientation = aLocalOrientation;
//
//   //      if (HasFlag(UtoShape::HORZ_STABILIZED))
//   //      {
//   //      postTranslate *= osg::Matrix::rotate(DtoR(LocalOrientation.roll), osg::Vec3(1, 0, 0));
//   //      postTranslate *= osg::Matrix::rotate(DtoR(LocalOrientation.pitch), osg::Vec3(0, 1, 0));
//   //      postTranslate *= osg::Matrix::rotate(DtoR(LocalOrientation.yaw), osg::Vec3(0, 0, 1));
//   //      }
//   //      else if (!s_VespaCoordinateSystem)
//   //      {
//   //      if (DoRoll && EntityOrientation.roll)
//   //      postTranslate *= osg::Matrix::rotate(DtoR(EntityOrientation.roll),
//   //      osg::Vec3(0, 1, 0));
//   //      if (DoPitch && EntityOrientation.pitch)
//   //      postTranslate *= osg::Matrix::rotate(DtoR(EntityOrientation.pitch),
//   //      osg::Vec3(1, 0, 0));
//   //      if (DoYaw && EntityOrientation.yaw)
//   //      postTranslate *= osg::Matrix::rotate(DtoR(-EntityOrientation.yaw),
//   //      osg::Vec3(0, 0, 1));
//   //      }
//   //      else
//   //      {
//   //      if (DoRoll && EntityOrientation.roll)
//   //      postTranslate *= osg::Matrix::rotate(DtoR(EntityOrientation.roll),
//   //      osg::Vec3(1, 0, 0));
//   //      if (DoPitch && EntityOrientation.pitch)
//   //      postTranslate *= osg::Matrix::rotate(DtoR(EntityOrientation.pitch),
//   //      osg::Vec3(0, 1, 0));
//   //      if (DoYaw && EntityOrientation.yaw)
//   //      postTranslate *= osg::Matrix::rotate(DtoR(EntityOrientation.yaw),
//   //      osg::Vec3(0, 0, 1));
//   //      }*/
//   //}
//   // Cache the transform if auto scale is enabled and scale from that one.
//   if (HasFlag(UtoShape::AUTO_SCALE))
//   {
//      // both are allocated or not
//      if (!m_PreTranslate.get())
//      {
//         m_PreTranslate = new osg::RefMatrix;
//         m_PostTranslate = new osg::RefMatrix;
//      }
//      m_PreTranslate->set(preTranslate);
//      m_PostTranslate->set(postTranslate);
//      // the transform is updated in the callback when autoscaling is enabled
//      // not here
//      return;
//   }
//   osg::Matrix translate;
//   translate.makeTranslate(m_Position);
//
//   static_cast<osg::MatrixTransform*>(m_RootNode.get())->
//      setMatrix(preTranslate * translate * postTranslate);
//}

void UtoShapeFrame::SetParentNode(osg::Group* aParentNode)
{
   mParentNode = aParentNode;
}

UtoShapeFrame::UtoShapeFrame(UtoShapeImp* aShapePtr, UtoViewer* aViewerPtr)
   : mShapePtr(aShapePtr)
   , mViewerPtr(aViewerPtr)
{
   mParentNode = nullptr;
}

osg::Group* UtoShapeFrame::GetParentNode() const
{
   return mParentNode ? mParentNode : mViewerPtr->GetScene();
}
