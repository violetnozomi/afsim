// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfAltLocEventResults.hpp"

#include "UtLatPos.hpp"
#include "UtLength.hpp"
#include "UtLonPos.hpp"
#include "WsfAltLocSimulationExtension.hpp"
#include "WsfEventUtils.hpp"
#include "WsfSimulation.hpp"

void wsf::altloc::ComponentInitialized::Print(std::ostream& aStream) const
{
   std::string drawType = GetDrawType();

   // 1. Print Index and draw type
   wsf::event::utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());

   aStream << "ALTERNATE_LOCATIONS_INITIALIZED" << ' ' << mPlatformPtr->GetName() << " Type: " << drawType;
   if (mComponentPtr->mBlockType == altloc::BlockType::LOCAL)
   {
      aStream << ": " << mComponentPtr->mDraw << " Sum-Of-Weights: " << mComponentPtr->mWeightsSum;
   }
   if (mComponentPtr->mBlockType != altloc::BlockType::OFF_ARA && mComponentPtr->mBlockType != altloc::BlockType::OFF_LLA)
   {
      aStream << " Location-Index: " << mComponentPtr->mLocationIndex + 1;
   }
   aStream << ' ' << wsf::event::utils::ContinueChar(mSettings.PrintSingleLinePerEvent());

   if (mComponentPtr->mReferenceComponent)
   {
      WsfPlatform* referencePlatform = mComponentPtr->mReferenceComponent->GetComponentParent();

      aStream << " Reference: " << referencePlatform->GetName();
      if (mComponentPtr->mBlockType == altloc::BlockType::OFF_ARA || mComponentPtr->mBlockType == altloc::BlockType::OFF_LLA)
      {
         aStream << " Location-" << mComponentPtr->mLocationIndex + 1;
      }
      wsf::event::utils::PrintLocationData(aStream, referencePlatform, mSettings);
      aStream << ' ' << wsf::event::utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   }

   // 2. Print offset, if applicable
   if (mComponentPtr->mBlockType == altloc::BlockType::OFF_LLA)
   {
      aStream << " Offset LLA: " << UtLatPos(mComponentPtr->mOffset.LLA.lat) << ' '
              << UtLonPos(mComponentPtr->mOffset.LLA.lon) << ' ' << UtLength(mComponentPtr->mOffset.LLA.alt);
      if (mComponentPtr->mOffset.LLA.hedSet)
      {
         aStream << " Heading: " << mComponentPtr->mOffset.LLA.hed * UtMath::cDEG_PER_RAD
                 << " deg"; // heading is absolute, not relative
      }
      aStream << ' ' << wsf::event::utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   }
   else if (mComponentPtr->mBlockType == altloc::BlockType::OFF_ARA)
   {
      aStream << " Offset ARA Angle: " << mComponentPtr->mOffset.ARA.angle * UtMath::cDEG_PER_RAD
              << " deg Radius: " << UtLength(mComponentPtr->mOffset.ARA.radius);
      if (mComponentPtr->mOffset.ARA.altSet)
      {
         aStream << " Altitude: " << UtLength(mComponentPtr->mOffset.ARA.alt);
      }
      if (mComponentPtr->mOffset.ARA.hedSet)
      {
         aStream << " Heading: " << mComponentPtr->mOffset.ARA.hed * UtMath::cDEG_PER_RAD
                 << " deg"; // heading is absolute, not relative
      }
      aStream << ' ' << wsf::event::utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   }

   // 3. In every case, finish with actual location
   wsf::event::utils::PrintLocationData(aStream, mPlatformPtr, mSettings);
   double yaw, pitch, roll;
   mPlatformPtr->GetOrientationNED(yaw, pitch, roll);
   aStream << " Heading: " << yaw * UtMath::cDEG_PER_RAD << " deg "
           << wsf::event::utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
}

