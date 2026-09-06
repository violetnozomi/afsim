// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: weaponsManagerAI.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef IADSLIB_WEAPONS_MANAGER_AI
#define IADSLIB_WEAPONS_MANAGER_AI

namespace il
{
   class AssetManagerInterface;
}

#include <iadsLib/weaponsManagerInterface.hpp>
#include <iadsLib/positionRecord.hpp>

namespace il
{
   const double C_DEFAULT_UNSET_HOME_BASE_POSITION_ALTITUDE = -1000000;

   class IADSLIB_EXPORT weaponsManagerAI : public WeaponsManagerInterface
   {
         // Abstract interface
      public:
         weaponsManagerAI();
         virtual ~weaponsManagerAI() = default;
         virtual WeaponsManagerInterface* Clone();

         void setAlertTime(const double alert_time_s);
         double getAlertTime() const;

         void setAssessEngageVia(const il::eAssessEngage assess_engage_via);
         il::eAssessEngage getAssessEngageVia() const;

         void setEscort(const bool escort);
         bool getEscort() const;

         void setInterceptSpeed(const double speed_m_per_s);
         double getInterceptSpeed() const;

         void setPlaceInFormation(const unsigned int place_in_formation);
         unsigned int getPlaceInFormation();

         void setPursuitRange(const double range_m);
         double getPursuitRange() const;

         void setSalvoDelay(const double delay_s);
         double getSalvoDelay() const;

         void setEvasionTakeAction(const bool enable);
         double getEvasionTakeAction() const;

         void setEvasionTakeActionDistance(const double distance_m);
         double getEvasionTakeActionDistance() const;

         void setEvasionAction1(const il::eTakeAction action);
         il::eTakeAction getEvasionAction1() const;

         void setEvasionAction1Duration(const double duration_s);
         double getEvasionAction1Duration() const;

         void setEvasionAction2(const il::eTakeAction action);
         il::eTakeAction getEvasionAction2() const;

         void setEvasionAction2Duration(const double duration_s);
         double getEvasionAction2Duration() const;

         void setEvasionAction3(const il::eTakeAction action);
         il::eTakeAction getEvasionAction3() const;

         void setEvasionAction3Duration(const double duration_s);
         double getEvasionAction3Duration() const;

         void setEvasionAction4(const il::eTakeAction action);
         il::eTakeAction getEvasionAction4() const;

         void setEvasionAction4Duration(const double duration_s);
         double getEvasionAction4Duration() const;

         void setEvasionRWRResponse(const bool enable);
         bool getEvasionRWRResponse() const;

         void setEvasionRWRResponseBeamDuration(const double duration_s);
         double getEvasionRWRResponseBeamDuration() const;

         void setEvasionRWRSAMResponseRange(const double range_m);
         double getEvasionRWRSAMResponseRange() const;

         void setEvasionRWRAIResponseRange(const double range_m);
         double getEvasionRWRAIResponseRange() const;

         void setEvasionRWRResponseRestoreAfterBreakLock(const double duration_s);
         double getEvasionRWRResponseRestoreAfterBreakLock() const;

         void setEvasionRWRResponsePriority(il::eRWRResponsePriority priority);
         il::eRWRResponsePriority getEvasionRWRResponsePriority() const;

         void setHomeBasePosition(const double lat, const double lon);
         il::positionRecord getHomeBasePosition() const;

         void setReturnToCAP(const bool return_to_CAP);
         bool getReturnToCAP() const;

         void setDebug(const bool debug);
         bool getDebug() const;

         void setIsLowOnFuel(const bool low);
         bool getIsLowOnFuel() const;

      protected:
         weaponsManagerAI(const weaponsManagerAI& from);

         bool returnToCAP;
         bool isLowOnFuel;

         class AIScriptingParams
         {
            public:
               AIScriptingParams();

               bool m_debug;
               double m_alert_time_s;
               il::eAssessEngage m_assess_engage_via;
               bool m_escort;
               double m_intercept_speed_m_per_s;
               unsigned int m_place_in_formation;
               double m_pursuit_range_m;
               double m_salvo_delay_s;
               // take action settings
               bool m_evasion_take_action;
               double m_evasion_take_action_distance_m;
               il::eTakeAction m_evasion_actions[4];
               double m_evasion_take_action_durations_s[4];
               // RWR response settings
               bool m_evasion_rwr_response;
               double m_evasion_rwr_response_beam_duration_s;
               double m_evasion_rwr_sam_response_range_m;
               double m_evasion_rwr_ai_response_range_m;
               double m_evasion_rwr_response_restore_after_break_lock_s;
               il::eRWRResponsePriority m_evasion_rwr_response_priority;
               il::positionRecord m_home_base_position;
         };

      private:
         AIScriptingParams m_ai_scripting_params;
   };
}

#endif
