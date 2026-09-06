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

#include "BrawlerEvaluation.hpp"

#include <cmath>

#include "BrawlerPlatform.hpp"

// Bring in BRAWLER constants, enumerations, and util functions
using namespace BRAWLER;

// ZU - 0.1 corresponds to attack posture
double BrawlerEvaluation::msnmlt = 0.1;

// These variables are set in evrtei()
double BrawlerEvaluation::sclimp = 0.0;
double BrawlerEvaluation::vcnom  = 0.0;

// These were originally initialized in evrtei()
// C     --SET ALTERNATIVE DESCRIPTIONS FOR CAP_TACTIC, ROUTE - MANEUVER,
// C     --AND CAP_TURN MANEUVER AS COMPACTED DESCRIPTIONS
const int BrawlerEvaluation::captac[4] = {1, 1, 3, 1};
const int BrawlerEvaluation::rtmnv[4]  = {3, 2, 1, 1};
const int BrawlerEvaluation::captn[4]  = {3, 2, 2, 1};


// C#ABSTRACT EVALUATES MANUEVER ALTERNATIVE IN /ALTHLD/
// C#PURPOSE  EVALUATES MANUEVER ALTERNATIVE IN /ALTHLD/
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// COUT ALTVLX REAL - VALUE OF ALTERNATIVE
// C#TECHNICAL DESCRIPTION
// C     AEVAL3 IS RESPONSIBLE FOR THE EVALUATION OF A MANEUVER
// C  ALTERNATIVE.  IT GENERALLY ACTS BY CALLING, CONDITIONALLY, A
// C  SUBROUTINE TO EVALUATE THE NORMALIZED (0-1) PORTION OF A MANEUVER
// C  VALUE COMPONENT, MULTIPLYING THE NORMALIZED VALUE BY AN IMPORTANCE
// C  MULTIPLIER, AND ADDING THE RESULT TO THE TOTAL VALUE OF THE
// C  MANEUVER.  THE ONLY CALCULATIONS OF ANY CONSEQUENCE CARRIED OUT
// C  DIRECTLY IN AEVAL3 ARE RELATED TO THE IMPORTANCE MULTIPLIER
// C  CALCULATIONS.  THE MOST COMPLEX OF THESE RELATES TO COMPUTING
// C  VOFFEF, THE EFFECTIVE OFFENSIVE VALUE.  VOFFEF IS INTENDED TO BE A
// C  BLEND OF PURSUIT VALUES, COMING FROM THE EVALUATION OF VALOFX, AND
// C  WEAPON AIM VALUES THAT COME FROM VMAIM.  WHEN THERE IS NO
// C  IMMEDIATE INTEREST IN FIRING (NOAIM = .TRUE.) THEN VALOFX IS USED,
// C  ENCOURAGING PURSUIT.  WHEN THERE IS INTEREST IN FIRING, BUT NOT
// C  IMMEDIATELY (INTRST = .TRUE.) VALOFX AND VMAIM ARE BLENDED
// C  TOGETHER ACCORDING TO THE FORMULA
// C
// C    VOFFEF = VMAIM + (1-VMAIMN)*VALOFX
// C
// C    THIS HAS THE EFFECT OF EMPHASIZING MISSILE AIMING WHEN THE TIME
// C  TO SHOOT NEARS (VMAIM APPROACHES 1.0) AND PURSUIT WHEN VMAIM IS
// C  SMALLER.  FINALLY, WHEN A SHOT IS IMMINENT, AIMING VALUES TAKE
// C  OVER COMPLETELY.  VOFFEF IS MODIFIED BY ADDING A TERM THAT TAKES
// C  INTO ACCOUNT THE POSSIBLITY OF GROUND IMPACT WHILE AIMING THE
// C  WEAPON. THIS IS NECESSARY OTHERWISE VOFFEF PLUS VMAIM CAN DOMINATE
// C  THE VALUE FOR AVOIDING THE GROUND.
// C
// C     ANOTHER CALCULATION OF SOME COMPLEXITY INVOLVES THE COMPUTATION
// C  OF VALOFX.  THIS VARIABLE IS BUILT UP FROM ONE-VERSUS-ONE PURSUIT
// C  VALUES BY TAKING THE MAXIMUM OVER THE PRODUCTS OF TARGET EFFECTIVE
// C  VALUES AND THE ONE-VERSUS-ONE PURSUIT VALUES FOR EACH TARGET.  THE
// C  INTENT IS TO RECOGNIZE THAT ONLY ONE HOSTILE IS PURSUED AT A TIME,
// C  AND THAT THE OBJECTIVE IS TO ATTACK (OUTPUT OF E1V1 IS CONSIDERED).
// C  THE PROBABILITY OF KILL, GIVEN ATTACK, IS CONSIDERED AND WHEN
// C  MULTIPLIED BY THE TARGET EFFECTIVE VALUE, GIVES EXPECTED UTILITY.
// C
// C     THE DEFENSIVE VALUE IS EVALUATED BY COMPUTING A SURVIVAL
// C  PROBABILITY AND MULTIPLYING ITS COMPLEMENT BY THE VALUE OF ONESELF
// C  AND A DEFENSIVE IMPORTANCE MULTIPLIER.  THE SURVIVAL PROBABILITY
// C  IS ESTIMATED ON THE BASIS OF INDEPENDENT ATTACKS BY EACH HOSTILE.
// C  THE SURVIVAL PROBABILITY AGAINST ANY HOSTILE IS GIVEN BY
// C  1-VALDEF*PATK(IAC,ME) WHERE VALDEF IS THE SECOND OUTPUT OF E1V1
// C  AND SERVES AS A SURROGATE PROBABILITY OF KILL GIVEN ATTACK (ON
// C  SELF BY THE HOSTILE) AND PATK(IAC,ME) IS THE ESTIMATED PROBABILITY
// C  THAT THE HOSTILE WILL IN FACT ATTACK.  THE OVERALL SURVIVAL
// C  PROBABILITY IS THE PRODUCT OF THESE INDIVIDUAL SURVIVAL
// C  PROBABILITIES.
// C
// C    AEVA3I IS THE INITIALIZATION ENTRY AND PERFORMS AS AN EXECUTIVE
// C  THAT CALLS INITIALIZATION ENTRIES FOR THE SUBROUTINES THAT
// C  EVALUATE SPECIFIC MANEUVER VALUE COMPONENTS.  SOME OF THE
// C  SUBROUTINES ARE CALLED CONDITIONALLY, DEPENDING UPON THE VALUES OF
// C  LOGICAL FLAGS.  ALSO, CERTAIN LOGICAL VARIABLES ARE EVALUATED BY
// C  AEVA3I DIRECTLY.  THESE CONTROL THE ACTIVATION OF MANEUVER VALUE
// C  COMPONENTS.
// C#VARIABLE DESCRIPTIONS
// C  ALTDSC   INT - Compacted description of the alternative
// C  VE       ARRAY DIM(MAC) OF (3-VEC) [FT/SEC] - Inertial velocity vectors
// C  HDDALT   REAL [FEET] - Hard-deck altitude that pilot will honor as his
// C  IACID    AC-IND [NA] - My aircraft index.
// C  TNL_TAU  REAL [SEC] - Tunnel vision time constant used to determine
// C           when to reset the tunnel vision multipliers back to 1.
// C           This is set as a function of pilot skill level.
// C  COMPR    REAL - Routepoint score
// C  GAVAL    REAL - Ground avoidance score
// C  HDDVAL   REAL - Hard-deck avoidance score.  Similar to ground
// C           avoidance except avoiding some mission imposed
// C           minimum altitude.
// C  SEPVAL   REAL - Score for maintaining adequate separation
// C  VALDFX   REAL - Defensive 1v1 score
// C  VALEVD   REAL - Missile evasion score
// C  VAVBDD   REAL - Value associated with avoiding bad aspects.
// C  VDISNG   REAL - Disengage score
// C  VESCRT   REAL - Score for maintaining bomber escort position
// C  VFORM    REAL - Formation following score
// C  VILLUM   REAL - Illumination score
// C  VLOSPD   REAL - Low speed recovery score
// C  VMAIM    REAL - Missile aiming score
// C  VMUSUP   REAL - Mutual support score
// C  VOFFEF   REAL - Offensive 1v1 score - a blend of the pursuit value
// C           valofx (local to aeval3), vmaim and a ground impact term
// C  VSAMZ    REAL - Not implemented
// C  VVEC     REAL - Vector following score
// C  VDEW     REAL - DEW aiming score
// C  CSMMSL   REAL - SMALL WHEN MISSILE THREAT HIGH.  USED TO LO
// C           IMPORTANCE OF COMPONENTS IN THE PRESENCE
// C  RCSMSL   REAL - A LESS EXTREME VERSION OF CSMMSL, FOR COMP
// C           THAT DECREASE LESS IN VALUE UNDER MIS
// C  OFFMUS   REAL - EFFECTIVE OFFENSIVE MULT
// C  DEFMUS   REAL - EFFECTIVE DEFENSIVE MULT
// C  BIAS_FAULTS ARRAY DIM(MFALTS) of REAL - HOLDS INHERENT BIAS
// C           ADJUSTMENT FACTORS
// C  TNL_MULT ARRAY DIM(NVALCP) of REAL - TUNNEL VISION ADJUSTMENT
// C           FACTORS FOR EACH VALUE COMPONENT.
// C####
// static
double BrawlerEvaluation::EvaluateAlternative(BrawlerPlatform& me, UtEntity* tgt)
{
   ////debug drawing
   // double locWCS[3];
   // projectedState.GetLocationWCS(locWCS);
   // mDrawPtr->SetId(mDrawPtr->GetNewId());
   // mDrawPtr->SetPointSize(3);
   // mDrawPtr->SetDuration(30);
   // UtVec3d color = this->GetCurrentColor();
   // mDrawPtr->SetColor(color[0], color[1], color[2]);
   // mDrawPtr->BeginPoints();
   // mDrawPtr->VertexWCS(locWCS);
   // mDrawPtr->VertexWCS(locWCS);                // Need two point because of VESPA bug
   // mDrawPtr->End();

   // mMyPro = projectedState;
   // UtVec3d xeua = BrawlerPlatform::PositionNED(me.ProjectedState());
   // UtVec3d veua = BrawlerPlatform::VelocityNED(me.ProjectedState());

   // lines 150-462 of aeval3.f
   // C  --subroutine argument declarations
   double altvlx = 0.0;
   // C  --external declarations
   // int indpk;
   // int ammo_lev;
   // double cauchy, rnsq, ramp;
   // double xmag;
   // double terr_ang_pen;
   // bool lbit;
   // char chralt[12];
   // C  --local declarations
   // save skip_1v1, offmus
   // int ilevel;
   // int kalt;
   // int icall;
   // int lcall;
   // int i;
   // bool skip_1v1 = false; // class member variable
   // bool tnl_reset; // class member variable
   // bool descnd;
   // double terrain_alt;
   // double tol = 0.0;
   // double zmax_hot;
   // double terrmult;
   // double alt_p;
   // double alt_n;
   // double diffxeua[3];
   // double diffxp[3];
   // double dgamma2;
   // double dgamma2p;
   // double dgamma2ua;
   double valofx = 0.0;
   double vmaimn = 0.0;
   // double offmus;
   // double tnl_p_reset; // class member variable
   // double dt;
   // double pdraw; // class member variable
   // double zmx;
   double greq   = 0.0;
   double crunch = 0.0;
   // double zmax_pos[3];
   // double hog;
   // double buff;
   // tol = 0.0001;
   // C*ENDDEC
   // call xmit(-nvalcp,0.0,valcmp); //initialize valcmp to 0.0
   // call xmit(-nhutil,0.0,vofutl); //initialize vofutl to 0.0
   altvlx = 0.0;

   // C     --GET RADAR STATUS AND CHARACTERISTICS CURRENT
   // call grdrc(iacid,prant); //LBM - fecthes common block data to working memory (for antenna "prant")
   // call grdrs(iacid,prant); //LBM - fecthes common block data to working memory (for antenna "prant")

   // LBM - do not worry about station keeping or mutual support right now
   ////C
   ////C     --STATION-KEEPING
   // if( (stkppp > 0.0) && (DegreeOfThreatCompliment() > 0.75) )
   //{
   //    call evform(vform);
   //    vform = vform*cauchy(DegreeOfThreat(),0.10)*TunnelVisionMultiplier(i_vform);
   //    altvlx += vform;
   // }

   // Routepoint value evaluation has been replicated, but will not currently be used
   // ZU 1/19/2016

   // C     --ROUTEPOINT VALUES
   // if((me.RoutePointValueMultiplier() != 0.0) && (me.DegreeOfThreat(*tgt) >= 0.75))
   //{
   //    bool continue_110 = false;

   //   if (me.Job() == ELEMENT_LEAD)
   //   {
   //      if (me.Mission() != CAP)
   //      {
   //         //goto 110;
   //         continue_110 = true;
   //      }
   //   }
   //   else if (me.Job() != FLIGHT_LEAD)
   //   {
   //      //goto 110;
   //      continue_110 = true;
   //   }
   //   // ZU - Currently not using GCI
   //   //else if (!igngc4 && altd1 == indpk(1,8,1,1))
   //   //{
   //   //   //C           --Following GCI vectors instead of route points
   //   //   //goto 110;
   //   //   computeRoutePointValue = false;
   //   //}
   //   //else if (altd1 == indpk(1,8,2,1))
   //   //{
   //   //   //C           --GCI drag tactics follow vectors instead of route points
   //   //   //goto 110;
   //   //   computeRoutePointValue = false;
   //   //}
   //   if (!continue_110)
   //   {
   //      double compr;
   //      InitializeRoutePointValues(me);
   //      CalcRoutepointValue(me, compr);
   //      compr = compr * cauchy(1.0-me.DegreeOfThreat(*tgt), 0.10) * me.TunnelVisionMultiplier(i_compr);
   //      me.SetManeuverValueComponent(i_compr, compr);
   //      altvlx += compr;
   //   }

   //// 110
   //}


   // WsfPlatform* aTgt = GetTargetPlatform();
   // WsfTrack* aTgtTrack = GetTarget();
   // C
   // C     --ONE-ON-ONE VALUES
   // if (GetTarget() == 0)
   if (me.Skip1v1())
   {
      me.SetRawManeuverValueComponent(i_valdfx, 0.0);
      me.SetManeuverValueComponent(i_valdfx, 0.0);
      valofx = 0.0;
      me.SetRawManeuverValueComponent(i_voffef, valofx);
   }
   else
   {
      // C        --THIS MUST BE CALLED FOR EACH ALTERNATIVE:
      double valdfx = 0.0;
      CalcPrimaryValues(me, valofx, valdfx);
      me.SetRawManeuverValueComponent(i_voffef, valofx);
      me.SetRawManeuverValueComponent(i_valdfx, valdfx);

      // LBM - these 5 lines moved here from CalcPrimaryValues()
      if (me.GetTarget() != nullptr)
      {
         valofx = valofx * me.LastComputedEngagementValue(*me.GetTarget());
      }
      valdfx = -(1.0 - valdfx) * me.AircraftValue(me.State()) * me.DefensiveMultiplier();

      // C        --ADD IN DEFENSIVE VALUE
      valdfx *= me.TunnelVisionMultiplier(i_valdfx);
      me.SetManeuverValueComponent(i_valdfx, valdfx);
      altvlx += valdfx;
   }
   // C     --ADD IN OFFENSIVE VALUE LATER, WHEN MISSILE-AIM VALUE IS KNOWN
   // C
   // C     --GROUND AVOIDANCE, TERRAIN, and HARD-DECK CONSIDERATIONS:
   // C     --GREQ IS THE GEES REQUIRED TO AVOID HITTING THE GROUND
   greq = 0.0;

   // TODO - LBM - do not worry about ground avoidance, hard-decks, or terrain (at this point in the replication)
   // if(splat || hddnear || terrain)
   //{
   //    //C
   //    //C        --CALCULATE MAX Z (AND ITS POSITION VECTOR) DURING MANUEVER
   //    call rtnmaxz(zmx, descnd, zmax_pos);
   //    //C
   //    //C        --TERRAIN AVOIDANCE PENALTY
   //    if(terrain)
   //    {
   //       //C           --IS ZMAX_POS LOCATED AT AN ENDPOINT OF THE MANEUVER?
   //       call vsub(zmax_pos,xeua,diffxeua);
   //       call vsub(zmax_pos,xp(1,me),diffxp);
   //       //C           --Set buffer to the minimum of hotmin and my current
   //       //C           --altitude. (Using a buffer greater than my altitude
   //       //C           --causes the clr_ang calculation to blow up.)
   //       call ht_ovr_grnd(xp(1,me),hog);
   //       buff = amin1(hog-1.0,hotmin);
   //       if(xmag(diffxeua) <= 50.0)
   //       {
   //          call clr_ang(xp(1,me),zmax_pos,xp(1,me),buff,dgamma2,iacid);
   //       }
   //       else if(xmag(diffxp) <= 50.0)
   //       {
   //          call clr_ang(zmax_pos,xeua,xp(1,me),buff,dgamma2,iacid);
   //       }
   //       else
   //       {
   //          call clr_ang(xp(1,me),zmax_pos,xp(1,me),buff,dgamma2p,iacid);
   //          call clr_ang(zmax_pos,xeua,xp(1,me),buff,dgamma2ua,iacid);
   //          dgamma2 = max(dgamma2p,dgamma2ua);
   //       }
   //       call evterr(dgamma2,terrval);
   //       greq = 4.0;
   //       terrval = terrval*TunnelVisionMultiplier(i_terrval);
   //    }
   //    //C
   //    //C        --GROUND AVOIDANCE PENALTY
   //    terrain_alt = 0.0;
   //    if(splat && !terrain)
   //    {
   //       terrval = 0.0;
   //       call evgrnd(zmx, descnd, terrain_alt, gaval, greq);
   //       gaval = gaval*TunnelVisionMultiplier(i_gaval);
   //    }
   //    //C
   //    //C        --HARD-DECK CONSIDERATION
   //    if(hddalt>terrain_alt)
   //    {
   //       call evhdd(zmx, descnd, hddval);
   //       hddval = hddval*TunnelVisionMultiplier(i_hddval);
   //    }
   //    altvlx += gaval + hddval + terrval;
   // }

   // LBM - no vector following (interactive piloting)
   ////C
   ////C     --VECTOR-FOLLOWING VALUES:
   // if (dirspc != 0.0)
   //{
   //    call evvec(vvec);
   //    vvec = vvec*dirval*TunnelVisionMultiplier(i_vvec);
   //    altvlx += vvec;
   // }
   // else

   if (tgt != nullptr && me.ValueVectoredFlight() != 0.0)
   {
      // if (disbng)
      //{
      //    //C           --VVEC only zero if disengage due to bingo fuel (6/18/91 sjc)
      //    vvec = 0.0;
      // }
      // else
      if (me.Ready() || me.InIntercept()) // intrst == InIntercept() ???
      {
         // C           --if ready VVEC limited to be same magnitude as VMAIM(6/18/91)
         // call evvec(vvec);
         double vvec;
         CalcVectoredFlightValue(me, vvec);
         vvec = vvec * me.DegreeOfThreatCompliment(*tgt) * me.TunnelVisionMultiplier(i_vvec) *
                amin1(me.ValueVectoredFlight(), 0.8 * me.LastComputedEngagementValue(*tgt));
         me.SetManeuverValueComponent(i_vvec, vvec);
         altvlx += vvec;
      }
      else
      {
         // call evvec(vvec);
         double vvec;
         CalcVectoredFlightValue(me, vvec);
         vvec = vvec * me.ValueVectoredFlight() * me.DegreeOfThreatCompliment(*tgt) * me.TunnelVisionMultiplier(i_vvec);
         me.SetManeuverValueComponent(i_vvec, vvec);
         altvlx += vvec;
      }

      // printf("i_vvec: %.2f, altvlx: %.2f\n", me.ManeuverValueComponent(i_vvec), altvlx);
   }

   // C
   // C     --MISSILE AIMING VALUES AND OFFENSIVE VALUES
   // C     --ADD IN FULL OFFENSIVE VALUES FOR MSLMD FIRST TWO BITS 0
   // C     --ADD IN PARTIAL OFFENSIVE VALUES FOR MISSILE MODE BIT 1
   // C     --AND NO ROT VALUE
   // C     --DO NOT ADD OFFENSIVE VALUES FOR MSLMD BIT 2
   // C     --COMPUTE A "CRUNCH" VALUE, WHICH IS A LINEAR FUNCTION OF THE
   // C     --GEES REQUIRED TO AVOID HITTING THE GROUND.  IF THIS IS MORE
   // C     --THAN THE AIRCRAFT CAN PULL, THEN CLEARLY THE VALUE OF THE
   // C     --AIRCRAFT IS LOST, SO THE NORMALIZED SCORE IS -1
   // double voffef = 0.0;
   if (me.NoAim())
   {
      // C        --WE DON'T USE IT HERE, BUT IT WILL BE PRINTED LATER
      crunch = 0.0;
      if (me.HaveWeapons() && tgt != nullptr)
      {
         double voffef = valofx * me.OffensiveMultiplier() * me.DegreeOfThreatCompliment(*tgt) *
                         me.InherentBiasFaults(i_ovroff) * me.TunnelVisionMultiplier(i_voffef);
         me.SetManeuverValueComponent(i_voffef, voffef);
         altvlx += voffef;
      }
   }
   else
   {
      vmaimn = 0.0;
      if (tgt != nullptr)
      {
         CalcMissileAimValue(me, *tgt, vmaimn);
      }
      if (me.InIntercept() && me.RollOverTopValue() != 0.0)
      {
         vmaimn = 0.0;
      }
      me.SetRawManeuverValueComponent(i_vmaim, vmaimn);
      if (tgt != nullptr)
      {
         double vmaim = vmaimn * me.LastComputedEngagementValue(*tgt) * me.InherentBiasFaults(i_ovroff) *
                        me.TunnelVisionMultiplier(i_vmaim);
         me.SetManeuverValueComponent(i_vmaim, vmaim);
         // C        --NOTE THAT THERE MUST BE WEAPONS IF MSLMD FIRST TWO BITS .NE. 0
         double voffef = 0.0;
         if (me.Ready())
         {
            voffef = me.ManeuverValueComponent(i_vmaim);
         }
         else if (me.InIntercept())
         {
            voffef = me.ManeuverValueComponent(i_vmaim) + 0.75 * (1.0 - vmaimn) * valofx;
         }
         crunch = -1.0 * ramp(0.0, greq, me.MaxInstantaneousGLoad_g());
         voffef += crunch;
         voffef *= me.OffensiveMultiplier() * me.DegreeOfThreatCompliment(*tgt) * me.InherentBiasFaults(i_ovroff) *
                   me.TunnelVisionMultiplier(i_voffef);
         me.SetManeuverValueComponent(i_voffef, voffef);
         altvlx += voffef;
      }
      else
      {
         // LBM - is this appropriate?
         me.SetManeuverValueComponent(i_voffef, 0.0); // TODO - verify we should do this?
      }
   }

   // rcsmsl: used below several places
   double rcsmsl = 0.0;
   if (tgt != nullptr)
   {
      rcsmsl = sqrt(me.DegreeOfThreatCompliment(*tgt));
   }

   // C
   // C     --ILLUMINATION VALUES
   if (me.MissileMode() == ILLUMINATE && tgt != nullptr)
   {
      double villum = 0.0;
      CalcIlluminationValue(me, *tgt, villum); // evilum(aTgt, villum);
      me.SetRawManeuverValueComponent(i_villum, villum);
      if (me.OffensiveMultiplier() >= 1.0)
      {
         villum *= me.OffensiveMultiplier();
      }
      else // if(me.OffensiveMultiplier() < 1.0)
      {
         villum *= sqrt(me.OffensiveMultiplier());
      }
      villum *= rcsmsl * me.TunnelVisionMultiplier(i_villum);
      me.SetManeuverValueComponent(i_villum, villum);
      altvlx += villum;
   }

   // TODO - LBM - do not worry about coordinated efforts or mutual support (at this point in the replication)
   ////C
   ////C     --MUTUAL SUPPORT VALUES
   // if(nsuppp != 0 && !l1opp)
   //{
   //    call evmusp(vmusup);
   //    vmusup = vmusup*DegreeOfThreatCompliment()*InherentBiasFaults(i_nmusup)*TunnelVisionMultiplier(i_vmusup);
   //    altvlx += vmusup;
   // }

   // C
   // C     --MISSILE EVASION VALUES

   // if(msltrt != 0.0)
   UtEntity* aThreat = me.GetThreat();
   if (aThreat != nullptr)
   {
      double valevd;
      CalcEvadeValue(me, valevd);
      me.SetRawManeuverValueComponent(i_valevd, valevd);
      valevd *= me.TunnelVisionMultiplier(i_valevd);
      me.SetManeuverValueComponent(i_valevd, valevd);
      altvlx += valevd;
   }
   // C
   // C     --LOW SPEED RECOVERY
   if (me.SlowFlight() && !me.OneVsOneOp())
   {
      double vlospd;
      CalcLowSpeedRecoveryValue(me, vlospd);
      me.SetRawManeuverValueComponent(i_vlospd, vlospd);
      vlospd *= rcsmsl * me.InherentBiasFaults(i_nospd) * me.TunnelVisionMultiplier(i_vlospd);
      me.SetManeuverValueComponent(i_vlospd, vlospd);
      altvlx += vlospd;
   }

   // LBM - no disengage right now
   ////C     --DISENGAGEMENT VALUE
   // if(DisEngage())
   //{
   //    evdis(aThreat, vdisng);
   //    vdisng = vdisng * rcsmsl * TunnelVisionMultiplier(i_vdisng);
   //    altvlx += vdisng;
   // }

   // LBM - dont need separation right now, just do 1v1 demo
   ////C
   ////C     --SEPARATION
   // if(sepmlt != 0.0 && !l1opp)
   //{
   //    call evsep(sepval);
   //    sepval = sepval * InherentBiasFaults(i_nmusup) * TunnelVisionMultiplier(i_sepval);
   //    altvlx += sepval;
   // }

   // TODO - LBM - do not worry about coordinated efforts or mutual support (at this point in the replication)
   ////C
   ////C     --ESCORT-BOMBER VALUE
   // if(escmlt != 0.0)
   //{
   //    call evesc(vescrt);
   //    vescrt = vescrt*DegreeOfThreatCompliment()*TunnelVisionMultiplier(i_vescrt);
   //    altvlx += vescrt;
   // }

   // TODO - LBM - do not worry about SAM sites (at this point in the replication)
   ////C
   ////C     --SAM ZONE AVOIDANCE
   // call evsamz(vsamz);
   // vsamz = vsamz*AircraftValue(this)*0.5*TunnelVisionMultiplier(i_vsamz);
   // altvlx += vsamz;


   // ZU - started replicating bad aspect avoidance
   // C
   // C     --BAD ASPECT AVOIDANCE
   // if(prbdas && bdval >tol)
   // {
   //    CalcBadAspectValue(me, me.ManeuverValueComponent[i_vavbdd]);
   //    me.ManeuverValueComponent[i_vavbdd] = me.ManeuverValueComponent[i_vavbdd] * bdval *
   //    me.TunnelVisionMultiplier(i_vavbdd); altvlx += me.ManeuverValueComponent[i_vavbdd];
   // }

   // LBM - do not worry about directed energy weapons right now
   ////C
   ////C     --DEW AIMING
   // if (ldew)
   //{
   //    call evdew(vdew);
   //    if(dew_tgt_typ != acent && dew_tgt_typ != sament)
   //    {
   //       call nabort('AEVAL3..bad DEW tgt type');
   //    }
   //       vdew = vdew * valeff(dew_tgt);
   //       vdew = vdew*InherentBiasFaults(i_ovroff)*TunnelVisionMultiplier(i_vdew);
   //       altvlx += vdew;
   // }

   // LBM - do not worry about guiding others missiles right now
   ////C
   ////C     --THIRD PARTY TARGETING
   // if (ltpt)
   //{
   //    call evtpt(vtpt);
   //    if(OffensiveMultiplier() >= 1.0)
   //    {
   //       vtpt = vtpt*OffensiveMultiplier();
   //    }
   //    if(OffensiveMultiplier() < 1.0)
   //    {
   //       vtpt = vtpt*sqrt(OffensiveMultiplier());
   //    }
   //    vtpt = vtpt*rcsmsl*TunnelVisionMultiplier(i_vtpt);
   //    altvlx += vtpt;
   // }

   // C
   // C     --find lowest maneuver alternative score
   if (altvlx < me.MinAlternativeValue())
   {
      me.SetMinAlternativeValue(altvlx);
   }

   me.UpdateStatistics(altvlx); // valst(altvlx);

   if (std::isnan(altvlx))
   {
      // break here
      altvlx = -1000000.0;
   }

   return altvlx;
}

