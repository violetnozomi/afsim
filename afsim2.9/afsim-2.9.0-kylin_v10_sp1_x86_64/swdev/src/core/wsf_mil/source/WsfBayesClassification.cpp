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

#include "WsfBayesClassification.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtStringUtil.hpp"
#include "UtWallClock.hpp"
#include "WsfBayesClassifier.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackClassifier.hpp"
#include "WsfTrackList.hpp"

namespace
{
void AssertLocalBayesClassification(bool aTruth, const char* aMsg)
{
   if (!aTruth)
   {
      ut::log::error() << "AssertLocalBayesClassification: " << aMsg;
      assert(aTruth);
   }
}

// std::string GetTypeName(const ClassificationReport* aReport)
//{
//    return aReport->mEntityTypeId.GetString();
// }

/** See CHI Squared tables.  Multipliers to expand hyper ellipsoids
 * to make 95% in many dimensions.  Each element is the next higher dimension.
 */
double CHI_SQU_SQU_95[] = {1.959846933, // 1D
                           2.447651936, // 2D
                           2.79553215,  // 3D
                           3.080259729, //...
                           3.327160952, 3.548520819, 3.750599952, 3.937892838, 4.113271204, 4.278668017, 4.435651023,
                           4.58541165,  4.72884764,  4.866723744, 4.999599984, 5.127962558, 5.252332815, 5.372987996,
                           5.49035518,  5.604462508, 5.715855142, 5.824431303, 5.930598621, 6.034484236, 6.136122554,
                           6.235783832, 6.333482454, 6.429385663, 6.523572641, 6.616116686};

size_t CHI_SQU_SQU_95_SIZE = sizeof(CHI_SQU_SQU_95) / sizeof(double);

// const char* ToString(WsfTrack::IFF_Status stat)
//{
//    switch (stat)
//    {
//       case WsfTrack::cIFF_UNKNOWN:
//          return "cIFF_UNKNOWN";
//          break;
//       case WsfTrack::cIFF_AMBIGUOUS:
//          return "cIFF_AMBIGUOUS";
//          break;
//       case WsfTrack::cIFF_FOE:
//          return "cIFF_FOE";
//          break;
//       case WsfTrack::cIFF_FRIEND:
//          return "cIFF_FRIEND";
//          break;
//       case WsfTrack::cIFF_NEUTRAL:
//          return "cIFF_NEUTRAL";
//          break;
//       default:
//          return "???";
//    }
// }
} // namespace

//-----------------------------------------------------------------
// WsfAttributeMeasure
//-----------------------------------------------------------------

/** Elliptical distance.  A "point" on the perimeter == 1.0.  The
 * peak occurs at the centroid, where aValue = mAverage **/
// virtual

// KWW-distance as stated. Peak is misleading, as aValue = mAverage
// results in xd = 0, which results in a return of 0. Peak is, I
// believe, referring to a visualization of clustering around a
//"peak". Values returned from this method decrease toward the
// center of the cluster, near the average value.
double WsfAttributeMeasure::GetEllipticalGate(double aValue, double aChiSqPointMult)
{
   double xd = aValue - mAverage;
   return ((xd * xd) / (mVariance * aChiSqPointMult));
}

// KWW-This is the typical parameter estimation portion of
// a naive Bayes implementation. There are various forms
// depending on the data being analyzed. This one is
// Gaussian (normal) distribution. Therefore, it is assumed
// that the data is continuous. The return value is the result
// of plugging an OBSERVED data point into an existing probability
// density function to help determine the validity of this data
// point fitting into this pdf model.

// EXAMPLE:Altitude data points of a specific aircraft type are
// provided apriori (this is the .csv input file). This is done for
// every type possible. Therefore, every type will have its own
// data set which may or may not be unique. A pdf can be constructed
// for each of these types. By running an observed data point
// through a pdf, a value will result which indicates the relative
// weight of being an observation related to that type. This
// function below performs this calculation given the existing data
// and a single observation.

/** Returns the standard BC statistical distance
 * or probability density function (PDF) value,
 * of the given data point given the set's variance
 * and average.
 */
