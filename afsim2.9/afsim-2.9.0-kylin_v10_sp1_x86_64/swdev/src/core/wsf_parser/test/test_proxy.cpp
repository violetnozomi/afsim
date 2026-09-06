// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "WsfPProxy.hpp"
#include "WsfPProxyCommon.hpp"

TEST(WsfProxyCommon, ValueKindToString)
{
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cSTRUCT), "Struct");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cLIST), "List");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cOBJECT_MAP), "ObjectMap");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cPOINTER), "Pointer");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cENUMERATION_VALUE), "Enumeration");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cBOOL_VALUE), "Bool");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cINT_VALUE), "Int");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cSTRING_VALUE), "String");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cDOUBLE_VALUE), "Double");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cLENGTH_VALUE), "Length");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cTIME_VALUE), "Time");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cSPEED_VALUE), "Speed");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cDATASIZE_VALUE), "DataSize");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cPOWER_VALUE), "Power");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cPOWERDB_VALUE), "PowerDB");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cSOLIDANGLE_VALUE), "SolidAngle");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cMASS_VALUE), "Mass");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cFORCE_VALUE), "Force");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cTORQUE_VALUE), "Torque");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cAREA_VALUE), "Area");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cAREADB_VALUE), "AreaDB");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cVOLUME_VALUE), "Volume");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cTIME2_VALUE), "Time2");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cACCELERATION_VALUE), "Acceleration");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cFREQUENCY_VALUE), "Frequency");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cANGLE_VALUE), "Angle");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cANGULARRATE_VALUE), "AngularRate");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cANGULARACCELERATION_VALUE), "AngularAcceleration");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cDATARATE_VALUE), "DataRate");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cMASSDENSITY_VALUE), "MassDensity");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cMASSTRANSFER_VALUE), "MassTransfer");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cENERGY_VALUE), "Energy");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cFLUENCE_VALUE), "Fluence");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cIRRADIANCE_VALUE), "Irradiance");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cRATIO_VALUE), "Ratio");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cNOISEPRESSURE_VALUE), "NoisePressure");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cPRESSURE_VALUE), "Pressure");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cTEMPERATURE_VALUE), "Temperature");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cSPECIFICRANGE_VALUE), "SpecificRange");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cANGULARINERTIA_VALUE), "AngularInertia");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cLATITUDE_VALUE), "Latitude");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cLONGITUDE_VALUE), "Longitude");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cPOSITION_VALUE), "Position");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cLENGTH2_VALUE), "Length2");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cLENGTH3_VALUE), "Length3");
   ASSERT_EQ(wsf::proxy::ValueKindToString(WsfProxy::cQUOTABLE_STRING_VALUE), "QuotableString");
   // The default case
   ASSERT_EQ(wsf::proxy::ValueKindToString(static_cast<WsfProxy::ValueKind>(-1)), "");
}

TEST(ProxyTest, Construction)
{
   WsfPProxy p;
   ASSERT_FALSE(p.mBeforeModifiedCallback);
   ASSERT_FALSE(p.mModifiedCallback);
   ASSERT_EQ(0, p.SequenceNumber());
   ASSERT_TRUE(nullptr == p.GetRegistry());
   ASSERT_FALSE(p.mHasModifications);
   ASSERT_TRUE(nullptr == p.mIndex);
   ASSERT_TRUE(p.SaveFiles().empty());
}