// C#ABSTRACT COMPUTES THE ROUTEPOINT - FOLLOWING VALUE
// C#PURPOSE  COMPUTES THE ROUTEPOINT - FOLLOWING VALUE
// C#AUTHOR   KERCHNER
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// COUT COMPR  REAL - ROUTEPOINT VALUE OF TRIAL MANEUVER
// C#TECHNICAL DESCRIPTION
// C    EVRTEI COMPUTES A SCALE SPEED CHANGE SCLIMP, AND DVIMAG, THE
// C NOMINAL MAGNITUDE OF THE VELOCITY ERROR.
// C    EVRTE IS A FUNCTION OF THE COMPONENT OF VELOCITY ALONG THE
// C DIRECTION TO THE ROUTEPOINT, THE CHANGE IN THE VELOCITY ERROR
// C(VERSUS BEING HEADING TOWARDS THE ROUTEPOINT AT SPEED SPDMPP) AND
// C THE RATE - OF - CLIMB ERROR(VERSUS THAT FOR AN IDEAL ROUTEPOINT
// C MANEUVER).COMPL IS LARGE WHEN THE VELOCITY COMPONENT IS ON
// C TARGET AND COMP3 IS LARGE WHEN THETE IS AN IMPROVEMENT IN THE
// C COMPONENT.SINCE COMP3 IS ONLY IMPORTANT WHEN COMPL IS SMALL(IT
// C IS MULTIPLIED BY 1. - COMP1), COMP4 IS PRESENT TO DISCOURAGE
// C(PENALIZE) AN INCREASE IN THE VELOCITY COMPONENT ERROR.COMP2
// C IS SENSITIVE TO THE RATE OF CLIMB.
// C#VARIABLE DESCRIPTION
// C   18  MSNMLT   REAL[NA] - Value multiplier for mission - related value aspects.
// C  VMSNPP   REAL[NA] - Routepoint value multiplier
// C####
void BrawlerEvaluation::CalcRoutepointValue(BrawlerPlatform& me, double& compr)
{
   // equivalence(lprnts(207), lprnt)
   // C     --subroutine argument declarations
   // C     --external declarations
   // integer indpk
   // C     --local declarations
   UtVec3d xeua = BrawlerPlatform::PositionNED(me.ProjectedState());
   UtVec3d veua = BrawlerPlatform::VelocityNED(me.ProjectedState());
   // UtVec3d rtepp = me.GetTargetRoutePoint();
   double spdua  = veua.Magnitude();
   double spdmpp = me.GetSpeed();       // speed used to approach the waypoint
   double clmxpp = me.VelocityNED()[2]; // ZU - Is this the right climb rate?

   int ilevel, kalt, icall, lcall;
   me.GetAlternative(ilevel, kalt, icall, lcall);

   UtVec3d dx;
   double  vc, rcdes, comp1, comp2, comp3, comp4;
   double  vcimp, direrr, comp5;

   // save sclimp, vcnom, captac, rtmnv, captn
   // double sclimp, vcnom;
   // int captac, rtmnv, captn;
   // double arccos;

   // C*ENDDEC
   // C     --CHECK ALTDSC TO MATCH CAPTAC, RTMNV, CAPTN
   bool altd1_eq_captac =
      true; //(ilevel == captac[0]) && (kalt == captac[1]) && (icall == captac[2]) && (lcall == captac[3]);
   bool altdsc_eq_rtmnv = (ilevel == rtmnv[0]) && (kalt == rtmnv[1]) && (icall == rtmnv[2]) && (lcall == rtmnv[3]);
   bool altdsc_eq_captn = (ilevel == captn[0]) && (kalt == captn[1]) && (icall == captn[2]) && (lcall == captn[3]);

   // ZU - Not sure we care about altd1
   if (altd1_eq_captac && (altdsc_eq_rtmnv || altdsc_eq_captn))
   {
      // C        --MAKE SURE ROUTE MANEUVER AND CAP_TURN SCORE HIGH IF FLYING
      // C        --CAP_TACTIC
      compr = me.RoutePointValueMultiplier() * msnmlt;
   }
   else
   {
      // call vdir(rtepp, xeua, dx)
      dx = xeua - me.GetTargetRoutePoint();
      dx.Normalize();
      vc    = -veua.DotProduct(dx);
      comp1 = cauchy(vc - spdmpp, spdmpp);
      // C        --desired rate - of - climb
      rcdes = dsxdot(me.GetTargetRoutePoint()[2], xeua[2], veua[2], clmxpp, 100.);
      comp2 = reward(rcdes - veua[2], clmxpp);
      // C        --calculate closure rate improvement :
      vcimp = vc - vcnom;
      comp3 = border(vcimp, sclimp);
      comp4 = amin1(1., 2. * comp3 - 1.);
      // C        --calculate directional term : vc / spdua is cosine of dir error
      direrr = arccos(vc / (spdua + 0.01));
      comp5  = cauchy(direrr, 20.0 * rad);
      // C**      compr = vmsnpp*msnmlt*
      // C**  1    (0.60*(comp1 + 0.7*(1. - comp1)*comp3) + 0.30*comp2 + 0.10*comp4)
      compr = (0.30 * (comp1 + 0.7 * (1. - comp1) * comp3) + 0.30 * comp2 + 0.10 * comp4 + 0.30 * comp5);
      // if (lprnt)
      // {
      //    write(ioutp, 6000)compr, comp1, comp1, comp3, comp2, comp4, comp5
      //    compr = compr*vmsnpp*msnmlt
      // }
   }
}

// C#TECHNICAL DESCRIPTION
// C    EVRTEI COMPUTES A SCALE SPEED CHANGE SCLIMP, AND DVIMAG, THE
// C NOMINAL MAGNITUDE OF THE VELOCITY ERROR.
void BrawlerEvaluation::InitializeRoutePointValues(BrawlerPlatform& me)
{
   // C     --SET ALTERNATIVE DESCRIPTIONS FOR CAP_TACTIC, ROUTE - MANEUVER,
   // C     --AND CAP_TURN MANEUVER AS COMPACTED DESCRIPTIONS
   // captac = indpk(1, 1, 3, 1);
   // rtmnv = indpk(3, 2, 1, 1);
   // captn = indpk(3, 2, 2, 1);

   UtVec3d dx;
   double  gmxmpp = me.GetMIND()->MaxGeesForRouteFlying(); // gmaxm from MIND
   double  tproj3 = me.ProjectedTimeDelta();               // projected time
   UtVec3d xeuan  = me.PositionNED(me.NominalState());
   UtVec3d veuan  = me.VelocityNED(me.NominalState());
   UtVec3d rtepp  = me.GetTargetRoutePoint();

   // C     --COMPUTE SCLIMP AND VCNOM FOR USE BY EVAL_RTE
   sclimp = .5 * grav * gmxmpp * tproj3;

   // call vdir(rtepp, xeuan, dx)
   dx = xeuan - rtepp;
   dx.Normalize();

   vcnom = -dx.DotProduct(veuan);

   // if (lprnt) write(ioutp, 6001) vmsnpp, msnmlt, sclimp, vcnom
   // return
   // format('EVRTE', t10, f6.3, ' = .3(', f6.2, '+.7(1-', f6.2, ')*(', f6.2,
   // 1 '))+.3(', f6.2, ')+.1(', f6.2, ')+.3(', f6.2, ')')
   // 6001  format(' evrtei...vmsnpp=', f6.2, ' msnmlt=', f6.3, ' sclimp=', f6.1,
   // 1 ' vcnom=', f7.0)
}

// C#ABSTRACT COMPUTES OFFENSIVE AND DEFENSIVE VALUES
// C#PURPOSE  CALLED BY AEVAL3 TO FIND VALOFX AND VALDFS FOR 1V1 AND 1VMANY
// C#PARAMETER DESCRIPTIONS
// COUT VALOFX REAL - OFFENSIVE VALUE OF 1 VS OTHERS
// COUT VALDFS REAL - DEFENSIVE VALUE OF 1 VS OTHERS
// C#TECHNICAL DESCRIPTION
// C  IF L1OPP IS TRUE, THERE IS A 1 VS 1 SITUATION.  A SCORING ROUTINE IS USED TO FIND VALOFF WHICH, IN THIS CASE,
// REFLECTS A TOTAL UTILIITY(OFFENSIVE AND DEFENSIVE). C  IF L1OPP IS FALSE, THERE IS A 1 VS MANY SITUATION.  E1V1 IS
// USED TO FIND VALOFX AND VALDFS. C  VALOFF IS THE MAXIMUM OF THE PRODUCT OF THE OFFENSIVE VALUE AND THE TARGET UTILITY
// FOR ALL TARGETS IN THE LOCAL CELL. C  VALDFX IS THE PROBABILITY OF SURVIVING FOR EACH HOSTILE IN THE LOCAL CELL
// CONVERTED TO THE NEGATIVE UTILITY OF NOT SURVIVING IN THE LOCAL CELL. C#VARIABLE DESCRIPTIONS C  VALOFF  REAL -
// OFFENSIVE VALUE OF POSITION ONLY C  VALDEF  REAL - DEFENSIVE VALUE OF POSITION ONLY C  DEFMUS  REAL - DEFENSIVE
// MULTIPLIER C  LNGTRN  LOG - INDICATES LONG TURN MANEU C####
void BrawlerEvaluation::CalcPrimaryValues(BrawlerPlatform& me, double& valofx, double& valdfs)
{
   // double valdfs, valofx;
   // int    iac;
   // int    ihutil;
   // int    icel;
   double valoff = 0.0;
   double valdef = 0.0; //, defmus;
   // C*ENDDEC
   valdfs = 1.0;
   valofx = 0.0;
   // LBM - no maneuver cells for now
   // call e1v1i3(); //only 1 line function: call xmit(-nmcel,.false.,mclevl)
   // call xmit(-nmcel, false, mclevl);

   // LBM - dont loop over all nhutil (possible targets hutil.f lists), just evaluate one target
   ////do 110 ihutil = 1,nhutil
   // for (ihutil = 1; ihutil <= nhutil; ++ihutil)
   //{
   //    iac = lhutil(ihutil);
   //    call e1v1(iac,valoff,valdef);
   //    if(valoff == 0.0 && valdef == 0.0)
   //    {
   //       break; //goto 110;
   //    }
   //    //LBM - no maneuver cells for now
   //    ////C        --INCLUDE EFFECT OF MANEUVER CELL SIZE:
   //    //icel = mcelid(iac);
   //    //valdfs = valdfs*(1.-valdef*patk(iac,me))**namcel(icel);
   //    //vofutl(ihutil) = valoff*valeff(iac);
   //    //valofx = amax1(valofx,vofutl(ihutil));
   // }
   // 110   continue
   // WsfPlatform* target = GetTargetPlatform();
   UtEntity* target = me.GetTarget();
   if (target != nullptr)
   {
      Evaluate1v1(me, *target, valoff, valdef);
      // e1v1defensive(target, valdef);
      // e1v1offensive(target, valoff);
      // valdfs = valdfs*(1.-valdef*patk(iac,me));
      valdfs = valdfs * (1.0 - valdef); // dont use patk() - assume target will attack (patk == 1)
      // LBM - modification moves outside this function
      // valofx = valoff * me.LastComputedEngagementValue(*target);
      valofx = valoff;
   }
   // LBM - modification moved outside this function
   // valdfs = -(1.0-valdfs) * me.AircraftValue(me.State()) * me.DefensiveMultiplier();
   return;
}

