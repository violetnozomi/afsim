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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFBAYESCLASSIFICATION_HPP
#define WSFBAYESCLASSIFICATION_HPP

#include "wsf_mil_export.h"

#include <map>
#include <string>

#include "WsfClassificationProcessor.hpp"
#include "WsfStringId.hpp"
class WsfTrackClassifier;
#include "WsfTrackListFwd.hpp"

/** These structures are used for parsing learning data,
 *  and the subsequent insertion into the prototype.
 */
struct WSF_MIL_EXPORT ColumnPair
{
   std::string mFirst;
   std::string mSecond;
};

struct WSF_MIL_EXPORT AttributePair // std tuple
{
   double mFirst;
   double mSecond;
};

struct WSF_MIL_EXPORT WsfBayesAttribute
{
   // The name should point to the return type of the accessor.
   const char* mName;
   double      mValue;
   // A;; was working without sigma, until we added the correlator
   double mSigma;
};

//-----------------------------------------------------------------
// WsfAttributeMeasure
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfAttributeMeasure
{
public:
   WsfAttributeMeasure()
      : mAverage(0.0)
      , mVariance(0.0)
   {
   }
   virtual ~WsfAttributeMeasure() = default;

   /** Elliptical distance.  A "point" on the perimeter == 1.0.  The
    * peak occurs at the centroid, where aValue = mAverage **/
   virtual double GetEllipticalGate(double aValue, double aChiSqPointMult);

   /** Returns the standard BC statistical distance
    * or probability density function (PDF) value,
    * of the given data point given the set's variance
    * and average.
    */
   virtual double GetProbabilityDistribution(double aValue);

   /** Data range from CSV, data inputs.  Used to create sigma.*/
   void         SetRange(const AttributePair& aAttributeValuePair);
   void         SetSigma(const AttributePair& aAttributeValuePair);
   virtual void AssertValid();

protected:
   friend class WsfBayesClassifier;
   double mAverage;
   double mVariance;
};

//-----------------------------------------------------------------
// WsfCompoundAttributeMeasure
//-----------------------------------------------------------------
//! Compound refers to multiple spreads for the same
//! cluster, like freq 10-20 &| 60-90
class WSF_MIL_EXPORT WsfCompoundAttributeMeasure : public WsfAttributeMeasure
{
public:
   WsfCompoundAttributeMeasure();
   ~WsfCompoundAttributeMeasure() override;
   double GetEllipticalGate(double aValue, double aChiSqPointMult) override;

   double GetProbabilityDistribution(double aValue) override;

   // Creates and returns a new attribute for a new mode.
   WsfAttributeMeasure* AddMode();

protected:
   friend class WsfBayesClassifier;
   std::vector<WsfAttributeMeasure*> mMultiModes;
};

//-----------------------------------------------------------------
// WsfAttributeAccessor
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfAttributeAccessor
{
public:
   WsfAttributeAccessor()                                                                  = default;
   virtual ~WsfAttributeAccessor()                                                         = default;
   virtual const char* GetName() const                                                     = 0;
   virtual bool        HasAttribute(const WsfTrack* aTrack)                                = 0;
   virtual bool        GetAttribute(const WsfTrack* aTrack, WsfBayesAttribute& aAttribute) = 0;
};

//-----------------------------------------------------------------
// WsfSignalAttributeAccessor
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfSignalAttributeAccessor
{
public:
   WsfSignalAttributeAccessor()                                                                     = default;
   virtual ~WsfSignalAttributeAccessor()                                                            = default;
   virtual const char* GetName() const                                                              = 0;
   virtual bool        HasAttribute(const WsfTrack::Signal& aSignal)                                = 0;
   virtual bool        GetAttribute(const WsfTrack::Signal& aSignal, WsfBayesAttribute& aAttribute) = 0;
};

//-----------------------------------------------------------------
// WsfSignalAccessor
//-----------------------------------------------------------------
//! Like the Tracker and used by a Tracker, it parses out all valid signal attributes
class WSF_MIL_EXPORT WsfSignalAccessor
{
public:
   WsfSignalAccessor();
   virtual ~WsfSignalAccessor() = default;
   virtual void SetTrack(const WsfTrack& aTrack, std::vector<WsfBayesAttribute>& aAttributeValueList);

protected:
   std::vector<WsfSignalAttributeAccessor*> mSignalAccessorsMaster;
};

//-----------------------------------------------------------------
// WsfTrackAccessorDeep
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfTrackAccessorDeep : public TrackAccessor
{
public:
   WsfTrackAccessorDeep();
   ~WsfTrackAccessorDeep() override;
   virtual void SetTrack(const WsfTrack& aTrack);
   size_t       GetAttributeCount() { return mAttributeValueList.size(); }
   size_t       GetAttributeCount() const { return mAttributeValueList.size(); }

   /** Polled by "each" entity proto **/
   void GetAttribute(size_t i, WsfBayesAttribute& aAttribute) const { aAttribute = mAttributeValueList[i]; }

protected:
   WsfSignalAccessor                  mSignalAccessor;
   std::vector<WsfAttributeAccessor*> mAttributeAccessorMaster;
   std::vector<WsfBayesAttribute>     mAttributeValueList;
};

