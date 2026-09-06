// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef L16_MESSAGE12_3_HPP
#define L16_MESSAGE12_3_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
//! The J12.3 Flight Path message is used by controlling units to provide air
//! units with multiple-leg flight path information.
namespace J12_3
{

class WaypointActionField : public NormalField<4, NoStatementZero>
{
public:
   enum ActionType
   {
      cORIGINAL_POINT,
      cREPLACEMENT_POINT,
      cCANCEL_POINT,
      cLAST_POINT_IN_SERIES,
      cORBIT_THIS_POINT,
      cRESTRICTED_AREA_BOUNDARY,
      cTARGET,
      cALL_POINTS_RECEIVED = 8,
      cLOST_POINT,
      cPILOT_CANNOT_COMPLY,
      cPILOT_WILL_COMPLY
   };
   DEFINE_ACCESSORS(WaypointActionField, ActionType);
};

class AltitudeAmplificationField : public NormalField<3, NoStatementZero>
{
public:
   enum AltitudeFunction
   {
      cNO_STATEMENT,
      cARRIVE_AT_SPECIFIED_ALTITUDE,
      cDEPART_AT_SPECIFIED_ALTITUDE,
      cHOT_BELOW_THIS_ALTITUDE = 6,
      cHOT_ABOVE_THIS_ALTITUDE
   };
   DEFINE_ACCESSORS(AltitudeAmplificationField, AltitudeFunction);
};

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(12, 3, 0);

   typedef SpareField<5>              FieldType1;
   typedef LatitudeField<23>          FieldType2;
   typedef LongitudeField<24>         FieldType3;
   typedef AltitudeField<13>          FieldType4;
   typedef AltitudeAmplificationField FieldType5;
   DEFINE_MEMBERS5(Spare, Latitude, Longitude, Altitude, AltitudeFunction);
};


class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(12, 3);

   typedef TrackNumberAddresseeField          FieldType1;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType2;
   typedef MinuteField                        FieldType3;
   typedef HourField                          FieldType4;
   typedef TimeFunctionField                  FieldType5;
   typedef WaypointActionField                FieldType6;
   typedef IntegerField<4>                    FieldType7;
   typedef SpareField<16>                     FieldType8;
   DEFINE_MEMBERS8(TrackNumber, SequenceNumber, Minute, Hour, TimeFunction, WaypointAction, TotalPoints, Spare);

   DEFINE_CONTINUATIONS0()
   DEFINE_EXTENSIONS1()

protected:
};

} // namespace J12_3
} // namespace Messages
} // namespace WsfL16

#endif