// C#ABSTRACT EVALUATES OFFENSIVE AND DEFENSIVE VALUES VS A SINGLE A/C
// C#PURPOSE  EVALUATES OFFENSIVE AND DEFENSIVE VALUES VS A SINGLE A/C
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// CIN  JAC    MM-IDX - THE HOSTILE TO BE CONSIDERED VS SELF
// COUT VALOFF REAL - NORMALIZED OFFENSIVE SCORE VS IAC
// COUT VALDEF REAL - NORMALIZED DEFENSIVE SCORE VS IAC
// C#TECHNICAL DESCRIPTION
// C     SCORES ARE BASED ON PROJECTED STATE VECTORS IN /PRJCT/.  THE
// C  OFFENSIVE VALUE FUNCTION IS ITSELF THE SUM OF SEVERAL
// C  "MINI-ASPECTS."  THESE INCLUDE THE FOLLOWING FEATURES:
// C
// C  OPPONENT IN FRONT OF ME (LOS ANGLE < 90 DEG) BUT NOT SO CLOSE THAT
// C  HE CAN FORCE AN OVERSHOOT.
// C
// C  BEHIND OPPONENT (ASPECT < 90 DEG) AND LOS ANGLE TO OPPONENT
// C  LESS THAN 60 DEG.
// C
// C  OPPONENT NOT TOO FAR IN FRONT (BASED ON HIS ABILITY TO TURN
// C  TOWARDS ME).
// C
// C  OPPONENT NOT TOO FAR TO THE SIDE.
// C
// C  A SUITABLE CLOSURE RATE--LARGE WHEN FAR AND SMALL WHEN CURRENTLY
// C  IN GOOD POSITION.
// C
// C  A GOOD SPECIFIC ENERGY--THAT OF THE HOSTILE WHEN IN GOOD POSITION,
// C  AND SOMEWHAT HIGHER THAN THE TARGET WHEN NOT IN GOOD OFFENSIVE
// C  POSITION.
// C
// C  ABLE TO SEE THE OPPONENT.
// C
// C  SMALL LOS ANGLE.
// C
// C  GOOD LOS RATE--SO THAT A SMALL LOS CAN BE ACHIEVED/MAINTAINED.
// C
// C  ROLL-OVER-THE-TOP VALUE--WHEN ACTIVE (A LARGE TURN IS NEEDED
// C  QUICKLY), THIS COMPONENT REWARDS PITCHING THE NOSE UP.
// C
// C     THE FUNCTIONAL FORM OF THE COMPONENTS LISTED ABOVE IS FAIRLY
// C  TRANSPARENT.  THE DEFENSIVE VALUE COMPONENT IS MORE OR LESS A
// C  MIRROR IMPAGE OF THE OFFENSIVE COMPONENT, WITH THINGS BEING LOOKED
// C  AT FROM THE STANDPOINT OF THE HOSTILE ATTACKING THE CONSCIOUS
// C  AIRCRAFT, RATHER THAN THE OTHER WAY AROUND.
// C
// C     ONLY ONE HOSTILE PER MANEUVER CELL IS CONSIDERED, AND THE
// C  EFFECTS OF MULTIPLE HOSTILES PER CELL IS TAKEN INTO ACCOUNT BY
// C  AEVAL3.
// C
// C     THE INITIALIZATION ENTRY E1V1I1 IS USED FOR TWO PURPOSES.
// C  THE FIRST IS TO COMPUTE A NUMBER OF PARAMETERS THAT WILL BE
// C  CONSTANT OVER ALL ALTERNATIVES CONSIDERED, SO AS TO CONSERVE
// C  COMPUTATION TIME.  FOR INSTANCE, THE SPECIFIC ENERGY OF EACH
// C  HOSTILE AND THE DESIRED CLOSURE RATE VERSUS EACH HOSTILE IS
// C  COMPUTED DURING INITIALIZATION.  THE OTHER FUNCTION IS TO COMPUTE
// C  THE DESIRABILITY OF A ROLL-OVER-THE-TOP (ROT) MANEUVER.  THE
// C  PURPOSE OF THIS IS TO INDUCE A ZOOM IN SITUATIONS WHERE A PILOT
// C  CANNOT MAKE A REQUIRED TURN IN A SHORT TIME.  THE ZOOM WILL REDUCE
// C  HIS SPEED (AND THUS HIS TURN RADIUS) WITHOUT SACRIFICING SPECIFIC
// C  ENERGY.
// C     THE GENERAL IDEA FOR ROT IS TO PICK A LIKELY TARGET POINT AND
// C  COMPUTE THE TIME IT WILL TAKE TO TURN IT.  IF THIS TIME IS
// C  EXCESSIVE, THE ROT VALUE IS TURNED ON, PROVIDING THAT SPEED IS
// C  COMFORTABLY ABOVE CORNER VELOCITY.  ACTUALLY, THE MULTIPLER VALROT
// C  TURNS ON AND OFF CONTINUOUSLY.  WHEN THE MISSILE MODE IS ZERO, THE
// C  TARGET POINT IS THE PROJECTED POSITION OF THE HOSTILE 5 SEC IN THE
// C  FUTURE.  WHEN THE MISSILE MODE IS 1, THE AIMPOINT IS USED, AND
// C  WHEN THE MISSILE MODE IS 2 (IMMINENT FIRE), ROT VALUES ARE TURNED
// C  OFF SINCE IT IS NOT DESIRABLE TO INTERFERE WITH AIMING.  THE
// C  IMPORTANCE OF ROT IS BASED UPON A FUNCTION
// C  BORDER(TTIME-TTIME0,TTIMES).  FOR MANEUVERING VERSUS A HOSTILE,
// C  TTIME0 = 3 SEC FOR RANGES LESS THAN 1 NMI AND INCREASES TO 10
// C  SECONDS UNTIL THE RANGE REACHES 3 NMI.  TTIMES = 0.5 SEC.
// C     THE ISSUE OF SPEED BEING COMFORTABLY ABOVE CORNER SPEED IS
// C  TRANSLATED INTO AVAILABLE LIFT TERMS BY COMPARING THE AVAILABLE
// C  LIFT WITH THE MAX LOAD LIMIT.  THE ROT VALUE MULTIPLIER VALROT IS
// C  SCALED BY THE BORDER FUNCTION
// C
// C     BORDER(GMXLFT - (GCAP(1) + GLEEWY, 0.5))
// C
// C  WHERE GCAP(1) IS THE STRUCTURAL LOAD LIMIT.  THE VALUE GLEEWY IS
// C  THERE BECAUSE IT IS NECESSARY TO TURN OFF THE ROT VALUE WHILE THE
// C  SPEED IS STILL ABOVE THE CORNER SPEED (GMXLFT = GCAP(1)) SINCE IF
// C  THIS OCCURS WHILE THE NOSE IS HIGHLY ELEVATED SPEED WILL CONTINUE
// C  TO DECREASE FOR TOO LONG A TIME AFTERWARDS.  GLEEWY IS NORMALLY
// C  SCALED, THEREFORE, BY THE CURRENT CLIMB ANGLE, AND HAS A VALUE OF
// C  4 G'S WHEEN GOING STRAIGHT UP.
// C#VARIABLE DESCRIPTIONS
// C   37  TIMROT   REAL [SEC] - Time flag-roll over the top.
// C   38  VALROT   REAL [NA] - Roll over the top value. UNUSED.
// C  RNG      ARRAY (MM-IDX) OF (REAL) - Range between aircraft
// C  RNGR     ARRAY (MM-IDX) OF (REAL) - Range rate between aircraft
// C  LOFFEN  LOG - CONSIDER A/C OFFENSIVELY - ELSE VALOFF
// C  LDEFEN  LOG - CONSIDER A/C DEFENSIVELY - ELSE VALDEF
// C  MCLINI  ARRAY DIM(MHOST) OF (LOG) - .TRUE. IF E1V1I1 HAS COMPUTED CERTAIN
// C         VARIABLES FOR THE LEAD A/C OF TH
// C  MCLEVL  ARRAY DIM(MHOST) OF (LOG) - .TRUE. IF E1V1 HAS EVALUATED THE LEAD
// C         A/C IN THIS CELL OFFENSIVELY AND DEFENS
// C  SCDES   ARRAY DIM(MM-IDX) OF (REAL) - CURRENT CLOSURE RATE
// C  SCIMP   ARRAY DIM(MM-IDX) OF (REAL) - CLOSURE RATE CHANGE WIDTH
// C  ET      ARRAY DIM(MM-IDX) OF (REAL) - HOSTILE SPECIFIC ENERGY
// C  OBAFUT  ARRAY DIM(MM-IDX) OF (REAL) - PROJECTED OFFBORE ANGLE OF TGT
// C  PSIDNM  ARRAY DIM(MM-IDX) OF (REAL) - OFFBORE ANGLE RATE
// C  VALROT  REAL - R.O.T. VALUE MULTIPL
// C####
void BrawlerEvaluation::Evaluate1v1(BrawlerPlatform& me, UtEntity& aTgt, double& valoff, double& valdef)
{
   // C     --local declarations
   UtVec3d     vtemp   = UtVec3d();
   UtVec3d     dx      = UtVec3d();
   UtVec3d     dv      = UtVec3d();
   UtVec3d     wneed   = UtVec3d();
   UtVec3d     waimp   = UtVec3d();
   UtVec3d     raimp   = UtVec3d();
   UtVec3d     vaimp   = UtVec3d();
   UtFortMat3d rotm    = UtFortMat3d();
   double      scdes   = 0.0; //(mac)
   double      voff    = 0.0;
   double      aturn   = 0.0;
   double      scimp   = 0.0; //(mac)
   double      vloscl  = 0.0;
   double      vpsidt  = 0.0;
   double      bort    = 0.0;
   double      psidtd  = 0.0;
   double      bora    = 0.0;
   double      d10     = 0.0;
   double      d60     = 0.0;
   double      extrae  = 0.0;
   double      ttime0  = 0.0;
   double      dengmx  = 0.0;
   double      rngaim  = 0.0;
   double      wnmag   = 0.0;
   double      rdaimp  = 0.0;
   double      losang  = 0.0;
   double      aspect  = 0.0;
   double      et      = 0.0; //(mac)
   double      ea      = 0.0;
   double      engdes  = 0.0;
   double      erdes   = 0.0;
   double      cosa    = 0.0;
   double      cost    = 0.0;
   double      cpsi    = 0.0;
   double      cpsiv   = 0.0;
   double      vrot    = 0.0;
   double      clmb    = 0.0;
   double      clmbnw  = 0.0;
   double      se      = 0.0;
   double      trate   = 0.0;
   double      ttime   = 0.0;
   double      btime   = 0.0;
   double      scclmb  = 0.0;
   double      b2      = 0.0;
   double      b1      = 0.0;
   double      smag    = 0.0;
   double      phi     = 0.0;
   double      rmin    = 0.0;
   double      d       = 0.0;
   double      dresp   = 0.0;
   double      dt      = 0.0;
   double      dtmax   = 0.0;
   double      gleewy  = 0.0;
   double      wtclos  = 0.0;
   double      gmxlft  = 0.0;
   double      psi     = 0.0;
   double      psidot  = 0.0;
   double      r       = 0.0;
   double      rbap    = 0.0;
   double      scerte  = 0.0;
   double      wt      = 0.0;
   double      obafut  = 0.0; //(mac)
   double      vlsimp  = 0.0;
   double      engds0  = 0.0;
   double      se1     = 0.0;
   double      se2     = 0.0;
   double      dsclmb  = 0.0;
   double      vconeo  = 0.0;
   double      vengd   = 0.0;
   double      vengo   = 0.0;
   double      vloso   = 0.0;
   double      vposd   = 0.0;
   double      vposo   = 0.0;
   double      vposfo  = 0.0;
   double      vtloso  = 0.0;
   double      vtrans  = 0.0;
   double      vtrnsm  = 0.0;
   double      vvisd   = 0.0;
   double      vviso   = 0.0;
   double      vcloso  = 0.0;
   double      gmax    = 0.0;
   double      wtfrnt  = 0.0;
   double      psidnm  = 0.0; //(mac);
   double      ttimes  = 0.0;
   double      e_term  = 0.0;
   double      er_term = 0.0;
   double      ruse    = 0.0;

   // bool loffen;
   bool ldefen = false;
   // bool mclini[17];  //(mhost);
   // bool mclevl[17];  //(mhost)
   bool highe = false;
   bool laimp = false;
   bool virtl = false;
   // int iactgt[17];   //(mhost);
   // int aidtgt[17];   //(mhost)
   // int iac;
   // int idiac;
   // int icel;
   // int ihutil;
   // int mmode;
   int nassign = 0;
   // C*ENDDEC

   // entry e1v1i

   double spdnow = 0;
   spdnow        = me.GetSpeed();

   // if(nhutil == 0)
   if (me.GetTarget() == nullptr || spdnow <= 0.0)
   {
      valoff = 0.0;
      valdef = 0.0;
      return; // goto 9999
   }
   d10 = 10.0 * rad;
   d60 = 60.0 * rad;

   UtVec3d xeua   = me.PositionNED(me.ProjectedState());
   UtVec3d veua   = me.VelocityNED(me.ProjectedState());
   double  spdua  = veua.Magnitude();
   UtVec3d xeut   = me.PositionNED(me.TargetNominalState());
   UtVec3d veut   = me.VelocityNED(me.TargetNominalState());
   double  spdut  = veut.Magnitude();
   UtVec3d xeuan  = me.PositionNED(me.NominalState());
   UtVec3d veuan  = me.VelocityNED(me.NominalState());
   double  spduan = veuan.Magnitude();
   // UtVec3d dxeuan = me.RelativePositionNED(me.NominalState(), me.TargetNominalState());
   UtVec3d dxeuan = xeut - xeuan;
   // UtVec3d dxeua  = me.RelativePositionNED(me.ProjectedState(), me.TargetNominalState());
   UtVec3d dxeua = xeut - xeua;
   // UtVec3d dveuan = me.RelativeVelocityNED(me.NominalState(), me.TargetNominalState()); //earth axes, nominal velocity delta
   UtVec3d dveuan = veut - veuan;
   // UtVec3d dveua  = me.RelativeVelocityNED(aTgt);  //THIS IS WRONG!!!
   UtVec3d dveua = veut - veua; // subvec(veut(1,iac),veua,dveua(1,iac))
   // double rng     = me.RelativePositionNED(aTgt).Magnitude();  //THIS IS WRONG!!!
   double  rng    = dxeua.Magnitude();             // aproj3 -> rng(iac) = xmag(dxeua(1,iac))
   double  rngr   = dxeua.DotProduct(dveua) / rng; // aproj3 -> rngr(iac) = dot(dxeua(1,iac),dveua(1,iac))/rng(iac)
   double  rngun  = dxeuan.Magnitude();            // svpred -> rngun(ispotd) = xmag(dxeuan(1,ispotd))
   UtVec3d dxwua  = me.ConvertNEDtoWind(dxeua);
   UtVec3d dxwut  = me.ConvertNEDtoWind(aTgt, dxeua);
   UtVec3d dvwua  = me.ConvertNEDtoWind(dveua);
   UtVec3d dvwuan = me.ConvertNEDtoWind(me.NominalState(), dveuan);

   ea           = energy(xeuan, veuan);
   engds0       = (-xeuan[3 - 1]) + pow(me.CorneringVelocity_fps(), 2) / (2.0 * grav);
   double psubs = me.MaxForwardAcceleration_fps2() * spdnow / grav; // from perfrm.f
   scerte       = psubs;
   double wmax  = me.MaxTacticalGLoad_g() * grav / spdnow; // from perfrm.f
   vloscl       = (wmax * 0.5 * me.ProjectedTimeDelta());
   // call asstgt(iactgt,aidtgt,virtl,nassign);  //use aTgt
   nassign = 0;
   virtl   = false;
   if (me.GetTarget() != nullptr)
   {
      nassign = 1; // LBM - set these for later
   }
   // call xmit(-nmcel,.false.,mclini); //no cells

   // LBM - dont loop over all nhutil (possible targets hutil.f lists), just evaluate one target
   //"iac" defined by aTgt
   // do 910 ihutil = 1,nhutil
   // for (ihutil = 1; ihutil <= nhutil; ++ihutil)
   //{
   // iac = lhutil(ihutil);
   // LBM - no cells
   // icel = mcelid(iac);
   // if(mclini(icel))
   //{
   //    continue;   //goto 910
   // }
   // mclini(icel) = true;
   // iac = mcelld(icel);

   // C        DESIRED VELOCITY WILL CLOSE TO POINT .5 SEC BEHIND IN 5 SECO
   dx = dxeuan + 4.5 * veut;
   if (dx.Magnitude() < eps)
   {
      return;
   }
   // call vmult(1.0/5.0,dx,vtemp);
   vtemp = dx / 5.0;
   // C        LIMIT VTEMP ACCORDING TO ACHIEVABLE LONG AND TRANSV ACCELERA
   // call vxfrmc(rweuan,vtemp,vtemp,1);
   vtemp = me.ConvertNEDtoWind(vtemp);
   // vtemp(1) = xlimit(vtemp(1),spduan+almax*ProjectedTimeDelta(),spduan+almin*ProjectedTimeDelta());
   vtemp[1 - 1] = xlimit(vtemp[1 - 1],
                         (spduan + me.MaxForwardAccelerationWithGravity_fps2() * me.ProjectedTimeDelta()),
                         (spduan + me.MinForwardAccelerationWithGravity_fps2() * me.ProjectedTimeDelta()));
   vtrans       = sqrt(pow(vtemp[2 - 1], 2) + pow(vtemp[3 - 1], 2));
   vtrnsm       = amin1(grav * me.MaxTacticalGLoad_g() * me.ProjectedTimeDelta(), vtrans);
   vtemp[2 - 1] = (vtrnsm / vtrans) * vtemp[2 - 1];
   vtemp[3 - 1] = (vtrnsm / vtrans) * vtemp[3 - 1];
   // call vxfrmc(rweuan,vtemp,vtemp,2);   //this transforms vtemp from wind to earth axes (vxfrmc(...,2) does transpose
   // multiply) vtemp = ConvertNEDtoWind(vtemp); //TODO - differentiate between nominal & actual
   vtemp = me.ConvertWindtoNED(vtemp); // TODO - is this accurate? did we solve right?  transpose?  //TODO -
                                       // differentiate between nominal & actual

   // call vsub(veut(1,iac),vtemp,dv); //vsub(a,b,c) --> c = a - b
   dv = veut - vtemp;

   // double rngun; //set above
   scdes          = (-dxeuan.DotProduct(dv) / rngun);
   scimp          = amax1(dist(vtemp, veuan), 100.0);
   et             = energy(xeut, veut);
   UtVec3d dxbuan = me.ConvertNEDtoECS(dxeuan);
   double  temp   = dxbuan[1 - 1] / rngun;
   temp           = (temp < -1) ? (-1) : ((temp > 1) ? (1) : (temp));
   obafut         = arccos(temp);
   psidnm         = (obafut - me.OffBoreSightAngle(me.State(), aTgt)) / me.ProjectedTimeDelta();
   //   //910   continue
   //}

   // LBM - I hate myself for utilizing a fake for loop with break statements to replicate this initialization routine
   for (int fake = 0; fake < 1; ++fake)
   {
      // C     --INITIALIZE ROLL-OVER-THE-TOP VALUE
      // if(nbg == 0 || me.GetSpeed() < 0.6f*CorneringVelocity())
      if (me.GetTarget() == nullptr || spdnow < 0.6f * me.CorneringVelocity_fps())
      {
         me.SetRollOverTopValue(0.0);
         break; // return;
      }

      UtVec3d vp    = me.VelocityNED();
      UtVec3d xp    = me.PositionNED();
      UtVec3d vpTgt = me.VelocityNED(aTgt);
      UtVec3d xpTgt = me.PositionNED(aTgt);
      UtVec3d apTgt = me.AccelerationNED(aTgt);

      // C        NO NEED IF ALREADY POINTING UP:
      clmbnw = arcsin(-vp[3 - 1] / spdnow);
      if (clmbnw > d60)
      {
         me.SetRollOverTopValue(0.0);
         break; // return;
      }
      // mmode = iand(mslmd,3)
      if (me.MissileMode() == INTERCEPT && me.MissileMode() == AIMFIRE)
      {
         // call nabort('e1v1...invalid mmode of 3');
         return;
      }

      // if (ppmjid != 0)
      if (me.GetTarget() != nullptr)
      {
         // iac = ppmiac;
         extrae = energy(xp, vp) - energy(xpTgt, vpTgt);
         highe  = extrae > 3000.0 && spdnow > me.GetSpeed(aTgt);

         if (spdnow <= me.CorneringVelocity_fps())
         {
            if (!highe || rng * ftnmi > 1.5)
            {
               me.SetRollOverTopValue(0.0);
               break; // return;
            }
         }
         // C     --ASSERT: FAST OR (NEAR AND HIGHER ENERGY, SPEED)
         GunTracking(xp, vp, xpTgt, vpTgt, apTgt, 1000.0, laimp, raimp, vaimp, waimp, rngaim, se);
         if (!laimp)
         {
            me.SetRollOverTopValue(0.0);
            break; // return;
         }

         // LBM - I hate myself for using a fake for loop with break statements to
         //       replicate this nasty "goto" riddled portion of code
         for (int fake2 = 0; fake2 == 0; ++fake2)
         {
            rdaimp = raimp.DotProduct(vaimp) / rngaim;
            if (rdaimp < 0.0 && rdaimp > -spdnow)
            {
               // C     --ASSERT: CLOSING IN AND NOT NOE-ON
               ttime = amin1(5.0, -rngaim / rdaimp);
               if (me.MissileMode() == AIMFIRE && me.WeaponType() == kndgun)
               {
                  break;
               }
               // C        ROT IF OVERSHOOT IN 2 SECONDS:
               if (ttime > 2.0)
               {
                  break;
               }
               btime = 1.0;

               dsclmb = btime * 60.0 * rad;
               me.SetRollOverTopValue(10.0 * amax1(0.5, btime));

               scclmb = (me.MaxTacticalGLoad_g() - 1.0) * grav * me.ProjectedTimeDelta() / spdnow;
               // timrot = time;
               me.SetRollOverTopTime(me.GetTime()); // TODO - verify?
               break;                               // return;
            }
            // C     --ASSERT: EITHER OPENING OR NOSE-ON
            ttime = 5.0;
         }
         // call cros1(vp,raimp,wneed);
         wneed.CrossProduct(vp, raimp);
         wneed.Normalize();
         // call vecinc(waimp,se/5.,wneed,wneed);   //vecinc(aa,b,bb,cc) -->  CC = AA + B*BB
         wneed = waimp + (se / 5.0) * wneed;
         // wnmag = xmag(wneed);
         wnmag = wneed.Magnitude();
         trate = me.MaximumInstantaneousLift_fps2() / spdnow;
         btime = border(wnmag - trate, grav / spdnow);
         if (btime < 0.2f)
         {
            me.SetRollOverTopValue(0.0);
            break; // return;
         }

         dsclmb = btime * 60.0 * rad;
         me.SetRollOverTopValue(10.0 * amax1(0.5, btime));
         scclmb = (me.MaxTacticalGLoad_g() - 1.0) * grav * me.ProjectedTimeDelta() / spdnow;
         // timrot = time;
         me.SetRollOverTopTime(me.GetTime()); // TODO - verify
         break;                               // return;
      }

      // C     PT 5 SEC AHEAD OF OPPONENT
      if (nassign == 0 || virtl)
      {
         me.SetRollOverTopValue(0.0);
         break; // return;
      }
      // iac = iactgt(1)
      extrae = energy(xp, vp) - energy(xpTgt, vpTgt);
      highe  = extrae > 3000.0 && spdnow > me.GetSpeed(aTgt);
      if (spdnow <= me.CorneringVelocity_fps())
      {
         // if(!highe || rngnow(iac,me)*ftnmi > 1.5)
         if (!highe || rng * ftnmi > 1.5)
         {
            me.SetRollOverTopValue(0.0);
            break; // return;
         }
      }

      // call vecinc(xpTgt,5.0,vpTgt,dx); //vecinc(aa,b,bb,cc) -->  CC = AA + B*BB
      dx = xpTgt + 5.0 * vpTgt;
      // call vsub(dx,xp,dx); //vsub(a,b,c) --> c = a - b
      dx    = dx - xp;
      trate = amax1(5.0, me.MaxTacticalGLoad_g(), me.MaxSustainedGLoad_g()) * grav / spdnow;
      rmin  = spdnow / trate;
      // call makecs(vp,dx,rotm);
      rotm = makecs(vp, dx);
      // call vxfrmc(rotm,dx,dx,1);
      dx   = rotm * dx;
      smag = rmin / sqrt(pow(dx[1 - 1], 2) + pow((rmin - dx[3 - 1]), 2));

      if (std::abs(smag) > 1.0)
      {
         se = pi;
      }
      else
      {
         phi  = atan2(rmin - dx[3 - 1], dx[1 - 1]);
         smag = arcsin(smag);
         se1  = smag - phi;
         if (se1 < 0.0)
         {
            se1 = se1 + twopi;
         }
         se2 = (pi - smag) - phi;
         if (se2 < 0.0)
         {
            se2 = se2 + twopi;
         }
         se = amin1(se1, se2);
         if (se < 0.0)
         {
            se = se + twopi;
         }
         if (se < 0.5)
         {
            me.SetRollOverTopValue(0.0);
            break; // return;
         }
      }
      ttime  = se / trate;
      ttime0 = 3.0 + 7.0 * ramp(1.0, rng * ftnmi, 3.0);
      ttimes = 0.5;
      btime  = border(ttime - ttime0, ttimes);
      // C
      // if(valrot == 0.0 && (time-timrot) <=0.9f)
      if (me.RollOverTopValue() == 0.0 && (me.GetTime() - me.RollOverTopTime()) <= 0.9f)
      {
         break; // return;
      }
      if (btime < 0.2f)
      {
         me.SetRollOverTopValue(0.0);
         break; // return;
      }
      // timrot = time;
      me.SetRollOverTopTime(me.GetTime());
      // gmxlft = MaximumAvailableLift_g(); //gliftm(fmachp,dyprsp,acmasp*grav,iacid);
      gmxlft = me.MaxInstantaneousGLoad_g(); // gliftm(fmachp,dyprsp,acmasp*grav,iacid);

      gleewy = 4.0 * (clmbnw / hafpi);
      if (me.CurrentWeight_lbs() > me.DesignWeightForMaxG_lbs() * grav)
      {
         // gmax = tbl1(fmachp,gcap2,mchval,nmach1);
         gmax = me.StructuralGLimitAboveDesignWeight_g();
      }
      else
      {
         // gmax = tbl1(fmachp,gcap,mchval,nmach1);
         gmax = me.StructuralGLimitAtDesignWeight_g();
      }
      dsclmb = border(gmxlft - (gmax + gleewy), 0.5);
      me.SetRollOverTopValue(10.0 * btime);
      dsclmb = 45.0 * rad * dsclmb;
      scclmb = (me.MaxTacticalGLoad_g() - 1.0) * grav * me.ProjectedTimeDelta() / spdnow;
      // return;
   }
   // end of e1v1i  (initialization)


   // LBM - definitely doing an offensive evaluation

   // iac = jac;
   // idiac=iacidx[iac-1];
   // C     --OFFENSIVE IF THIS IS ASSIGNED TARGET
   // loffen = !virtl && lmatch(aidtgt,nassign,idiac);
   ////C     --OR IF THIS IS THE SELECTED TARGET
   // loffen = loffen || idiac == ppmjid;
   // ldefen = sem2(iac,me) > 0.25 && (sem2(iac,me) > 0.50f || patk(iac,me) > 0.3333f);
   // ldefen = sem2(iac,me) > 0.25;  //LBM - assume patk == 1 ???
   ldefen = SelfEngagementMeasure(me, me.State(), aTgt) > 0.25; // LBM - assume patk == 1 ???
   valoff = 0.0;
   valdef = 0.0;
   // if(!(loffen || ldefen))
   //{
   //    return;
   // }

   // icel = mcelid(iac);
   // LBM - no cells
   // if(mclevl(icel))
   //{
   //    return;
   // }
   // mclevl(icel) = true;
   // iac = mcelld(icel);
   // C     --THESE ARE NEEDED FOR OFFENSIVE AND DEFENSIVE COMPONENTS:
   // C     --SCALE DOWN SLIGHTLY TO AVOID ROUNDOFF PROBLEMS


   // LBM - do all this at beginning of method
   // UtVec3d xeua   = me.PositionNED(me.ProjectedState());
   // UtVec3d veua   = me.VelocityNED(me.ProjectedState());
   ////UtVec3d xeut   = PositionNED(me.TargetNominalState());
   ////UtVec3d veut   = VelocityNED(me.TargetNominalState());   //already have

   // UtVec3d dxeua = me.RelativePositionNED(me.ProjectedState(), me.TargetNominalState());
   // UtVec3d dxwua  = me.ConvertNEDtoWind(dxeua);
   // UtVec3d dxwut  = me.ConvertNEDtoWind(aTgt, dxeua);

   // UtVec3d dveua  = me.RelativeVelocityNED(aTgt);
   // UtVec3d dvwua  = me.ConvertNEDtoWind(dveua);
   // UtVec3d dveuan = me.RelativeVelocityNED(me.NominalState(), me.TargetNominalState()); //earth axes, nominal
   // velocity delta UtVec3d dvwuan = me.ConvertNEDtoWind(me.NominalState(), dveuan);

   // double  rngr   = dxeua.DotProduct(dveua) / rng; //aproj3 - rngr(iac) = dot(dxeua(1,iac),dveua(1,iac))/rng(iac)

   d    = dxwua[1 - 1] * 0.999999;
   cosa = d / rng;
   // C     --COST IS COSINE OF ASPECT, NOT OBA, BECAUSE DXWUT POINTS FROM ME TO HOST
   cost   = dxwut[1 - 1] * 0.999999 / rng;
   losang = arccos(cosa);                                          // line of sight angle
   aspect = arccos(cost);                                          // aspect angle
   voff   = arccos(veua.DotProduct(veut) / (spdua * spdut + 1.0)); // voff = ~angle between velocity vectors
   aturn  = pi - voff;
   ea     = energy(xeua, veua);

   // if(!loffen)
   //{
   //    //goto 200;
   // }
   // C
   // C     --OFFENSIVE VALUE
   // C
   // C     --BYPASS POSITION, CLOSURE, CONE WHEN TARGET BEHIND ME AND EITHER
   // C     --RANGE OR RUNNING:
   // if(obang(me,iac) > hafpi)  //obang = offbore angle from 'me' to 'iac'
   if (me.OffBoreSightAngle(me.State(), aTgt) > hafpi)
   {
      vposo  = 0.0;
      vposfo = 0.0;
      vconeo = 0.0;
      wtclos = 0.0;
   }
   else
   {
      // C     --POSITION COMPONENT
      wtfrnt = ramp(hafpi, me.OffBoreSightAngle(me.State(), aTgt), d60);
      dresp  = amax1(100.0, spdut * 0.5);
      vposo  = border(d - dresp, dresp);
      // C
      // C     --OPPONENT IN FRONT OF ME:
      // vposfo = vposo+0.75*(1.-vposo)*border(dvwua(1,iac)-dvwuan(1,iac),grav*ProjectedTimeDelta());
      vposfo = vposo + 0.75 * (1.0 - vposo) * border(dvwua[1 - 1] - dvwuan[1 - 1], grav * me.ProjectedTimeDelta());
      // C
      // C     --OPPONENT IN MY CONE AND I BEHIND OPPONENT:
      bort   = border(hafpi - aspect, d10);
      bort   = bort + 0.75 * (1. - bort) * border((pi - me.OffBoreSightAngle(aTgt, me.State())) - aspect, vloscl);
      bora   = border(d60 - losang, d10);
      bora   = bora + 0.75 * (1. - bora) * border(me.OffBoreSightAngle(me.State(), aTgt) - losang, vloscl);
      vconeo = bora * bort;
      // C
      // C     --SEE IF TOO FAR BEHIND
      ruse = amin1(6000.0, me.MinInstantaneousTurnRadius_ft());
      // C     --Next AMAX1 prevents bombs when target is stationary
      r = ruse * spdua * aturn / amax1(spdut, 1.0);
      // C     AMAX1 IN NEXT STATEMENT PREVENTS BOMB WHEN AC ARE EXACTLY HE
      vposo = border(r - d, amax1(0.2f * r, 1.0)) * vposo;
      // C     --AVOID BEING TOO FAR TO THE SIDE
      dtmax = pow(d, 2) / (4.0 * me.MinTacticalTurnRadius_ft());
      dt    = sqrt(pow(rng, 2) - pow(d, 2));
      vposo = border(dtmax - dt, 0.5 * dtmax) * vposo;
      // C
      // C     --CLOSURE RATE COMPONENT
      wtclos = 2.0;
      if (d > 0.0 && d <= 5000.0 && cost > 0.0)
      {
         wtclos = 4.0;
      }
      if (rngun * ftnmi > 4.0)
      {
         vcloso = border(-rngr - scdes, scimp);
      }
      else
      {
         vcloso = cauchy(-rngr - scdes, scimp);
      }
   }
   // C     --ENERGY COMPONENT
   // C     --DES ENGERGY WHEN OPP AND ME BEHIND EACHOTHER IS ENERGY AT MACH 1
   // C     --ELSE IT IS OPP ENERGY + 20000 FT
   wt      = amax1(-cosa, 0.0) * amax1(cost, 0.0);
   engdes  = wt * engds0 + (1.0 - wt) * (et + 20000.0);
   engdes  = vposo * et + (1.0 - vposo) * engdes;
   erdes   = xlimit((engdes - ea) / 5.0, scerte, -scerte);
   e_term  = cauchy(ea - engdes, 5000.0);
   er_term = (1.0 - e_term) * cauchy(me.SpecificEnergyRate_fps() - erdes, scerte);
   vengo   = e_term + er_term;
   // C
   // C     --VISIBILITY COMPONENT
   UtVec3d dxbua = me.ConvertNEDtoECS(me.RelativePositionNED(aTgt));
   UtVec3d dvbua = me.ConvertNEDtoECS(me.RelativeVelocityNED(aTgt));
   cpsi          = dxbua[1 - 1] / rng;
   cpsi          = (cpsi < -1) ? (-1) : ((cpsi > 1) ? (1) : (cpsi));
   psi           = arccos(cpsi);
   rbap          = sqrt(pow(dxbua[2 - 1], 2) + pow(dxbua[3 - 1], 2));
   psidot        = (dxbua[1 - 1] * rngr - rng * dvbua[1 - 1]) / (rng * rbap);
   // C     --ADD MY RATE TERM:
   UtVec3d abody = me.AccelerationECS();
   psidot        = psidot - (dxbua[2 - 1] * abody[2 - 1] + dxbua[3 - 1] * abody[3 - 1]) / (spdua * rbap);
   cpsiv         = (0.500f * dxbua[1 - 1] - 0.866f * dxbua[3 - 1]) / rng;
   // C                   .9659=COS15; .0262=COS15-COS20
   vviso = 0.5 * border(cpsi - 0.9659f, 0.0262f) + 0.5 * border(cpsiv, 0.1f);
   // C
   // C     --LOS ANGLE SMALL; LOS RATE OK:
   vloso  = 0.6f * cauchy(psi, 30.0 * rad) + 0.4f * cauchy(psi, 15.0 * rad);
   psidtd = amax1(-10.f * rad, -psi / 5.0);
   vtloso = cauchy(psidot - psidtd, 5. * rad);
   vpsidt = border(psidnm - psidot - 0.5 * me.MaxAngularRate_rps(), 0.5 * me.MaxAngularRate_rps());
   vtloso = vloso * vtloso + (1.0 - vloso) * vpsidt;
   vlsimp = border(obafut - psi - vloscl, vloscl);
   vloso  = vloso + 0.50f * (1.0 - vloso) * vlsimp;
   // C
   // C     --ROLL-OVER-THE-TOP VALUE
   vrot = 0.0;
   // if(valrot != 0.0 && !diseng)
   if (me.RollOverTopValue() != 0.0) // && !diseng)  /LBM - no disengaging right now
   {
      clmb = arcsin(-veua[3 - 1] / spdua);
      b1   = border(clmb - clmbnw, scclmb) / 0.8;
      if (b1 > 1.0)
      {
         b1 = 1.0;
      }
      b2   = border(clmb - dsclmb, 0.175);
      vrot = b2 + (1.0 - b2) * b1;
   }
   // C
   // C     --NET OFFENSIVE VALUE:
   if (wtclos == 0.0)
   {
      // C        --"BEHIND ME" SITUATION:
      valoff = 1.0 * vengo + 2.0 * vtloso + 2.0 * vviso + me.RollOverTopValue() * vrot + 2.0 * vloso;
      valoff = valoff / (7.0 + me.RollOverTopValue());
   }
   else
   {
      valoff = 2.0 * vengo + 2.0 * vloso + 2.0 * vtloso + 2.0 * vviso + me.RollOverTopValue() * vrot;
      valoff = valoff + wtfrnt * (2.0 * vposo + 1.0 * vposfo + 1.0 * vconeo + wtclos * vcloso);
      valoff = valoff / (8.0 + me.RollOverTopValue() + wtfrnt * (4.0 + wtclos));
   }

   // C
   // C
   // C     --DEFENSIVE VALUE
   // 200   continue
   if (!ldefen)
   {
      return;
   }
   // C
   // C     --POSITION COMPONENT
   vposd = WeaponTimeSelfEngageMeasure(xeut, veut, xeua, veua);
   // C
   // C     --ENERGY COMPONENT
   engdes = vposd * et + (1.0 - vposd) * (et + 20000.0);
   // C     --dengmx = 0.5*spdut(iac)*ProjectedTimeDelta()
   dengmx = 0.5 * amax1(spdut, 300.0) * me.ProjectedTimeDelta();
   engdes = xlimit(engdes, et + dengmx, et - dengmx);
   vengd  = cauchy(et - engdes, dengmx);
   // C
   // C     --VISIBILITY COMPONENT
   UtVec3d dxbut = me.ConvertNEDtoECS(aTgt, me.RelativePositionNED(aTgt));
   cpsi          = -dxbut[1 - 1] / rng;
   cpsiv         = (-0.500f * dxbut[1 - 1] + 0.866f * dxbut[3 - 1]) / rng;
   // C     --.9659 = COS15; .0262=COS15-COS20
   vvisd = 0.5 * border(cpsi - 0.9659f, 0.0262f) + 0.5 * border(cpsiv, 0.1f);
   // C
   valdef = (4.0 * vposd + 1.0 * vengd + 1.0 * vvisd);
   valdef = valdef / 6.0;
   // C
}


