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
// UtoShapeImp.cpp : implementation file

#include "UtoShapeImp.hpp"

#include <cassert>
#include <sstream>
#include <string.h>

#include <osg/AutoTransform>
#include <osg/NodeVisitor>
#include <osg/Texture1D>
#include <osgDB/ReadFile>

#include "UtoEntity.hpp"
#include "UtoOptions.hpp"
#include "UtoPriv.hpp"
#include "UtoRenderInfo.hpp"
#include "UtoShape.hpp"
#include "UtoShapeFrame.hpp"
#include "UtoViewer.hpp"
#include "UtoViewerProjection.hpp"

/* UTO Function Block **********************************************************

Function: UtoShapeImp

Description:
   Constructor

*******************************************************************************/

int  UtoShapeImp::s_Id(0);
bool UtoShapeImp::s_VespaCoordinateSystem(false);

UtoShapeImp::UtoShapeImp()
   : m_PublicPtr(nullptr)
   , m_RootNode(new osg::Group)
   , m_Owner(nullptr)
   , m_StaticScale(1.0)
   , m_UseStaticScale(true)
   , m_oBlendColor(0xFF, 0xFF, 0xFF, 0xFF)
   , m_Id(s_Id++)
   , m_Scaling(1.0)
   , m_lFlags(0)
   , m_ViewerId(0)
   , m_Zoom(1.0)
   , mAllowUpdates(true)
   , mSamplerIndex(0)
   , mFrameTemplate(nullptr)
{
   mMoveCount = 0;
   mFrame     = UtoShape::cFRAME_ENTITY;
}

/* UTO Function Block **********************************************************

Function: UtoShapeImp

Description:
   Copy Constructor

*******************************************************************************/

UtoShapeImp::UtoShapeImp(const UtoShapeImp& rhs)
   : // start with an empty transform, children get added later
   m_PublicPtr(nullptr)
   , m_RootNode(new osg::Group)
   , mFrame(rhs.mFrame)
   , m_Owner(nullptr)
   ,
   // this is to share the rotation matrix
   m_StaticRotation(const_cast<osg::RefMatrix*>(rhs.m_StaticRotation.get()))
   , m_StaticScale(rhs.m_StaticScale)
   , m_UseStaticScale(rhs.m_UseStaticScale)
   , m_StaticOffset(rhs.m_StaticOffset)
   , m_oBlendColor(rhs.m_oBlendColor)
   , m_Position(rhs.m_Position)
   , m_Orientation(rhs.m_Orientation)
   , m_Id(s_Id++)
   , m_Scaling(rhs.m_Scaling)
   , m_AuxData(rhs.m_AuxData)
   , m_lFlags(rhs.m_lFlags)
   , m_ViewerId(rhs.m_ViewerId)
   , m_Zoom(rhs.m_Zoom)
   , mAllowUpdates(rhs.mAllowUpdates)
   , mSamplerToIndex(rhs.mSamplerToIndex)
   , mSamplerIndex(rhs.mSamplerIndex)
   , mFrameTemplate(rhs.mFrameTemplate)
{
   mMoveCount = 0;
   if (mFrameTemplate)
   {
      mFrameTemplate = mFrameTemplate->Clone(this, nullptr);
   }

   // duplicate or share the state set.
   if (const osg::StateSet* stateset = rhs.m_RootNode->getStateSet())
   {
      if (HasFlag(SHARED_STATESET))
      {
         m_RootNode->setStateSet(const_cast<osg::StateSet*>(stateset));
      }
      else
      {
         m_RootNode->setStateSet(new osg::StateSet(*stateset, osg::CopyOp::DEEP_COPY_ALL));
      }
   }

   //// Setup the autotransform if billboarding is enabled.
   // if(rhs.HasFlag(UtoShape::BILLBOARD))
   //{
   //    UpdateBillboard();
   // }

   // Now that the nodes are in position set the location information.
   UpdateTransform();
}

/* UTO Function Block **********************************************************

Function: UtoShapeImp

Description:
   Destructor

*******************************************************************************/

UtoShapeImp::~UtoShapeImp()
{
   for (size_t i = 0; i < mFrames.size(); ++i)
   {
      mFrames[i]->Unref();
   }
   if (mFrameTemplate)
      mFrameTemplate->Unref();
}