// virtual
double WsfAttributeMeasure::GetProbabilityDistribution(double aValue)
{
   double v   = aValue;
   double var = mVariance;
   double u   = mAverage;
   double p   = exp(-(v - u) * (v - u) / (2 * var));
   double den = sqrt(2 * UtMath::cPI * var);
   p          = p / den;
   return p;
}

// From a pair of values, calculates mean and variance.
// Stored in class members mAverage and mVariance.
void WsfAttributeMeasure::SetRange(const AttributePair& aAttributeValuePair)
{
   // Q: How do you calculate sigma from a range?
   // A: Of course you have CI 68%, 95%, ect.  But in general
   // sigma ~= semi-axis.  If you pass in the edge, and you would
   // like it to give an elliptical score of 1.0, then:
   // siga = ave - range/2 & var = sig^2
   mAverage = (aAttributeValuePair.mFirst + aAttributeValuePair.mSecond) / 2.0;
   // Check ;}  This is Excel stdevP
   mVariance = std::abs(mAverage - aAttributeValuePair.mFirst);
   mVariance *= mVariance;
   AssertValid();
}

// Takes a pair of values. Sets first value to class members mAverage. Squares
// the second and stores as mVariance(???).

// Unsure of purpose and/or intent of this method.
void WsfAttributeMeasure::SetSigma(const AttributePair& aAttributeValuePair)
{
   mAverage  = aAttributeValuePair.mFirst;
   mVariance = aAttributeValuePair.mSecond;
   // The user enters sigma.  We could store sigma vs var?
   mVariance *= mVariance;
   AssertValid();
}

// virtual
void WsfAttributeMeasure::AssertValid()
{
   AssertLocalBayesClassification(mVariance > 0.0, "variance is not positive as expected");
}

//-----------------------------------------------------------------
// WsfCompoundAttributeMeasure
//-----------------------------------------------------------------
WsfCompoundAttributeMeasure::WsfCompoundAttributeMeasure()
   : WsfAttributeMeasure()
   , mMultiModes()
{
}

// virtual
WsfCompoundAttributeMeasure::~WsfCompoundAttributeMeasure()
{
   for (auto& mMultiMode : mMultiModes)
   {
      delete mMultiMode;
   }
   mMultiModes.clear();
}

// Class member mMultiModes consists of a vector of WsfAttributeMeasures.
// The distance from center is found using GetEllipticalGate, and the
// lowest value of all the pairs in the vector is returned from this
// method (representing the closest point to the center).
// virtual
double WsfCompoundAttributeMeasure::GetEllipticalGate(double aValue, double aChiSqPointMult)
{
   // Max or min?  We can't invert (if that is desired) until
   // we have the sum.  The score is 1.0 is on the perimeter
   // and zero, or minimum at the middle/ave.
   // So we want the min.  Then the sum of mins,
   // is inverted to get the Gaussian, or max.
   // size_t iMin = 0;
   double edMin = DBL_MAX;
   for (auto& multiMode : mMultiModes)
   {
      double ed = multiMode->GetEllipticalGate(aValue, aChiSqPointMult);
      if (ed < edMin)
      {
         edMin = ed;
         // iMin = i;
      }
   }
   return edMin;
}


// virtual
double WsfCompoundAttributeMeasure::GetProbabilityDistribution(double aValue)
{
   // Max or min?  For BC, we want max.
   // size_t iMax = 0;
   double vMax = -DBL_MAX;
   for (auto& multiMode : mMultiModes)
   {
      double pd = multiMode->GetProbabilityDistribution(aValue);
      if (pd > vMax)
      {
         vMax = pd;
         // iMax = i;
      }
   }
   return vMax;
}

// Creates and returns a new attribute for a new mode.
WsfAttributeMeasure* WsfCompoundAttributeMeasure::AddMode()
{
   WsfAttributeMeasure* attMeas = new WsfAttributeMeasure();
   mMultiModes.push_back(attMeas);
   return attMeas;
}

//-----------------------------------------------------------------
// WsfSignalAccessor
//-----------------------------------------------------------------
WsfSignalAccessor::WsfSignalAccessor()
   : mSignalAccessorsMaster()
{
   if (mSignalAccessorsMaster.empty())
   {
      mSignalAccessorsMaster.push_back(new WsfAccessorPW);
      mSignalAccessorsMaster.push_back(new WsfAccessorFreq);
      mSignalAccessorsMaster.push_back(new WsfAccessorPRI);
   }
}