// C#ABSTRACT COMPUTES DATA NEEDED FOR GUN TRACKING
// C#PURPOSE  THIS SUBROUTINE EXCEEDS THE FUNCTION OF THE AIMP SUBROUTINE
// C  BY COMPUTING THE ANGULAR RATE OF THE AIMPOINT RELATIVE TO A
// C  MOVING, BUT NON-ROTATING ATTACKER. THE DATA IS NEEDED TO
// C  ACHIEVE AND MAINTAIN A GUN SOLUTION.
// C#PARAMETER DESCRIPTIONS
// CIN  XA     3-VEC - ATTACKER POSITION
// CIN  VA     3-VEC - ATTACKER VELOCITY
// CIN  XT     3-VEC - TARGET POSITION
// CIN  VT     3-VEC - TARGET VELOCITY
// CIN  AT     3-VEC - TARGET ACCELERATION
// CIN  SBUL   REAL - BULLET SPEED RELATIVE TO ATTACKER
// COUT LAIMP  LOG - .TRUE. IF AN AIMPOINT SOLUTION EXISTS: REMAINING
// C           OUTPUT VARIABLES ARE ONLY DEFINED IF LAIMP=.TRUE.
// COUT RAIMP  3-VEC - POSITION OF AIMPT RELATIVE TO ATTACKER
// COUT VAIMP  3-VEC - VELOCITY OF AIMPT RELATIVE TO ATTACKER
// COUT WAIMP  3-VEC - ANGULAR RATE VECTOR OF RAIMP FOR NON-TURNING ATTACKER
// COUT RNGAP  REAL - MAGNITUDE OF RAIMP
// COUT SENOW  REAL - CURRENT STEERING ERROR
// C#TECHNICAL DESCRIPTION
// C     STRAIGHTFOWARD VECTOR CALCULUS. DERIVATION IS TOO LENGTHY
// C  FOR HERE. USES LAW OF SINES ALOT, AND TIME DERIVATIVES OF FUNCTIONS
// C  OF VECTORS.
// C#VARIABLE DESCRIPTIONS
// C  D       REAL - TARGET ASPECT ANGLE
// C  G       REAL - REQUIRED LEAD ANGLE
// C  R       3-VEC - RANGE VECTOR
// C  SB      REAL - BULLET SPEED
// C  K       REAL - BULLET TIME-OF-FLIGHT
// C####
void BrawlerEvaluation::GunTracking(const UtVec3d& xa,
                                    const UtVec3d& va,
                                    const UtVec3d& xt,
                                    const UtVec3d& vt,
                                    const UtVec3d& at,
                                    double         sbul,
                                    bool&          laimp,
                                    UtVec3d&       raimp,
                                    UtVec3d&       vaimp,
                                    UtVec3d&       waimp,
                                    double&        rngap,
                                    double&        senow)
{
   // C     --local declarations
   double k    = 0.0;
   double kdot = 0.0;
   double rmag = 0.0;
   // double rdot = 0.0;
   double sind = 0.0;
   double cosd = 0.0;
   double sing = 0.0;
   double cosg = 0.0;
   double x    = 0.0;
   double z    = 0.0;
   double w    = 0.0;
   double sb   = 0.0;
   double sa   = 0.0;
   double st   = 0.0;

   UtVec3d r     = UtVec3d();
   UtVec3d vr    = UtVec3d();
   UtVec3d u     = UtVec3d();
   UtVec3d uu    = UtVec3d();
   UtVec3d rnorm = UtVec3d();
   // C*ENDDEC

   // C     --COMPUTE BASIC VARIABLES:
   sa = va.Magnitude();
   sb = sa + sbul;
   st = vt.Magnitude();
   // call vsub(xt,xa,r);  //vsub(a,b,c) --> c = a - b
   r    = xt - xa;
   rmag = r.Magnitude();
   // call vmult(1.0/rmag,r,rnorm);
   rnorm = (1.0 / rmag) * r;
   // call vsub(vt,va,vr); //vsub(a,b,c) --> c = a - b
   vr = vt - va;
   // rdot = vr.DotProduct(rnorm);

   if (st == 0.0)
   {
      sing  = 0.0;
      cosg  = 1.0;
      x     = sb;
      z     = 0.0;
      laimp = true;
   }
   else
   {
      cosd = (0.999999f) * rnorm.DotProduct(vt) / st;
      sind = sqrt((1.0 - cosd) * (1.0 + cosd));
      // C        --COMPUTE AIMPT, USING LAW OF SINES:
      sing  = (st / sb) * sind;
      laimp = sing < 1.0;
      if (!laimp)
      {
         // goto 9999
         return;
      }
      cosg = sqrt((1.0 - sing) * (1.0 + sing));
      x    = sb * cosg;
      z    = st * cosd;
   }
   w = x - z;
   if (std::abs(w) <= eps)
   {
      // C        --BULLET OVERTAKE SPEED PROJECTED ONTO
      // C        --THE LINE OF SIGHT IS (PRACTICALLY) ZERO.
      laimp = false;
      // goto 9999
      return;
   }
   k = rmag / w;
   // call vecinc(r,k,vt,raimp); //vecinc(aa,b,bb,cc) -->  CC = AA + B*BB
   raimp = r + (k)*vt;
   rngap = raimp.Magnitude();
   // C     --NOW COMPUTE VELOCITY OF AIMPT, VAIMP
   // C     --U IS PART OF VT NORMAL TO R
   // call vecinc(vt,-z,rnorm,u);   //vecinc(aa,b,bb,cc) -->  CC = AA + B*BB
   u = vt + (-z) * rnorm;
   // call vecinc(rnorm,1./x,u,uu); //vecinc(aa,b,bb,cc) -->  CC = AA + B*BB
   uu = rnorm + u / x;

   kdot = vr.DotProduct(uu) / w;
   // call veclin(kdot,vt,k,at,vaimp);  //veclin(a,aa,b,bb,cc)  -->  cc = a * aa + b * bb
   vaimp = kdot * vt + k * at;
   // call vsum(vr,vaimp,vaimp);
   vaimp = vr + vaimp;
   // C     --NOW GET WAIMP AND SENOW:
   senow = va.DotProduct(raimp) / ((sa + 0.001f) * rngap);
   senow = arccos(senow);
   // call cross(raimp,vaimp,waimp);
   waimp.CrossProduct(raimp, vaimp);
   // call vmult(1.0/pow(rngap,2),waimp,waimp);
   waimp = (1.0 / pow(rngap, 2)) * waimp;
   // C
   // 9999  continue
   return;
}


// C#ABSTRACT EVALUATES SELF-ENGAGMENT MEASURES FOR AN A/C PAIR
// C#PURPOSE  EVALUATES THE HIGH LEVEL SURROGATE KILL PROBABILITIES FOR
// C  A PAIR OF AIRCRAFT.
// C#PARAMETER DESCRIPTIONS
// CIN  IAC    MM-IDX - FIRST AIRCRAFT
// CIN  JAC    MM-IDX - SECOND AIRCRAFT
// COUT SEM2IJ REAL - SURROGATE KILL PROBABILITY OF IAC VS JAC
// COUT SEM2JI REAL - SURROGATE KILL PROBABILITY OF JAC VS IAC
// C#TECHNICAL DESCRIPTION
// C     THE INITIAL SECTION OF CODE SETS UP VARIABLES USED BY THE SEM2F
// C  FUNCTION, WHICH EVALUATES THE "RAW" SURROGATE KILL PROBABILITIES.
// C  GSEM2 THEN MAKES AN ADJUSTMENT FOR THE COMBAT EFFECTIVENESS
// C  FACTOR.  THE HEURISTIC VIEWS THE OUTPUT OF SEM2F AS A SHORT-TERM
// C  SURROGATE KILL PROBABILITY, THAT IS, STRONGLY GEOMETRY DEPENDENT.
// C  THE COMBAT EFFECTIVENESS FACTORS ARE USED, TOGETHER WITH WEAPON
// C  RANGE CHARACTERISTICS TO FORM SIJLNG AND SJILNG, THE SURROGATE
// C  PROBABILITIES FOR LONG ENGAGEMENTS WITH RANDOM STARTING
// C  CONDITIONS.  WHEN THE SHORT-TERM KILL PROBABILITIES ARE LOW THERE
// C  IS A GREATER CHANCE THAT THE LONG TERM FACTORS ARE IMPORTANT, AND
// C  THEY GET MORE WEIGHT (IN THAT CASE) IN AN AVERAGING OF THE SHORT
// C  AND LONG TERM FACTORS TO PRODUCE A NET SEM2.
// C#VARIABLE DESCRIPTION
// C  IACIDT   ARRAY (MM-IDX) OF (AC-IND) [NA] - Actual aircraft ID.   Identical  to
// C           IACIDX.
// C####
double BrawlerEvaluation::SelfEngagementMeasure(BrawlerPlatform& me, UtEntity& aRef, UtEntity& aTgt)
{
   // double posi(3),posj(3),veli(3),velj(3);
   double     rwpni  = 0.0;
   double     rwpnj  = 0.0;
   double     sijsht = 0.0;
   double     sjisht = 0.0;
   double     sijlng = 0.0;
   double     sjilng = 0.0;
   double     wt     = 0.0;
   bool       allasp = false;
   bool       lkdown = false;
   WeaponType kndbst = nowepn;
   // real    BORDER,CAUCHY,EGYWID,ENERGY,FR0   ,SEM2F ,XLIMIT
   double egywid = 5000.0;
   double fr0    = 0.5;
   // C
   // C    GET POSITIONS AND VELOCITIES OF IAC AND JAC
   // C    IF EITHER AC IS SELF USE /EXTSTAT/ OTHERWISE USE /MIND3/

   // integer iac   ,jac
   double aspect = 0.0;
   double egysem = 0.0;
   double ei     = 0.0;
   double ej     = 0.0;
   double ofbore = 0.0;
   double range  = 0.0;
   double rij    = 0.0;
   double sem2ij = 0.0;
   // double sem2ji = 0.0;
   double spdi = 0.0;
   double spdj = 0.0;
   // C*ENDDEC
   // call xmit(3,xp(1,iac),posi)
   // call xmit(3,vp(1,iac),veli)
   // call xmit(3,xp(1,jac),posj)
   // call xmit(3,vp(1,jac),velj)
   UtVec3d posi = me.PositionNED(aRef);
   UtVec3d posj = me.PositionNED(aTgt);
   UtVec3d veli = me.VelocityNED(aRef);
   UtVec3d velj = me.VelocityNED(aTgt);

   spdi = me.GetSpeed(aRef); // veli.Magnitude();   //spdnow(iac)
   spdj = me.GetSpeed(aTgt); // velj.Magnitude();   // spdnow(jac)
   // C    COMPUTE SEM2IJ
   UtVec3d dpos = posj - posi;
   range        = dpos.Magnitude();                      // rngnow(iac,jac);
   ofbore       = me.OffBoreSightAngle(aRef, aTgt);      // obang(iac,jac);
   aspect       = pi - me.OffBoreSightAngle(aTgt, aRef); // obang(jac,iac);
   ei           = energy(posi, veli);
   ej           = energy(posj, velj);
   egysem       = border(ei - ej, egywid);
   GetBestWeapon(me, aRef, kndbst, allasp, lkdown);
   if (kndbst == nowepn)
   {
      kndbst = kndgun;
   }
   rwpni = me.GetMIND()->RangeWeapon(kndbst);
   rij   = me.CombatEffectiveness(aRef) / me.CombatEffectiveness(aTgt);
   // double rr = me.RangeRate(aTgt,aRef); // Currently unused
   // double rr2 = me.RangeRate(aRef,aTgt); // Currently unused
   sijsht =
      NominalSelfEngagementMeasure(range, me.RangeRate(aTgt, aRef), ofbore, aspect, egysem, spdj, posi, posj, rwpni, allasp, lkdown);
   sijsht = sijsht * xlimit(rij, 1.0, 0.5);
   // call getarm(iacidx(jac),kndbst,allasp,lkdown);
   GetBestWeapon(me, aTgt, kndbst, allasp, lkdown);
   if (kndbst == nowepn)
   {
      kndbst = kndgun;
   }
   rwpnj  = me.GetMIND()->RangeWeapon(kndbst);
   sjisht = NominalSelfEngagementMeasure(range,
                                         me.RangeRate(aTgt, aRef),
                                         pi - aspect,
                                         pi - ofbore,
                                         1. - egysem,
                                         spdi,
                                         posj,
                                         posi,
                                         rwpnj,
                                         allasp,
                                         lkdown);
   sjisht = sjisht * xlimit(1. / rij, 1.0, 0.5);
   // C  ADJUST SEM2'S FOR COMBAT EFFECTIVENESS FACTOR
   rwpni  = amax1(rwpni, 60000.0);
   rwpnj  = amax1(rwpnj, 60000.0);
   sijlng = cauchy(range, rwpni);
   sijlng = cauchy(1.0 / rij, fr0) * sijlng;
   sjilng = cauchy(range, rwpnj);
   sjilng = cauchy(rij, fr0) * sjilng;
   wt     = (1.0 - sijsht) * (1.0 - sjisht) * 0.5;
   sem2ij = sijsht + sijlng * wt;
   // sem2ji = sjisht+sjilng*wt;  //TODO - only compute from ref to tgt
   return sem2ij;
}


// C#ABSTRACT DETERMINES BEST WEAPON ONBOARD SPECIFIED AIRCRAFT
// C#PURPOSE  RETURNS INFORMATION ON THE LONGEST-RANGE WEAPON SYSTEM ON
// C  A SPECIFIED A/C.  THE INFORMATION INCLUDES WEAPON CLASS (KNDMSL OF
// C  /MSLDAT/) ALL-ASPECT CAPABILITY (DEFINED AS ABILITY TO SHOOT AT
// C  ASPECT ANGLES GREATER THAN 90 DEG, AND LOOKDOWN-SHOOTDOWN
// C  CAPABILITY.
// C#PARAMETER DESCRIPTIONS
// CIN  JACID  AC-IDX - A/C OF INTEREST
// COUT KNDBST INT - WEAPON CLASS OF LONGEST-RANGE SYSTEM.  DEFINED
// C           LIKE KNDMSL OF /MSLDAT/
// COUT ALLASP LOG - .TRUE. IF CAPABLE OF SHOTS AT ASPECTS GREATER THAN
// C           90 DEG
// COUT LKDOWN LOG - .TRUE. IF CAPABLE OF LOOKDOWN-SHOOTDOWN
// C#TECHNICAL DESCRIPTION
// C     VARIABLE ARMNT(JACID) IN /EXTST/ CONTAINS A 10-BIT BYTE FOR
// C  EACH WEAPON SYSTEM ON THE A/C.  IN EACH BYTE, BITS 1-3 HOLD THE
// C  WEAPON'S VALUE OF KNDMSL, OR KNDGUN OR KNDDEW.
// C  THE ROUTINE FINDS THE LONGEST RANGE WEAPON THAT THE AIRCRAFT TYPE
// C  CARRIES, IF ANY AMMO OF THAT TYPE REMAINS (IN THE OPINION OF THE
// C  CALLING PILOT).
// C  IT ASSUMES THE RANGES, FROM SHORTEST TO LONGEST, ARE:
// C   GUN, DEW, SRM, MRM, LRM.
// C  BIT 4 INDICATES ALL-ASPECT CAPABILITY, AND BIT 5 INDICATES
// C  LOOKDOWN-SHOOTDOWN CAPABILITY.  SUBROUTINE SETARM IN MODULE TACFL
// C  INITIATES ARMNT(JACID).
// C#VARIABLE DESCRIPTIONS:  none
// C####
void BrawlerEvaluation::GetBestWeapon(BrawlerPlatform& me, UtEntity& aRef, WeaponType& kndbst, bool& allasp, bool& lkdown)
{
   // if (aRef == 0)
   //{
   //    kndbst = nowepn;
   //    allasp = false;
   //    lkdown = false;
   //    return;
   // }
   // int order[5] = {2,3,4,0,1};
   // C     --Indeces for order are: SRM, MRM, LRM, GUN, DEW.
   // C*ENDDEC
   // tstbst = -1;
   // kndbst = nowepn;  //0
   ////do 10 i=1,numwpn
   // for (int i=1; i<= numwpn; ++i)
   //{
   //    byte = armnt(i,jacid);
   //    test1 = iand(armnt(i,jacid),7);
   //    if (test1 != 0)
   //    {
   //       //C           --Check to see if there are any weapons of this type left.
   //       //C           --Note that if this is not a flight-mate, wpn_out will
   //       //C           --always be false, since you dont know if he has any left,
   //       //C           --so you must assume he has some.
   //       if (!wpn_out(test1,mmindx(jacid)))
   //       {
   //          test = order(test1);
   //          if(test > tstbst)
   //          {
   //             tstbst = test;
   //             kndbst = byte;
   //          }
   //       }
   //    }
   ////10    continue
   //}
   // allasp = lbit(kndbst,4);
   // lkdown = lbit(kndbst,5);
   // kndbst = iand(kndbst,7);
   // LBM only check first weapon for now


   // if (me.GetWeaponCount() == 0 || me.GetWeaponEntry(0)->GetQuantityRemaining() < 1)
   if (!me.HaveWeapons())
   {
      allasp = false;
      lkdown = false;
      kndbst = nowepn; // 0
   }
   else
   {
      allasp = (me.MaxWeaponAspectOffsetAngle() > hafpi); // from setarm.f line 78
      lkdown = me.LookDownShootCapable();
      kndbst = kndsrm;
   }
   return;
}


// C#ABSTRACT COMPUTES THE NOMINAL SEM2 OF ATTACKER VS TARGET
// C#PURPOSE  COMPUTES THE SEM2 OF ATTACKER VERSUS TARGET, NOT
// C  CONSIDERING COMBAT EFFECTIVENESS FACTORS.
// C#TYPE     SITUATION ASSESSMENT
// C#PARAMETER DESCRIPTIONS
// CIN  RANGE  REAL - RANGE TO TARGET
// CIN  RDOT   REAL - RANGE RATE
// CIN  OFBORE REAL - OFFBORE ANGLE OF TGT, RELATIVE TO ATTACKER
// CIN  ASPECT REAL - TGT ASPECT ANGLE
// CIN  EGYSEM REAL - SPECIFIC ENERGY ADVANTAGE FACTOR, NORMALIZED
// CIN  SPDTGT REAL - TARGET SPEED
// CIN  XA     3-VEC - ATTACKER POSITION VECTOR
// CIN  XT     3-VEC - TARGET POSITION
// CIN  RBORD  REAL - RANGE DEFINING MEDIUM (WEAPONS) RANGE
// CIN  ALLASP LOG - .TRUE. IF ATTACKER HAS ALL ASPECT WEAPONS
// CIN  LKDOWN LOG - .TRUE. IF ATTACKER HAS LOOKDOWN-SHOOTDOWN
// COUT SEM2   REAL - NOMINAL SEM2 OF ATTACKER VS TARGET
// C#TECHNICAL DESCRIPTION
// C     THE METHODOLOGY IS TOO DETAILED TO BE GIVEN INLINE.  PLEASE REFER TO
// C  SECTION ON SITUATION ASSESSMENT IN THE ANALYST MANUAL.
// C#VARIABLE DESCRIPTION
// C####
double BrawlerEvaluation::NominalSelfEngagementMeasure(double         range,
                                                       double         rdot,
                                                       double         ofbore,
                                                       double         aspect,
                                                       double         egysem,
                                                       double         spdtgt,
                                                       const UtVec3d& xa,
                                                       const UtVec3d& xt,
                                                       double         rbord,
                                                       bool           allasp,
                                                       bool           lkdown)
{
   double theta  = 0.0;
   double temp1  = 0.0;
   double temp2  = 0.0;
   double reff   = 0.0;
   double rfut   = 0.0;
   double v1     = 0.0;
   double v2     = 0.0;
   double v3     = 0.0;
   double rngfcl = 0.0;
   double rngfcm = 0.0;
   double rngfcs = 0.0;
   double angfcl = 0.0;
   double angfcm = 0.0;
   double angfcs = 0.0;
   double seml   = 0.0;
   double semm   = 0.0;
   double sems   = 0.0;
   double shrink = 0.0;
   double ahor   = 0.0;
   double adown  = 0.0;
   double ovrhor = 0.0;
   double wtalt  = 0.0;
   double wtovr  = 0.0;
   double altfac = 0.0;
   double ovrfac = 0.0;
   bool   masked = false;
   // C
   // C  COMPUTE EFFECTIVE RANGE - NORMALIZED TO BEAM. USES AN
   // C    APPROXIMATION OF TGT TURNING
   // C  TO BEAM AT 5 G'S AND ESTIMATES RANGE WHEN HE REACHES THE BEAM.
   // C
   // double    BORDER,CAUCHY,RAMP
   // C*ENDDEC
   theta = aspect - hafpi;
   if (theta == 0.0)
   {
      reff = range;
   }
   else
   {
      temp1 = std::abs(theta) * spdtgt / (5.0 * grav);
      temp2 = spdtgt * (sin(theta) - (1.0 - cos(theta)) / theta);
      reff  = range + temp1 * (rdot + temp2);
   }
   // C
   // C  SEML INTENDED TO BE HIGH IF TARGET IN GENERAL VICINITY
   // C
   rngfcl = border(100000.0 - reff, 50000.0) / 0.9;
   angfcl = (0.3 * border(hafpi - ofbore, hafpi) + 0.7 * border(aspect - 0.75 * pi, pi / 4.0)) / 0.8;
   seml   = rngfcl * (0.4 + 0.6 * angfcl);
   // C
   // C  SEMM INTENDED TO BE HIGH IF HOSTILE IN WEAPON RANGE.
   // C  GETARM RETURNS WEAPON TYPE OF LONGEST RANGE
   // C
   rngfcm = border(rbord - reff, 0.25 * rbord);
   angfcm = 1.0 - (0.5 * ofbore + 0.5 * aspect) / pi;
   // C  SHRINK FACTORS IN INABILITY TO USE WEAPON BECAUSE OF LKDOWN OR NO-ALLASP
   if (!lkdown)
   {
      // goto 10
   }

   shrink = 1.0;
   // goto 20
   // 10
   AngleAboveHorizon(xa, xt, ahor, adown, ovrhor, masked);
   wtalt  = border(reff - 18000.0, 18000.0);
   wtovr  = 1.0 - wtalt;
   altfac = border(-xt[3 - 1] - 5000.0, 5000.0);
   ovrfac = border(ovrhor, 5.0 * rad);
   shrink = 0.4 + 0.6 * (wtalt * altfac + wtovr * ovrfac);
   // 20    continue
   if (!allasp)
   {
      shrink = shrink * cauchy(aspect, hafpi);
   }

   semm = rngfcm * (0.3 + 0.7 * angfcm) * shrink;
   // C
   // C  SEMS INTENDED TO BE HIGH AT SHORT RANGES
   // C
   rngfcs = border(15000.0 - reff, 3750.0);
   angfcs = 1.0 - (0.5 * ofbore + 0.5 * aspect) / pi;
   sems   = rngfcs * (0.4 + 0.6 * angfcs) * (0.7 + 0.3 * egysem);
   // C
   // C  ADJUSTMENTS FOR FUTURE SHOTS - RMK 6 OCT 1980
   // C
   rfut = amax1(0.0, range + rdot * 10.0);
   // C  WILL HE BE IN WEAPONS RANGE IN 10 SECONDS?
   v1   = ramp(rbord, rfut, 0.0);
   semm = 0.5 * semm + 0.5 * v1 * shrink * angfcm;
   // C  WILL HE BE IN 2 NM IN 10 SECONDS?
   v2 = ramp(2.0, rfut * ftnmi, 1.0);
   // C  CURRENT GOOD SHOT?
   v3   = angfcs * ramp(2.0, range * ftnmi, 1.0);
   sems = v3 + (1.0 - v3) * 0.60 * (0.6 * sems + 0.4 * v2);
   // C
   // C  SEM2F COMPUTED SO THAT IF SHORT RANGE FACTOR IS SUFFICIENTLY HIGH,
   // C  LONG RANGE TERMS ARE IRRELEVANT
   return (sems + 0.75 * (1.0 - sems) * (semm + 0.50 * (1.0 - semm) * seml));
}