/* UTO Function Block **********************************************************

Function: SetScaling

Description:
   Sets the shapes scaling

*******************************************************************************/

void UtoShapeImp::SetScaling(double fScaling)
{
   m_Scaling = fScaling;

   // Scaling the matrix causes the vertices and normals to be scaled
   // which will affect the amount of light it gets.  About the only
   // way to get the correct lighting is if OpenGL rescales the normals, that
   // or go through and prescale the normals.  With the normals shared
   // between models that could have different scaling factors, just use
   // GL_RESCALE_NORMAL when it is available.  Otherwise punt.
   if (UtoOptions::Instance()->RescaleNormal())
   {
      CopyStateSet();
      osg::StateSet* stateset = m_RootNode->getStateSet();
      assert(stateset);

      if (m_Scaling == 1.0)
         stateset->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::INHERIT);
      else
         stateset->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
   }

   UpdateTransform();
}

/* UTO Function Block **********************************************************

Function: SetPosition

Description:
   Sets the shapes position realative to the entity

*******************************************************************************/

bool UtoShapeImp::SetPosition(const UtoPosition& Position)
{
   m_Position = Position;
   UpdateTransform();
   return true;
}

/* UTO Function Block **********************************************************

Function: SetOrientation

Description:
   Sets the shape's orientation

*******************************************************************************/

bool UtoShapeImp::SetOrientation(const UtoOrientation& Orientation)
{
   m_Orientation = Orientation;
   UpdateTransform();
   return true;
}

bool UtoShapeImp::SetPositionOrientation(const UtoPosition& Position, const UtoOrientation& Orientation)
{
   m_Position    = Position;
   m_Orientation = Orientation;
   UpdateTransform();
   return true;
}

void UtoShapeImp::SetShapeFlags(long mask, long state)
{
   m_lFlags &= (~mask | state);
   m_lFlags |= (mask & state);

   /*if(HasFlag(UtoShape::BILLBOARD) != billboard)
      UpdateBillboard();*/
   UpdateTransform();
}

bool UtoShapeImp::CullingActive()
{
   return m_RootNode->getCullingActive();
}

void UtoShapeImp::SetCullingActive(bool aCullingActive)
{
   m_RootNode->setCullingActive(aCullingActive);
}

void UtoShapeImp::SetOwner(UtoEntity* owner)
{
   m_Owner = owner;
   if (m_Owner)
   {
      UpdateTransform();
   }
}

void UtoShapeImp::UpdateTransform()
{
   if (m_Owner)
   {
      m_Owner->ForceMoveUpdate();
   }
   ++mMoveCount;
}

// void UtoShapeImp::UpdateTransform_World(UtoShapeFrameTransform* aTransform)
//{
//    // nothing to do
// }
//
// void UtoShapeImp::UpdateTransformEntityYawOnly(UtoShapeFrameTransform* aTransform)
//{
//    UtEntity& e = Owner()->BodyCF();
//    osg::Matrix m;
//    aTransform->GetViewer().Projection()->GetNED_Frame(e, m);
//    double heading, pitch, roll;
//    e.GetOrientationNED(heading, pitch, roll);
//    m.preMult(osg::Matrix::rotate(-heading, osg::Vec3(0, 0, 1)));
//    aTransform->setMatrix(m);
// }
//
// void UtoShapeImp::UpdateTransformNED_AtEntity(UtoShapeFrameTransform* aTransform)
//{
//    osg::Matrix m;
//    aTransform->GetViewer().Projection()->GetNED_Frame(Owner()->BodyCF(), m);
//    aTransform->setMatrix(m);
// }
//
// void UtoShapeImp::UpdateTransform_Entity(UtoShapeFrameTransform* aTransform)
//{
//    osg::Matrix m;
//    aTransform->GetViewer().Projection()->GetECS_Frame(Owner()->BodyCF(), m);
//    aTransform->setMatrix(m);
// }
//
// void UtoShapeImp::UpdateTransform_WorldAtEntityPosition(UtoShapeFrameTransform* aTransform)
//{
//    UtEntity& e = Owner()->BodyCF();
//    UtoPosition p = aTransform->GetViewer().Projection()->CalcPoint(e);
//    osg::Matrix m(1, 0, 0, 0,
//                  0, 1, 0, 0,
//                  0, 0, 1, 0
//                  p[0], p[1], p[2], 1);
//    aTransform->setMatrix(m);
// }
//
// void UtoShapeImp::UpdateTransform(UtoShapeFrameTransform* aTransform)
//{
//    switch (mFrame)
//    {
//    case UtoShape::cFRAME_ENTITY:
//       UpdateTransform_Entity(aTransform);
//       break;
//    case UtoShape::cFRAME_ENTITY_YAW_ONLY:
//       UpdateTransformEntityYawOnly(aTransform);
//       break;
//    case UtoShape::cFRAME_NED_AT_ENTITY:
//       UpdateTransformNED_AtEntity(aTransform);
//       break;
//    case UtoShape::cFRAME_WORLD_AT_ENTITY:
//       UpdateTransform_WorldAtEntityPosition(aTransform);
//       break;
//    case UtoShape::cFRAME_WORLD:
//       UpdateTransform_World(aTransform);
//       break;
//    default:
//       break;
//    }
// }