// virtual
// Sets/saves a track to a vector of tracks. Only one signal per
// track is allowed in the vector. For each entry in the vector
// of tracks, an attribute vector is populated with WsfBayesAttributes
void WsfSignalAccessor::SetTrack(const WsfTrack& aTrack, std::vector<WsfBayesAttribute>& aAttributeValueList)
{
   WsfTrack::Signal signal;
   unsigned int     ns = aTrack.GetSignalCount();
   if (ns > 1)
   {
      ns                        = 1;
      static bool warnedYouOnce = false;
      if (!warnedYouOnce)
      {
         warnedYouOnce = true;
         ut::log::error() << "BC-SignalAccessor: Only one 'fused' signal allowed per track.";
      }
   }

   for (unsigned int i = 0; i < ns; ++i)
   {
      // Get the single signal
      aTrack.GetSignalEntry(i, signal);
      for (WsfSignalAttributeAccessor* attAccessor : mSignalAccessorsMaster)
      {
         // Get any attributes for the signal: prf, pri, ...
         if (attAccessor->HasAttribute(signal))
         {
            WsfBayesAttribute a;
            a.mName = attAccessor->GetName();
            attAccessor->GetAttribute(signal, a);
            // Copy the name pointer, double value, and future-uture
            // the sigma.
            aAttributeValueList.push_back(a);
         }
      }
   }
}

//-----------------------------------------------------------------
// WsfTrackAccessorDeep
//-----------------------------------------------------------------
WsfTrackAccessorDeep::WsfTrackAccessorDeep()
   : TrackAccessor()
   , mSignalAccessor()
{
   if (mAttributeAccessorMaster.empty())
   {
      mAttributeAccessorMaster.push_back(new WsfAccessorSpeed);
      mAttributeAccessorMaster.push_back(new WsfAccessorAltitude);
      mAttributeAccessorMaster.push_back(new WsfAccessorIFF);
      // range_rate is measured, but it should only be correlated
      // on the same platform.
   }
}

// virtual
WsfTrackAccessorDeep::~WsfTrackAccessorDeep()
{
   for (auto& i : mAttributeAccessorMaster)
   {
      delete i;
   }
   mAttributeAccessorMaster.clear();
}


// virtual
// Sets the WsfAttributeAccessor vector with the appropriate
// platform attribute values. Sensor tracking is updated via
// call to WsfSignalAccessor::SetTrack
void WsfTrackAccessorDeep::SetTrack(const WsfTrack& aTrack)
{
   mTrack = &aTrack;

   // Remove the data.
   mAttributeValueList.clear();

   for (WsfAttributeAccessor* attributeAccessor : mAttributeAccessorMaster)
   {
      if (attributeAccessor->HasAttribute(&aTrack))
      {
         // mAttributeAccessor.push_back(mAttributeAccessorMaster[i]);
         WsfBayesAttribute a;
         attributeAccessor->GetAttribute(&aTrack, a);
         a.mName = attributeAccessor->GetName();
         // Copy the name pointer, double value, and future-uture
         // the sigma.
         mAttributeValueList.push_back(a);
      }
   }

   // Append all valid signal fields.  It could be pri from sigA,
   // freq from sigB ...
   mSignalAccessor.SetTrack(aTrack, mAttributeValueList);
}

//-----------------------------------------------------------------
// WsfEntityPrototype
//-----------------------------------------------------------------
WsfEntityPrototype::WsfEntityPrototype()
   : mTypeName()
   , mDebug(true)
{
}

// Ensure that a specific WsfCompoundAttributeMeasure exists in the
// class mapping (mName2Attribase). Add it if it doesn't exist.
// private
WsfAttributeMeasure* WsfEntityPrototype::CreateOrFindAttributeProto(const std::string& aAttName)
{
   /* Here is where we can change whether or not we use compound for
    * all (which we can) or if we 1st use a single, then grow a compound.
    */
   if (mName2Attribase[aAttName] == nullptr)
   {
      mName2Attribase[aAttName] = new WsfCompoundAttributeMeasure();
      // TODO: Shouldn't we be "adding" here, to ensure an add only if an entry
      // doesn't currently exist? Or does map ignore addition attempts to already
      // existing entries?
   }
   return mName2Attribase[aAttName]->AddMode();
}