// C#ABSTRACT SCORES SEM'S VIA WEAPON TIME CONCEPT
// C#PURPOSE  SCORES SEM'S VIA WEAPON TIME CONCEPT
// C#TYPE     SITUATION ASSESSMENT
// C#PARAMETER DESCRIPTIONS
// CIN  XA     3-VEC - ATTACKER POSITION
// CIN  VA     3-VEC - ATTACKER VELOCITY
// CIN  SA     REAL - ATTACKER SPEED
// CIN  XT     3-VEC - TARGET POSITION
// CIN  VT     3-VEC - TARGET VELOCITY
// CIN  ST     REAL - TARGET SPEED
// COUT WTSEM  REAL - WEIGHT OF SEM
// C#TECHNICAL DESCRIPTION
// C  50% WEIGHT IS GIVEN TO THE TIME WEAPON WILL NOMINALLY BE IN
// C  FLIGHT, 50% WEIGHT GIVEN TO IMPROVEMENT (OR MAINTENANCE) OF THE
// C  DESIRED WEAPON TIME RATE.  CURRENT SHOT IS EVALUATED BY DOING AN
// C  AIMPT-TYPE SOLUTION USING A WEAPON SPEED 1000 FT/SEC FASTER THAN
// C  THE ATTACKER.  SHOT IMPROVEMENT IS BASED ON THE RATE OF CHANGE OF
// C  THE WEAPON TOF.  THE BASIS OF THE FORMULAS IS INVOLVED, BUT CAN BE
// C  DERIVED BY DIFFERENTIATION OF THE TOF FORMULA.
// C#VARIABLE DESCRIPTIONS
// C  SEMA    REAL - MEASURES INSTANTANEOUS SHOT, WITH FACTORS FAVORI
// C         A WEAPON TOF OF 5 SECONDS, AND A LOW STEERING ERROR V
// C         VIS AN AIMPO
// C  SEMB    REAL - MEASURES RATE OF IMPROVEMENT OF TOF TOWARD
// C  SEMC    REAL - MEASURES TIME TO MATCH VELOCITIES WITH TARGET
// C  TWPN    REAL - WPN TOF FOR SHOT NOW
// C  VWPNT   REAL - WPN TOF RATE-OF-CHANGE
// C  VWPNMN  REAL - MIN POSSIBLE VWPNT VALUE FOR DIFFERI
// C         DIRECTIONS OF TRAVEL BY ATTACKER,
// C  VWPNMX  REAL - MAX POSSIBLE VWPNT VALUE FOR DIFFERI
// C         DIRECTIONS OF TRAVEL BY ATTACKER,
// C####
//(weapon time evaluation - A MEASURE OF COMBAT ADVANTAGE SENSITIVE TO HAVING target IN A NOMINAL SHORT-RANGE MISSILE
// ENVELOPE AND THE ABILITY TO MAINTAIN HIM THERE
double BrawlerEvaluation::WeaponTimeSelfEngageMeasure(const UtVec3d& xa, const UtVec3d& va, const UtVec3d& xt, const UtVec3d& vt)
{
   double sa = va.Magnitude();
   double st = vt.Magnitude();

   double sb = 0.0;
   // double dx(3);
   double range = 0.0;
   double cosd  = 0.0;
   double sind  = 0.0;
   double cosg  = 0.0;
   double sing  = 0.0;
   double x     = 0.0;
   double z     = 0.0;
   double w     = 0.0;
   double rdot  = 0.0;
   double twpn  = 0.0;
   double sema  = 0.0;
   // double daimp[3];
   double semb   = 0.0;
   double raimp  = 0.0;
   double vwpn   = 0.0;
   double vwpnt  = 0.0;
   double vwpnmn = 0.0;
   double vwpnmx = 0.0;
   double vwpnds = 0.0;
   double width  = 0.0;
   // double vr[3];
   double cse    = 0.0;
   double semc   = 0.0;
   double tmatch = 0.0;
   double ttgo   = 0.0;
   // double cauchy, dot ,xlimit, xmag
   // C*ENDDEC
   // call vsub(xt,xa,dx);
   UtVec3d dx = xt - xa;
   range      = dx.Magnitude();
   cosd       = 0.999999f * dx.DotProduct(vt) / (range * amax1(st, 1.0));
   sind       = sqrt((1.0 + cosd) * (1.0 - cosd));
   sb         = amax1(sa + 1000.0, st + 100.0);
   sing       = st * sind / sb;
   cosg       = sqrt((1.0 + sing) * (1.0 - sing));
   x          = sb * cosg;
   z          = st * cosd;
   w          = x - z;
   // call vsub(vt,va,vr);
   UtVec3d vr = vt - va;
   rdot       = vr.DotProduct(dx) / range;
   twpn       = range / w;
   // call vecinc(dx,twpn,vt,daimp);
   UtVec3d daimp = dx + twpn * vt;
   raimp         = daimp.Magnitude();
   // C     --BASE SHORT-TERM SEM (SEMA) ON WPN TIME AND STEERING ERROR
   if (sa == 0.0)
   {
      // C        --ASSUME STATIONARY ATTACKER IS OMNI-DIRECTIONAL
      cse = 1.0;
   }
   else
   {
      cse = daimp.DotProduct(va) / (raimp * sa);
   }
   sema = 0.5 * cauchy(twpn - 5.0, 10.0) * (0.5 + 0.5 * cse) + 0.5 * cauchy(twpn - 5.0, 20.0) * (0.7f + 0.3f * cse);
   // C     --BASE IMPROVEMENT TERM (SEMB) ON DESIRED WEAPON TIME RATE
   vwpn   = (rdot + vr.DotProduct(vt) / w) / x;
   vwpnt  = st * (cosd * cosg + sind * sing) / (w * cosg);
   width  = sa / (w * cosg);
   vwpnmn = vwpnt - width;
   vwpnmx = vwpnt + width;
   vwpnds = xlimit((5.0 - twpn) / 5.0, vwpnmx, vwpnmn);
   if (width == 0.0)
   {
      // C        --CAN CERTAINLY HAPPEN IF ATTACKER AT REST
      semb = 0.0;
   }
   else
   {
      semb = cauchy(vwpn - vwpnds, width);
   }
   tmatch = vr.Magnitude() / (5.0 * grav);
   if (vwpnds == 0.0)
   {
      ttgo = 100.0;
   }
   else
   {
      ttgo = (5.0 - twpn) / vwpnds;
   }
   // C        LIMIT TTGO IF VWPNDS HAS WRONG SIGN FOR IMPROVEMENT
   // C        THIS CAN HAPPEN BECAUSE OF VWPNMN,VWPNMX LIMITS ON VWPNDS.
   if (ttgo < 0.0 || ttgo > 100.0)
   {
      semc = cauchy(tmatch, 100.0);
   }
   else if (ttgo != 0.0)
   {
      semc = cauchy(tmatch, ttgo);
   }
   else
   {
      // C        --TTGO = 0.0
      semc = 0.0;
   }
   return (0.4f * sema + 0.6f * semb * (0.4f + 0.6f * sema * semc));
}


// C#ABSTRACT CALCULATES ANGLE ABOVE HORIZON OF TARGET
// C#PURPOSE  CALCULATES ANGLE ABOVE HORIZON OF TARGET
// C#TYPE     WEAPON
// C#PARAMETER DESCRIPTIONS
// CIN  XA     3-VEC - OBSERVER COORDINATES
// CIN  XT     3-VEC - TARGET COORDINATES
// COUT AHOR   REAL - LOOKDOWN ANGLE TO HORIZON
// C           ALSO GREAT CIRCLE ANGULAR DISTANCE TO HORIZON
// COUT ADOWN  REAL - LOOKDOWN ANGLE TO TARGET
// COUT OVRHOR REAL - ANGLE BETWEEN LOS TO TARGET AND LOS TO
// C           HORIZON, POSITIVE IF TARGET ABOVE HORIZON.
// COUT MASKED LOGICAL - .TRUE. IF TARGET MASKED BY THE EARTH
// C#TECHNICAL DESCRIPTION
// C    THIS IS A STRAIGHTFORWARD GEOMETRY/TRIGONOMETRY CALCULATION.
// C  THE CARTESIAN COORDINATE SYSTEM USED IN BRAWLER IS SLIGHTLY
// C  REINTERPRETED AS FOLLOWS:  X AND Y DIFFERENCES IN TWO COORDINATES
// C  ARE TREATED AS DOWNRANGE DISTANCES, AND -Z COORDINATES ARE TAKEN
// C  AS THE HEIGHT ABOVE LOCAL TERRAIN.
// C    ADDITIONAL FORMULAS:
// C  COS(GRAZING ANGLE)=COS(ADOWN)*(RE+HTA)/(RE+HBACKGROUND)
// C  WHERE HBACKGROUND=HEIGHT OF APPLICABLE BACKGROUND
// C#VARIABLE DESCRIPTIONS
// C  HTA     REAL - OBSERVER
// C  DWNRNG  REAL - DOWNRANGE DISTANCE (ON SURFACE OF EARTH) TO
// C  PHI     REAL - GREAT CIRCLE ANGLE TO
// C  DPOSZ   REAL - DISTANCE TARGET IS DISPLACED FROM OBSERVER
// C         LINE RUNNING FROM OBSERVER TO EARTH CEN
// C         MEANS OBSERVER LOOKS DO
// C  DPOSH   REAL - DISTANCE TARGET IS DISPLACED FROM OBSERVER P
// C         DICULAR TO THE LINE RUNNING FROM OBSE
// C  GCRNGT  REAL - GREAT CIRCLE RANGE TO TARGET
// C#ENTRY
// C  AOHOR - Returns horizon masking for earth of radius Re.
// C  AOHOR_RF - Returns horizon masking for earth of radius (4/3)Re.
// C####
void BrawlerEvaluation::AngleAboveHorizon(const UtVec3d& xa,
                                          const UtVec3d& xt,
                                          double&        ahor,
                                          double&        adown,
                                          double&        ovrhor,
                                          bool&          masked)
{
   // C     --external declarations
   // real arcsin
   // C     --local declarations
   UtVec3d dx     = UtVec3d();
   double  dwnrng = 0.0;
   double  phi    = 0.0;
   double  dposz  = 0.0;
   double  dposh  = 0.0;
   double  hta    = 0.0;
   double  dhor   = 0.0;
   double  gcrngt = 0.0;
   double  rad_e  = 0.0;
   // C*ENDDEC
   rad_e = re;

   // call subvec(xt,xa,dx);
   dx     = xt - xa;
   hta    = amax1(0.0, -xa[3 - 1]);
   dhor   = sqrt(+hta * (rad_e + rad_e + hta));
   ahor   = arcsin(dhor / (rad_e + hta));
   dwnrng = sqrt(pow(dx[1 - 1], 2) + pow(dx[2 - 1], 2));
   phi    = dwnrng / rad_e;
   dposz  = xt[3 - 1] + hta + 2.0 * (rad_e - xt[3 - 1]) * pow(sin(phi / 2.0), 2);
   dposh  = (rad_e - xt[3 - 1]) * sin(phi);
   adown  = atan2(dposz, dposh);
   ovrhor = ahor - adown;
   gcrngt = sqrt(pow(dx[1 - 1], 2) + pow(dx[2 - 1], 2));
   if ((ovrhor <= 0.0) && (gcrngt >= (rad_e * ahor)))
   {
      masked = true;
   }
   else
   {
      masked = false;
   }
   return;
}


// C#ABSTRACT EVALUATES VECTORED FLIGHT MANEUVER VALUE COMPONENT
// C#PURPOSE  EVALUATES VECTORED FLIGHT MANEUVER VALUE COMPONENT
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// COUT VVEC   REAL - NORMALIZED VECTOR-FOLLOWING VALUE
// CIN  XTPROJ REAL - VECTOR-FOLLOWING SCALE TIME
// C#TECHNICAL DESCRIPTION
// C     THE VALUE FUNCTION HAS SUBCOMPONENTS, EACH IN THE RANGE (0,1)
// C     C1 - STEERING ERROR MEASURE
// C     B1 - STEERING ERROR IMPROVEMENT MEASURE
// C     C2 - SPEED MATCH MEASURE
// C     B2- SPEED MATCH IMPROVEMENT MEASURE
// C     C1 AND B1 COMBINE TO FORM V1, A VARIABLE THAT REWARDS LOW
// C  STEERING ERROR OR GOOD STEERING ERROR IMPROVEMENT (WHEN STEERING
// C  ERROR IS HIGH).  IT IS GIVEN BY:
// C     V1 = C1 + (1 - C1)*B1
// C     A SECOND TERM V2 REWARDS GOOD SPEED MATCH.  WHEN THE STEERING
// C  ERROR IS SMALL C2 IS USED, BUT WHEN IT IS LARGE, B2 IS USED.  THE
// C  FORM OF THIS TERM IS:
// C     V2 = C2 + (1 - C2)*B2
// C     VVEC IS CONSTRUCTED AS THE PRODUCT OF V1 WITH A TERM THAT
// C  RANGES FROM 0.6 WHEN V2 IS ZERO TO 1.0 WHEN V2 EQUALS ONE.
// C
// C  EVALUATION FOR DIRECT_MANEUVERS IS ALSO PROVIDED.  THE ALGORITHM IS
// C  TRIVIAL: THESE SCORE 1.0 AND OTHERS SCORE 0.0 (WHEN DIRSPC NON-ZERO)
// C####
// static
void BrawlerEvaluation::CalcVectoredFlightValue(BrawlerPlatform& me, double& vvec)
{
   // C     --local declarations
   double cse    = 0.0;
   double se     = 0.0;
   double c1     = 0.0;
   double b1     = 0.0;
   double v1     = 0.0;
   double dspd   = 0.0;
   double b2     = 0.0;
   double c2     = 0.0;
   double v2     = 0.0;
   double sclang = 0.0;
   double csenow = 0.0;
   double senow  = 0.0;
   double dspdnw = 0.0;
   double divnow = 0.0;
   double divang = 0.0;
   double v3     = 0.0;
   double c3     = 0.0;
   // int ind;
   double tproj3 = me.ProjectedTimeDelta();
   double xtproj = tproj3;
   // double arcsin;
   // C*ENDDEC
   UtVec3d vecfpp = me.DirectionVectoredFlight();
   double  valfpp = me.ValueVectoredFlight();
   double  sflypp = me.SpeedVectoredFlight();

   // entry evveci(xtproj)
   // dirspc always zero for now (no interactive pilot)
   // if (dirspc != 0.0)
   //{
   //    return;
   // }
   double  gmxsu  = me.MaxTacticalGLoad_g();
   UtVec3d veuan  = me.VelocityNED(me.NominalState());
   double  spduan = veuan.Magnitude();

   sclang = amax1(0.333 * xtproj * grav * gmxsu / spduan, 10.0 * rad);

   csenow = vecfpp.DotProduct(veuan) / (spduan + 0.01);
   senow  = arccos(csenow);
   dspdnw = std::abs(spduan - sflypp);
   divnow = arcsin(vecfpp[3 - 1]);
   // end evveci()


   // begin evvec()

   // dirspc always zero for now (no interactive pilot)
   // if (dirspc != 0.0)
   //{
   //    //ind = indpk(3,6,2,1)
   //    //if (ind == altdsc)
   //    int ilevel,kalt,icall,lcall;
   //    me.GetAlternative(ilevel,kalt,icall,lcall);
   //    if (ilevel == 3 && kalt == 6 && icall == 2 && lcall == 1)
   //    {
   //       vvec = 1.0;
   //    }
   //    else
   //    {
   //       vvec = 0.0;
   //    }
   // }
   // else
   if (valfpp != 0.0)
   {
      UtVec3d veua  = me.VelocityNED(me.ProjectedState());
      double  spdua = veua.Magnitude();

      divang = arcsin(veua[3 - 1] / spdua);
      cse    = vecfpp.DotProduct(veua) / (spdua + 0.01);
      se     = arccos(cse);
      c1     = 0.7 * cauchy(se, 30.0 * rad) + 0.3 * cauchy(se, 15.0 * rad);
      b1     = 0.5 * border(senow - se - sclang, sclang) + 0.5 * border(senow - se - 2.0 * sclang, sclang);
      v1     = c1 + (1.0 - c1) * b1;
      dspd   = std::abs(spdua - sflypp);
      c2     = cauchy(dspd, 0.2 * sflypp);
      b2     = border(dspdnw - dspd, 0.25 * grav * tproj3);
      v2     = c2 + (1.0 - c2) * b2;
      // C        --BE SENSITIVE TO DIVE ANGLE
      c3   = cauchy(senow, 45.0 * rad);
      v3   = cauchy(divang - divnow, 10.0 * rad) * c3;
      vvec = 0.4 * v1 + 0.3 * v2 * v1 + 0.3 * v3;
   }
   else
   {
      vvec = 0.0;
   }
}