/* UTO Function Block **********************************************************

Function: GetOrCreateStaticRotation

Description:
   Return the current static rotation matrix for this shape creating the
   identity matrix if there isn't one.

*******************************************************************************/

osg::RefMatrix* UtoShapeImp::GetOrCreateStaticRotation()
{
   osg::RefMatrix* matrix = m_StaticRotation.get();
   if (!matrix)
   {
      m_StaticRotation = matrix = new osg::RefMatrix;
   }
   return matrix;
}

bool UtoShapeImp::PreLoad()
{
   return true;
}

bool UtoShapeImp::PostLoad()
{
   return true;
}

bool UtoShapeImp::ReadAttribute(const std::string& attr, const std::string& buf)
{
   // DEBUG_LEV (2, ("Unknown attribute: %s\n", attr.c_str ()));
   return false;
}

/* UTO Function Block **********************************************************

Function: AddShapeChildNode

Description:
   If billboarding is enabled and there is a static transform then subnodes
   need to be added to the lower transform not the root node.

*******************************************************************************/
void UtoShapeImp::AddShapeChildNode(osg::Node* node)
{
   // If billboarding is enabled and there are rotations/scaling from the
   // resource file there is a transform node between the root and where
   // other shape nodes are added.
   if (HasFlag(UtoShape::BILLBOARD) && m_StaticRotation.get())
   {
      osg::Group* group = dynamic_cast<osg::Group*>(m_RootNode->getChild(0));
      if (group)
      {
         group->addChild(node);
      }
      else
      {
         // LOG("Error, UtoShape's tree is not as expected, node failed "
         //    "to be added to %s\n", Name().c_str());
      }
   }
   else
   {
      m_RootNode->addChild(node);
   }
}

