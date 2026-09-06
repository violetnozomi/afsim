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
#ifndef L16_MESSAGE3_0_HPP
#define L16_MESSAGE3_0_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_0
{

class Extension0 : public ExtensionBase
{
public:
   enum NuclearBurstType
   {
      cNO_STATEMENT,
      cUNKNOWN,
      cAIR,
      cSURFACE_GROUND,
      cUNDERGROUND,
      cSURFACE_WATER,
      cUNDERWATER,
      cSPACE
   };
   class NuclearBurstTypeField : public NormalField<3, NoStatementZero>
   {
   public:
      DEFINE_ACCESSORS(NuclearBurstTypeField, NuclearBurstType);
   };
   DEFINE_EXTENSION(3, 0, 0);

   typedef NuclearBurstTypeField              FieldType1;
   typedef LatitudeField<20>                  FieldType2;
   typedef IntegerFieldNS<2, NoStatementZero> FieldType3;
   typedef SpareField<1>                      FieldType4;
   typedef LongitudeField<21>                 FieldType5;
   typedef IntegerFieldNS<10, NoStatementMax> FieldType6;
   typedef IntegerFieldNS<10, NoStatementMax> FieldType7;
   typedef SpareField<1>                      FieldType8;
   DEFINE_MEMBERS8(BurstType, Latitude, EstimatedYield, Spare, Longitude, Altitude1, Altitude2, Spare2);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 0, 1);
   typedef DeltaLatLonField FieldType1;
   typedef DeltaLatLonField FieldType2;
   typedef BooleanField     FieldType3;
   typedef SpareField<2>    FieldType4;
   typedef DeltaLatLonField FieldType5;
   typedef DeltaLatLonField FieldType6;
   typedef BooleanField     FieldType7;
   typedef SpareField<3>    FieldType8;

   DEFINE_MEMBERS8(DeltaLat1, DeltaLon1, IsEndPoint1, Spare, DeltaLat2, DeltaLon2, IsEndPoint2, Spare2);
};

class Continuation2 : public ContinuationBase
{
public:
   class AreaTypeField : public NormalField<2, NoStatementZero>
   {
   public:
      enum AreaType
      {
         cNO_STATEMENT,
         cRECTANGULAR,
         cELLIPTICAL,
         cUNDEFINED
      };
      DEFINE_ACCESSORS(AreaTypeField, AreaType);
   };
   DEFINE_CONTINUATION(3, 0, 2);
   typedef SpeedField           FieldType1;
   typedef SpareField<1>        FieldType2;
   typedef TrackNumberField     FieldType3;
   typedef AngleField           FieldType4;
   typedef SpareField<1>        FieldType5;
   typedef AreaTypeField        FieldType6;
   typedef AxisOrientationField FieldType7;
   typedef AreaAxisLengthField  FieldType8;
   typedef AreaAxisLengthField  FieldType9;

   DEFINE_MEMBERS9(Speed, Spare, TrackNumber, Course, Spare2, AreaType, AxisOrientation, AreaMajorAxis, AreaMinorAxis);
};

class Continuation3 : public ContinuationBase
{
public:
   class SonobuoyTypeField : public NormalField<4, NoStatementZero>
   {
   public:
      enum SonobuoyTypeTypes
      {
         cST_NO_STATEMENT,
         cST_BT,
         cST_LOFAR,
         cST_RO,
         cST_DIFAR,
         cST_VLA,
         cST_CAMBS,
         cST_BARRA,
         cST_VLAD            = 10,
         cST_WIDE_BAND_LOFAR = 12,
         cST_DICASS,
         cST_ADAR,
         cST_NUAMP
      };
      DEFINE_ACCESSORS(SonobuoyTypeField, SonobuoyTypeTypes);
   };

   class SonobuoyPatternField : public NormalField<4, NoStatementZero>
   {
   public:
      enum SonobuoyPatternTypes
      {
         cSP_NO_STATEMENT,
         cSP_BARRIER,
         cSP_WEDGE,
         cSP_CIRCLE_NO_CENTER_BUOY,
         cSP_CIRCLE_CENTER_BUOY,
         cSP_BRUSHTAC,
         cSP_DISTRIBUTIVE
      };
      DEFINE_ACCESSORS(SonobuoyPatternField, SonobuoyPatternTypes);
   };

   class SonobuoySpacingField : public NormalField<9, NoStatementZero>
   {
   public:
      typedef double        accessor_type;
      SonobuoySpacingField& operator=(double aMeters)
      {
         mValue = (value_type)(0.9144 / 125.0 * aMeters);
         return *this;
      }
      operator double() { return mValue / (0.9144 / 125.0); }
   };
   class SonobuoyBearingField : public NormalField<9, NoStatementMax>
   {
   public:
      typedef double        accessor_type;
      SonobuoyBearingField& operator=(double aRadians)
      {
         mValue = (value_type)NumericConversion::NormalizeAngle0_360(NumericConversion::RadiansToDeg(aRadians));
         return *this;
      }
      operator double() { return NumericConversion::DegToRadians(mValue); }
   };