// Find an entry if it exists (if not, create it), and set the range attribute.
void WsfEntityPrototype::AddAttributeProtoRange(const std::string& aAttName, const AttributePair& aAttributeValuePair)
{
   WsfAttributeMeasure* atpro = CreateOrFindAttributeProto(aAttName);
   atpro->SetRange(aAttributeValuePair);
}

// Find an entry if it exists (if not, create it), and set the sigma attribute.
void WsfEntityPrototype::AddAttributeProtoSigma(const std::string& aAttName, const AttributePair& aAttributeValuePair)
{
   WsfAttributeMeasure* atpro = CreateOrFindAttributeProto(aAttName);
   atpro->SetSigma(aAttributeValuePair);
}

// Count and returns how many of the tracks attributes are
// defined in this prototype. (E.g. How many attributes/variables
// are we tracking in order to determine a pdf used to classify
// an entity. Assertion to ensure we aren't using more than
// what are defined on the chi squared table (30), which is
// referred to the dimension of assumed independent variables
// used in naive bayes)
// virtual
size_t WsfEntityPrototype::GetCorrelationCount(const TrackAccessor& aTrackAccessor)
{
   const WsfTrackAccessorDeep* trackAccessorPtr = dynamic_cast<const WsfTrackAccessorDeep*>(&aTrackAccessor);
   AssertLocalBayesClassification(trackAccessorPtr != nullptr, "This is not the TrackAccessor type for this class.");
   // EntityPrototype* ep = dynamic_cast<EntityPrototype*>(aWsfBayesClassifier);
   size_t corrCnt = 0;
   // Q: What's the score is a track has no attributes? (inited)
   // A: ES: The track is the point, the proto is the gate.  So
   //  if there are no atts, its undefined.  And we will divide
   //  by zero at the return.
   WsfBayesAttribute attribute;
   // double sum = 0.0;
   for (size_t i = 0; i < trackAccessorPtr->GetAttributeCount(); ++i)
   {
      trackAccessorPtr->GetAttribute(i, attribute);
      if (mName2Attribase[attribute.mName] != nullptr)
      {
         ++corrCnt;
      }
   }
   AssertLocalBayesClassification(corrCnt < CHI_SQU_SQU_95_SIZE, "correlation count is not in expected range");
   return corrCnt;
}

// Same method as GetEllipticalGate below, except the distance returned is that
// required to belong to the 95th percentile of the associated return value of a
// data point injected into a pdf.
// virtual
double WsfEntityPrototype::GetEllipticalGate95Percent(const TrackAccessor& aTrackAccessor)
{
   size_t df = GetCorrelationCount(aTrackAccessor);
   // The 0th element is 1 deg.
   // To expand an ellipse, you use sqrt.  In contains, we need the square
   return GetEllipticalGate(aTrackAccessor, CHI_SQU_SQU_95[df - 1] * CHI_SQU_SQU_95[df - 1]);
}

/** Get the elliptical distance.  See elliptical gating.  The inputs
 * should be a TrackAccessor, since the same one is used for all protos.
 */