// C#ABSTRACT EVALUATES MISSILE AIMING MANEUVERS
// C#PURPOSE  EVALUATES MISSILE AIMING MANEUVERS
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// COUT  VMAIM REAL - NORMALIZED MISSILE AIMING VALUE
// C#TECHNICAL DESCRIPTION
// C  Initialization constructs best solution based on matching the
// C  direction and angular rate of the predicted impact point.  The scoring
// C  automatically gives maximum value to the weapon aiming maneuver
// C  (3,8,1,1) when boresighted..
// C  Other maneuvers score according to how well they match in direction and
// C  angular rate.
// C#VARIABLE DESCRIPTIONS
// C  Following calculated in initialization entry and SAVED
// C  AIMDIR     3-VEC - POINTS TO AIMPOINT AT TIME TPROJ3 IN FUTURE
// C  WAIMP      3-VEC - ANGULAR RATE OF AIMPOINT
// C  SENOW      REAL - Steering error without projection
// C  DSEMAX     REAL - (defined for gun only)
// C  BMAX       REAL - (defined for gun only)
// C  LPOINT     LOG - True if initial missile phase is infra-red and seeker
// C             is caged.
// C  GIMBAL_LIM REAL - Maximum gimbal limit of all seekers that are on at launch
// C  Following not saved.
// C  SE         REAL - Projected steering error (with respect to aimpoint)
// C  AZAIMP     REAL - Azimuth (rel to North) of current aimpoint (Earth frame)
// C  ELAIMP     REAL - Elevation of current aimpoint (Earth frame)
// C####
void BrawlerEvaluation::CalcMissileAimValue(BrawlerPlatform& me, UtEntity& aTgt, double& vmaim)
{
   bool lgimprb = false;
   // C     --local declarations
   // save aimdir,waimp,senow,dsemax,bmax,lpoint,gimbal_lim
   UtVec3d     dx     = UtVec3d();
   UtVec3d     aimdir = UtVec3d();
   UtVec3d     dv     = UtVec3d();
   UtVec3d     raimp  = UtVec3d();
   UtVec3d     vaimp  = UtVec3d();
   UtVec3d     waimp  = UtVec3d();
   UtFortMat3d r      = UtFortMat3d();
   // double dsemax;
   // double bmax;
   double senow  = 0.0;
   double xbulvl = 0.0;
   double aof    = 0.0;
   double se     = 0.0;
   double chi    = 0.0;
   // double v1 = 0.0;
   // double dw = 0.0;
   double rngap      = 0.0;
   double gimbal_lim = 0.0;
   bool   lpoint     = false;
   // bool seek_on;
   bool any_on = false;
   // double wt = 0.0;
   double azaimp = 0.0;
   double elaimp = 0.0;
   double spddes = 0.0;
   int    ilevel = 0;
   int    kalt   = 0;
   int    icall  = 0;
   int    lcall  = 0;
   // int i = 0;   //, dummy = 0;
   // int launch_mode = 0;
   // int iseek = 0;
   // C*ENDDEC

   // start of evaimi()
   // entry evaimi
   // C     --CHECK IF NOT INTERESTED IN FIRING
   if (me.MissileMode() != INTERCEPT && me.MissileMode() != AIMFIRE)
   {
      return; // goto 9999
   }
   // C     --SET gimbal_lim TO VALUE INDICATING THAT GIMBAL LIMITING
   // C     --NOT IMPORTANT
   gimbal_lim = -xlarge;
   // LBM - ppmknd should never be anything other than a missile (for now)
   // if (ppmknd == kndgun)
   //{
   //    //C        --GUN
   //    call ggund(ppmptr);
   //    xbulvl = bulvel;
   //    call gunpar(xp(1,me),vp(1,me),xp(1,ppmiac),vp(1,ppmiac),ap(1,ppmiac),xbulvl,laimp,raimp,vaimp,waimp,rngap,senow);
   //    if(!laimp)
   //    {
   //       //C           --IF AIMPOINT CALC FAILS
   //       //C           --LET ATKR ASSUME SPEED OF TARGET PLUS 200 FPS
   //       xbulvl = (spdnow(ppmiac)+200.0)-spdnow(me);
   //       call gunpar(xp(1,me),vp(1,me),xp(1,ppmiac),vp(1,ppmiac),ap(1,ppmiac),xbulvl,laimp,raimp,vaimp,waimp,rngap,senow);
   //    }
   //    dsemax = (gmxin*ProjectedTimeDelta())/spduan;
   //    bmax = 0.7/border(senow,dsemax);
   //    //C        --COMPUTE A GOOD SPEED:  WT INDICATES GOOD RIDING POSITION
   //    wt = ramp(90.*rad,obang(me,ppmiac),60.*rad) * ramp(90.*rad,obang(ppmiac,me),120.*rad) * ramp(2500.0,rngap,1500.0);
   // }
   // else if(ppmknd == knddew)
   //{
   //    wt = 1.0;
   //    azaimp = 0.0;
   //    elaimp = 0.0;
   //    return;
   // }
   // else
   {
      // C        --MISSILE
      lpoint = me.BoreSight(); // borsit(dummy);
      if (lpoint)
      {
         // C           --POINT "RIGHT AT" TARGET.  EFFECTED BY HAVING A HUGE MISSILE SPEED
         // call xmit(3,dxpme(1,ppmiac),raimp);
         raimp = me.RelativePositionNED(aTgt);
         // rngap=rngnow(ppmiac,me);
         rngap = raimp.Magnitude();
         // call vsub(vp(1,ppmiac),vp(1,me),dv);
         dv = me.VelocityNED(aTgt) - me.VelocityNED();
         // call cross(raimp,dv,waimp);
         waimp.CrossProduct(raimp, dv);
         // call vmult(1./rngap**2,waimp,waimp);
         waimp *= (1.0 / pow(rngap, 2));
      }
      else
      {
         // C           --MISSILE AND AIMING AT ANTICIPATED IMPACT POINT

         // LBM - gmisld just copies common block data into working memory sections for access
         //       we shouldn't have to replicate that kind of stuff...
         // call gmisld(ppmptr,l_config);

         // LBM - if any weapon sensor (seeker) on when fired, then set gimbal limit to max sensor angle
         ////C           --CALCULATE gimbal_lim, LARGEST GIMBAL LIMIT FOR on SEEKERS
         ////lnchma = actual launch mode
         ////lnchmd = desired launch mode
         // if (lnchma != 0)
         //{
         //    launch_mode = lnchma;
         // }
         // else if (lnchmd != 0)
         //{
         //    launch_mode = lnchmd;
         // }
         // else
         //{
         //    //call nabort(' EVAIMI...lnchma and lnchmd both zero!')
         // }
         // any_on = false;
         ////do 90 iseek = 1,num_seekers
         // for (int iseek = 1; iseek <= num_seekers; ++iseek)
         //{
         //    seek_on = on_at_lnch(iseek,plnch(launch_mode));
         //    if (seek_on)
         //    {
         //       any_on = true;
         //       call get_skrdat(iseek);
         //       if (ztamx > gimbal_lim)
         //       {
         //          gimbal_lim = ztamx;
         //       }
         //    }
         //    //90          continue
         // }

         if (me.WeaponSeekerOnWhenFired())
         {
            any_on = true;
            if (me.WeaponSeekerMaxAngle() > gimbal_lim)
            {
               gimbal_lim = me.WeaponSeekerMaxAngle();
            }
         }
         // C           --If no seekers are on, set the gimbal limit to
         // C           --180 degrees, so that later decisions won't be
         // C           --affected.
         if (!any_on)
         {
            gimbal_lim = 180.0 * rad;
         }

         xbulvl  = 0.5 * me.GetSpeed() + 2500.0;
         lgimprb = false;

         UtVec3d vp    = me.VelocityNED();
         UtVec3d xp    = me.PositionNED();
         UtVec3d vpTgt = me.VelocityNED(aTgt);
         UtVec3d xpTgt = me.PositionNED(aTgt);
         UtVec3d apTgt = me.AccelerationNED(aTgt);

         // LBM - I hate myself for using a fake do-while loop to replicate this nasty goto logic
         // 100         continue
         bool continue_100 = false;
         do
         {
            bool laimp;
            GunTracking(xp, vp, xpTgt, vpTgt, apTgt, xbulvl, laimp, raimp, vaimp, waimp, rngap, senow);
            if (!laimp && !lgimprb)
            {
               // C              --IF AIMPOINT CALC FAILS
               // C              --LET ATKR ASSUME SPEED OF TARGET PLUS 200 FPS
               // C              --SHOULD GET SOLUTION SINCE NOW HAVE CLOSING VELOCITY
               xbulvl = (vpTgt.Magnitude() + 200.0) - vp.Magnitude();
               GunTracking(xp, vp, xpTgt, vpTgt, apTgt, xbulvl, laimp, raimp, vaimp, waimp, rngap, senow);
            }
            // call vdir(xp(1,me),xp(1,ppmiac),dx);
            dx           = (xp - xpTgt).Normalize();
            continue_100 = false;
            if (dx.DotProduct(raimp) / rngap < cos(gimbal_lim * 0.8))
            {
               // C              --LEAD EXCEEDS 80% OF GIMBAL LIMIT.
               // C              --INCREASE MISL SPEED TO REDUCE LEAD, AND REPEAT
               xbulvl  = xbulvl * 1.5;
               lgimprb = true;
               // goto 100
               continue_100 = true;
            }
         } while (continue_100 == true);
      }
      // LBM - not used
      ////C        --COMPUTE A GOOD SPEED:  WT INDICATES GOOD RIDING POSITION
      // wt  = ramp(90.0*rad,OffBoreSightAngle(this, aTgt),60.0*rad);
      // wt *= ramp(90.0*rad,OffBoreSightAngle(this, aTgt),120.0*rad);
      // wt *= ramp(6000.0,rngap,1500.0);
   }
   // LBM - I never see slow_aim set to anything but true
   // C     --MISSILE AND GUN COME TOGETHER HERE
   // if(slow_aim)
   {
      // C        --THIS FLAG USED BY SOME UTILITIES TO PREVENT ATTACKER SPEED
      // C        --CHANGES (/OPMODE/)
      spddes = me.GetSpeed(); // spdnow(me);
   }
   // else
   //{
   //    //C        --WEIGHTED BETWEEN MY SPEED PROJECTED WITH MAX AXIAL ACCELERATION
   //    //C        --AND PROJECTED TARGET SPEED
   //    spddes = (1.0-wt)*(spdnow(me)+ProjectedTimeDelta()*almax*1.1)+wt*spdut(ppmiac);
   // }
   // C     --CONSTRUCT PARAMETERS FOR MISSILE AIM MNVR AND SCORING BY THE
   // C     --MAIN EVMAIM ENTRY:
   azaimp = atan2(raimp[1 - 1], -raimp[2 - 1]);
   elaimp = arcsin(-raimp[3 - 1] / rngap);
   // LBM - check for target being in read hemisphere???
   // C     --FLIP THE AIMING VECTOR IF THE TARGET IS BEHIND THE ATTACKER
   // C     --THIS IS FOR THE SELF PROTECTION MISSILE CASE ONLY.
   // LBM - always assume missile for now
   // if ( mslpp <= nummis && lbit(mskmsl,bitspm) )
   // LBM - no self protect missiles for now?
   // if ( lbit(mskmsl,bitspm) ) //checking if this the current weapon is a self protect missile
   //{
   //    azaimp = -azaimp;
   //    elaimp = -elaimp;
   // }
   InitMissileAiming(me, aTgt, waimp, azaimp, elaimp, spddes, gimbal_lim); // LBM - why are we calling this init here???
   // C     --CONSTRUCT AIMING DIRECTION AIMDIR A TIME TPROJ3 IN THE FUTURE:
   chi           = waimp.Magnitude() * me.ProjectedTimeDelta();
   r             = makecs(raimp, waimp);
   aimdir[1 - 1] = cos(chi);
   aimdir[2 - 1] = sin(chi);
   aimdir[3 - 1] = 0.0;
   // call vxfrmc(r,aimdir,aimdir,2);
   aimdir = r.TransposeMultiply(aimdir);
   // LBM - check for target being in rear hemisphere???
   // C     --FLIP THE AIMING VECTOR IF THE TARGET IS BEHIND THE ATTACKER
   // C     --THIS IS FOR THE SELF PROTECTION MISSILE CASE ONLY.
   // LBM - no self protect missiles for now?
   // if ( mslpp != nummis+1 && lbit(mskmsl,bitspm) )
   //{
   //    call vmult(-1.0,aimdir,aimdir);
   // }
   // 9999  continue
   // return;


   // start of evmaim()
   vmaim = 0.0;
   if (me.MissileMode() != INTERCEPT && me.MissileMode() != AIMFIRE)
   {
      return; // goto 9999
   }

   // call indupk(altdsc,ilevel,kalt,icall,lcall);
   me.GetAlternative(ilevel, kalt, icall, lcall);
   // do we have a missile selected?
   // if (mslpp <= nummis)
   if (true) // LBM - for now we always have a missile selected
   {
      // C        --MISSILE AIMING VALUE
      if (lpoint)
      {
         if (kalt == 8 && (icall == 1 || icall == 4) && lcall == 1)
         {
            // C              --AIMING MANEUVER TYPE 1 AUTOMATICALLY GETS MAXIMUM VALUE
            vmaim = 1.0;
         }
         else
         {
            // C              --ALL OTHERS GET ZERO
            vmaim = 0.0;
         }
      }
      else
      {
         // call vxfrmc(rbeua,aimdir,dx,1);   //earth to body axes
         dx = me.ConvertNEDtoECS(aimdir);
         se = arccos(dx[1 - 1] / 1.0000005);
         // sepa(dxeua(1,ppmiac),veut(1,ppmiac))
         UtVec3d dxeua = me.RelativePositionNED(me.ProjectedState(), me.TargetNominalState());
         UtVec3d veut  = me.VelocityNED(me.TargetNominalState());
         aof           = dxeua.AngleWith(veut);

         // LBM - gmisld just copies common block data into working memory sections for access
         //       we shouldn't have to replicate that kind of stuff...
         // call gmisld(ppmptr,l_config);
         // if ( lbit(mskmsl,bitspm) ) //LBM - no self protect missiles for now
         //{
         //    aof = pi - aof;
         // }
         UtVec3d dxbua = me.ConvertNEDtoECS(me.RelativePositionNED(aTgt));
         double  rng   = me.RelativePositionNED(aTgt).Magnitude();
         // UtVec3d dxeua = RelativePositionNED(mMyPro, mTgNom);
         UtVec3d dveua = me.RelativeVelocityNED(aTgt);
         double  rngr  = dxeua.DotProduct(dveua) / rng; // aproj3 - rngr(iac) = dot(dxeua(1,iac),dveua(1,iac))/rng(iac)
         vmaim         = WeaponAimingEnvelope(me,
                                      dxbua,
                                      rng,
                                      rngr,
                                      se,
                                      aof,
                                      me.WeaponMinEnvelopeRange(),
                                      me.WeaponMaxEnvelopeRange(),
                                      me.MaxWeaponSteeringError(),
                                      me.MaxWeaponAspectOffsetAngle(),
                                      me.WeaponVerticalTrackCheck(aTgt),
                                      me.TargetAngleAboveHorizon(aTgt),
                                      gimbal_lim,
                                      me.WeaponType());
      }
   }

   // LBM - no guns (for now)
   // else if (mslpp <= nummis+numgun) //do we have a gun selected?
   //{
   //    //C        --GUN AIMING VALUE
   //    if (kalt == 8 && icall == 1 && lcall == 1)
   //    {
   //       //C           --AIMING MANEUVER AUTOMATICALLY GETS MAXIMUM VALUE
   //       vmaim = 1.0;
   //    }
   //    else
   //    {
   //       //C           --DIFFERENT VALUE FUNCTION FOR STEERING ERRORS VS 45 DEG
   //       call ggund(ppmptr);
   //       call vxfrmc(rbeua,aimdir,dx,1);
   //       se = arccos(dx[1-1]/1.0000005);
   //       if (senow < 45.0*rad)
   //       {
   //          //C              --FOR SMALL SENOW, SCORE VS DESIRED ANGULAR (PITCH) RATE:
   //          v1 = cauchy(se,gsemax);
   //          dw = dist(waimp,weua);
   //          double spduan = GetSpeed();   //TODO - differentiate between nominal speed & speed
   //          vmaim = v1*cauchy(dw,grav/spduan)+0.5*(1.0-v1)*cauchy(se,10.*rad)*cauchy(dw,10.*rad);
   //       }
   //       else
   //       {
   //          //C              --FOR LARGE SE, SCORE SE IMPROVEMENT:
   //          vmaim = border(senow-se,dsemax)*bmax;
   //       }
   //    }
   // }
   // else
   //{
   //    //C        --DEW AIMING IS HANDLED BY VDEW, SO GET NO CREDIT HERE.
   //    vmaim = 0.0;
   // }
   return;
}


// C#ABSTRACT SCORES MANEUVERS FOR TARGET ILLUMINATION
// C#PURPOSE  SCORES MANEVUERS FOR TARGET ILLUMINATION
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// COUT VILLUM REAL - WEIGHTED SCORE FOR MANEUVER VS TGT ILLUMINATION
// C#TECHNICAL DESCRIPTION
// C  EVALUATION IS A WEIGHTED SUM OVER BVR TGTS, WITH WEIGHT DEPENDENT On
// C  WHETHER THERE IS A SEMI-ACTIVE MISSILE VS THAT TGT, OR ONLY
// C  UNACQUIRED ACTIVE MISSILES
// C  VILLUM = B1*B2*C1*VALEFF(BVR_TARGET) WHERE B1 IS A BORDER
// C  FUNCTION ON THE TRACKING AZIMUTH LIMIT (WIDTH IS 5% OF LIMIT), B2
// C  IS A BORDER FUNCTION ON THE TRACKING ELEVATION LIMIT (WIDTH 5% OF
// C  LIMIT), AND C1 IS A BROAD CAUCHY FUNCTION INTENDED TO PROVIDE A
// C  MILD IMPETUS TO MAINTAIN AN LOS ANGLE AT ABOUT 75% OF THE AZIMUTH
// C  AND/OR ELEVATION LIMITS, WHILE ILLUMINATING.  SPECIFICALLY:
// C
// C     B1 = B(0.9*RAZMX - ABS(AZ), 0.05*RAZMX)
// C     B2 = B(0.9*TELL -  ABS(EL), 0.05*TELL)
// C     C1 = C(EDGE, 1.0)
// C
// C  WHERE
// C
// C     EDGE = SQRT((AZ/RAZMX)**2+(EL/TELL)**2) - 0.75
// C
// C  WHEN USED IN THE CAUCHY FUNCTION, WILL MAXIMIZE WHEN THE RADAR IS
// C  ABOUT 75% WITHIN AZIMUTH/ELEVATION LIMITS.
// C
// C  TARGET WEIGHTS ARE SCORED USING A CAUCHY FUNCTION BASED ON ONE OF
// C TWO TIME-TO-GO'S -- FOR ACTIVE MISSILES, IT IS TIME FOR SEEKER TO
// C TURN ON, FOR SEMI-ACTIVE MISSILES IT IS TIME-TO-IMPACT.  PRESUMABLY
// C THE ACTIVE MISSILE WOULD BE HELD UNTIL THE PILOT THOUGHT THE TARGET
// C WAS ACTUALLY ACQUIRED, HOWEVER THE "SLOP" IN CAUCHY ACCOUNTS FOR
// C A GRADUAL ROLL-OFF, THEREBY CAPTURING SOME UNCERTAINTY AS TO WHEN
// C THE SEEKER ACTUALLY ACQUIRES.
// C#VARIABLE DESCRIPTIONS
// C  AZ      REAL - TGT LOS AZIMUTH, BODY A
// C  EL      REAL - TGT LOS ELEVATION, BODY A
// C  EDGE    REAL - EDGE+0.75 IS RADIUS OF A CIRCLE IN A COORD
// C         WHERE ONE AXIS IS SCALE
// C         OTHER OTHER IN UNITS
// C         THE TGT IS NEAR THE EDGE
// C####
// replicates evilum() from evilum.f
// static
void BrawlerEvaluation::CalcIlluminationValue(BrawlerPlatform& me, UtEntity& aTgt, double& villum)
{
   // C  ---local declarations
   // int tgts[20];  //(mac);
   // int tgts;
   // int ntgts;
   // int iac;
   // int ibvr;
   // int itgt;
   // int banten;
   // int imisl;
   // bool infor;
   double az   = 0.0;
   double edge = 0.0;
   double el   = 0.0;
   double v    = 0.0;
   // double wttgt[20]; //(mac);
   double wttgt = 0.0; //(mac);
   // data kndwt(psema)/1.0/,kndwt(pcomg)/1.0/
   // double kndwt[4] = {0,1,0,1};
   double tell  = 0.0;
   double acqwt = 0.0;
   // C*ENDDEC


   // C----------------------------------------------------------
   // entry evilui
   // ntgts = 0;

   // LBM - only one target for now
   ////do 100 ibvr=1,nbvr
   // for (ibvr = 1; ibvr <= nbvr; ++ibvr)   //for all missiles in midbvr[]
   //{
   // make sure missile exists
   // call match(mislid,nmisl,imisl,midbvr(ibvr));
   // if (imisl == 0)
   //{
   //    //call nabort('EVILUM$evilui... missile not on list')
   // }

   // LBM - no command guided missiles for now
   // if(kndbvr(ibvr) == pcomg)  //if missile phase is 4 (command guided)
   //{
   //    //acqwt = kndwt(pcomg) * (0.25 + 0.75 * cauchy(mislta(imisl) - time - 15, 10.0));
   //    acqwt = (0.25 + 0.75 * cauchy(WeaponSimTimeOfIntercept(aTgt) - GetTime() - 15, 10.0));
   // }
   // else
   {// LBM - always this (if in this function at all)
    // if (kndbvr(ibvr) == psema)  //if missile phase is 2 (semi active)
    {// acqwt = kndwt(psema) * (0.25 + 0.75 * cauchy(mislta(imisl) - time, 10.0));
     acqwt = (0.25 + 0.75 * cauchy(me.WeaponSimTimeOfIntercept(aTgt) - me.GetTime(), 10.0));
}
// else
//{
//    //call nabort(' evilum$evilui...kndbvr value illegal')
// }
}
// LBM - no detailed consideration checks for now
// if (mmindx(jtgbvr(ibvr)) > nspotd)  //detailed consideration group
//{
//    //C           --Skip him because he isn't important enough to
//    //C           --consider in detail
//    continue;   //goto 100
// }
// call match(tgts,ntgts,itgt,jtgbvr(ibvr)); //check if target exists
// if (itgt == 0)
{
   // ntgts = ntgts+1;
   // call ckrngi(ntgts,1,20,'ntgts...evilum');
   // tgts(ntgts) = jtgbvr(ibvr);   //save off id
   wttgt = acqwt;
}
// else
//{
//    wttgt = amax1(wttgt(itgt),acqwt);
// }
// 100   continue
// }


// start of evilum()
villum = 0.0;
// if(ibextr(mslmd,3,1) == 0)
if (me.MissileMode() != ILLUMINATE)
{
   return;
}
// if(ntgts == 0)
//{
//    //call nabort(' EVILUM...NTGTS=0');
// }

// LBM - just one target for now?
// do 50 itgt = 1,ntgts
// for (itgt = 1; itgt <= ntgts; ++itgt)
//{
//    call match(iacidt,nspotd,iac,tgts(itgt));
//    if(iac == 0)
//    {
//       continue;   //goto 50
//    }
// LBM - use 1st sensor on platform for now
// call rrgbnw(iacid,tgts(itgt),banten,infor);  //returns best sensor for looking at target
// call grdrc(iacid,banten);  //load radar characteristics
// call grdrs(iacid,banten);  //load radar status
UtVec3d dxbua = me.ConvertNEDtoECS(me.RelativePositionNED(aTgt));
double  rng   = dxbua.Magnitude();
az            = atan2(dxbua[2 - 1], dxbua[1 - 1]);
el            = arcsin(-dxbua[3 - 1] / rng);
tell          = me.SensorGimbalElevationLimit();
double tazl   = me.SensorGimbalAzimuthLimit();
edge          = sqrt(pow((az / tazl), 2) + pow((el / tell), 2)) - 0.75;
v = border(0.9 * tazl - std::abs(az), 0.05 * tazl) * border(0.9 * tell - std::abs(el), 0.05 * tell) * cauchy(edge, 1.0);
v = v * me.LastComputedEngagementValue(aTgt) * wttgt;
villum = villum + v;
// 50    continue
// }
return;
}


// C#ABSTRACT VALUE OF MANEUVER FOR EVADING A MISSILE THREAT
// C#PURPOSE  VALUE OF MANEUVER FOR EVADING A MISSILE THREAT
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// COUT VALEVD REAL - FULL (WEIGHTED) VALUE MANEUVER GETS FOR EVADING A
// C           MISSILE
// C#TECHNICAL DESCRIPTION
// C    TWO DIFFERENT VALUATIONS ARE USED, DEPENDING UPON WHETHER OR NOT
// C  THE PILOT'S ESTIMATE OF THE TIME-TO-IMPACT EXCEEDS 5 SECONDS.  FOR
// C  SHORT TIMES TO IMPACT THE MANEUVER TRIES TO BREAK NORMAL TO THE
// C  PLANE CONTAINING THE LOS TO THE MISSILE AND THE A/C'S VELOCITY.
// C  THE BORDER FUNCTION SCORES USING THE BREAK ANGLE TO THE CURRENT
// C  PLANE THAT IS ACHIEVED.  THIS ANGLE IS, AT THE TIME THE DECISION
// C  IS MADE, BY DEFINITION PI/2.  EVDSCL IS THE NOMINAL ACHIEVABLE
// C  VALUE OF THIS ANGLE IN A TIME TPROJ3.  THUS THE BORDER FUNCTION
// C  SCORES WELL WHEN THE ACTUAL VALUE ACHIEVED, HAFPI-DSE, EXCEEDS
// C  EVDSCL.
// C    FOR LONG TIMES DIREVD IS USED AS A DESIRED EVASION DIRECTION,
// C  CHOSEN TO RUN AWAY FROM THE MISSILE.  THE IMPROVEMENT IN POINTING
// C  TOWARDS THAT DIRECTION IS THE BASIS FOR SCORING MANEUVERS.
// C#VARIABLE DESCRIPTIONS
// C  MSLTRT   INT - Index (to /MINDMS/ arrays) of highest threat missile
// C  MISLID   ARRAY DIM(MMISL) OF (INT) [NA] - Missile ID
// C  SEMMSL  REAL - DEGREE OF THREAT OF MOST THREATENING MISS
// C         PERCEIVED AS TARGETED AT CONSCIOUS PILOT. 0=NO T
// C         1=GREATEST T
// C  TOAMSL  REAL - TIME TO IMPACT OF MOST THREATENING MISS
// C  CSMMSL  REAL - COMPLEMENT OF SEMMSL.  USED TO REDUCE WEIGH
// C         CERTAIN VALUE FUNCTION COMPONENTS SO PREVENT COMPE
// C         WITH MISSILE EVASION V
// C  RCSMSL  REAL - A LESS EXTREME WEIGHT REDUCTION MULTIPLIER
// C         OTHER VALUE FUNCTION COMPO
// C  OUTPLN  3-VEC - POINTS NORMAL TO PLANE CONTAINING LOS TO MISS
// C         AND A/C VE
// C  DIREVD  3-VEC - RUN-AWAY DIRECTION FOR LONG IMPACT TI
// C  EVDSCL  REAL - SCALE DIRECTION CHA
// C  EVDMLT  REAL - VALUE MULTIPLIER FOR MISSILE EVASI
// C####
void BrawlerEvaluation::CalcEvadeValue(BrawlerPlatform& me, double& valevd)
{
   double  toamsl = 0.0;
   double  semsln = 0.0;
   double  evdscl = 0.0;
   double  evdmlt = 0.0;
   UtVec3d direvd = UtVec3d();

   UtEntity* aThreat = me.GetThreat();
   if (aThreat == nullptr)
   {
      // should NEVER get here
      // valevd = 0.0;
      direvd = me.VelocityNED(); // No missile threat, just set to current vel vector
      evdmlt = 1.0;
   }
   else
   {
      UtVec3d dxetmn = UtVec3d();
      UtVec3d dlong  = UtVec3d();
      UtVec3d outpln = UtVec3d();
      UtVec3d dtrans = UtVec3d(); //,direvd;
      double  semmsl = 0;         // ,toamsl,semsln,semsl,evdscl,evdmlt;
      // int imisl;
      // char zfmt[8];
      // C*ENDDEC

      // C     -------------------------------------------
      // entry evevdi
      // C  INITIALIZES VARIABLES FOR EVAL_MEVD AND VARIABLES
      // C  MSLTRT, CSMMSL, AND RCSMSL FOR /EVAL3/
      // msltrt = 0;  //highest threat missile
      semmsl = 0.0;
      // if(nmisl != 0)
      // if (aThreat != 0)  //redundant
      {
         // do 1000 imisl = 1,nmisl
         // for (imisl=1; imisl<=nmisl;++imisl)
         {
            // assume aThreat is targeting me
            // if(misltg(imisl) != iacid)
            //{
            //    continue;   //goto 1000
            // }
            // if(mislse(imisl) < semmsl)
            //{
            //    continue;   //goto 1000
            // }
            // msltrt = imisl;
            semmsl = me.DegreeOfThreat(*aThreat);                          // mislse(imisl);
            toamsl = me.WeaponSimTimeOfIntercept(*aThreat) - me.GetTime(); // mislta(imisl) - time;
            // 1000  continue
         }
      }

      // csmmsl = 1.0 - semmsl;
      // rcsmsl = sqrt(csmmsl);

      // if(msltrt == 0)
      //{
      //    call ali39(msltrt,direvd);
      //    return;
      // }

      // C     CALCULATE STEERING ANGLE TO MISSILE
      UtVec3d vp    = me.VelocityNED();
      UtVec3d xp    = me.PositionNED();
      UtVec3d mislx = me.PositionNED(*aThreat);
      // call vsub(mislx(1,msltrt),xp(1,me),dxetmn);
      dxetmn = mislx - xp;
      // call vnorm(dxetmn,dxetmn);
      dxetmn.Normalize();
      vsplit(dxetmn, vp, dlong, dtrans);
      // C  TURN AWAY IF TIME TO IMPACT LONG
      if (toamsl <= 5.0)
      {
         // call cros1(dtrans,vp,outpln);
         outpln.CrossProduct(dtrans, vp);
         outpln.Normalize();
         // call vxfrmc(rbep,outpln,dlong,1);
         dlong = me.ConvertNEDtoECS(outpln);
         if (dlong[3 - 1] < 0.0)
         {
            // call xmit(3,outpln,direvd);
            direvd = outpln;
         }
         else
         {
            dlong[3 - 1] = -dlong[3 - 1];
            // call vxfrmc(rbep,direvd,dlong,2);
            UtFortMat3d rbep = me.GetNEDToECSRotation(); // brawler earth to body transformation matrix
            direvd           = rbep.TransposeMultiply(dlong);
         }
         // C     CALL VSUM(DIREVD,DTRANS,DIREVD)
         // call vnorm(direvd,direvd);
         direvd.Normalize();
      }
      else
      {
         // call vmult(-1.0,dxetmn,direvd);
         direvd = dxetmn * -1.0;
      }
      evdscl = me.MaximumInstantaneousLift_fps2() * me.ProjectedTimeDelta() /
               me.GetSpeed(); // TODO - differentiate between nominal & actual speed

      semsln = vp.DotProduct(direvd);
      semsln = arccos(semsln / me.GetSpeed());
      evdscl = amin1(evdscl, amax1(semsln, pi / 4.0));
      // C     EVASION MULTIPLIER
      evdmlt = (me.AircraftValue(me.State()) + me.ExpectedValueDestroyed(me.State(), *aThreat)) * semmsl;
   }
   me.SetEvasionDirection(direvd);

   // start of evevd()
   double dse   = 0;
   double semsl = 0;
   // C     --COMPUTE DP
   UtVec3d veua = me.VelocityNED(me.ProjectedState());
   double  dp   = veua.DotProduct(direvd);
   dp           = dp / me.GetSpeed(me.ProjectedState());
   if (toamsl > 5.0)
   {
      // C  TOF IS LONG
      semsl  = arccos(dp);
      dse    = semsln - semsl;
      valevd = border(dse, evdscl);
      valevd = 2.0 * amin1(1.0, valevd / 0.8) - 1.0;
   }
   else
   {
      // C  TOF IS SHORT
      dse    = arccos(std::abs(dp) * 0.999999);
      valevd = border((hafpi - dse) - evdscl, evdscl);
   }
   valevd = valevd * evdmlt;
   return;
}