/* UTO Function Block **********************************************************

Function: RemoveShapeChildNode

Description:
   If billboarding is enabled and there is a static transform then subnodes
   need to be removed from the lower transform not the root node.

*******************************************************************************/
void UtoShapeImp::RemoveShapeChildNode(osg::Node* node)
{
   // If billboarding is enabled and there are rotations/scaling from the
   // resource file there is a transform node between the root and where
   // other shape nodes are removed.
   if (HasFlag(UtoShape::BILLBOARD) && m_StaticRotation.get())
   {
      osg::Group* group = dynamic_cast<osg::Group*>(m_RootNode->getChild(0));
      if (group)
      {
         group->removeChild(node);
      }
      else
      {
         // LOG("Error, UtoShape's tree is not as expected, node failed "
         //    "to be removed from %s\n", Name().c_str());
      }
   }
   else
   {
      m_RootNode->removeChild(node);
   }
}
//
///* UTO Function Block **********************************************************
//
// Function: UpdateBillboard
//
// Description:
//   Enable or disable billboard mode.  Only call when changing the mode,
//   don't call it when the old mode is the same as the new mode.
//
//*******************************************************************************/
// void UtoShapeImp::UpdateBillboard()
//{
//   // Must keep a reference to the state set for when the original m_RootNode
//   // goes away.
//   osg::ref_ptr<osg::StateSet> stateset = m_RootNode->getStateSet();
//   // Billboarding can have an extra node, in any case nodes are added to
//   // "nextGroup" and the next m_RootNode will be nextRoot, sometimes they can
//   // be the same.
//   osg::Transform *nextGroup, *nextRoot;
//   // specifies which node children are copied from
//   osg::Group* oldGroup;
//   if(HasFlag(UtoShape::BILLBOARD))
//   {
//      osg::AutoTransform* atrans = new osg::AutoTransform;
//      atrans->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_CAMERA);
//      atrans->setAutoScaleToScreen(false);
//
//      // If there are scaling or rotations loaded from the resource file
//      // we add a transform node to take care of them.
//      osg::Matrix matStaticTransRot(1.0,              0.0,              0.0,              0.0,
//                                    0.0,              1.0,              0.0,              0.0,
//                                    0.0,              0.0,              1.0,              0.0,
//                                    m_StaticOffset.x(), m_StaticOffset.y(), m_StaticOffset.z(), 1.0);
//
//      // obtain the static rotation matrix
//      osg::RefMatrix * pMatRot = m_StaticRotation.get();
//
//      // multiply the static translation by the static rotation
//      if (pMatRot) matStaticTransRot *= *pMatRot;
//
//      // determine how to add the transform to the group
//      if (pMatRot || m_StaticOffset.x() || m_StaticOffset.y() || m_StaticOffset.z())
//      {
//         // create a new matrix transform to handle the shift and rotation
//         osg::MatrixTransform * pMatTrans = new osg::MatrixTransform(matStaticTransRot);
//         // add the shift and rotation matrix to the auto transform
//         atrans->addChild(pMatTrans);
//         // setup the next group to process
//         nextGroup = pMatTrans;
//      }
//      else
//      {
//         // setup the next group to process
//         nextGroup = atrans;
//      }
//
//      nextRoot = atrans;
//      // coming from !billboard, always the root node
//      oldGroup = m_RootNode.get();
//   }
//   else
//   {
//      nextRoot = nextGroup = new osg::MatrixTransform;
//      // Figure out which node to read the old children from, if we have
//      // a static rotation matrix it is one level deeper.
//      if(m_StaticRotation.get())
//      {
//         oldGroup = static_cast<osg::Group*>(m_RootNode->getChild(0));
//      }
//      else
//      {
//         oldGroup = m_RootNode.get();
//      }
//   }
//
//   // copy the relavant data from the old transform to the new one
//   int i, stop;
//   for(i=0, stop=oldGroup->getNumChildren(); i<stop; ++i)
//   {
//      nextGroup->addChild(oldGroup->getChild(i));
//   }
//
//   // have the parents point to the new node
//   for(i=0, stop=m_RootNode->getNumParents(); i<stop; ++i)
//   {
//      m_RootNode->getParent(i)->addChild(nextRoot);
//   }
//   // remove the old node from the parents being careful to reference
//   // the list that won't change as they are modified
//   for(i=0, stop=nextRoot->getNumParents(); i<stop; ++i)
//   {
//      nextRoot->getParent(i)->removeChild(m_RootNode.get());
//   }
//
//   // finally replace the node
//   m_RootNode = nextRoot;
//   m_RootNode->setStateSet(stateset.get());
//}

void UtoShapeImp::CopyStateSet()
{
   osg::StateSet* stateset = m_RootNode->getStateSet();
   if (stateset == nullptr)
   {
      m_RootNode->getOrCreateStateSet();
   }
   else if (HasFlag(SHARED_STATESET) && stateset->referenceCount() > 1)
   {
      // need to make a copy of the stateset, but retain shared flag
      // so that clones of this shape will share the new stateset
      stateset = (osg::StateSet*)stateset->clone(osg::CopyOp::DEEP_COPY_ALL);
      m_RootNode->setStateSet(stateset);
   }
}

void UtoShapeImp::SetColor(const UtoColor& rColor, unsigned int nClrMask, unsigned int nReserved)
{
   m_oBlendColor.Set(rColor, nClrMask);
}

void UtoShapeImp::OverrideRenderBin(int aBin, bool aNested)
{
   m_RootNode->getOrCreateStateSet()->setRenderBinDetails(aBin, "DepthSortedBin");
   m_RootNode->getOrCreateStateSet()->setNestRenderBins(aNested);
}

void UtoShapeImp::SetNodeMask(unsigned int aMask, bool aState)
{
   unsigned int on_or_mask   = aMask;
   unsigned int off_and_mask = ~on_or_mask;
   if (aState)
   {
      m_RootNode->setNodeMask(m_RootNode->getNodeMask() | on_or_mask);
   }
   else
   {
      m_RootNode->setNodeMask(m_RootNode->getNodeMask() & off_and_mask);
   }
}