// virtual
double WsfEntityPrototype::GetEllipticalGate(const TrackAccessor& aTrackAccessor /*, XXX& aXXX*/,
                                             double               aChiSqPointMult /* = 1.0*/)
{
   const WsfTrackAccessorDeep* trackAccessorPtr = dynamic_cast<const WsfTrackAccessorDeep*>(&aTrackAccessor);
   AssertLocalBayesClassification(trackAccessorPtr != nullptr, "This is not the TrackAccessor type for this class.");

   // The example uses 0.5 female 0.5 male.  If the prototypes are
   // are all equal, it can be any scaler (except 0) because the
   // final score is normalized.  But if there is knowledge about
   // a distribution of the entity types, that would be the 1st multiplier.
   // For example, 80% in the AS are F18, 15% are C5, the scaler
   // would be 0.80 and 0.15.
   WsfBayesAttribute attribute;

   if (mDebug)
   {
      auto out = ut::log::debug() << "EntityPrototype: Elliptical-Gating Track:";
      out.AddNote() << "Type: " << GetTypeName();
      out.AddNote() << "True Type (2.0 good): " << aTrackAccessor.GetTrueType();
   }

   int corrCnt = 0;
   // Q: What's the score is a track has no attributes? (inited)
   // A: ES: The track is the point, the proto is the gate.  So
   //  if there are no atts, its undefined.  And we will divide
   //  by zero at the return.
   double sum = 0.0;
   for (size_t i = 0; i < trackAccessorPtr->GetAttributeCount(); ++i)
   {
      trackAccessorPtr->GetAttribute(i, attribute);
      WsfCompoundAttributeMeasure* atproc = mName2Attribase[attribute.mName];
      if (atproc != nullptr)
      {
         // TODO: We have the proto sigma, and the measure sigma.
         // The compound att, searches for the best mode (min)
         double ed = atproc->GetEllipticalGate(attribute.mValue, aChiSqPointMult);
         ////aXXX.PushAttributeScore(attribute.mName, ed);
         sum += ed;
         ++corrCnt;
         if (mDebug)
         {
            auto out = ut::log::debug() << "Track Attribute " << i << ":";
            out.AddNote() << "Attribute: " << attribute.mName;
            out.AddNote() << "Element Score (0.0 good): " << ed;
         }
      }
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "Entity Prototype Axillary Data:";
      out.AddNote() << "EntityPrototype: " << GetTypeName();
      out.AddNote() << "Track True Type: " << aTrackAccessor.GetTrueType();
      out.AddNote() << "# Attributes that Correlated: " << corrCnt;
      out.AddNote() << "Total Elliptical-Gate Score: " << (2.0 / (1.0 + sum)) << " / 2.0";
   }
   // A high score is good.  As the point approaches the centroid,
   // the sum minimizes and the inverse maximizes.
   ////////return sum == 0 ? 0.0: 1.0/sum;
   // New equation.  This will give 2 if on centroid, 1 if on perimeter, and
   // < 1.0 if outide (1 sigma), and 1< x < 2 if contains.
   return 2.0 / (1.0 + sum);
}

/** The standard un-normalizied BC. **/
// virtual
double WsfEntityPrototype::GetProbabilityDistribution(const TrackAccessor& aTrackAccessor)
{
   const WsfTrackAccessorDeep* trackAccessorPtr = dynamic_cast<const WsfTrackAccessorDeep*>(&aTrackAccessor);
   AssertLocalBayesClassification(trackAccessorPtr != nullptr, "This is not the TrackAccessor type for this class.");

   // The example uses 0.5 female 0.5 male.  If the prototypes are
   // are all equal, it can be any scaler (except 0) because the
   // final score is normalized.  But if there is knowledge about
   // a distribution of the entity types, that would be the 1st multiplier.
   // For example, 80% in the AS are F18, 15% are C5, the scaler
   // would be 0.80 and 0.15.
   WsfBayesAttribute attribute;

   // size_t corrCnt = 0;
   double prod = 1.0;
   for (size_t i = 0; i < trackAccessorPtr->GetAttributeCount(); ++i)
   {
      trackAccessorPtr->GetAttribute(i, attribute);
      WsfCompoundAttributeMeasure* atproc = mName2Attribase[attribute.mName];
      if (atproc != nullptr)
      {
         // TODO: We have the proto sigma, and the measure sigma.
         // The compound att, searches for the best mode (min)
         prod *= atproc->GetProbabilityDistribution(attribute.mValue);
         //++corrCnt;
      }
   }
   ////if (mDebug) { ut::log::debug() << "XXX: # attributes that correlated = " << corrCnt; }
   return prod;
}

//-----------------------------------------------------------------
// WsfBayesClassification
//-----------------------------------------------------------------
//! There can be multiple classifiers.
// static
WsfTrackClassifier* WsfBayesClassification::CreateClassifier()
{
   return new WsfBayesClassifier();
}