// C#ABSTRACT EVALUATE LOW SPEED RECOVERY MANEUVER
// C#PURPOSE  EVALUATE LOW SPEED RECOVERY MANEUVER
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// COUT VLOSP  REAL - LOW SPEED RECOVERY VALUE
// CIN  SPLATI LOG - .TRUE. IF GROUND AVOIDANCE IS CURRENTLY IMPORTANT
// C#TECHNICAL DESCRIPTION
// C     IF GROUND AVOIDANCE IS NOT AN ISSUE TWO COMPONENTS ARE
// C  CONSIDERED.  THE FIRST SCORES WELL WHEN THE SPEED OF THE AIRCRAFT
// C  HAS INCREASED OVER ITS CURRENT SPEED, AND THE SECOND SCORES WELL
// C  WHEN THE DIVE ANGLE HAS INCREASED.  IF GROUND AVOIDANCE IS A
// C  PROBLEM ONLY THE SPEED COMPONENT CONTRIBUTES TO THE SCORE.
// C     THE INITIALIZATION COMPUTES THE VALUE OF LOW SPEED RECOVERY,
// C  LOSPDM, AS THE PRODUCT OF TWO FACTORS.  THE FIRST IS LARGE WHEN
// C  THE AVAILABLE LIFT IS LESS THAN THE LOAD LIMIT (SPEED IS BELOW
// C  CORNER VELOCITY), CORRECTED FOR DIVE ANGLE IN A WAY THAT REDUCES
// C  THE IMPORTANCE WHEN THE NOSE IS DOWN.  THE SECOND FACTOR IS NEVER
// C  LESS THAN 1.0, BUT FURTHER INCREASES LOSPDM WHEN THE MACH IS LESS
// C  THAN FMHI.  FMHI VARIES BETWEEN 0.6 WHEN THE DIVE ANGLE IS LEVEL
// C  OR POSITIVE AND 0.9 WHEN THE AIRCRAFT IS POINTING STRAIGHT UP.
// C#VARIABLE DESCRIPTIONS
// C  SPLAT   LOG - INDICATES THAT GROUND AVOIDANCE IS IMPORTANT; U
// C         TO PREVENT LOW SPEED RECOVERY FROM INTERFERI
// C         GROUND AVOI
// C####
void BrawlerEvaluation::CalcLowSpeedRecoveryValue(BrawlerPlatform& me, double& vlospd)
{
   // C     --local declarations
   bool   splat     = false;
   bool   splati    = false; // LBM - assume ground avoidance not currently important
   double lospdm    = 0.0;
   double speedf    = 0.0;
   double divang    = 0.0;
   double divef     = 0.0;
   double divimp    = 0.0;
   double gmxlft    = 0.0;
   double sclspd    = 0.0;
   double scldiv    = 0.0;
   double divnow    = 0.0;
   double fmlo      = 0.0;
   double fmhi      = 0.0;
   double mach_proj = 0.0;
   // double dyp_proj = 0.0;
   double fact1 = 0.0;
   double fact2 = 0.0;
   double gmax  = 0.0;
   // C*ENDDEC

   // C  ------------------------------------
   // entry evlosi(splati)
   splat          = splati;
   lospdm         = me.AircraftValue(me.State());
   UtEntity* aTgt = me.GetTarget();
   if (aTgt != nullptr)
   {
      lospdm += me.ExpectedValueDestroyed(me.State(), *aTgt);
   }
   // gmxlft = gliftm(fmachp,dyprsp,acmasp*grav,iacid);
   // gmxlft = MaximumAvailableLift_g();
   gmxlft = me.MaxInstantaneousGLoad_g();
   // C     --changed multiplier from 0.5 to 1.0 (sjc 3/6/91);
   sclspd        = 1.0 * grav * me.ProjectedTimeDelta();
   double spdnow = me.GetSpeed();
   scldiv        = 1.0 * grav * amax1(gmxlft, 2.0) * me.ProjectedTimeDelta() / spdnow;
   UtVec3d vp    = me.VelocityNED();
   divnow        = arcsin(vp[3 - 1] / spdnow);
   divef         = border(divnow, 0.5);
   // return;


   // start of evlosp()
   // speedf = border(spdua-spduan,sclspd);
   speedf = border(me.GetSpeed() - me.GetSpeed(me.NominalState()), sclspd);
   if (splat)
   {
      // C        CONSIDER ONLY SPEED
      vlospd = lospdm * speedf;
      return;
   }

   // C        CONSIDER BOTH SPEED AND DIVE ANGLE
   // call getqm(GetSpeed(),-xeua(3),dyp_proj,mach_proj);
   // dyp_proj = me.DynamicPressure_psf();   //TODO - pass in altitude from xeua???
   mach_proj = me.Mach();
   // gmxlft = gliftm(mach_proj,dyp_proj,acmasp*grav,iacid);
   // gmxlft = MaximumAvailableLift_g();
   gmxlft       = me.MaxInstantaneousGLoad_g();
   UtVec3d veua = me.VelocityNED(me.ProjectedState());
   divang       = arcsin(veua[3 - 1] / me.GetSpeed());
   divimp       = border(divang - (divnow + (0.5 * scldiv)), scldiv);
   if (me.CurrentWeight_lbs() > me.DesignWeightForMaxG_lbs() * grav)
   {
      // gmax = tbl1(mach_proj,gcap2,mchval,nmach1);
      gmax = me.StructuralGLimitAboveDesignWeight_g();
   }
   else
   {
      // gmax = tbl1(mach_proj,gcap,mchval,nmach1);
      gmax = me.StructuralGLimitAtDesignWeight_g();
   }
   fact1 = border(gmax - (3.0 * divang) - gmxlft, 2.0);
   fmhi  = 0.6 + 0.3 * ramp(0.0, divang, -hafpi);
   fmlo  = fmhi - 0.2;
   fact2 = (1.0 + 1.0 * ramp(fmhi, mach_proj, fmlo));
   // C        CONSIDER BOTH SPEED AND DIVE ANGLE
   vlospd = speedf * divef + (1.0 - divef) * (0.6 * divimp + 0.4 * speedf);
   vlospd = vlospd * lospdm * fact1 * fact2;
   return;
}


void BrawlerEvaluation::CalcBadAspectValue(BrawlerPlatform& me, double& vavbdd) {}

// C#ABSTRACT GENERATES MISSILE AIM MANEUVER
// C#PURPOSE  GENERATES MISSILE AIM MANEUVER
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// CIO  ICALL      INT - ICALL INDEX OF (PREVIOUS - INPUT) (NEW - OUTPUT)
// C               ALTERNATIVE'S INDEX SET.  ICALL=0 THE FIRST TIME THE
// C               ROUTINE IS CALLED FOR A CONSCIOUSNESS EVENT.  INPUT VALUE
// C               USED TO DETERMINE WHICH ALTERNATIVE TO GENERATE ON
// C               SUCCESSIVE CALLS BY AN ASLCT ROUTINE.
// COUT SUBMOR     LOG - .TRUE. IF AN ALTERNATIVE IS GENERATED.
// CIN  WAP        REAL - Angular rate of aimpoint
// CIN  AZAP       REAL - Azimuth of aimpoint wrt North
// CIN  ELAP       REAL - Elevation of aimpoint
// CIN  SPDTMP     REAL - Desired speed
// CIN  GIMBAL_LIM REAL - Max gimbal limit of all gimbals on at launch
// C#TECHNICAL DESCRIPTION
// C    THE INDEX SET OF ONE OF THESE ALTERNATIVES IS (3,8,ICALL,1).
// C  THE SPTR(IND) VALUE DETERMINES IF THE CURRENT ALTERNATIVE IS
// C  ENABLED FOR THE CURRENT DECISION.
// C
// C  Type 1:
// C  Generated if not under altitude control.  This is a
// C  commanded AOA maneuver format.
// C
// C  Type 2:
// C  Generated if trying to control the attacker altitude when
// C  at long ranges.  This maneuver is generated the same way as a
// C  vectored flight maneuver with a desired velocity vector format.
// C
// C  Type 3:
// C  Generated when vector following is active and the angle
// C  between the nominal projected target position and the vector
// C  following direction, VECFPP, exceeds 80% of the gimbal limit.
// C  Type 3 also generates a desired velocity vector maneuver format.
// C
// C  Type 4:
// C  Used in manned simulator applications only.
// C
// C  Type 5:
// C  Generated if the user has set a desired lofting angle through the
// C  MISSILE_LOFT special handler (pcode 23).
// C  Generated as a desired velocity vector maneuver format.
// C#VARIABLE DESCRIPTION
// C  5  DIR0     3-VEC - Direction of desired velocity
// C  8  GMX      REAL - Max G's for the alternative
// C  9  SPD0     REAL -  Desired speed
// C  MANSIM           INT - Indicates that BRAWLER is running in simulator mode.
// C END      CHAR - A character string containing 'END' - [NA]
// C  PPM_SEMAX  REAL [RAD] - Maximum steering angle to fire.
// C  PPMIAC   MM-IND [NA] - Index of best target
// C  PPMJID   AC-IND [NA] - Index of selected target
// C  PPMSE    REAL [RAD] - Steering angle
// C  SFLYPP   REAL [FT/SEC] - Speed to use for vectored flight
// C  VALFPP   REAL [NA] - Value for vectored flight - ValueVectoredFlight()
// C  VECFPP   3-VEC [FT] - Vector on which to fly
// C  DXEUAN   ARRAY (MM-IDX) OF (3-VEC) - Pos of target rel to attacker
// C           (nominal maneuver) in Earth coords.
// C  AOFFMX           REAL [RAD] - Maximum aspect angle off tail to fire.
// C####
void BrawlerEvaluation::InitMissileAiming(BrawlerPlatform& me,
                                          UtEntity&        aTgt,
                                          const UtVec3d&   wap,
                                          double           azap,
                                          double           elap,
                                          double           spdtmp,
                                          double           gimbal_lim)
{
   // C  --local declarations
   // save waimp,azaimp,elaimp,spddes,alt_ctrl,zeta_lim
   UtVec3d waimp = UtVec3d();
   // double azaimp = 0.0;
   // double elaimp = 0.0;
   // double spddes = 0.0;
   UtVec3d dpos = UtVec3d();
   // TODO_NOT_USED double range = 0.0;
   // TODO_NOT_USED double zeta = 0.0;
   // TODO_NOT_USED double aof = 0.0;
   // TODO_NOT_USED double caof = 0.0;
   UtVec3d dpos_b = UtVec3d();
   // TODO_NOT_USED double rng_lim = 0.0;
   // double theta;
   // double semin;
   // double sewid;
   // double wt;
   // TODO_NOT_USED double zeta_lim = 0.0;
   // UtVec3d dir_new = UtVec3d();
   // UtVec3d desdir = UtVec3d();
   // double rdes_to_e(3,3);
   UtFortMat3d rdes_to_e = UtFortMat3d();
   // double az;
   // double el;
   // int i;
   // bool alt_ctrl = false;
   // bool lngtrn;
   // bool msl_loft;
   // TODO_NOT_USED double vis_rng = 5.0 / ftnmi;
   // C*ENDDEC

   // entry ali38(wap,azap,elap,spdtmp,gimbal_lim)
   // TODO_NOT_USED if (gimbal_lim != -xlarge)
   // TODO_NOT_USED {
   // TODO_NOT_USED zeta_lim = gimbal_lim;
   // TODO_NOT_USED }
   // TODO_NOT_USED else
   // TODO_NOT_USED {
   // TODO_NOT_USED zeta_lim = twopi;
   // TODO_NOT_USED }
   // C     --GENERATE IACTN=4 MANEUVER IF BEYOND RMAX AND MEET ANGLE CRITERIA
   // call vsub(xp(1,ppmiac),xp(1,me),dpos);
   dpos = me.RelativePositionNED(aTgt);
   // TODO_NOT_USED range = dpos.Magnitude();  //xmag(dpos);
   // call vxfrmc(rbep,dpos,dpos_b,1);
   dpos_b = me.ConvertNEDtoECS(dpos);
   // C     --COMPUTE SEEKER ANGLE - ANGLE BTWN BODY X-AXIS AND LOS TO TGT
   // TODO_NOT_USED zeta = arccos(dpos_b[1 - 1] / range);
   // LBM - assume no sam site (ground) targets for now
   ////C     --COMPUTE ANGLE OFF - ANGLE BTWN LOS TO TARGET AND TGT VEL VECTOR
   // if(mm_entity(ppmiac) == sament || GetSpeed(aTgt) == 0)
   //{
   //    //C        --INSURE THAT aof PLAYS NO PART FOR SAM SITE
   //    //C        --ALSO, IF HE JUST DIED, HIS SPEED MAY BE ZERO, WHICH WOULD
   //    //C        --ABORT THE OTHER CALCULATION.
   //    aof = 0.0;
   // }
   // else
   {
      // TODO_NOT_USED UtVec3d vpTgt = me.VelocityNED(aTgt);
      // TODO_NOT_USED caof = dpos.DotProduct(vpTgt) / (range * me.GetSpeed(aTgt));
      // TODO_NOT_USED aof = arccos(caof);
   }
   // C     --ALT_CTRL IS TO SET UP A MANEUVER SIMILAR TO VECTORED FLIGHT
   // C     --WHEN BEYOND RNG_LIM AND STILL MEET ANGLE CRITERIA
   // TODO_NOT_USED rng_lim = amin1(me.WeaponMaxEnvelopeRange(), vis_rng);
   // alt_ctrl = me.ValueVectoredFlight() > 0.0 &&
   //            me.WeaponSteeringError(aTgt) <= me.MaxWeaponSteeringError() &&
   //            aof <= me.MaxWeaponAspectOffsetAngle() &&
   //            zeta <= zeta_lim &&
   //            range > rng_lim;
   // call xmit(3,wap,waimp);
   waimp = wap;
   // azaimp = azap;
   // elaimp = elap;
   // spddes = spdtmp;
   // LBM - not doing missile loft maneuver for now (requires user rules anyways)
   ////C     --set variables in /prods/ for use by MISSILE_LOFT Rules handler
   // pr_loft = false;
   // pr_tgtrng = range;
   // pr_tgtalt = -xp(3,ppmiac);
   // pr_tgtaof = aof;
   return;
}


// C#ABSTRACT RETURNS A SMOOTH ENVELOPE LEVEL FOR AIM DECISION
// C#PURPOSE  RETURNS A SMOOTH ENVELOPE LEVEL FOR THE AIM WEAPON EVALUATION
// C#TYPE     WEAPON
// C#PARAMETER DESCRIPTIONS
// CIN  XLOSA  ARRAY DIM(3-VEC) of REAL - Position of target relative to
// C           attacker in attacker coords.
// CIN  RNG    REAL - RANGE TO TARGET
// CIN  RDOT   REAL - RANGE RATE OF TARGET
// CIN  SE     REAL - STEERING ERROR (WITH RESPECT TO THE AIMPOINT)
// CIN  AOF    REAL - ASPECT ANGLE OF TARGET
// CIN  RMIN   REAL - MINIMUM FIRING RANGE
// CIN  RMAX   REAL - MAXIMUM FIRING RANGE
// CIN  SEMAX  REAL - MAXIMUM ALLOWABLE STEERING ERROR TO FIRE
// CIN  AOFFMX REAL - MAXIMUM ALLOWABLE AOF TO FIRE
// CIN  GIMBAL_LIM  REAL - MAXIMUM GIMBAL LIMIT
// CIN  TRKBL  LOG - TRUE IF TARGET ABOVE HORIZON OR MISSILE HAS
// C           LOOKDOWN/SHOOTDOWN CAPABILITY
// CIN  OVRHOR REAL - ANGLE BETWEEN TARGET AND HORIZON (TARGET IS ABOVE
// C           THE HORIZON)
// CIN  KIND   INT - MISSILE KIND INDEX
// COUT ENVLVL_AIM REAL - THE ENVELOPE LEVEL, ON A 0-1 SCALE
// C#TECHNICAL DESCRIPTION
// C    The 'AIM' mode computes RFACT based on two border functions that
// C  require the range being between 1.2*RMAX and .8*RMIN.  Tighter
// C  steering error tolerances are also used. If a seeker is on at launch,
// C  the envelope level is scored with being within gimbal limits considered.
// C#VARIABLE DESCRIPTIONS
// C####
double BrawlerEvaluation::WeaponAimingEnvelope(BrawlerPlatform& me,
                                               const UtVec3d&   xlosa,
                                               double           rng,
                                               double           rdot,
                                               double           se,
                                               double           aof,
                                               double           rmin,
                                               double           rmax,
                                               double           semax,
                                               double           aoffmx,
                                               bool             trkbl,
                                               double           ovrhor,
                                               double           gimbal_lim,
                                               int              kind)
{
   // C  local declarations
   bool   revers = false;
   double rw2    = 0.0;
   double rw3    = 0.0;
   double drange = 0.0;
   double aofwid = 0.0;
   double rfact  = 0.0;
   double rvalue = 0.0;
   double seval1 = 0.0;
   double ztaval = 0.0;
   double theta  = 0.0;
   // data rwidth/1000./
   double envlvl_aim = 0.0;
   // C*ENDDEC
   // C

   revers = false;
   rfact  = border(rmax * 1.2 - rng, 0.1 * rmax) * border(rng - 0.8 * rmin, amax1(100.0, 0.1 * rmin));
   rw2    = amax1((rmax - rmin) * 0.25, 1000.0);
   rw3    = 0.4 * rw2;
   drange = me.RangePercent((WeaponType)kind) * rmax - rng;
   rvalue = 0.5 * rfact + 0.35 * cauchy(drange, rw2) + 0.15 * cauchy(drange, rw3);
   rw2    = amin1(0.2 * semax, 5.0 * rad);
   seval1 = border(semax - se, rw2);
   if (gimbal_lim != -xlarge)
   {
      // --Consider gimbal limit when a seeker is on at launch
      theta      = std::abs(arccos(xlosa[1 - 1] / rng));
      ztaval     = border(-theta + gimbal_lim, 0.1 * gimbal_lim);
      envlvl_aim = rvalue * (0.8 * ztaval + 0.2 * seval1);
   }
   else
   {
      envlvl_aim = rvalue * (0.8 + 0.2 * seval1);
   }
   // C     --Compute final envelope level
   // C
   // 100
   // C     --NOTE: IF .NOT.TRKBL OVRHOR < 0
   if (!trkbl)
   {
      envlvl_aim *= (border(ovrhor + 0.05, 0.05) + 0.2);
   }
   if (aoffmx < pi)
   {
      aofwid = amin1(amin1(15.0 * rad, amax1(pi - aoffmx, 0.001)), 0.25 * aoffmx);
      if (revers)
      {
         envlvl_aim *= border(aoffmx + aofwid - (aof - pi), aofwid);
      }
      else
      {
         envlvl_aim *= border(aoffmx + aofwid - aof, aofwid);
      }
   }
   return envlvl_aim;
}


// void BrawlerPlatform::evdis(WsfPlatform* aThreat,
//                                double&      vdseng)
//{
//    //C#ABSTRACT EVALUATE DISENGAGEMENT VALUE OF A MANEUVER
//    //C#PURPOSE  EVDIS EVALUATES DISENGAGEMENT VALUE OF A MANEUVER.
//    //C  INITIALIZATION ENTRY DETERMINES DESIRABILITY/FEASIBILITY OF
//    //C  DISENGAGING.
//    //C#TYPE     MANEUVER
//    //C#PARAMETER DESCRIPTIONS
//    //COUT VDSENG REAL - DISENGAGEMENT VALUE
//    //C#TECHNICAL DESCRIPTION
//    //C     THE DISENGAGEMENT VALUES FUNCTION OPERATES BY MATCHING THE
//    //C  PROJECTED A/C STATE AGAINST THAT PREDICTED FOR AN OPTIMAL
//    //C  DISENGAGEMENT MANEUVER GENERATED BY EVDISI.  A THREE-COMPONENT
//    //C  VALUE FUNCTION WEIGHS MATCHING HEADING, SPEED, AND ALTITUDE RATE.
//    //C     EVDISI FIRST DETERMINES THE DESIRABILITY OF DISENGAGING
//    //C  BY CALLING EVDWNT.  IF DESIRED, EVDABL DETERMINES IF THE
//    //C  SITUATION PERMITS DISENGAGE.  IF SO, EVDSET IS CALLED. OTHERWISE
//    //C  WHEN DISENGAGEMENT IS UNDESIRABLE, BUT WOULD BE DESIRABLE IF
//    //C  DIFFERENT OFFMLT AND DEFMLT VALUES APPLIED, A FLIGHT POSTURE
//    //C  DECISION IS REQUESTED
//    //C#VARIABLE DESCRIPTION:  none
//    //C   7   DEFMLT   REAL [NA] - Overall scale multiplier for defensive value aspects.
//    //C   72  DESDIS   LOG [NA] - .true. if disengagement is desired.
//    //C   41  DISENG   LOG [NA] - Disengagement decision made by evdisi.
//    //C   1   IACID    AC-IND [NA] - My aircraft index.
//    //C   6   OFFMLT   REAL [NA] - Overall scale multiplier for offensive value aspects.
//    //C  SPDUA    REAL - Attacker speed
//    //C  VEUA     3-VEC - Attacker velocity in the earth frame
//    //C####
//
//    bool candis;
//    bool wantd;
//    UtVec3d vt;
//    double ruse;
//    double dxmag;
//    double b1;
//    double c1;
//    double c2;
//    double dismlt;
//    double dtheta;
//    double ek;
//    double er;
//    double headd;
//    double hscale;
//    double hyst;
//    double scale;
//    double st;
//    //int iacthr;
//    //C*ENDDEC
//
//    //C     --------------------------------------------
//    //entry evdisi
//    wantd = evdwnt(aThreat,ek,er,hyst);
//    if (wantd)
//    {
//       candis = evdabl(aThreat,ruse,dxmag);
//    }
//    else
//    {
//       candis = false;
//    }
//    if (candis)
//    {
//       //C        --SET UP DISENGAGE
//       if(!diseng)
//       {
//          tdseng = GetTime();
//       }
//       diseng=true;
//       evdset(aThreat,ruse,dxmag,headd,scale,hscale,vt,st,dismlt);
//    }
//    else
//    {
//       //C        --NO DISENGAGE -- If different offmlt/defmlt values would have
//       //C        --resulted in a disengage set up, then request a flt post decision
//       diseng=false;
//       dismlt=0.0;
//       if((ek>er) && ((ek/(OffensiveMultiplier()/FlightAggressiveness())) < (0.5*er/DefensiveMultiplier())) && (!desdis))
//       {
//          //LBM - not necessary right now, all decisions are at lowest level
//          //lmdlev[4-1] = true;
//       }
//    }
//
//
//    //start of subroutine evdis()
//    vdseng = 0.0;
//    if(!diseng)
//    {
//       //goto 999
//       vdseng = vdseng*dismlt;
//       return;
//    }
//    dtheta = std::abs(HeadingNED()-headd);
//    if(dtheta > pi)
//    {
//       dtheta = twopi-dtheta;
//    }
//
//    //C     --20 AND 5 DEGREES
//    UtVec3d veua = VelocityNED();
//    b1 = border(0.3491-dtheta,0.0873);
//    c2 = cauchy(GetSpeed()-st,hscale);
//    c1 = cauchy(veua[3-1]-vt[3-1],scale);
//    vdseng = b1*(0.4+0.3*c1+0.3*c2);
////999
//   vdseng = vdseng * dismlt;
//   return;
//}