void UtoShapeImp::SetUseStaticScale(bool useStaticScale)
{
   m_UseStaticScale = useStaticScale;
   UpdateTransform();
}

double UtoShapeImp::FinalScale() const
{
   return m_StaticScale * m_Scaling * m_Zoom;
}

void UtoShapeImp::SetAllowUpdates(bool aState)
{
   mAllowUpdates = aState;
}

void UtoShapeImp::SetUniformf(const char* aName, float aValue)
{
   m_RootNode->getOrCreateStateSet()->getOrCreateUniform(aName, osg::Uniform::FLOAT)->set(aValue);
}

void UtoShapeImp::SetUniform3f(const char* aName, float aValueX, float aValueY, float aValueZ)
{
   osg::Vec3 vec(aValueX, aValueY, aValueZ);
   m_RootNode->getOrCreateStateSet()->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC3)->set(vec);
}

void UtoShapeImp::SetUniform3i(const char* aName, int aValueX, int aValueY, int aValueZ)
{
   m_RootNode->getOrCreateStateSet()->getOrCreateUniform(aName, osg::Uniform::INT_VEC3)->set(aValueX, aValueY, aValueZ);
}

void UtoShapeImp::SetUniform4f(const char* aName, float aValueX, float aValueY, float aValueZ, float aValueW)
{
   osg::Vec4 vec(aValueX, aValueY, aValueZ, aValueW);
   m_RootNode->getOrCreateStateSet()->getOrCreateUniform(aName, osg::Uniform::FLOAT_VEC4)->set(vec);
}

void UtoShapeImp::SetUniform4i(const char* aName, int aValueX, int aValueY, int aValueZ, int aValueW)
{
   m_RootNode->getOrCreateStateSet()->getOrCreateUniform(aName, osg::Uniform::INT_VEC4)->set(aValueX, aValueY, aValueZ, aValueW);
}

void UtoShapeImp::SetUniformSampler(const char*          aName,
                                    const unsigned char* aSampler1D,
                                    const unsigned int   aComponents,
                                    const unsigned int   aLength)
{
   if (aComponents <= 4)
   {
      unsigned int samplerIndex = AllocateSamplerIndex(aName);

      unsigned char* datamem = new unsigned char[aLength * aComponents];
      memcpy(datamem, aSampler1D, aLength * aComponents);
      osg::ref_ptr<osg::Image> hmimage = new osg::Image();
      hmimage->setImage(aLength, 1, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, datamem, osg::Image::USE_NEW_DELETE);
      osg::ref_ptr<osg::Texture1D> hmtexture = new osg::Texture1D();
      hmtexture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
      hmtexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
      hmtexture->setImage(hmimage.get());

      m_RootNode->getOrCreateStateSet()->setTextureAttributeAndModes(samplerIndex, hmtexture.get(), osg::StateAttribute::ON);
      m_RootNode->getOrCreateStateSet()->getOrCreateUniform(aName, osg::Uniform::SAMPLER_1D)->set((int)samplerIndex);
   }
}

void UtoShapeImp::SetUniformSampler(const char*     aName,
                                    const char*     aFilename,
                                    unsigned char*& aData,
                                    unsigned int&   aComponents,
                                    unsigned int&   aSize)
{
   osg::ref_ptr<osg::Image> hmimage = osgDB::readImageFile(aFilename);
   if (nullptr != hmimage)
   {
      unsigned int samplerIndex = AllocateSamplerIndex(aName);

      aSize                  = hmimage->getTotalDataSize() / 3;
      unsigned char* datamem = new unsigned char[aSize * 3];
      memcpy(datamem, (unsigned char*)(hmimage->getDataPointer()), aSize * 3);
      aData = datamem;

      hmimage->setAllocationMode(osg::Image::USE_NEW_DELETE);
      osg::ref_ptr<osg::Texture1D> hmtexture = new osg::Texture1D();
      hmtexture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      hmtexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
      hmtexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
      hmtexture->setImage(hmimage.get());

      m_RootNode->getOrCreateStateSet()->setTextureAttributeAndModes(samplerIndex, hmtexture.get(), osg::StateAttribute::ON);
      m_RootNode->getOrCreateStateSet()->getOrCreateUniform(aName, osg::Uniform::SAMPLER_1D)->set((int)samplerIndex);
   }
}