//-----------------------------------------------------------------
// WsfEntityPrototype
//-----------------------------------------------------------------
//! A prototype is the expected value set of attributes for an entity type.
class WSF_MIL_EXPORT WsfEntityPrototype
{
public:
   WsfEntityPrototype();
   virtual ~WsfEntityPrototype() = default;
   std::string GetTypeName() { return mTypeName; }
   void        AddAttributeProtoRange(const std::string& aAttName, const AttributePair& aAttributeValuePair);

   void AddAttributeProtoSigma(const std::string& aAttName, const AttributePair& aAttributeValuePair);

   virtual size_t GetCorrelationCount(const TrackAccessor& aTrackAccessor);
   virtual double GetEllipticalGate95Percent(const TrackAccessor& aTrackAccessor);
   virtual double GetEllipticalGate(const TrackAccessor& aTrackAccessor, double aChiSqPointMult = 1.0);

   virtual double GetProbabilityDistribution(const TrackAccessor& aTrackAccessor);

public:
   std::string                                         mTypeName;
   WsfStringId                                         mTypeNameID;
   std::map<std::string, WsfCompoundAttributeMeasure*> mName2Attribase;
   bool                                                mDebug;

private:
   WsfEntityPrototype(const WsfEntityPrototype& aSrc) = delete;
   WsfEntityPrototype&  operator=(const WsfEntityPrototype& aSrc) = delete;
   WsfAttributeMeasure* CreateOrFindAttributeProto(const std::string& aAttName);
};

//-----------------------------------------------------------------
// WsfBayesClassification
//-----------------------------------------------------------------
//! A track classifier that is a Bayesian Classifier, which
//! is not the same thing as a Bayesian (network) engine.  It
//! uses configured data sources to determine probabilities of
//! being entity types
//!    This class creates a instantiation, which protects all
//! the utility classes it uses.
//!
class WSF_MIL_EXPORT WsfBayesClassification
{
public:
   static WsfTrackClassifier* CreateClassifier();
   static WsfTrackClassifier* CreateClassifier(WsfLocalTrackList& aTrackList);
};

//-----------------------------------------------------------------
// WsfAccessorSpeed
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfAccessorSpeed : public WsfAttributeAccessor
{
public:
   WsfAccessorSpeed()           = default;
   ~WsfAccessorSpeed() override = default;

   const char* GetName() const override { return "speed"; }
   bool        HasAttribute(const WsfTrack* aTrack) override { return aTrack->VelocityValid(); }
   bool        GetAttribute(const WsfTrack* aTrack, WsfBayesAttribute& aAttribute) override;
};

//-----------------------------------------------------------------
// WsfAccessorAltitude
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfAccessorAltitude : public WsfAttributeAccessor
{
public:
   WsfAccessorAltitude()           = default;
   ~WsfAccessorAltitude() override = default;

   const char* GetName() const override { return "alt"; }
   bool        HasAttribute(const WsfTrack* aTrack) override { return aTrack->LocationValid(); }
   bool        GetAttribute(const WsfTrack* aTrack, WsfBayesAttribute& aAttribute) override;
};

//-----------------------------------------------------------------
// WsfAccessorIFF
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfAccessorIFF : public WsfAttributeAccessor
{
public:
   WsfAccessorIFF()           = default;
   ~WsfAccessorIFF() override = default;

   const char* GetName() const override { return "iff"; }
   bool        HasAttribute(const WsfTrack* aTrack) override;
   bool        GetAttribute(const WsfTrack* aTrack, WsfBayesAttribute& aAttribute) override;
};

//-----------------------------------------------------------------
// WsfAccessorPRI
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfAccessorPRI : public WsfSignalAttributeAccessor
{
public:
   WsfAccessorPRI()           = default;
   ~WsfAccessorPRI() override = default;

   const char* GetName() const override { return "pri"; }
   bool HasAttribute(const WsfTrack::Signal& aSignal) override { return (aSignal.mPulseRepetitionInterval != 0.0); }

   bool GetAttribute(const WsfTrack::Signal& aSignal, WsfBayesAttribute& aAttribute) override;
};

//-----------------------------------------------------------------
// WsfAccessorFreq
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfAccessorFreq : public WsfSignalAttributeAccessor
{
public:
   WsfAccessorFreq()           = default;
   ~WsfAccessorFreq() override = default;

   const char* GetName() const override { return "freq"; }
   bool        HasAttribute(const WsfTrack::Signal& aSignal) override
   {
      return ((aSignal.mUpperFrequency != 0.0) || (aSignal.mLowerFrequency != 0.0));
   }

   bool GetAttribute(const WsfTrack::Signal& aSignal, WsfBayesAttribute& aAttribute) override;
};

//-----------------------------------------------------------------
// WsfAccessorPW
//-----------------------------------------------------------------
class WSF_MIL_EXPORT WsfAccessorPW : public WsfSignalAttributeAccessor
{
public:
   WsfAccessorPW()           = default;
   ~WsfAccessorPW() override = default;

   const char* GetName() const override { return "pw"; }
   bool        HasAttribute(const WsfTrack::Signal& aSignal) override { return aSignal.mPulseWidth != 0.0; }

   bool GetAttribute(const WsfTrack::Signal& aSignal, WsfBayesAttribute& aAttribute) override;
};

#endif
