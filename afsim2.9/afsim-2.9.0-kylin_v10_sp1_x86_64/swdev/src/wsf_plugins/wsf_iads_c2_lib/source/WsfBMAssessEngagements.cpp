// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include "WsfBMAssessEngagements.hpp"
#include "WsfBMWeaponTable.hpp"

WsfBMAssessEngagements::WsfBMAssessEngagements()
   : WsfBMUnmanagedTypeWrapper<il::assessEngagementsWSPT>("WsfBMAssessEngagements")
{
}

WsfBMAssessEngagements::WsfBMAssessEngagements(il::assessEngagementsWSPT* data_to_wrap)
   : WsfBMUnmanagedTypeWrapper<il::assessEngagementsWSPT>("WsfBMAssessEngagements", data_to_wrap)
{
}

bool WsfBMAssessEngagements::ProcessInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "assess_engagements")
   {
      UtInputBlock block(aInput, "end_assess_engagements");

      while (block.ReadCommand(command) && processed)
      {
         if (command == "report_settings")
         {
            processed = ProcessReportInput(aInput, "end_" + command);
         }
         else if (command == "allocate_to")
         {
            processed = ProcessAllocationInput(aInput, "end_" + command);
         }
         else if (command == "threat_level_threshold")
         {
            double threshold = -1;
            block.GetInput().ReadValue(threshold);
            GetRawPtr()->SetThreatLevelThreshold(threshold);
         }
         else
         {
            WsfBMWeaponTable wt(&GetRawPtr()->GetWeaponTable());
            processed = wt.ProcessInput(aInput, false, true);
         }
      }
   }
   else // unknown block
   {
      processed = false;
   }

   return processed;
}

bool WsfBMAssessEngagements::ProcessReportInput(UtInput& aInput, const std::string& end_block_str)
{
   bool processed = true;
   UtInputBlock block(aInput, end_block_str);

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      if (command == "zone_allocation")
      {
         bool value = false;
         block.GetInput().ReadValue(value);
         GetRawPtr()->SetReportZoneAllocation(value);
      }
      else if (command == "go_summary")
      {
         bool value = false;
         block.GetInput().ReadValue(value);
         GetRawPtr()->SetReportGoSummary(value);
      }
      else if (command == "go_details")
      {
         bool value = false;
         block.GetInput().ReadValue(value);
         GetRawPtr()->SetReportGoDetails(value);
      }
      else if (command == "no_go_summary")
      {
         bool value = false;
         block.GetInput().ReadValue(value);
         GetRawPtr()->SetReportNoGoSummary(value);
      }
      else // unknown block
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMAssessEngagements::ProcessAllocationInput(UtInput& aInput, const std::string& end_block_str)
{
   bool processed = true;
   UtInputBlock block(aInput, end_block_str);

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      if (command == "MEZ")
      {
         bool value = false;
         block.GetInput().ReadValue(value);
         GetRawPtr()->SetZoneAllocationMEZ(value);
      }
      else if (command == "FEZ")
      {
         bool value = false;
         block.GetInput().ReadValue(value);
         GetRawPtr()->SetZoneAllocationFEZ(value);
      }
      else if (command == "JEZ")
      {
         bool value = false;
         block.GetInput().ReadValue(value);
         GetRawPtr()->SetZoneAllocationJEZ(value);
      }
      else // unknown block
      {
         processed = false;
      }
   }

   return processed;
}