// The CSV Event Output for this function is as follows:
// 1. SimTime
// 2. Event Name
// 3. Platform Name
// 4. Random Draw Type
// 5. Component Random Number Draw
// 6. Component Weighted Sum
// 7. Location Index
// 8. Reference Platform Name
// 9. Reference Platform Index
// 10. Reference Plat LLA
// 11. Reference Plat LLA
// 12. Reference Plat LLA
// 13. Reference Plat ECI
// 14. Reference Plat ECI
// 15. Reference Plat ECI
// 16. Offset LLA
// 17. Offset LLA
// 18. Offset LLA
// 19. Offset LLA Heading
// 20. Offset ARA
// 21. Offset ARA
// 22. Offset ARA
// 23. Offset ARA Heading
// 24. Platform LLA
// 25. Platform LLA
// 26. Platform LLA
// 27. Platform ECI
// 28. Platform ECI
// 29. Platform ECI
// 30. Platform Heading
void wsf::altloc::ComponentInitialized::PrintCSV(std::ostream& aStream) const
{
   // Check draw type: Global, Referenced, or Local
   std::string drawType = GetDrawType();

   // 1. Print Index and draw type
   wsf::event::utilsCSV::PrintTime(aStream, mSimTime);

   aStream << ',' << "ALTERNATE_LOCATIONS_INITIALIZED";
   aStream << ',' << mPlatformPtr->GetName();
   aStream << ',' << drawType;

   if (mComponentPtr->mBlockType == altloc::BlockType::LOCAL)
   {
      aStream << "," << mComponentPtr->mDraw << "," << mComponentPtr->mWeightsSum;
   }
   else
   {
      aStream << ",,";
   }

   if (mComponentPtr->mBlockType != altloc::BlockType::OFF_ARA && mComponentPtr->mBlockType != altloc::BlockType::OFF_LLA)
   {
      aStream << "," << mComponentPtr->mLocationIndex + 1;
   }
   else
   {
      aStream << ',';
   }

   if (mComponentPtr->mReferenceComponent)
   {
      WsfPlatform* referencePlatform = mComponentPtr->mReferenceComponent->GetComponentParent();

      aStream << "," << referencePlatform->GetName();
      if (mComponentPtr->mBlockType == altloc::BlockType::OFF_ARA || mComponentPtr->mBlockType == altloc::BlockType::OFF_LLA)
      {
         aStream << "," << mComponentPtr->mLocationIndex + 1;
      }
      else
      {
         aStream << ",";
      }
      wsf::event::utilsCSV::PrintLocationData(aStream, referencePlatform);
   }
   else
   {
      aStream << ",,,,,,,,";
   }

   // 2. Print offset, if applicable
   if (mComponentPtr->mBlockType == altloc::BlockType::OFF_LLA)
   {
      aStream << "," << UtLatPos(mComponentPtr->mOffset.LLA.lat) << "," << UtLonPos(mComponentPtr->mOffset.LLA.lon)
              << "," << UtLength(mComponentPtr->mOffset.LLA.alt);
      if (mComponentPtr->mOffset.LLA.hedSet)
      {
         aStream << "," << mComponentPtr->mOffset.LLA.hed * UtMath::cDEG_PER_RAD; // heading is absolute, not relative
      }
      else
      {
         aStream << ",";
      }
   }
   else
   {
      aStream << ",,,,";
   }

   if (mComponentPtr->mBlockType == altloc::BlockType::OFF_ARA)
   {
      aStream << "," << mComponentPtr->mOffset.ARA.angle * UtMath::cDEG_PER_RAD << ","
              << UtLength(mComponentPtr->mOffset.ARA.radius);
      if (mComponentPtr->mOffset.ARA.altSet)
      {
         aStream << "," << UtLength(mComponentPtr->mOffset.ARA.alt);
      }
      else
      {
         aStream << ",";
      }

      if (mComponentPtr->mOffset.ARA.hedSet)
      {
         aStream << "," << mComponentPtr->mOffset.ARA.hed * UtMath::cDEG_PER_RAD; // heading is absolute, not relative
      }
      else
      {
         aStream << ",";
      }
   }
   else
   {
      aStream << ",,,,";
   }

   // 3. In every case, finish with actual location
   wsf::event::utilsCSV::PrintLocationData(aStream, mPlatformPtr);
   double yaw, pitch, roll;
   mPlatformPtr->GetOrientationNED(yaw, pitch, roll);

   aStream << ',' << yaw * UtMath::cDEG_PER_RAD << '\n';
}

std::string wsf::altloc::ComponentInitialized::GetDrawType() const
{
   std::string drawType;
   switch (mComponentPtr->mBlockType)
   {
   case altloc::BlockType::LOCAL:
   {
      const auto& extension = SimulationExtension::Get(*mPlatformPtr->GetSimulation());
      if (extension.GetGlobalDrawEnabled())
      {
         drawType = "Global Weighted Draw";
      }
      else
      {
         drawType = "Local Weighted Draw";
      }
      break;
   }
   case altloc::BlockType::REF_LOC:
      drawType = "Reference";
      break;
   case altloc::BlockType::OFF_ARA:
   case altloc::BlockType::OFF_LLA:
      drawType = "Offset Location";
      break;
   case altloc::BlockType::NONE:
      drawType = "Unrecognized Draw Type";
      break;
   }

   return drawType;
}
