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

#ifndef _UTOSPHERE_SHAPE_IMP_HPP_
#define _UTOSPHERE_SHAPE_IMP_HPP_

// uto includes
#include "UtoShapeImp.hpp"

class UtoSphereShapeImp : public UtoShapeImp
{
   // private typedefs
   using GeometryVector = std::vector<osg::ref_ptr<osg::Geometry>>;
   using osgIndexArray  = osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 1, GL_UNSIGNED_INT>;

public:
   // constructor / destructor
   UtoSphereShapeImp(const double& rRadius     = 1.0,
                     unsigned int  nSlices     = 30,
                     unsigned int  nStacks     = 30,
                     const double& rCFactorX   = 1.0,
                     const double& rCFactorY   = 1.0,
                     const double& rCFactorZ   = 1.0,
                     bool          bShowSlices = false,
                     bool          bShowStacks = false);
   UtoSphereShapeImp(const UtoSphereShapeImp& rSphereImp);
   ~UtoSphereShapeImp() override;

   void LineAndVertexCopy(const GeometryVector& rGeoVec, GeometryVector& aNewGeoVec, std::vector<osg::Array*> aVertices);

   // allows access to the radius
   void   SetRadius(double rRadius);
   double Radius() const { return m_dRadius; }

   void   SetMinimumRadius(double rRadius);
   double MinimumRadius() const { return m_dMinimumRadius; }

   void SetMinAltitude(double aMinAltitude);
   void SetMaxAltitude(double aMaxAltitude);

   void SetOrientationTransform(const UtoOrientation& aOrientation);

   void SetAngleBounds(double aStartAngle, double aStopAngle);

   // allows access to the number of slices
   // the slice angle is calculated by taking 360 / nSlices
   void         SetSlices(unsigned int nSlices);
   unsigned int Slices() const { return m_nSlices; }

   // allows access to the number of stacks
   // the stack angle is calculated by taking 180 / nStacks
   void         SetStacks(unsigned int nStacks);
   unsigned int Stacks() const { return m_nStacks; }

   // allows access to the compression factor
   // the compression factor will compress or stretch the sphere along each axis to create an ellipsoid
   void          SetCompressionFactorX(double rFactor);
   const double& GetCompressionFactorX() const { return m_dCFactorX; }
   void          SetCompressionFactorY(double rFactor);
   const double& GetCompressionFactorY() const { return m_dCFactorY; }
   void          SetCompressionFactorZ(double rFactor);
   const double& GetCompressionFactorZ() const { return m_dCFactorZ; }

   // allows the user to set all three attributes of the sphere at once
   void Set(double       rRadius,
            double       rMinRadius,
            unsigned int nSlices,
            unsigned int nStacks,
            double       rCFactorX = 1.0,
            double       rCFactorY = 1.0,
            double       rCFactorZ = 1.0);

   // allows access to show and hide the slice lines that make up the sphere
   void ShowSliceLines(bool bShow);
   bool SliceLinesVisible() const { return m_bSlicesVisible; }

   // allows access to show and hide the stack lines that make up the sphere
   void ShowStackLines(bool bShow);
   bool StackLinesVisible() const { return m_bStacksVisible; }

   // allows access to show and hide the sphere geometry
   void ShowSphereGeometry(bool bShow);

   // allows access to the width of the slice and stack lines
   void   SetLineWidth(double width);
   double LineWidth() const { return m_dLineWidth; }

   // allows access to the line stipple pattern for slice and stack lines
   void SetPattern(short pattern, int factor);

   // overrides from UtoShapeImp
   // sets the color of the shape
   void SetColor(const UtoColor& rColor, unsigned int nClrMask, unsigned int nReserved = 0) override;
   void SetLineColor(const UtoColor& aColor);

   void EnableLighting(bool aState);

private:
   // prohibit the copy operator
   UtoSphereShapeImp& operator=(const UtoSphereShapeImp& rSphereImp);

   //! This function is designed to map a 2D array index to a 1D array index
   //! @param aRowPos is the row index
   //! @param aColumnPos is the column index
   //! @param aColumnSize is the number of rows in each column
   //! @returns the position in the 1D array
   //! @note This assumes the 2D array is column-major
   int ColumnToRowIndexing(int aRowPos, int aColumnPos, int aColumnSize) const;

   // calculates a sphere shape with the given parameters
   osg::Geode* CreateSphere(double       rRadius,
                            double       rMinRadius,
                            unsigned int m_nStacks,
                            unsigned int m_nSlices,
                            double       rCFactorX,
                            double       rCFactorY,
                            double       rCFactorZ);

   // constructs the sphere and all the different levels
   // Potentially helpful note for future devs: draw trianges in a clockwise fashon for them to be visible
   void ConstructSphere();

   // binds the sphere lines to the shape
   void BindSphereLines();

   // private member variables
   // defines the radius of the shape
   double m_dRadius;
   double m_dMinimumRadius;

   // defines the min and max altitude bounds of the shape
   double m_dMinimumAltitude;
   bool   m_bBoundedMin;
   double m_dMaximumAltitude;
   bool   m_bBoundedMax;

   // defines the start and stop angles of the shape
   double m_dStartAngle;
   double m_dStopAngle;

   // defines the compression factor
   double m_dCFactorX;
   double m_dCFactorY;
   double m_dCFactorZ;

   // defines the stack and slice values
   unsigned int m_nStacks;
   unsigned int m_nSlices;

   // defines if the stack or slices lines, or sphere geometry, are visible
   bool m_bStacksVisible;
   bool m_bSlicesVisible;
   bool m_bSphereVisible;

   // defines the width of the stack and slice lines
   double m_dLineWidth;

   // defines the line stippling pattern of the stack and slice lines
   short m_sStipplePattern;
   int   m_iStippleFactor;

   bool mLighting;

   // defines the level of detail for the sphere
   osg::ref_ptr<osg::LOD> m_pLODRoot;

   // defines the color of the lines wrapping the sphere
   UtoColor mLineColor;

   // vectors that define the geometry to draw the stack and slice lines
   GeometryVector        m_vSliceLines;
   GeometryVector        m_vSliceLinesMinRad;
   GeometryVector        m_vStackLines;
   GeometryVector        m_vStackLinesMinRad;
   GeometryVector        m_vSphereGeometry;
   osg::MatrixTransform* mOrientationTransform;
};

#endif // _UTOSPHERE_SHAPE_IMP_HPP_
