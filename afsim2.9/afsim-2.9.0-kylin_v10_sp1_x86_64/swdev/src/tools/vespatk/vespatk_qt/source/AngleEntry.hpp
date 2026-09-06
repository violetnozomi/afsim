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

#ifndef ANGLEENTRY_HPP
#define ANGLEENTRY_HPP

#include "vespatk_qt_export.h"

#include <QColor>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>

#include "UtLLAPos.hpp"
#include "UtVec3.hpp"
#include "UtoMemory.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"

namespace vespa
{
//! AngleEntry handles the calculation of the angle between two vectors
//! while also construction and managing the geometry and state variables
//! to render said geometry as a semi circle between the vectors.
//! This shape makes use of OpenSceneGraph shapes
//! and uses shaders to render. Shader names can be found in code, I'm not writing them
//! here because they may be subject to forgotten comment syndrome.
class VESPATK_QT_EXPORT AngleEntry
{
public:
   AngleEntry() = default;
   AngleEntry(const UtVec3f& aFirst, const UtVec3f& aSecond, const QColor& aColor, const std::string& aName, float aRadius);
   //! Calculate the angle between the two member vectors
   //! result is in radians.
   double CalculateAngle();
   //! Hook into render update to redraw geometry if vectors
   //! have changes. If they have not changed, or if the
   //! user has made this entry invisible, update will not
   //! recalculate the angle and reform the geometry.
   void Update();
   //! Returns the shape that represents the angle sector.
   //! This allows the shape to hook into the scene graph via its parent.
   //! This is accompanied by the method SetSectorFromParent which will
   //! take in the pointer from the parenting operation.
   UtoRawShape& GeSectorForTree();
   //! Takes in the pointer from the shape parenting operation.
   //! This ptr is the SceneGraph data structure to operate on.
   void SetSectorFromParent(UtoShape* aSectorInParent);
   //! Updates Start vector aka mVectorOne with new vector.
   void UpdateStartVector(const UtVec3f& aVector);
   //! Updates Start vector aka mVectorTwo with new vector.
   void UpdateEndVector(const UtVec3f& aVector);
   //! Sets the radius used to draw the geometry in the rendering pipeline.
   void SetRadius(float aRadius);
   //! Is the angle visible?
   bool IsAngleVisible() const;
   //! Set the visibility of the angle.
   void SetVisibility(bool aIsVisible);
   //! Should render as triangle fan?
   //! Triangle fan means that the sector will be
   //! colored in, ie filled.
   void ShouldFill(bool aShouldFill);
   //! Does Render as triangle fan?
   bool IsModeFill() const;
   //! The color of the angle shape when rendered.
   const QColor& GetColor() const;
   //! Gets the value of the angle between the two vectors
   //! without recalculating.
   double GetAngle() const;
   //! Returns the name of the shape. Used in the overlay and context menus.
   const std::string& GetName() const;

private:
   //! Takes initial values and sets up the needed constructs for rendering
   //! ie the vertex buffer, shader compilation, uniform set up etc.
   void LoadShape();
   //! Updates the geometry by setting the new vectors
   //! in their associated shader uniform.
   void UpdateGeometry();
   //! Initial vertex creation calculation.
   void LoadGeometry();
   //! Radius of the angle to be drawn as a semi circle.
   float mRadius = 12;
   //! Shape given back to use from parenting process.
   UtoRawShape* mSectorInGraph = nullptr;
   //! OSG Geometry object
   osg::ref_ptr<osg::Geometry> mGeom = new osg::Geometry{};
   //! OSG Geometry node
   osg::ref_ptr<osg::Geode> mGeode = new osg::Geode{};
   //! Start vector for angle
   UtVec3f mVectorOne{};
   //! Terminating vector for angle.
   UtVec3f mVectorTwo{};
   //! Is the angle visible?
   bool mIsVisible = true;
   //! Angle between the two vectors in radians
   double mAngleInRadians;
   //! Used for performance, no need to update shaders if vectors have not changed.
   bool mGeometryUpdateNeeded = true;
   //! Should we use the triangle fan to fill the shape?
   bool mIsModeFill = true;
   //! Name of the entry.
   const std::string mName;
   //! Color of the entry
   QColor mColorOfEntry;
   //! Shape that will be given to parent for parenting process,
   //! DO NOT WRITE OSG OPERATIONS TO THIS.
   UtoRawShape mSector{};
   //! Vertices that make up the semi circle representing the angle
   osg::ref_ptr<osg::Vec3Array> mVertices = new osg::Vec3Array{};
   //! Transform to make sure were using the parents matrix transforms to be in the correct position
   osg::ref_ptr<osg::PositionAttitudeTransform> mTransform = new osg::PositionAttitudeTransform();
   //! Shader program object, used to represent the Shader to set uniforms and Vertex buffers for.
   static osg::ref_ptr<osg::Program> mShaderProgram;
};
} // namespace vespa

#endif
