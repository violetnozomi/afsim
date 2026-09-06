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

#include "script/WsfScriptCovarianceClass.hpp"

#include <vector>

#include "UtCovariance.hpp"
#include "UtCovarianceEllipsoid.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptVec3.hpp"
#include "WsfTrack.hpp"
#include "WsfUtil.hpp"

namespace
{
void ComputeAxesBearing(const double aOriginatorLocWCS[3], const UtCovariance* aCovPtr, double aCovInfo[3])
{
   // Assume Perfection???
   aCovInfo[0] = 0.0; // Semi-Major Axis
   aCovInfo[1] = 0.0; // Semi-Minor Axis
   aCovInfo[2] = 0.0; // Bearing

   if (aCovPtr != nullptr)
   {
      UtCovariance cov(*aCovPtr);
      double       lat, lon, alt;
      UtEllipsoidalEarth::ConvertECEFToLLA(aOriginatorLocWCS, lat, lon, alt);
      double locWCS[3];
      double WCS_ToNED_Transform[3][3];
      UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, locWCS);
      UtCovariance::ConvertFromFrameB_ToFrameA(cov, WCS_ToNED_Transform);

      // Now that we have an NED transform, copy the NE part to a 2x2 vector:
      std::vector<double> neVec;
      neVec.push_back(cov(0, 0));
      neVec.push_back(cov(0, 1));
      neVec.push_back(cov(1, 0));
      neVec.push_back(cov(1, 1));

      WsfUtil::CovarianceToEllipse_2x2(neVec, aCovInfo[0], aCovInfo[1], aCovInfo[2]);
   }
}
} // namespace

WsfScriptCovarianceClass::WsfScriptCovarianceClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCovariance");

   mConstructible = true;
   mCloneable     = true;

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<CenterLocation>());
   AddMethod(ut::make_unique<MajorAxis>());
   AddMethod(ut::make_unique<MinorAxis>());
   AddMethod(ut::make_unique<Bearing>());
   AddMethod(ut::make_unique<CovarianceAtTime>());
   AddMethod(ut::make_unique<Trace>());
   AddMethod(ut::make_unique<Rows>());
   AddMethod(ut::make_unique<Columns>());
   AddMethod(ut::make_unique<Element>());
   AddMethod(ut::make_unique<EllipsoidNED>());
}

WsfScriptCovarianceClass::~WsfScriptCovarianceClass() {}

// virtual
void* WsfScriptCovarianceClass::Create(const UtScriptContext& aInstance)
{
   UtCovariance* covariancePtr = new UtCovariance();
   return covariancePtr;
}

// virtual
void* WsfScriptCovarianceClass::Clone(void* aObjectPtr)
{
   return new UtCovariance(*(static_cast<UtCovariance*>(aObjectPtr)));
}

// virtual
void WsfScriptCovarianceClass::Destroy(void* aObjectPtr)
{
   UtCovariance* objPtr = static_cast<UtCovariance*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, Construct, 1, "WsfCovariance", "WsfTrack")
{
   WsfTrack*     trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   UtCovariance* covarPtr = UtCovariance::CreateMeasurementCovarianceWCS(*trackPtr);
   // double covInfo[3] = { 0.0, 0.0, 0.0 };
   // double originatorLocWCS[3];
   // trackPtr->GetOriginatorLocationWCS(originatorLocWCS);
   // ComputeAxesBearing(originatorLocWCS, covarPtr, covInfo);
   aReturnVal.SetPointer(new UtScriptRef(covarPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, MajorAxis, 0, "double", "")
{
   double covInfo[3] = {0.0, 0.0, 0.0};
   double centerLocWCS[3];
   aObjectPtr->GetTargetLocationWCS(centerLocWCS);
   ComputeAxesBearing(centerLocWCS, aObjectPtr, covInfo);
   aReturnVal.SetDouble(covInfo[0] * 2.0);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, CenterLocation, 0, "Vec3", "")
{
   UtVec3d targetLocWCS;
   aObjectPtr->GetTargetLocationWCS(targetLocWCS.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(targetLocWCS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, MinorAxis, 0, "double", "")
{
   double covInfo[3] = {0.0, 0.0, 0.0};
   double originatorLocWCS[3];
   aObjectPtr->GetOriginLocationWCS(originatorLocWCS);
   ComputeAxesBearing(originatorLocWCS, aObjectPtr, covInfo);
   aReturnVal.SetDouble(covInfo[1] * 2.0);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, Bearing, 0, "double", "")
{
   double covInfo[3] = {0.0, 0.0, 0.0};
   double originatorLocWCS[3];
   aObjectPtr->GetOriginLocationWCS(originatorLocWCS);
   ComputeAxesBearing(originatorLocWCS, aObjectPtr, covInfo);
   aReturnVal.SetDouble(covInfo[2] * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, Trace, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Trace());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, CovarianceAtTime, 1, "WsfCovariance", "double")
{
   UtCovariance* covariancePtr = new UtCovariance(*aObjectPtr);
   double        simTime       = aVarArgs[0].GetDouble();
   if (!covariancePtr->ComputePredictedState(simTime, *aObjectPtr))
   {
      delete covariancePtr;
      covariancePtr = nullptr;
   }
   aReturnVal.SetPointer(new UtScriptRef(covariancePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, Rows, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetRows());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, Columns, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetCols());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, Element, 2, "double", "int, int")
{
   int    row     = aVarArgs[0].GetInt();
   int    col     = aVarArgs[1].GetInt();
   double element = (*aObjectPtr)(row, col);
   aReturnVal.SetDouble(element);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCovarianceClass, UtCovariance, EllipsoidNED, 0, "Ellipsoid", "")
{
   UtCovariance covariance(*aObjectPtr);

   UtCovariance covarianceNED(3, 3);
   covarianceNED.ComputeNED_FromWCS(covariance);

   UtCovarianceEllipsoid* ellipsoidPtr = new UtCovarianceEllipsoid;
   *ellipsoidPtr                       = UtCovarianceEllipsoid::CovarianceNED2Ellipsoid(covarianceNED);
   aReturnVal.SetPointer(new UtScriptRef(ellipsoidPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
