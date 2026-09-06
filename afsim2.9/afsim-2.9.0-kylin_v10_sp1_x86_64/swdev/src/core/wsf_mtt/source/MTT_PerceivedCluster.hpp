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

#ifndef MTT_PERCEIVED_CLUSTER_HPP
#define MTT_PERCEIVED_CLUSTER_HPP

#include <map>

#include "MTT_NonlocalTrack.hpp"
#include "WsfStringId.hpp"
class MTT_Measurement;
class SupBlock;

class WsfTrack;
class WsfMTT_Interface;

//! This is a representation of a Suppressor perceived cluster (block 17).
class MTT_PerceivedCluster
{
public:
   MTT_PerceivedCluster();

   virtual ~MTT_PerceivedCluster();

   void SetCombinedTrack(const MTT_CombinedTrack& aCombinedTrack);

   void SetMeasurement(const MTT_Measurement& aMeasurement);

   void CopyFrom(double aSimTime, WsfMTT_Interface* aMTT_InterfacePtr, const WsfTrack& aTrack);

   void ConvertFrom(const SupBlock& aBlock);
   // Suppressor equivalent offset
   MTT_CombinedTrack* mCombinedTrackPtr; // 37; for t2t
   // MTT_InteractionKey       mIndirectSource;    // 3+2 (21 case 1 + 2)
   MTT_Measurement* mMeasurementPtr; // 39; for m2t
   int              mTrackNumber;    // 41 (value==0 if measurement)

   // The following are stored here for convenience; they are not part of the suppressor block
   WsfStringId mLastOriginatorId;            // measurements only
   WsfStringId mLastSensorId;                // measurements only
   int         mConsecutiveSingleSourceHits; // measurements only

   // static std::map<int, MTT_CombinedTrack*> mTrackIdToCombinedTrackMap;
};

#endif