// bool BrawlerPlatform::evdwnt(WsfPlatform* aThreat,
//                                 double&      ek,
//                                 double&      er,
//                                 double&      hyst)
//{
//    //C#ABSTRACT Determines whether pilot might wish to attempt disengaging
//    //C#PURPOSE  Called by evdis' evdisi entry to determine whether disengaging
//    //C          is desired if possible
//    //C#TYPE     Maneuver
//    //C#PARAMETER DESCRIPTIONS
//    //COUT IACTHR MM-IDX - worst threat
//    //COUT EK     REAL - surrogate probability giving my expected kill, given
//    //C           pilot stays in fight.
//    //COUT ER     REAL - surrogate probability giving my expected risk
//    //C           associated with not disengaging. Evaluated as a function
//    //C           of the pilot's estimated survival probability adjusted
//    //C           for fuel state and weighted by the A/C value and the
//    //C           defensive multiplier DEFMLT.
//    //COUT HYST   REAL - measure of hysteresis, function of time since disengage
//    //C           started.  The factor HYST is intended to encourage consistent
//    //C           behavior if disengagement has been initiated previously.
//    //C           It has the effect of lowering EK during the (approximately)
//    //C           first 30 sec after disengagement has been initiated.
//    //COUT WHYTRY CHAR - Why disengage is desired.  Defined if WANTD= .TRUE.
//    //COUT WHYnot CHAR - Why disengage is not desired.  Defined if
//    //C           WANTD=.FALSE.
//    //COUT WANTD  LOG - .true. if the pilot desires disengaging
//    //C#TECHNICAL DESCRIPTION
//    //C     This routine determines if the pilot wishes to consider disengage at
//    //C  the maneuver level.  If this routine routine returns with disengage
//    //C  "desired", then the ability to disengage is evaluated in EVDABL.
//    //C  If both routines consent, then the disengage value multiplier is
//    //C  activated and the disengage maneuver alternative is activated.
//    //C  This routine operates through the use of a decision tree.  The various
//    //C  considerations are listed in the order in which they appear in the
//    //C  code.
//    //C
//    //C  INHIBITED        If disengage has been inhibited through production
//    //C                   rules then disengage in not desired.
//    //C  LOW FUEL         If fuel is low, then disengage is desired.
//    //C  IMMINENT SHOT    If I have an imminent shot, then disengage is not
//    //C                   desired.
//    //C  FRND NEEDS HELP  If a friend needs help, then disengage is not
//    //C                   desired.
//    //C  NO TARGETS       If I have no good targets, then disengage is
//    //C                   desired.
//    //C  SLOW FIGHT       If in a "slow fight", then disengage is desired
//    //C                   unless there is little danger or I have an
//    //C                   energy advantage over the greatest threat.
//    //C  CURRENT TACTIC   If current tactic requests disengage (DESDIS=.t.)
//    //C                   then disengage is desired.
//    //C  GEOMETRY         If I am behind my worst threat and my best target
//    //C                   is behind me, then disengage is desired.
//    //C  Kill/Risk        If the expected kill value associated with staying
//    //C                   exceeds the risk value of disengaging, then
//    //C                   disengage is not desired.
//    //C  Default          If no action has been taken as a result of above
//    //C                   considerations, then disengage is desired.
//    //C#VARIABLE DESCRIPTIONS
//    //C  IACTGT MM-IND - Best target
//    //C####
//
//    //C
//    double r3nm;
//    double d60;
//    //double thrtlv;
//    double bfuel;
//    double dfuel;
//    double dfuelt;
//    double fworry;
//    double ekglbl;
//    //double z;
//    //double s;
//    //double uamax;
//    //double uemax;
//    double pdeadf;
//    double jfuel;
//    double ffact;
//    //int ibg;
//    //int iach;
//    //int igg;
//    //int iacf;
//    //int ithrt;
//    //int iac;
//    //int itgt;
//    //int iactgt;
//    r3nm = 3.0/ftnmi;
//    d60 = 60.0*rad;
//    //C*ENDDEC
//    //egy(z,s) = -z+s*s/(2.0*grav);
//    //C
//    ek = 0.0;
//    er = 0.0;
//    hyst = 0.0;
//    //whynot = " ";
//    //whytry = " ";
//    //iacthr = 0;
//    //C
//    //C     --PRODUCTION RULE INHIBIT
//    bool dishib = false; //values array not used right now
//    if(dishib)
//    {
//       //whynot = "DISENGAGE INHIBITED";
//       return false;  //goto 900;
//    }
//    //C     --FIND WORST THREAT
//    //uemax = 0.0;
//
//    //LBM - assume threat given is worst
//    ////iacthr = 0;
//    ////do 200 ithrt=1,nthrpp
//    //for (ithrt = 1; ithrt <= nthrpp; ++ithrt)
//    //{
//    //   iac = lthrpp(ithrt);
//    //   if(uevd(iac) > uemax)
//    //   {
//    //      iacthr = iac;
//    //      uemax = uevd(iac); //calculated in majud.f / minud.f
//    //   }
//    ////200   continue
//    //}
//
//    //C     --FUEL CONSIDERATIONS
//    //C     --Determine low fuel factor
//    bingo(this,bfuel,dfuel,dfuelt,fworry);
//    //C     --A close hostile can add up to 30 seconds more fuel to
//    //C     --disengagement ("joker" fuel)
//    UtVec3d dxt = RelativePositionNED(*aThreat);
//    double rhst = dxt.Magnitude();
//    jfuel = fworry*ramp(20.0/ftnmi,rhst,0.0);
//    ffact = border(dfuel-(fworry+jfuel),0.5*fworry);
//    disbng = (ffact < 0.2);
//    if (disbng)
//    {
//       //whytry = 'LOW FUEL';
//       return true;   //goto 800;
//    }
//
//    //C
//    //C     --DAMAGED
//    //if(damage(iacid) == 1)
//    if (Damaged())
//    {
//       //whytry = 'A/C DAMAGED';
//       return true;   //goto 800;
//    }
//
//    //C
//    //C     --NO HOSTILES
//    if(aThreat == 0)
//    {
//       //whynot = 'NO BAD GUYS';
//       return false;  //goto 900;
//    }
//
//    //C
//    //C     --DON'T RUN IF IMMINENT SHOT (Within 5 seconds)
//    //whynot = 'GOOD SHOT W/IN 5 SEC';
//    //if(lbit(mslmd,2) && (amxin/spduan)*5.0 > ppmse)
//    double spduan = GetSpeed();   //TODO - differentiate between actual & nominal
//    if(mMissileMode == AIMFIRE && (MaximumInstantaneousLift_fps2()/spduan)*5.0 > WeaponSteeringError())
//    {
//       //whynot = 'GOOD SHOT W/IN 5 SEC';
//       return false;  //goto 900;
//    }
//
//    //LBM - not right now - only demo 1v1
//    ////C     --DISALLOW DISENGAGE IF FRIENDLIES NEED HELP
//    ////do 100 ibg=1,nbg
//    //for (ibg=1; ibg<=nbg; ++ibg)
//    //{
//    //   iach = listh(ibg);
//    //   //C        --DISCOUNT HOSTILE IN TROUBLE - HE'S NO THREAT
//    //   if(psrv(iach) < 0.6)
//    //   {
//    //      continue;   //goto 100;
//    //   }
//
//    //   thrtlv = wtsem(xp(1,me),vp(1,me),spdnow(me), xp(1,iach),vp(1,iach),spdnow(iach));
//    //   //do 50 igg=1,ngg
//    //   for (igg=1; igg<=ngg; ++igg)
//    //   {
//    //      iacf = listf(igg)
//    //      if(iacf == me)
//    //      {
//    //         continue;   //goto 50
//    //      }
//    //      if(rngnow(iacf,iach) > r3nm)
//    //      {
//    //         continue;   //goto 50
//    //      }
//    //      if(obang(iach,iacf) > d60)
//    //      {
//    //         continue;   //goto 50
//    //      }
//    //      if(pi-obang(iacf,iach) > d60)
//    //      {
//    //         continue;   //goto 50
//    //      }
//    //      //C           --ASSERT:ON TAIL INSIDE 3 NM
//    //      //C           --NOTE: 0.828 IS WTSEM 2NM BACK, COSPEED, LINEAR GEOMETRY
//    //      if(thrtlv > 0.828)
//    //      {
//    //         //write(whynot,'(A,I3,A,I3)') 'HELP',iacidt(iacf), ' Versus',iacidt(iach);
//    //         return false;  //goto 900;
//    //      }
//    //   //50       continue
//    //   }
//    ////100   continue
//    //}
//
//    pdeadf = ProbabilityOfSurvival() * border(fworry+jfuel-dfuel,0.5*fworry);
//    er = ((1.0-ProbabilityOfSurvival())+pdeadf)*AircraftValue(this)*DefensiveMultiplier();
//    hyst = 1.0;
//    //LBM - do not worry about flight right now (assume 1v1)
//    //if(diseng && nfltp != 2)
//    {
//       hyst = border(GetTime()-(tdseng+30.0), 10.0);
//    }
//    ek = ExpectedValueDestroyed(this) * sqrt(ProbabilityOfSurvival()) * OffensiveMultiplier() * hyst;
//
//
//    //LBM - assume target given is best target
//    //C     --DETERMINE BEST TARGET
//    //uamax = 0.0;
//    //iactgt = 0;
//    ////do 300 itgt = 1,ntgtpp
//    //for (itgt=1; itgt <= ntgtpp; ++itgt)
//    //{
//    //   iac = ltgtpp(itgt);
//    //   if(uatk(iac) > uamax)
//    //   {
//    //      iactgt = iac;
//    //      uamax = uatk(iac); //calculated in majud.f / minud.f
//    //   }
//    ////300   continue
//    //}
//
//    //C     --RUN IF SLOW UNLESS THE WORST
//    //C     --THREAT HAS LESS ENERGY THAN ME OR I AM IN LITTLE DANGER
//    if(SlowFlight() && aThreat != 0)
//    {
//       if(ProbabilityOfSurvival() < 0.90 )
//       {
//          UtVec3d xp = PositionNED();
//          double spdnow = GetSpeed();
//          UtVec3d xpTgt = PositionNED(*aThreat);
//          double spdnowTgt = GetSpeed(aThreat);
//          if(egy(xp[3-1],spdnow) < egy(xpTgt[3-1],spdnowTgt))
//          {
//             //write(whytry,'(A,I3)') 'SLOW/ LOW Es VS ',iacidt(iacthr);
//             return true;   //goto 800;
//          }
//       }
//    }
//    //C
//    //C     --FLIGHT TACTIC SPECIFICATION
//    if(desdis)
//    {
//       //whytry = 'FLIGHT TACTIC';
//       return true;   //goto 800;
//    }
//    //C
//    //if(iactgt > 0)
//    if(aThreat != 0)
//    {
//       ekglbl = (1.0-ProbabilityOfSurvival(aThreat)) * AircraftValue(aThreat) * OffensiveMultiplier() * hyst;
//       ek = 0.5*(ek+ekglbl);
//    }
//    //C     --DETERMINE QUARTERS RELATIVE TO BEST TARGET AND WORST THREAT:
//
//    //C     --RUN IF BEHIND WORST THREAT AND BEST TARGET BEHIND ME
//    //C     --BAD QUARTERS Message relative to best tgt, worst threat
//    //if(iactgt > 0 && iacthr > 0)
//    if(aThreat != 0)  //LBM - target & threat are same for 1v1
//    {
//       UtVec3d dxwuan = ConvertNEDtoWind(dxt);
//       UtVec3d dxwutn = ConvertNEDtoWind(*aThreat, dxt);
//       if(dxwuan[1-1] < 0.0 && dxwutn[1-1] > 0.0 && (ek < 1.5*er || er > 0.2*AircraftValue(this)) )
//       {
//          //write(whytry,'(A,I2,I3)') 'BAD QTR;THR/TGT',iacidt(iacthr), iacidt(iactgt);
//          return true;   //goto 800;
//       }
//    }
//    //C     --HIGH EK OVER ER Message relative to best tgt, worst threat
//    //write(whynot,'(A,I2,I3)') 'EK/ER>1;THR/TGT',iacidt(iacthr), iacidt(iactgt)
//    if(ek > er)
//    {
//       //write(whynot,'(A,I2,I3)') 'EK/ER>1;THR/TGT',iacidt(iacthr), iacidt(iactgt);
//       return false;  //goto 900;
//    }
//    else
//    {
//       //write(whytry,'(A,I2,I3)') 'EK/ER<1;THR/TGT',iacidt(iacthr), iacidt(iactgt);
//       return true;   //goto 800;
//    }
//    //C
//////800   continue
////   //C     --WANT TO DISENGAGE
////   //wantd = true;
////   //C     --Here WHYTRY is why the disengage is desired; WHYnot is undefined
////   //C     --(even if it's been set).  WHYnot will be set by EVDABL from EVDIS.
////   return true;
////   //C
//////900   continue
////   //C     --DON'T WANT TO DISENGAGE
////   //wantd = false;
////   //whytry = 'DON''T WANT DISENG';
////   //C     --Here WHYNOT is why disengage CANNOT be tried; set somewhere above
////   return false;
//}
//
// bool BrawlerPlatform::evdabl(WsfPlatform* aThreat,
//                                double&      ruse,
//                                double&      dxmag)
//{
//   //C#ABSTRACT Given disengagement desirability, finds whether possible
//   //C#PURPOSE  Called by evdis's evdisi entry to determine whether it is
//   //C  possible, given its desirability, to disengage
//   //C#TYPE     Maneuver
//   //C#PARAMETER DESCRIPTIONS
//   //CIN  IACTHR MM-IND - Worst threat
//   //COUT WHYCAN CHAR - Reason why we're able or not able to disengage
//   //COUT CANDIS LOG - True when disengaging is possible
//   //COUT RUSE   REAL - Assessed maximum weapon range of pursuer
//   //COUT DXMAG  REAL - Assessed distance from pursuer after disengage
//   //C           maneuver attempt
//   //C#TECHNICAL DESCRIPTION
//   //C     This code evaluates the feasibility of disengaging.  If the
//   //C  threat cannot get a shot at me (as determined by the AIMPT
//   //C  routine) then disengagement is feasible.  If HSTMSL determines
//   //C  that the A/C is in the envelope of the threat's weapons then
//   //C  disengagement is infeasible.  This is also true if he has only
//   //C  to pull his nose over by 0.75 radians (43 deg) or less to have a
//   //C  shot.  If this is not so, then DISMAN is called to examine the
//   //C  situation that would occur if disengagement was attempted, at
//   //C  the time when the threat succeeded in pointing his nose at the
//   //C  conscious A/C.  If the A/C is not then in the threat's weapons
//   //C  envelope and a suitable range/range-rate situation exists
//   //C  (including a hysteresis affect) then disengagement is initiated.
//   //C#VARIABLE DESCRIPTIONS
//   //C  ?t     3-VEC - position,velocity etc. of target, also st==speed
//   //C  ?a     3-VEC - position,velocity etc. of attacker, also sa
//   //C####
//
//   //C  --local declarations
//   UtVec3d xt;
//   UtVec3d vt;
//   UtVec3d xa;
//   UtVec3d va;
//   UtVec3d aimp;
//   UtVec3d dx;
//   UtVec3d dir;
//   UtVec3d dv;
//   UtFortMat3d rweatt;
//   double sa;
//   double st;
//   double sm;
//   double cdthta;
//   double ctlos;
//   double se;
//   double rgrun;
//   double test;
//   double rmax;
//   bool laimp;
//   bool inenv;
//   bool havany;
//   int acidth;
//   int wpns[8];   //wpns[numwpn];
//
//   //C*ENDDEC
//   //whycan = " ";
//   //if (iacthr == 0)
//   if (aThreat == 0)
//   {
//      //whycan = 'NO THREATS';
//      //candis = true;
//      return true;  //goto 9000;
//   }
//   //LBM - dont worry about sam sites right now
//   //if (mm_entity(iacthr) == sament)
//   //{
//   //   //whycan = 'SAM THREAT';
//   //   //candis = true;
//   //   return true;  //goto 9000;
//   //}
//
//   //LBM - dont count enemy stores right now, assume valid 1v1 fight
//   //call mystor(iacid,wpns,havany);
//   //if(!havany)
//   //{
//   //   //whycan = 'NO MISSILES';
//   //   //candis = true;
//   //   return true;  //goto 9000;
//   //}
//
//   //call xmit(3,xp(1,me),xt);
//   //call xmit(3,vp(1,me),vt);
//   //call xmit(3,xp(1,iacthr),xa);
//   //call xmit(3,vp(1,iacthr),va);
//   xt = PositionNED();
//   vt = VelocityNED();
//   xa = PositionNED(*aThreat);
//   va = VelocityNED(*aThreat);
//
//   sa = va.Magnitude(); //xmag(va);
//   st = GetSpeed();  //spdnow(me);
//   sm = sa+1000.0;
//   laimp = aimpt(xa,sm,xt,vt,st,aimp);
//   ruse = 2.0/ftnmi;
//   //call vsub(xa,xt,dx);
//   dx = xa - xt;
//   dxmag = dx.Magnitude(); //xmag(dx);
//   if(!laimp)
//   {
//      //write(whycan,'(A,I3)') 'NO AIMPT SOLN FOR',iacidt(iacthr);
//      //candis = true;
//      return true;  //goto 9000;
//   }
//   //C     --CANNOT IMMEDIATELY RULE OUT SHOT AT ME
//   //C     --TRY A QUICK CUT:
//   //call vnorm(vt,dir);
//   dir = vt;   dir.Normalize();
//   cdthta = va.DotProduct(dir)/sa;
//   ctlos = dir.DotProduct(dx)/dxmag;
//   if(ctlos < 0.0 && cdthta < -1.0/root2)
//   {
//      //whycan = 'THREAT HAS NO SHOT';
//      //candis = true;
//      return true;  //goto 9000;
//   }
//
//   //acidth = iacidx(iacthr);
//   //C     --Form estimate of rwe(attacker) from perception of his vel, acc:
//   //C     --It will also be used as a body-axis estimator, for this application
//   UtVec3d aa = AccelerationNED(*aThreat);
//   rweatt = makcsi(va, aa);
//   //LBM - only one weapon for now, call mslenv() directly
//   //call hstmsl(xa,va,xt,vt,acidth,rweatt,rweatt,rmax,aimp,se,inenv);
//   //subr hstmsl(xa,va,xt,vt,jacid ,rbeatt,rweatt,rmax,aimp,se,inenv)
//   call
//   mslenv(jacid,xa,va,aa,alpha,xt,vt,at,misl(indmsl),rbeatt,rweatt,.false.,.true.,renv_typ,rmin,rmax,aimpm,semsl,aof,inenv,trkbl,ovrhor,1);
//
//   if(inenv && dxmag < rmax*0.75)
//   {
//      //write(whycan,'(A,I3)') 'IN THE ENV OF THR',iacidt(iacthr);
//      //candis = false;
//      return false;  //goto 9000;
//   }
//   //C     --IF HE IS NEAR A SHOT, THEN CANT DISENGAGE. DEFINE NEAR A SHOT AS
//   //C     --FUNCTION OF STEERING ERROR AND RANGE/MAX_RANGE
//   ruse = amax1(rmax,ruse);
//   test = cauchy(se,45.*rad)*border(.5*ruse-dxmag,.25*ruse);
//   if(test > 0.5)
//   {
//      //write(whycan,'(A,I3)') 'NEAR ENV OF THR',iacidt(iacthr);
//      //candis = false;
//      return false;  //goto 9000;
//   }
//   //C     --ATTACKER MUST CHASE ME TO GET A SHOT
//   //C     --SEE IF HE CAN CATCH ME
//   call disman(xt,vt,st,xa,va,sa,acmasp,0.,xbase,ruse);
//   call hstmsl(xa,va,xt,vt,acidth,rweatt,rweatt,rmax,aimp,se,inenv);
//   if(!inenv)
//   {
//      //write(whycan,'(I2,A)') iacidt(iacthr),' CANT CATCH ME';
//      //candis = true;
//      return true;  //goto 9000;
//   }
//   call vsub(xt,xa,dx);
//   call vsub(vt,va,dv);
//   dxmag = xmag(dx);
//   rdot = dot(dx,dv)/dxmag;
//   //C     --COMPUTE RUNNING RANGE
//   rgrun = 15000.0;
//   if(diseng || desdis)
//   {
//      rgrun = 10000.0+5000.0*border(GetTime()-(tdseng+30.0),10.0);
//   }
//
//   if(dxmag+20.0*rdot > rgrun)
//   {
//      //write(whycan,'(I2,A,I2)') iacidt(iacthr),' CANT CHASE TO R',ifix(rgrun/999.99)
//      return true;  //goto 9000; //LBM - is this accurate?
//   }
//   //write(whycan,'(A,I3)') 'CANT OPEN RNG vs',iacidt(iacthr);
//   //C
//   //9000  continue
//   //LBM - no need to do this now, in c++
//   //C     --GET /PRFORM/ CURRENT SINCE HSTMSL CALLS CHANGED IT
//   //call gtypd(iacid,2);
//   return false;  //LBM - is this accurate?
//}
//
// void BrawlerPlatform::evdset(WsfPlatform* aThreat,
//                                double       ruse,
//                                double       dxmag,
//                                double&      headd,
//                                double&      scale,
//                                double&      hscale,
//                                UtVec3d&     vt,
//                                double&      st,
//                                double&      dmult)
//{
//   /*
//   //C#ABSTRACT Sets parameters for use by evdis, given we'll try disengaging
//   //C#PURPOSE  Called by evdis's evdisi entry to set up parameters for use
//   //C          in evdis when evaluating candidate maneuver alternatives.
//   //C#TYPE     Maneuver
//   //C#PARAMETER DESCRIPTIONS
//   //Cin  IACTHR MM-IND - Worst threat
//   //Cout HEADD  REAL - ideal disengage heading; it IS a heading but it's in rad
//   //Cout SCALE  REAL - Scale width for deviation from nominal downhill spd VT(3)
//   //Cout HSCALE REAL - SCALE/2 = scale width for deviation from nominal spd ST
//   //Cout VT     3-VEC - Nominal (best) disengage direction vector
//   //Cout ST     REAL - Nominal (best) disengage overall speed
//   //Cout DMULT  REAL - Disengage multiplier (DISMLT)
//   //Cin  RUSE   REAL - Assessed maximum weapon range of pursuer
//   //Cin  DXMAG  REAL - Assessed distance from pursuer after disengage
//   //C           maneuver attempt
//   //C#TECHNICAL DESCRIPTION
//   //C  Initiates disengagement.  Variables used are set up by calling DISMAN.
//   //C  Maneuver is initiated by call to ALI3C.
//   //C#VARIABLE DESCRIPTIONS
//   //C  WTVEC  REAL - Sum over threats of the evade utility, uevd(iacthr)
//   //C  STVEC  REAL - Sum over threats of spd*uevd(iacthr), used to obtain
//   //C         an average best disengage speed st (spd starts as my speed
//   //C         and is modified by disman)
//   //C####
//
//   //C
//   UtVec3d vdis, xa, va, xt, vscrch;
//   double wtvec,stvec,sa,dscale,tprj;
//   int ithrt,iac, i;
//   //C*ENDDEC
//   //C     Turn off Roll-over-the-top value, if any:
//
//   call e1v1i2;
//   tprj = amin1(tproj3,5.0);
//   if (iacthr == 0)
//   {
//      //C        --No threat, form return to base vector
//      call xmit(3,xp(1,me),xt);
//      call xmit(3,vp(1,me),vt);
//      st = xmag(vt);
//      //C        --Set up imaginary threat opposite base from which to run.
//      //C        --Let disman find an attainable maneuver.
//      call vsub(xt,xbase,vscrch);
//      call vsum(xt,vscrch,xa);
//      sa = st;
//      call vmake(sa,vscrch,va);
//      call disman(xt,vt,st,xa,va,sa,acmasp,tprj,xbase,ruse);
//      call xmit(3,vt,vdis);
//   }
//   else if (nthrpp == 0)
//   {
//      call nabort('EVDSET...NTHRPP=0 & IACTHR/=0 !');
//   }
//   else
//   {
//      wtvec = 0.0;
//      stvec = 0.0;
//      call xmit(-3,0.0,vdis);
//
//      //do 1995 ithrt=1,nthrpp
//      for (ithrt=1; ithrt<=nthrpp; ++ithrt)
//      {
//         iac = lthrpp(ithrt);
//         call xmit(3,xp(1,iac),xa);
//         call xmit(3,vp(1,iac),va);
//         sa = xmag(va);
//         call xmit(3,xp(1,me),xt);
//         call xmit(3,vp(1,me),vt);
//         st = spdnow(me);
//         call disman(xt,vt,st,xa,va,sa,acmasp,tprj,xbase,ruse);
//         call vecinc(vdis,uevd(iac),vt,vdis);
//         stvec = stvec+st*uevd(iac);
//         wtvec = wtvec+uevd(iac);
//      //1995     continue
//      }
//      //C        --GET AVERAGE SPEED, VELOCITY:
//      st = stvec/wtvec;
//      call vmult(1./wtvec,vdis,vt);
//   }
//   if (disbng)
//   {
//      dmult = AircraftValue(this)*amax1(1.0,defmlt);
//      offmlt = amin1(0.5,offmlt);
//   }
//   else
//   {
//      //C        --IF XMAG(VT) IS MUCH LESS THAN ST, INDICATES VECTORS ADDED
//      //C        --DESTRUCTIVELY. THIS IS USED TO SCALE THE VALUE MULTIPLIER DMULT:
//      dmult = AircraftValue(this)*defmlt*(0.25+0.75*ramp(0.0,xmag(vt),0.75*st));
//   }
//   call vmake(st,vdis,vt);
//   scale = grav*tproj3;
//   hscale = 0.5*scale;
//   headd = atan2(vt[1-1],-vt[2-1]);
//   if(headd < 0.0)
//   {
//      headd = headd+twopi;
//   }
//   call ali3c(iacthr,vt,st);
//   return;
//   */
//}
