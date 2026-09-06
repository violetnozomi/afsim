// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtInertiaTensor.hpp"

#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtVec3dX.hpp"

// ==============================================================================
// static
const UtInertiaTensor UtInertiaTensor::cNULL = UtInertiaTensor();

// ==============================================================================
UtInertiaTensor::UtInertiaTensor()
   : UtMatrixd()
{
   SetSize(3, 3);
   Fill(0.0);
}

// ==============================================================================
bool UtInertiaTensor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if ((command == "tensor") || (command == "inertia_tensor"))
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         UtInput& input = inputBlock.GetInput();
         command        = input.GetCommand();
         int i;
         int j;
         if ((command == "ixx") || (command == "Ixx"))
         {
            i = 0;
            j = 0;
         }
         else if ((command == "ixy") || (command == "Ixy"))
         {
            i = 0;
            j = 1;
         }
         else if ((command == "ixz") || (command == "Ixz"))
         {
            i = 0;
            j = 2;
         }
         else if ((command == "iyx") || (command == "Iyx"))
         {
            i = 1;
            j = 0;
         }
         else if ((command == "iyy") || (command == "Iyy"))
         {
            i = 1;
            j = 1;
         }
         else if ((command == "iyz") || (command == "Iyz"))
         {
            i = 1;
            j = 2;
         }
         else if ((command == "izx") || (command == "Izx"))
         {
            i = 2;
            j = 0;
         }
         else if ((command == "izy") || (command == "Izy"))
         {
            i = 2;
            j = 1;
         }
         else if ((command == "izz") || (command == "Izz"))
         {
            i = 2;
            j = 2;
         }
         else
         {
            throw UtInput::BadValue(inputBlock.GetInput());
         }

         double inertia;
         aInput.ReadValueOfType(inertia, UtInput::cANGULAR_INERTIA);
         (*this)(i, j) = inertia;
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// ==============================================================================
UtInertiaTensor UtInertiaTensor::Inverse() const
{
   // Copy the current tensor into the mInverse, then invert it.
   UtInertiaTensor result(*this);
   result.Invert();
   return result;
}

// ==============================================================================
UtVec3dX UtInertiaTensor::operator*(const UtVec3dX& aVect) const
{
   return UtVec3dX((*this)(0, 0) * aVect.Get(0) + (*this)(0, 1) * aVect.Get(1) + (*this)(0, 2) * aVect.Get(2),
                   (*this)(1, 0) * aVect.Get(0) + (*this)(1, 1) * aVect.Get(1) + (*this)(1, 2) * aVect.Get(2),
                   (*this)(2, 0) * aVect.Get(0) + (*this)(2, 1) * aVect.Get(1) + (*this)(2, 2) * aVect.Get(2));
}

// ==============================================================================
void UtInertiaTensor::Multiply(const UtDCM& aDCM, UtInertiaTensor& aResult) const
{
   // (The following multiply algorithm came from UtMatrix.)
   for (unsigned int i = 0; i != 3; ++i)
   {
      for (unsigned int j = 0; j != 3; ++j)
      {
         double sum = 0.0;
         for (unsigned int k = 0; k != 3; ++k)
         {
            sum += (*this)(i, k) * aDCM.Get(k, j);
         }
         aResult(i, j) = sum;
      }
   }
}

// ==========================================================================
void UtInertiaTensor::Null()
{
   for (int i = 0; i != 3; ++i)
   {
      for (int j = 0; j != 3; ++j)
      {
         (*this)(i, j) = 0.0;
      }
   }
}

// ==========================================================================
void UtInertiaTensor::Fill3By3(double aMat[3][3]) const
{
   for (int i = 0; i != 3; ++i)
   {
      for (int j = 0; j != 3; ++j)
      {
         aMat[i][j] = (*this)(i, j);
      }
   }
}

// ==========================================================================
// static
UtInertiaTensor UtInertiaTensor::Interpolate(double                 aXtarget,
                                             double                 aX0,
                                             double                 aX1,
                                             const UtInertiaTensor& aY0,
                                             const UtInertiaTensor& aY1)
{
   double interpolant = (aXtarget - aX0) / (aX1 - aX0);
   return Interpolate(interpolant, aY0, aY1);
}

// ==========================================================================
// static
UtInertiaTensor UtInertiaTensor::Interpolate(double aInterpolant, const UtInertiaTensor& aY0, const UtInertiaTensor& aY1)
{
   UtInertiaTensor result;
   for (int i = 0; i != 3; ++i)
   {
      for (int j = 0; j != 3; ++j)
      {
         result(i, j) = aY0(i, j) + aInterpolant * (aY1(i, j) - aY0(i, j));
      }
   }
   return result;
}

// ==========================================================================
UtInertiaTensor UtInertiaTensor::ParallelAxisTheorem(double aMassValue, const UtVec3dX& aOffsetFromCG)
{
   UtInertiaTensor result;
   // aOffsetFromCG is in the parent frame, and measured bewteen cg and object.
   AggregateInto(aMassValue, aOffsetFromCG, result);
   return result;
}

// ==========================================================================
// static
void UtInertiaTensor::AggregateInto(double aMassValue, const UtVec3dX& aOffsetFromCG, UtInertiaTensor& aAggregatedTensor)
{
   // Consult a dynamics text for the Parallel Axis Theorem:

   // When one UtInertiaTensor object "foo" is aggregated into "bar",
   // "foo" must first be transformed into the frame of "bar".
   // ("this" is 'foo'.)   (Here we assume foo is already transformed.)

   // Second, the components of "foo" are aggregated into "bar":
   for (int i = 0; i != 3; ++i)
   {
      for (int j = 0; j != 3; ++j)
      {
         aAggregatedTensor(i, j) += (*this)(i, j);
      }
   }

   // Ixx component addition = Ixx of the small object (done above), plus M * (dy * dy + dz * dz)
   aAggregatedTensor(0, 0) += aMassValue * (aOffsetFromCG[1] * aOffsetFromCG[1] + aOffsetFromCG[2] * aOffsetFromCG[2]);
   aAggregatedTensor(1, 1) += aMassValue * (aOffsetFromCG[0] * aOffsetFromCG[0] + aOffsetFromCG[2] * aOffsetFromCG[2]);
   aAggregatedTensor(2, 2) += aMassValue * (aOffsetFromCG[0] * aOffsetFromCG[0] + aOffsetFromCG[1] * aOffsetFromCG[1]);

   // Ixy component addition = Ixy of the small object (done above), plus M * (dx * dy);
   double dIxy = aMassValue * aOffsetFromCG[0] * aOffsetFromCG[1];
   double dIyz = aMassValue * aOffsetFromCG[1] * aOffsetFromCG[2];
   double dIxz = aMassValue * aOffsetFromCG[0] * aOffsetFromCG[2];

   // Remaining off-diagonal terms are reflected about the diagonal:
   aAggregatedTensor(0, 1) += dIxy;
   aAggregatedTensor(1, 2) += dIyz;
   aAggregatedTensor(0, 2) += dIxz;

   aAggregatedTensor(1, 0) += dIxy;
   aAggregatedTensor(2, 1) += dIyz;
   aAggregatedTensor(2, 0) += dIxz;
}