   DEFINE_CONTINUATION(3, 0, 3);
   typedef IntegerFieldNS<7, NoStatementZero> FieldType1;
   typedef IntegerFieldNS<2, NoStatementZero> FieldType2;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType3;
   typedef SonobuoyTypeField                  FieldType4;
   typedef SonobuoyPatternField               FieldType5;
   typedef IntegerFieldNS<5, NoStatementZero> FieldType6;
   typedef SonobuoySpacingField               FieldType7;
   typedef SonobuoySpacingField               FieldType8;
   typedef SonobuoySpacingField               FieldType9;
   typedef SonobuoyBearingField               FieldType10;
   typedef BooleanField                       FieldType11;

   DEFINE_MEMBERS11(ChannelNumber,
                    DepthIndicator,
                    DepthTransducer,
                    SonobuoyType,
                    SonobuoyPattern,
                    SonobuoyTypeNumber,
                    SonobuoySpacing,
                    SonobuoyRowSpacing,
                    SonobuoyPatternRadius,
                    Bearing,
                    HasContact);

   void SetDepth(double aMeters)
   {
      if (aMeters >= 300)
      {
         DepthIndicator  = 3;
         DepthTransducer = (int)aMeters / 300;
      }
      else if (aMeters >= 30)
      {
         DepthIndicator  = 2;
         DepthTransducer = (int)aMeters / 30;
      }
      else
      {
         DepthIndicator  = 1;
         DepthTransducer = std::max(1, (int)aMeters / 3);
      }
   }
};

class Continuation4 : public ContinuationBase
{
public:
   class SonobuoyTypeField : public NormalField<4, NoStatementZero>
   {
   public:
      enum SonobuoyTypeTypes
      {
         cNO_STATEMENT,
         cHIDAR,
         cSSQ110,
         cCAMBS6,
         cALFEA,
         cSSQ911,
         cSSQ981D
      };
      DEFINE_ACCESSORS(SonobuoyTypeField, SonobuoyTypeTypes);
   };
   DEFINE_CONTINUATION(3, 0, 4);
   typedef TrackNumberField  FieldType1;
   typedef SonobuoyTypeField FieldType2;
   typedef SpareField<20>    FieldType3;
   typedef SpareField<20>    FieldType4;
   DEFINE_MEMBERS4(TrackNumber, SonobuoyType, Spare, Spare2);
};

class Initial : public InitialBase
{
public:
   class PointLineDescriptorField : public FieldBase<2>
   {
   public:
      enum PointLineDescriptor
      {
         cPLD_POINT,
         cPLD_SINGLE_POINT_AREA,
         cPLD_LINE,
         cPLD_MULTI_POINT_AREA
      };
      DEFINE_ACCESSORS(PointLineDescriptorField, PointLineDescriptor);
   };
   class PointTypeField : public FieldBase<4>
   {
   public:
      enum PointType
      {
         cPT_HAZARD,
         cPT_REFERENCE_POINT,
         cPT_STATION,
         cPT_STATION_AIR,
         cPT_LINE,
         cPT_AREA,
         cPT_AREA_HAZARD,
         cPT_ASW,
         cPT_ASW_1
      };
      DEFINE_ACCESSORS(PointTypeField, PointType);
   };

   DEFINE_INITIAL(3, 0);

   typedef BooleanField                       FieldType1;
   typedef BooleanField                       FieldType2;
   typedef BooleanField                       FieldType3;
   typedef SpareField<1>                      FieldType4;
   typedef BooleanField                       FieldType5;
   typedef BooleanField                       FieldType6;
   typedef TrackNumberField                   FieldType7;
   typedef BooleanField                       FieldType8;
   typedef BooleanField                       FieldType9;
   typedef PeriodicReportField                FieldType10;
   typedef TimeReportField                    FieldType11;
   typedef PointLineDescriptorField           FieldType12;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType13;
   typedef SpareField<1>                      FieldType14;
   typedef PointTypeField                     FieldType15;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType16;
   typedef MinuteField                        FieldType17;
   typedef HourField                          FieldType18;
   DEFINE_MEMBERS18(IsExerciseTrackUnit,
                    IsResponse,
                    IsForceTell,
                    Spare,
                    IsSpecialProcessingRequired,
                    IsSimulated,
                    TrackNumber,
                    IsSlavedToUnit,
                    IsLineAreaContinuation,
                    ReportIndicator,
                    TimeReportFunction,
                    PointLineDescriptor,
                    Priority,
                    Spare1,
                    PointType,
                    PointTypeAmplification,
                    Minute,
                    Hour);

   DEFINE_CONTINUATIONS4(1, 2, 3, 4);
   DEFINE_EXTENSIONS1();
};

} // namespace J3_0
} // namespace Messages
} // namespace WsfL16


#endif
