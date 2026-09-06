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

#ifndef _UTOSPHERE_SHAPE_HPP_
#define _UTOSPHERE_SHAPE_HPP_

// uto includes
#include "UtoShape.hpp"

class UTILOSG_EXPORT UtoSphereShape : public UtoShape
{
public:
   // constructor / destructor
   UtoSphereShape(const double& rRadius     = 1.0,
                  unsigned int  nSlices     = 30,
                  unsigned int  nStacks     = 30,
                  const double& rCFactorX   = 1.0,
                  const double& rCFactorY   = 1.0,
                  const double& rCFactorZ   = 1.0,
                  bool          bShowSlices = true,
                  bool          bShowStacks = true);
   UtoSphereShape(const UtoSphereShape& rSphere);
   ~UtoSphereShape() override;

   // allows the shape to be copied
   UtoSphereShape& operator=(const UtoSphereShape& rSphere);

   void SetLineColor(const UtoColor& aColor);

   // allows access to the radius
   void   SetRadius(double rRadius);
   double Radius() const;

   void   SetMinimumRadius(double rMinRadius);
   double MinimumRadius() const;

   void SetMinAltitude(double aMinAltitude);
   void SetMaxAltitude(double aMaxAltitude);

   void SetOrientation(const UtoOrientation& aOrientation);

   void SetAngleBounds(double aStartAngle, double aStopAngle);

   // allows access to the number of slices
   // the slice angle is calculated by taking 360 / nSlices
   void         SetSlices(unsigned int nSlices);
   unsigned int Slices() const;

   // allows access to the number of stacks
   // the stack angle is calculated by taking 180 / nStacks
   void         SetStacks(unsigned int nStacks);
   unsigned int Stacks() const;

   // allows access to the compression factor
   // the compression factor will compress or stretch the sphere along each axis to create an ellipsoid
   void          SetCompressionFactorX(const double& rFactor);
   const double& GetCompressionFactorX() const;
   void          SetCompressionFactorY(const double& rFactor);
   const double& GetCompressionFactorY() const;
   void          SetCompressionFactorZ(const double& rFactor);
   const double& GetCompressionFactorZ() const;

   // allows the user to set all three attributes of the sphere at once
   void Set(const double& rRadius,
            unsigned int  nSlices,
            unsigned int  nStacks,
            const double& rFactorX   = 1.0,
            const double& rFactorY   = 1.0,
            const double& rFactorZ   = 1.0,
            const double& rMinRadius = 0.0);

   // allows access to show and hide the slice lines that make up the sphere
   void ShowSliceLines(bool bShow);
   bool SliceLinesVisible() const;

   // allows access to show and hide the stack lines that make up the sphere
   void ShowStackLines(bool bShow);
   bool StackLinesVisible() const;

   // allows access to show and hide the sphere geometry
   void ShowSphereGeometry(bool bShow);

   // allows access to the width of the slice and stack lines
   void   SetLineWidth(double width);
   double LineWidth() const;

   // allows access to the line stipple pattern for slice and stack lines
   void SetPattern(int pattern, int factor = 1);

   // clones the shape
   UtoShape* Clone() const override;

   void EnableLighting(bool aState);
};

#endif // _UTOSPHERE_SHAPE_HPP_