void UtoShapeImp::SetProgram(osg::Program* aProgramPtr, bool aEnable)
{
   m_RootNode->getOrCreateStateSet()->setAttribute(aProgramPtr,
                                                   aEnable ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
}

unsigned int UtoShapeImp::AllocateSamplerIndex(const char* aName)
{
   unsigned int                                  samplerIndex = mSamplerIndex;
   std::map<std::string, unsigned int>::iterator it           = mSamplerToIndex.find(aName);
   if (it != mSamplerToIndex.end())
   {
      samplerIndex = it->second;
   }
   else
   {
      mSamplerToIndex[aName] = samplerIndex;
      ++mSamplerIndex;
   }
   return samplerIndex;
}

bool UtoShapeImp::BindToViewer(UtoViewer& aViewer, osg::Group* aParentNode)
{
   UtoShapeFrame* framePtr = nullptr;
   if (mFrameTemplate == nullptr)
   {
      framePtr = new UtoShapeFrameTransform(this, &aViewer);
      framePtr->SetName(Name());
   }
   else
   {
      // allow for custom frame implementations
      framePtr = mFrameTemplate->Clone(this, &aViewer);
      framePtr->SetName(Name());
   }
   if (aParentNode)
   {
      framePtr->SetParentNode(aParentNode);
   }
   framePtr->GetNode()->setUserData(new UtoViewerShapeData(this));
   mFrames.push_back(framePtr);
   framePtr->GetNode()->addChild(m_RootNode.get());
   aViewer.ShapeAdded(GetPublicPtr());
   return true;
}

void UtoShapeImp::UnbindViewer(UtoViewer& aViewer)
{
   UtoShapeFrame* framePtr = FindFrame(aViewer);
   if (framePtr)
   {
      aViewer.ShapeRemoved(GetPublicPtr());
      framePtr->SetVisible(false);
      framePtr->Unref();
      mFrames.erase(std::find(mFrames.begin(), mFrames.end(), framePtr));
   }
}

void UtoShapeImp::UnbindViewers()
{
   while (!mFrames.empty())
   {
      UnbindViewer(*mFrames.front()->GetViewer());
   }
}

//
// UtoSymbol* UtoShapeImp::FindSymbol(UtoViewer& aViewer) const
//{
//   for (size_t i = 0; i < mSymbols.size(); ++i)
//   {
//      if (mSymbols[i]->Viewer() == &aViewer)
//      {
//         return mSymbols[i];
//      }
//   }
//   return 0;
//}

// show or hide a shape that has already been bound to a viewer
bool UtoShapeImp::Show(bool aShow, UtoViewer& aViewer)
{
   UtoShapeFrame* frame = FindFrame(aViewer);
   if (frame)
   {
      frame->SetVisible(aShow);
      return true;
   }
   return false;
}
bool UtoShapeImp::IsVisible(UtoViewer* aViewer) const
{
   if (!aViewer)
      return false;
   UtoShapeFrame* frame = FindFrame(*aViewer);
   return frame && frame->IsVisible();
}


void UtoShapeImp::SetReferenceFrame(UtoShapeFrame* aFrame)
{
   if (mFrameTemplate)
      mFrameTemplate->Unref();
   mFrameTemplate = aFrame;
   if (mFrameTemplate)
   {
      for (size_t i = 0; i < mFrames.size(); ++i)
      {
         UtoShapeFrame* f = mFrameTemplate->Clone(this, mFrames[i]->GetViewer());
         std::swap(mFrames[i], f);
         f->Unref();
      }
   }
}

void UtoShapeImp::SetReferenceFrame(const UtoShape::ReferenceFrame& aFrame)
{
   mFrame = aFrame;
   for (size_t i = 0; i < mFrames.size(); ++i)
   {
      mFrames[i]->FrameChanged();
   }
}


const UtoShape::ReferenceFrame& UtoShapeImp::GetReferenceFrame() const
{
   return mFrame;
}

UtoShapeFrame* UtoShapeImp::FindFrame(const UtoViewer& aViewer) const
{
   for (size_t i = 0; i < mFrames.size(); ++i)
   {
      if (mFrames[i]->GetViewer() == &aViewer)
      {
         return mFrames[i];
      }
   }
   return nullptr;
}