// static
WsfTrackClassifier* WsfBayesClassification::CreateClassifier(WsfLocalTrackList& aLocalTrackList)
{
   WsfBayesClassifier* bc = new WsfBayesClassifier();

   WsfTrackAccessorDeep* trackAccessor = new WsfTrackAccessorDeep();

   for (unsigned int trackNum = 0; trackNum < aLocalTrackList.GetTrackCount(); ++trackNum)
   {
      WsfLocalTrack* localTrackPtr = aLocalTrackList.GetTrackEntry(trackNum);

      trackAccessor->SetTrack(*localTrackPtr);
      // bc->AddPrototype(trackAccessor);

      if (trackAccessor->GetAttributeCount() < 1)
      {
         continue;
      }

      WsfEntityPrototype* ep = new WsfEntityPrototype;

      WsfBayesAttribute attribute;
      for (size_t i = 0; i < trackAccessor->GetAttributeCount(); ++i)
      {
         trackAccessor->GetAttribute(i, attribute);
         if (attribute.mSigma < 0)
         {
            // Kinematics and attributes with no sigma are not correlatable.
            ut::log::info() << "CreateTrackType: Attribute flagged as not 'correlatable'.";
            continue;
         }

         // Add it.
         /// TODO Merge Attribute and AttributePair
         AttributePair attributeValuePair;
         attributeValuePair.mFirst  = attribute.mValue;
         attributeValuePair.mSecond = attribute.mSigma;

         ////////////trackAccessor->GetAttributeSigma(i, attributeValuePair.mFirst);
         std::string temp(attribute.mName);
         ep->AddAttributeProtoSigma(temp, attributeValuePair);
      }

      if (true)
      {
         // std::stringstream ss;
         std::string entityType = "track_x";
         // We are using the track ID as the ID
         // int entityTypeId = localTrackPtr->GetTrackId().GetLocalTrackNumber();
         WsfStringId entityTypeId = WsfStringId::UnsafeFindStringId(localTrackPtr->GetTrackId().GetLocalTrackNumber());
         AssertLocalBayesClassification(bc->mEntType2Proto[entityTypeId] == nullptr,
                                        "This track id has already been processed!");
         ep->mTypeName                    = entityType;
         ep->mTypeNameID                  = entityTypeId;
         bc->mEntType2Proto[entityTypeId] = ep;
      }
   }

   // for (size_t i = 0; i < aAttributeNamePairs.size(); ++i)
   //{
   //    ColumnPair& columnNamePair = aAttributeNamePairs[i];
   //    if (columnNamePair.mFirst == columnNamePair.mSecond)
   //    {
   //       // The EntityProtype manages compounds or multi modals.
   //       ep->AddAttributeProtoRange(columnNamePair.mFirst, aAttributeValuePairs[i]);
   //    }
   //    else if (columnNamePair.mSecond == "sigma")
   //    {
   //       ep->AddAttributeProtoSigma(columnNamePair.mFirst, aAttributeValuePairs[i]);
   //    }
   // }
   delete trackAccessor;
   return bc;
}

//-----------------------------------------------------------------
// WsfAccessorSpeed
//-----------------------------------------------------------------
bool WsfAccessorSpeed::GetAttribute(const WsfTrack* aTrack, WsfBayesAttribute& aAttribute)
{
   double VelocityWCS[3];
   aTrack->GetVelocityWCS(VelocityWCS);
   // To be consistant, let the caller set the name
   //////aAttribute.mName = GetName();

   aAttribute.mValue = UtVec3d::Magnitude(VelocityWCS);

   // Arguably, any kinematic based field should
   // be gated using the full covariance matrix.  What if
   // we are using the AB filter (there is no P)?
   //////if (aTrack->GetStateCovariance())
   //////{
   //////   const WsfCovariance& covar = *aTrack->GetStateCovariance();
   //////   // Give an estimate of the "speed" sigma from the trace
   //////   ////See UtMeasurementUtil::GetQuantitativeMeasurementQuality
   //////   double rmsVelocitySigma = sqrt(covar(3, 3) + covar(4, 4) + covar(5, 5));
   //////
   //////   aAttribute.mSigma = rmsVelocitySigma;
   //////}
   aAttribute.mSigma = -1;
   return true;
}

//-----------------------------------------------------------------
// WsfAccessorAltitude
//-----------------------------------------------------------------
// virtual
bool WsfAccessorAltitude::GetAttribute(const WsfTrack* aTrack, WsfBayesAttribute& aAttribute)
{
   double locationLLA[3];
   aTrack->GetLocationLLA(locationLLA[0], locationLLA[1], locationLLA[2]);
   aAttribute.mValue = locationLLA[2];
   // An estimate would be the covariance Z if P is in the NED frame (and
   // the covariance isn't tilted).
   // Otherwise, we could 1) take the ellipsoid, and find the range of
   // tip points. 2) keep a separate 1D filter just for altitude.
   // Or mark is at not usable for track correlation
   aAttribute.mSigma = -1;
   return true;
}

//-----------------------------------------------------------------
// WsfAccessorIFF
//-----------------------------------------------------------------
// virtual
bool WsfAccessorIFF::HasAttribute(const WsfTrack* aTrack)
{
   // FIXME.  How do we know if IFF is reported? It defaults to cIFF_UNKNOWN
   //  but it could also get set to that.
   if (false)
   {
      // WsfTrack::IFF_Status stat = aTrack->GetIFF_Status();
      // const char* iffs = ToString(stat);
      return (aTrack->GetIFF_Status() != WsfTrack::/*IFF_Status::*/ cIFF_UNKNOWN);
   }
   else
   {
      return (aTrack->GetIFF_Status() != WsfTrack::cIFF_UNKNOWN); // TODO, check if valid or set somehow?
   }
}


// KWW-This doesn't appear to be implemented correctly. As noted,
// the current implementation has issues when a non-continuous
// variable is encountered, when previously assumed. There are
// methods to overcome this difficulty however, it just needs
// to be inserted below.

/** How do enumerations work?  A hostile proto will have
 * cIFF_AMBIGUOUS (1) and the track will have a value.  If the
 * track is 1, it scores high.  The problem will be inputting
 * prototypes; since the spread is discontinuous.
 */
// virtual
bool WsfAccessorIFF::GetAttribute(const WsfTrack* aTrack, WsfBayesAttribute& aAttribute)
{
   // FIXME: We need another subclass and some research
   // about discrete variables
   aAttribute.mValue = aTrack->GetIFF_Status();
   aAttribute.mSigma = -1;
   return true;
}

//-----------------------------------------------------------------
// WsfAccessorPRI
//-----------------------------------------------------------------
// virtual
bool WsfAccessorPRI::GetAttribute(const WsfTrack::Signal& aSignal, WsfBayesAttribute& aAttribute)
{
   aAttribute.mValue = aSignal.mPulseRepetitionInterval;
   // The signal attributes currently don't have sigmas from
   // the receiver.  Also the values are not currently blurred.
   // So for now we'll use +/- 5%.  The effect will be a
   // unblurred measurement will match the unblurred prototype
   // especially given a 5% window.  Most importantly, not
   // setting it to -1 will mean it will be used.
   aAttribute.mSigma = 0.05 * aAttribute.mValue;
   return true;
}

//-----------------------------------------------------------------
// WsfAccessorFreq
//-----------------------------------------------------------------
// virtual
bool WsfAccessorFreq::GetAttribute(const WsfTrack::Signal& aSignal, WsfBayesAttribute& aAttribute)
{
   aAttribute.mValue = (aSignal.mUpperFrequency + aSignal.mLowerFrequency) / 2.0;
   aAttribute.mSigma = 0.05 * aAttribute.mValue;
   return true;
}

//-----------------------------------------------------------------
// WsfAccessorPW
//-----------------------------------------------------------------
// This method appears to have the same issue as the method above
//(WsfAccessorPRI::GetAttribute()) with the lack of availability
// of a sigma. Method internals are the same.
// virtual
bool WsfAccessorPW::GetAttribute(const WsfTrack::Signal& aSignal, WsfBayesAttribute& aAttribute)
{
   aAttribute.mValue = aSignal.mPulseWidth;
   aAttribute.mSigma = 0.05 * aAttribute.mValue;
   return true;
}
