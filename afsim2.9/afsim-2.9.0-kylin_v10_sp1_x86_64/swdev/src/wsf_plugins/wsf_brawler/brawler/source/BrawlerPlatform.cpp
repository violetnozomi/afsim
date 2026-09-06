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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "BrawlerPlatform.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

#include "BrawlerCoordinateConversion.hpp"
#include "BrawlerEvaluation.hpp"
#include "UtMath.hpp"

// Bring in BRAWLER constants, enumerations, and util functions
using namespace BRAWLER;

BrawlerPlatform::BrawlerPlatform()
   : mMyState()
   , mMyNom()
   , mTgNom()
   , mMyPro()
   , mTimeAllowedPerSectorSearch(10.0)
   , mMissileMode(NOTASK)
   , milevel(0)
   , mkalt(0)
   , micall(0)
   , mlcall(0)
   //, skip_1v1(false)
   , tnl_reset(false)
   , tnl_p_reset(0)
   , pdraw(0)
   , aggfac(2.0)
   , cmbeff(2.0)
   , fratio(1.0)
   //, l1opp(true)  //hard code to true for now, replicate "hutil.f" later
   //, noaim(false)
   //, wepons(false)
   //, intrst(false)
   //, ready(false)
   , valrot(0.0)
   , timrot(0.0)
   , tseen(0.0)
   , nalt(0)
   , vofutl(0.0)
   , vavutl(0.0)
   //, valbar[20]()[nvalcp]  //below
   , iacoff(0)
   , mslmod(0)
   , altval_min(xlarge)
   , s_w(0.0)
   //, s_v[20]()[nvalcp]     //below
   //, s_wv[20]()[nvalcp]    //below
   //, s_vv[20]()[nvalcp]    //below
   //, s_wvv[20]()[nvalcp]   //below
   //, valcmp[20]()[nvalcp]  //below
   , tnl_vis(false)
   //, tnl_mult[20]()[nvalcp]//below
   , prbdas(false)
   , vecfpp()
   , valfpp(0.0)
   , sflypp(0.0)
   , mDesignWeightForMaxG_lbs(31820)
   , mStructuralGLimit_g(7.0)
   , mStructuralGLimitAboveDesignWeight_g(7.0)
   , mSpeedbrakeCDA_sqft(0.0)
   , rnsq()
   , mMyJob(BRAWLER::FLIGHT_LEAD) // TODO ZU- Pick proper default
   , mMission(BRAWLER::ROUTE)     // TODO ZU- Pick proper default
   , vmsnpp(5.0)                  // TODO ZU- Pick proper default
{
   for (int i = 0; i < nvalcp; ++i)
   {
      valbar[i]    = 0.0;
      s_v[i]       = 0.0;
      s_wv[i]      = 0.0;
      s_vv[i]      = 0.0;
      s_wvv[i]     = 0.0;
      rawvalcmp[i] = 0.0;
      valcmp[i]    = 0.0;
      tnl_mult[i]  = 1.0;
   }

   rtepp.Set(0.0);
   mEvasionDirection.Set(0.0);
}

BrawlerPlatform::BrawlerPlatform(const BrawlerPlatform& aSrc)
   : mMyState(aSrc.mMyState)
   , mMyNom(aSrc.mMyNom)
   , mTgNom(aSrc.mTgNom)
   , mMyPro(aSrc.mMyPro)
   , mMIND(aSrc.mMIND)
   , mTimeAllowedPerSectorSearch(aSrc.mTimeAllowedPerSectorSearch)
   , mMissileMode(aSrc.mMissileMode)
   , milevel(aSrc.milevel)
   , mkalt(aSrc.mkalt)
   , micall(aSrc.micall)
   , mlcall(aSrc.mlcall)
   , tnl_reset(aSrc.tnl_reset)
   , tnl_p_reset(aSrc.tnl_p_reset)
   , pdraw(aSrc.pdraw)
   , aggfac(aSrc.aggfac)
   , cmbeff(aSrc.cmbeff)
   , fratio(aSrc.fratio)
   , valrot(aSrc.valrot)
   , timrot(aSrc.timrot)
   , tseen(aSrc.tseen)
   , nalt(aSrc.nalt)
   , vofutl(aSrc.vofutl)
   , vavutl(aSrc.vavutl)
   , iacoff(aSrc.iacoff)
   , mslmod(aSrc.mslmod)
   , altval_min(aSrc.altval_min)
   , s_w(aSrc.s_w)
   , tnl_vis(aSrc.tnl_vis)
   , prbdas(aSrc.prbdas)
   , vecfpp(aSrc.vecfpp)
   , valfpp(aSrc.valfpp)
   , sflypp(aSrc.sflypp)
   , mDesignWeightForMaxG_lbs(aSrc.mDesignWeightForMaxG_lbs)
   , mStructuralGLimit_g(aSrc.mStructuralGLimit_g)
   , mStructuralGLimitAboveDesignWeight_g(aSrc.mStructuralGLimitAboveDesignWeight_g)
   , mSpeedbrakeCDA_sqft(aSrc.mSpeedbrakeCDA_sqft)
   , rnsq(aSrc.rnsq)
   , mMyJob(aSrc.mMyJob)
   , mMission(aSrc.mMission)
   , vmsnpp(aSrc.vmsnpp)
{
   for (int i = 0; i < nvalcp; ++i)
   {
      valbar[i]    = aSrc.valbar[i];
      s_v[i]       = aSrc.s_v[i];
      s_wv[i]      = aSrc.s_wv[i];
      s_vv[i]      = aSrc.s_vv[i];
      s_wvv[i]     = aSrc.s_wvv[i];
      rawvalcmp[i] = aSrc.rawvalcmp[i];
      valcmp[i]    = aSrc.valcmp[i];
      tnl_mult[i]  = aSrc.tnl_mult[i];
   }

   rtepp.Set(aSrc.rtepp);
   mEvasionDirection.Set(aSrc.mEvasionDirection);
}

void BrawlerPlatform::Init(UtEntity& state, ut::Random& random)
{
   mMyState = state; // deep copy, right?

   // Initialize the random number stream.  Each instance has a different seed.
   unsigned int seed = random.Uniform<unsigned>();
   rnsq.SetSeed(seed);

   BrawlerCoordinateConversion::RegisterLocationEntity(state);
}

////virtual
// void BrawlerPlatform::Update(float aSimTime)
//{
// }

double BrawlerPlatform::MindUpdate(double aSimTime)
{
   // update our knowledge of all threats & assets
   //  <   >

   double temp = 0;
   // update evasion direction (if necessary)
   BrawlerEvaluation::CalcEvadeValue(*this, temp);

   // get maneuver value component statistics ready for this update
   InitializeStatistics();
   // update other mental model knowledge:
   UpdateMissileMode();
   // do simple projections
   NominalProjection(ProjectedTimeDelta());

   // InitializeAlternativeEvaluation(); // Called to set initial conditions used when evaluating alternatives

   // consider all alternatives (call appropriate evaluate...() method for each one)
   // This takes the place of BRAWLER model() called in conevt.f
   // <   >


   CalculateStatistics(); // after all alternatives evaluated

   // Based on the current event execution time and the interval to
   // the next event, schedule the new self called event time
   double dtime         = GetConsciousnessEventTime();
   double nextEventTime = aSimTime + dtime;
   return nextEventTime;
}

// virtual
void BrawlerPlatform::InitializeStatistics()
{
   // entry valsti
   nalt = 0;
   // call xmit(-mac,0.0,vavutl);
   vavutl = 0.0;
   // call xmit(-nvalcp,0.0,valbar);
   std::fill(valbar, valbar + nvalcp, 0.0);
   iacoff = 0;
   // C     --INITIALIZE TUNNEL VISION SUMMED ARRAYS
   // call xmit(-nvalcp,0.0,s_wvv);
   // call xmit(-nvalcp,0.0,s_vv);
   // call xmit(-nvalcp,0.0,s_wv);
   // call xmit(-nvalcp,0.0,s_v);
   std::fill(s_wvv, s_wvv + nvalcp, 0.0);
   std::fill(s_vv, s_vv + nvalcp, 0.0);
   std::fill(s_wv, s_wv + nvalcp, 0.0);
   std::fill(s_v, s_v + nvalcp, 0.0);
   s_w        = 0.0;
   altval_min = xlarge;

   // lines 552-587 of aeva3i() from aeval3.f
   // C     --IF FIRST CE IGNORE TUNNEL VISION INITIALIZTION
   // if (conev1)
   if (GetTime() <= 0.1)
   {
      tnl_p_reset = 1.0;
      tnl_vis     = false;
      // goto 1100
   }
   // C     --TUNNEL VISION INITIALIZATION
   if (mMIND.TunnelTauHi() < 0.01f)
   {
      tnl_p_reset = 1.0;
      tnl_vis     = false;
   }
   else
   {
      // C        --The probability of not being in a "tunnel vision" mode is
      // C        --a function of how long since the last maneuver update and
      // C        --skill level.  The time constant decreases with increased
      // C        --skill level.  Long periods between updates will have the
      // C        --effect of decreasing the probability of being in a fixated
      // C        --mode.
      double dt   = GetTime() - LastConsciousnessEventTime();
      tnl_p_reset = 1.0 - exp(-dt / mMIND.TunnelTauHi());
      // pdraw = rnsq(0);
      pdraw     = Uniform(); // uniform random number in [0,1]
      tnl_reset = pdraw < tnl_p_reset;
      if (tnl_reset)
      {
         tnl_vis = false;
      }
      else
      {
         tnl_vis = true;
      }
   }
   // 1100  continue
   // C     --RESET MULTIPLIERS IF TNL_VIS .FALSE.
   if (!tnl_vis)
   {
      for (int i = 0; i < nvalcp; ++i)
      {
         tnl_mult[i] = 1.0;
      }
   }
}

// C#ABSTRACT COMPUTES AVG OF EACH MANVR VALUE COMPONENT OVER ALTERNS
// C#PURPOSE  COMPUTES AVG OF EACH MANVR VALUE COMPONENT OVER ALTERNS
// C  USED BY GRAPHICS IN "DOMINANT FACTOR" DISPLAY.
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTION
// CIN ALTVAL  REAL - Alternative score
// C#TECHNICAL DESCRIPTION
// C    MOST VALUE COMPONENTS ARE EQUIVALENCED INTO /VALHST/ AS VALCMP
// C  ARRAY COMPONENTS.  EACH CALL IS MADE AFTER ANOTHER MANEUVER IS
// C  EVALUATED.  SUM OF COMPONENT VALUES AND HIGH-UTILITY HOSTILES ARE
// C  MADE FOR AVERAGING PURPOSES.
// C     THE VARIOUS SUMS OF THE VALUE COMPONENTS NEEDED TO CALCULATE
// C  THE TUNNEL VISION ADJUSTMENT FACTORS ARE SUMMED IN VALST.
// C  THE ADJUSTMENT FACTORS ARE COMPUTED FOR THE NEXT MANEUVER UPDATE
// C  IN THE VALSTH ENTRY
// C    THE VALSTH ENTRY, CALLED AT END OF MANEUVER DECISION, TAKES
// C  ACTUAL AVERAGES, AND DETERMINES THE HOSTILE WITH THE HIGHEST
// C  AVERAGE OFFENSIVE SCORE.
// C#VARIABLE DESCRIPTION
// C  NHUTIL   INT [NA] - Number of aircraft on the high-utility list
// C  VALBAR   ARRAY DIM(NVALCP) OF (REAL) - VALCMP averaged over all
// C           maneuvers considered
// C  VAVUTL   ARRAY DIM(MAC) OF (REAL) - Average of VOFUTL over all
// C           maneuvers considered
// C####
// virtual
void BrawlerPlatform::UpdateStatistics(double altval)
{
   // C  --local declarations
   int i = 0;
   // int iac;
   // int imxutl;
   // int k;
   // double dummy;
   double term = 0;
   // save nalt    //LBM - this means its a class variable
   // C*ENDDEC
   nalt = nalt + 1;
   // C     --SUM THE MANEUVER ALTERNATIVE SCORES
   s_w = s_w + altval;

   // do 10 i = 1,nvalcp
   for (i = 1; i <= nvalcp; ++i)
   {
      valbar[i - 1] = valbar[i - 1] + valcmp[i - 1];
      if (tnl_vis)
      {
         // C           --FILL TUNNEL VISION SUMMED ARRAYS
         term         = valcmp[i - 1] * altval;
         s_wvv[i - 1] = s_wvv[i - 1] + term * valcmp[i - 1];
         s_wv[i - 1]  = s_wv[i - 1] + term;
         s_vv[i - 1]  = s_vv[i - 1] + pow(valcmp[i - 1], 2);
         s_v[i - 1]   = s_v[i - 1] + valcmp[i - 1];
      }
   }
   // 10       continue

   ////LBM - not doing hutil right now???
   ////do 20 i = 1,nhutil
   // for (i=1; 1<= nhutil; ++i)
   //{
   //    vavutl(i) = vavutl(i) + vofutl(i);
   //    //20       continue
   // }
   return;
}

void BrawlerPlatform::CalculateStatistics()
{
   // entry valsth
   if (nalt == 0)
   {
      // goto 9900;
      // 9900
      // mslmod=mslmd;   //LBM - doing our own version of missile mode

      // return;
   }
   else
   {
      // do 30 i=1,nvalcp
      for (int i = 1; i <= nvalcp; ++i)
      {
         valbar[i - 1] = std::abs(valbar[i - 1] / nalt);
         // 30       continue
      }
      ////LBM - not doing hutil right now
      // if(nhutil.eq.0)
      //{
      //    goto 50;
      // }

      // LBM - not doing hutil right now
      ////do 40 i=1,nhutil
      // for (int i=1; i<=nhutil; ++i)
      //{
      //    vavutl[i-1] = vavutl[i-1]/nalt;
      // }
      ////40         continue

      // LBM - not doing hutil right now
      // call vmax(vavutl,nhutil,imxutl,dummy)
      // iac = lhutil(imxutl)
      // iacoff = iacidt(iac)

      // 50    continue
      if (tnl_vis)
      {
         // C        --ADJUST TUNNEL VISION MULTIPLIERS
         AdjustTunnelVisionMultipliers();
      }
      // 9900
      // mslmod=mslmd;   //LBM - doing our own version of missile mode
   }
}


// C#ABSTRACT ADJUST TUNNEL VISION MULTIPLIERS
// C#PURPOSE  CALLED BY VALSTH TO FIND TUNNEL VISION MULTIPLIERS FOR
// C           MANEUVER VALUE COMPONENTS THE NEXT CE.
// C#TYPE     Maneuver
// C#PARAMETER DESCRIPTIONS
// C#TECHNICAL DESCRIPTION
// C This routine biases high scoring maneuver value components to create
// C  a positive feedback system in which factors (objectives) that are
// C  dominating the decision are used to "lock" the pilot into one mode.
// C  The first step is to determine which components play the largest role
// C  in the just-completed decision.  A variance analysis is performed on
// C  the value components, where the variance at issue is the variance of
// C  a component's score over the maneuver alternatives considered.  For
// C  the next maneuver decision, component weights will be raised by an
// C  amount that scales with their variance in this decision.  These
// C  components will now be more likely to dominate the next decision,
// C  so they will likely be increased in size again.  The effect will be
// C  to create a positive feedback mechanism that locks the pilot into
// C  considering only a subset of the factors he ought to consider.
// C  The updated tnl_mult(i), for the next maneuver decision, is given
// C  as a weighting between tnl_mult(i), q(i), and 1.0.  The purpose of
// C  weighting between tnl_mult(i) and q(i) is to provide smoothing.
// C
// C     V(j) - score for jth alternative
// C     v(i,j) - value of ith component for jth alternative
// C     sigma_sqr(i) - standard deviation
// C     vbar(i) - average of ith component
// C
// C                    SUM [V(j)*(v(i,j) - vbar(i))**2]
// C  sigma_sqr(i) =     j ----------------------------
// C                    SUM [V(j)]
// C                     j
// C
// C                    SUM [V(j)*v(i,j)]
// C        vbar(i) =    j ----------------
// C                    SUM [V(j)]
// C                     j
// C
// C           V(j) = SUM [v(i,j)]
// C                   i
// C
// C           q(i) = nvcp_used*sigma(i)**tnl_cntrst
// C                  ---------------------------
// C                    SUM [sigma(k)**tnl_cntrst]
// C                     k
// C#VARIABLE DESCRIPTIONS
// C   1   IACID    AC-IND [NA] - My aircraft index.
// C     6 TNL_MULT    ARRAY DIM(NVALCP) of REAL [NA] - Tunnel vision multiplers
// C                   for each maneuver value component.
// C  ALTVAL_MIN REAL - Lowest scoring alternative (V)
// C  S_W        REAL - Sum total of all alternative scores (V[j])
// C Q            REAL -  Factor that is a monotonic function of sigma but has
// C              an average value of 1.
// C TNL_CNTRST   REAL - Contrast exponent.  Larger values exaggerate the
// C              differences between weights given to large versus small
// C              sigmas.
// C VARIANCE     ARRAY DIM(NVALCP) of REAL - Variance of a component's
// C              score over the maneuver alternatives considered.
// C####
void BrawlerPlatform::AdjustTunnelVisionMultipliers()
{
   // C  --local declarations:
   double max_mult = 3.0; // parameter (max_mult=3.)
   double variance[nvalcp];
   double wbar = 0;
   double vbar = 0;
   double svar = 0;
   double term = 0;
   double q    = 0;
   int    i    = 0;
   // C*ENDDEC
   if (s_w == 0.0)
   {
      return;
   }
   // call xmit(-nvalcp,0.0,variance);
   std::fill(variance, variance + nvalcp, 0.0);
   wbar = s_w - nvcp_used * altval_min;
   svar = 0.0;
   if (wbar == 0.0)
   {
      nabort("ADJ_TNL_MLT...wbar is zero!");
   }

   // do 10 i = 1,nvcp_used
   for (i = 1; i <= nvcp_used; ++i)
   {
      // C        --average value of component i over alternatives
      vbar = (s_wv[i - 1] - altval_min * s_v[i - 1]) / wbar;
      // C        --variance of component i raised to the power of tnl_cntrst
      term = (s_wvv[i - 1] - altval_min * s_vv[i - 1]) / wbar - pow(vbar, 2);
      if (term > 1.e-10)
      {
         variance[i - 1] = pow(term, mMIND.TunnelContrast());
      }
      else
      {
         variance[i - 1] = 0.0;
      }
      svar = svar + variance[i - 1];
      // 10    continue
   }
   // C     --svar becomes a weighting factor
   // C     --RETURN IF VARIANCE IS ZERO. THIS CAN HAPPEN IF ONLY ONE
   // C     --IS CONSIDERED.
   if (svar == 0)
   {
      return;
   }
   svar = nvcp_used / svar;

   // do 20 i = 1,nvcp_used
   for (i = 1; i <= nvcp_used; ++i)
   {
      q = variance[i - 1] * svar;
      tnl_mult[i - 1] =
         mMIND.TunnelAlpha() * 1.0 +
         (1.0 - mMIND.TunnelAlpha()) * (mMIND.TunnelBetaHi() * tnl_mult[i - 1] + (1.0 - mMIND.TunnelBetaHi()) * q);
      // C        --limit the amount tnl_mult can be increased
      if (tnl_mult[i - 1] > max_mult)
      {
         tnl_mult[i - 1] = max_mult;
      }
      // 20    continue
   }
   return;
}

UtVec3d BrawlerPlatform::PositionNED()
{
   return PositionNED(State());
}

UtVec3d BrawlerPlatform::VelocityNED()
{
   return VelocityNED(State());
}

UtVec3d BrawlerPlatform::VelocityECS()
{
   return VelocityECS(State());
}

UtVec3d BrawlerPlatform::AccelerationNED()
{
   return AccelerationNED(State());
}

UtVec3d BrawlerPlatform::AccelerationECS()
{
   return AccelerationECS(State());
}

UtVec3d BrawlerPlatform::OrientationNED()
{
   return BrawlerCoordinateConversion::GetOrientationNED(State());
}

double BrawlerPlatform::GetSpeed()
{
   return GetSpeed(State());
}

double BrawlerPlatform::GetAlpha()
{
   return GetAlpha(State());
}

UtVec3d BrawlerPlatform::RelativePositionNED(UtEntity& aTgt)
{
   return RelativePositionNED(State(), aTgt);
}

UtVec3d BrawlerPlatform::RelativeVelocityNED(UtEntity& aTgt)
{
   return BrawlerCoordinateConversion::RelativeVelocityNED(State(), aTgt);
}

UtVec3d BrawlerPlatform::RelativePositionECS(UtEntity& aTgt)
{
   return RelativePositionECS(State(), aTgt);
}

UtVec3d BrawlerPlatform::RelativeVelocityECS(UtEntity& aTgt)
{
   return ConvertNEDtoECS(State(), RelativeVelocityNED(aTgt));
}

UtVec3d BrawlerPlatform::ConvertNEDtoECS(UtVec3d ned)
{
   return ConvertNEDtoECS(State(), ned);
}

UtVec3d BrawlerPlatform::ConvertNEDtoWind(UtVec3d ned)
{
   return ConvertNEDtoWind(State(), ned);
}

UtVec3d BrawlerPlatform::ConvertECStoNED(UtVec3d ecs)
{
   return ConvertECStoNED(State(), ecs);
}

UtVec3d BrawlerPlatform::ConvertECStoWind(UtVec3d ecs)
{
   return ConvertECStoWind(State(), ecs);
}

UtVec3d BrawlerPlatform::ConvertWindtoNED(UtVec3d wind)
{
   return ConvertWindtoNED(State(), wind);
}

UtVec3d BrawlerPlatform::ConvertWindtoECS(UtVec3d wind)
{
   return ConvertWindtoECS(State(), wind);
}

UtFortMat3d BrawlerPlatform::GetNEDToECSRotation()
{
   return NEDToECSRotation(State());
}

// static
UtVec3d BrawlerPlatform::PositionNED(UtEntity& aTgt)
{
   return BrawlerCoordinateConversion::GetPositionNED(aTgt);
}

// static
UtVec3d BrawlerPlatform::VelocityNED(UtEntity& aTgt)
{
   return BrawlerCoordinateConversion::GetVelocityNED(aTgt);
}

// static
UtVec3d BrawlerPlatform::VelocityECS(UtEntity& aTgt)
{
   UtVec3d vECS(0.0, 0.0, 0.0);
   aTgt.GetVelocityECS(vECS.GetData());
   vECS *= UtMath::cFT_PER_M;
   return vECS;
}

// static
UtVec3d BrawlerPlatform::AccelerationNED(UtEntity& aTgt)
{
   return BrawlerCoordinateConversion::GetAccelerationNED(aTgt);
}

// static
UtVec3d BrawlerPlatform::AccelerationECS(UtEntity& aTgt)
{
   UtVec3d ecs(0.0, 0.0, 0.0);
   aTgt.GetAccelerationECS(ecs.GetData());
   ecs *= UtMath::cFT_PER_M;
   return ecs;
}

// static
UtVec3d BrawlerPlatform::RelativePositionNED(UtEntity& ref, UtEntity& aTgt)
{
   return BrawlerCoordinateConversion::RelativePositionNED(ref, aTgt);
}

// static
UtVec3d BrawlerPlatform::RelativeVelocityNED(UtEntity& ref, UtEntity& aTgt)
{
   return (VelocityNED(aTgt) - VelocityNED(ref));
}

// static
UtVec3d BrawlerPlatform::RelativePositionECS(UtEntity& ref, UtEntity& aTgt)
{
   // UtVec3d ecs;
   // ref.GetRelativeLocationECS(&aTgt, ecs.GetData());
   // ecs *= UtMath::cFT_PER_M;
   // return ecs;

   // UtVec3d relNED = RelativePositionNED(ref, aTgt);
   // UtVec3d ecs2 = ConvertNEDtoECS(ref, relNED);
   // return ecs2;

   return ConvertNEDtoECS(ref, RelativePositionNED(ref, aTgt));
}

// static
UtVec3d BrawlerPlatform::RelativeVelocityECS(UtEntity& ref, UtEntity& aTgt)
{
   return ConvertNEDtoECS(ref, RelativeVelocityNED(ref, aTgt));
}

// static
UtVec3d BrawlerPlatform::ConvertNEDtoLLA(UtVec3d ned)
{
   return BrawlerCoordinateConversion::ConvertNEDtoLLA(ned);
}

// static
UtVec3d BrawlerPlatform::ConvertNEDtoECS(UtEntity& ref, UtVec3d ned)
{
   UtVec3d wcs(0.0, 0.0, 0.0);
   UtVec3d ecs(0.0, 0.0, 0.0);
   ned *= UtMath::cM_PER_FT;
   ref.ConvertNEDVectorToWCS(wcs.GetData(), ned.GetData());
   // wcs -= PositionWCS(ref);
   ref.ConvertWCSVectorToECS(ecs.GetData(), wcs.GetData());
   ecs *= UtMath::cFT_PER_M;
   return ecs;
}

// static
UtVec3d BrawlerPlatform::ConvertNEDtoWind(UtEntity& ref, UtVec3d ned)
{
   UtVec3d ecs = ConvertNEDtoECS(ref, ned);
   return ConvertECStoWind(ref, ecs);
}

// static
UtVec3d BrawlerPlatform::ConvertECStoNED(UtEntity& ref, UtVec3d ecs)
{
   UtVec3d wcs(0.0, 0.0, 0.0);
   UtVec3d ned(0.0, 0.0, 0.0);
   ;
   ecs *= UtMath::cM_PER_FT;
   ref.ConvertECSVectorToWCS(wcs.GetData(), ecs.GetData());
   ref.ConvertWCSVectorToNED(ned.GetData(), wcs.GetData());
   ned *= UtMath::cFT_PER_M;
   return ned;
}

// static
UtVec3d BrawlerPlatform::ConvertECStoWind(UtEntity& ref, UtVec3d ecs)
{
   UtVec3d vECS(0.0, 0.0, 0.0);
   // find ECS vector of velocity
   ref.GetVelocityECS(vECS.GetData());
   vECS.Normalize();
   // we use the fact that we can rotate a vector out the nose in body coordinates [1,0,0] to the...
   //...velocity vector using a DCM generated from alpha & beta only, we know:
   // vECS[0] = cos(alfa)*cos(beta)
   // vECS[1] = sin(beta)
   // vECS[2] = sin(alfa)*cos(beta)
   // solve for alpha & beta angles
   double beta    = asin(vECS[1]); // [-pi/2, pi/2]
   double cosBeta = cos(beta);
   double alfa    = 0.0;
   if (std::abs(cosBeta) > eps && std::abs(vECS[2] / cosBeta) < 1.0) // protect from asin() range error
   {
      alfa = asin(vECS[2] / cosBeta); // [-pi/2, pi/2]
   }                                  // else denominator is too small, so lets say no alpha, leave it at zero

   // generate rotation matrix to convert from ECS to Wind
   UtFortMat3d rotateECStoWindAxes(sin(0.0), cos(0.0), sin(alfa), cos(alfa), sin(beta), cos(beta));

   // use the rotation matrix
   // UtVec3d wind = rotateECStoWindAxes * ecs;
   // return wind;
   return (rotateECStoWindAxes * ecs);
}

// static
UtVec3d BrawlerPlatform::ConvertWindtoNED(UtEntity& ref, UtVec3d wind)
{
   UtVec3d ecs = ConvertWindtoECS(ref, wind);
   return ConvertECStoNED(ref, ecs);
}

// static
UtVec3d BrawlerPlatform::ConvertWindtoECS(UtEntity& ref, UtVec3d wind)
{
   UtVec3d vECS(0.0, 0.0, 0.0);
   // find ECS vector of velocity
   ref.GetVelocityECS(vECS.GetData());
   vECS.Normalize();
   // we use the fact that we can rotate a vector out the nose in body coordinates [1,0,0] to the...
   //...velocity vector using a DCM generated from alpha & beta only, we know:
   // vECS[0] = cos(alfa)*cos(beta)
   // vECS[1] = sin(beta)
   // vECS[2] = sin(alfa)*cos(beta)
   // solve for alpha & beta angles
   double beta    = asin(vECS[1]); // [-pi/2, pi/2]
   double cosBeta = cos(beta);
   double alfa    = 0.0;
   if (std::abs(cosBeta) > eps && std::abs(vECS[2] / cosBeta) < 1.0) // protect from asin() range error
   {
      alfa = asin(vECS[2] / cosBeta); // [-pi/2, pi/2]
   }                                  // else denominator is too small, so lets say no alpha, leave it at zero

   // generate rotation matrix to convert from Wind to ECS
   //(transpose of matrix to go from ecs to wind)
   UtFortMat3d rotateWindToECS = UtFortMat3d();
   rotateWindToECS.BuildTranspose(sin(0.0), cos(0.0), sin(alfa), cos(alfa), sin(beta), cos(beta));

   // use the rotation matrix
   // UtVec3d ecs = rotateWindToECS * wind;
   // return ecs;
   return (rotateWindToECS * wind);
}

// static
double BrawlerPlatform::GetSpeed(UtEntity& aRef)
{
   return (aRef.GetSpeed() * UtMath::cFT_PER_M);
}

// static
double BrawlerPlatform::GetAlpha(UtEntity& aRef)
{
   UtVec3d vECS(0.0, 0.0, 0.0);
   // find ECS vector of velocity
   aRef.GetVelocityECS(vECS.GetData());
   vECS.Normalize();
   // we use the fact that we can rotate a vector out the nose in body coordinates [1,0,0] to the...
   //...velocity vector using a DCM generated from alpha & beta only, we know:
   // vECS[0] = cos(alpha)*cos(beta)
   // vECS[1] = sin(beta)
   // vECS[2] = sin(alpha)*cos(beta)
   // solve for alpha & beta angles
   double beta    = asin(vECS[1]); // [-pi/2, pi/2]
   double cosBeta = cos(beta);
   double alpha   = 0.0;
   if (std::abs(cosBeta) > eps && std::abs(vECS[2] / cosBeta) < 1.0) // protect from asin() range error
   {
      alpha = asin(vECS[2] / cosBeta); // [-pi/2, pi/2]
   }                                   // else denominator is too small, so lets say no alpha, leave it at zero
   return alpha;
}

double BrawlerPlatform::SlantRange(UtEntity& aTgt, double defaultRange)
{
   double otherLocWCS[3] = {0, 0, 0};
   aTgt.GetLocationWCS(otherLocWCS);
   double thisToOtherLocWCS[3];
   State().GetRelativeLocationWCS(otherLocWCS, thisToOtherLocWCS);
   return UtVec3d::Magnitude(thisToOtherLocWCS);
   return defaultRange;
}

void BrawlerPlatform::SetAlternative(int ilevel, int kalt, int icall, int lcall)
{
   milevel = ilevel;
   mkalt   = kalt;
   micall  = icall;
   mlcall  = lcall;
}

void BrawlerPlatform::GetAlternative(int& ilevel, int& kalt, int& icall, int& lcall)
{
   ilevel = milevel;
   kalt   = mkalt;
   icall  = micall;
   lcall  = mlcall;
}


bool BrawlerPlatform::SlowFlight()
{
   UtVec3d vthen(0.0, 0.0, 0.0); // vector for forward projection
   double  slowg = std::min(mMIND.SlowGee(), 4.0);
   // call vecinc(vp(1,me),tproj3,ap(1,me),vthen);
   UtVec3d::AddProduct(vthen.GetData(), VelocityNED().GetData(), ProjectedTimeDelta(), AccelerationNED().GetData());
   double climb_ang   = arcsin(std::max(-0.999, std::min(0.999, -vthen[3 - 1] / vthen.Magnitude())));
   double leeway_term = (climb_ang / hafpi) * mMIND.SlowGeeLeeway();
   bool   slow        = (MaxInstantaneousGLoad_g() <= (slowg + leeway_term));
   return slow;
}

// replaces use of "offmus" and "offmlt"
// LBM - forced into "ATTACK" flight & pilot posture for now
//(akshn4.f line 74 + aeval3.f line 468-469)
double BrawlerPlatform::OffensiveMultiplier()
{
   // LBM - forcing into "ATTACK" mode for now
   double offmlt = 1.0 * FlightAggressiveness(); // akshn4.f
   double offmus = offmlt;                       // aeval3.f
   if (offmus > 1.0)
   {
      offmus = pow(offmus, ProbabilityOfSurvival(State(), GetTarget())); // **psrv(me)
   }
   return offmus;
}

// replace use of "defmus" and "defmlt"
// LBM - forced into "ATTACK" flight & pilot posture for now
//(akshn4.f line 75 + eclin.f line 110-11)
double BrawlerPlatform::DefensiveMultiplier()
{
   double defmlt = amin1(1.0, ForceRatio()); // akshn4.f
   double defmus = defmlt;                   // eclin.f
   if (defmus < 1.0)
   {
      defmus = pow(defmus, ProbabilityOfSurvival(State(), GetTarget())); // **psrv(me)
   }
   return defmus;
}

bool BrawlerPlatform::BoreSight()
{
   bool borsit = false;

   // replaces borsit() from borsit.f
   // In the BRAWLER code borsit.f depends heavily on fcstat.fi, akshn7.f and aslct7.f
   // These files set status used to check that the seeker has locked on the target.
   // Launch modes and seeker types are check to see if any missiles have acquired
   // If we are currently launching, or no missile has acquired the target we need to boresight

   // We don't have a model for waiting on seeker lock in WSF (missile doesn't exist
   // until fired) so for now will always assume the missile has locked on the track
   // and we don't need to boresight. If we don't have a track we need to boresight
   // to a ... track.. that ... doesn't exist.....
   if (GetTarget() != nullptr)
   {
      borsit = true;
   }

   return borsit;
}


// replaces use of "obang" set in obangs() called from minud/majud
double BrawlerPlatform::OffBoreSightAngle(UtEntity& aRef, UtEntity& aTgt)
{
   // function args:
   // int nac;
   // double /**x[3], *v[3], oba_old[20][20]*/;//(macmnd,macmnd);
   //////////////////////////////////////////////////

   // NOTE:  treat "i" index as aRef and "j" index as aTgt
   UtVec3d x1 = PositionNED(aRef);
   UtVec3d v1 = VelocityNED(aRef);
   UtVec3d x2 = PositionNED(aTgt);
   // UtVec3d v2 = VelocityNED(aTgt);

   // double wt;
   // double wtc;
   double coba = 0;
   // double dv[3];
   double spdi = 0;
   // double spdj;
   // int i;
   // int jstart;
   // int j;
   // double arcsin;
   // C*ENDDEC
   // if (nac == 1)
   // if (aRef == 0 || aTgt == 0)
   //{
   //    return 0.0;
   // }
   // if (mMIND.SmoothingTimeSeconds() != 0.0)
   //{
   //    wt = exp((LastConsciousnessEventTime()-GetTime())/mMIND.SmoothingTimeSeconds());
   //    wtc = 1.0 - wt;
   // }
   // do 20 i=1,nac-1
   // LBM - just calculating from aRef to aTgt - not all of them
   // for (int i=1; i<nac; ++i)
   //{
   //    jstart = i+1;
   spdi = v1.Magnitude(); // xmag(v(1,i));
   // do 10 j=jstart,nac
   // for (int j=jstart; j <= nac; ++j)
   //{
   // spdj = v2.Magnitude();  // xmag(v(1,j));
   // call vsub(x(1,i),x(1,j),dxp(1,i,j));
   UtVec3d dxp1 = x1 - x2;
   UtVec3d dxp2 = x2 - x1;
   // call vmult(-1.0,dxp(1,i,j),dxp(1,j,i));
   // LBM - reverse vector not needed
   // rngnow(i,j) = xmag(dxp(1,i,j));
   // rngnow(j,i) = rngnow(i,j);
   double rngnow = dxp1.Magnitude();
   if (rngnow <= eps)
   {
      // obang(i,j) = 0.0;
      // obang(j,i) = 0.0;
      return 0.0;
      // rdotpr(i,j) = 0.0;
      // rdotpr(j,i) = 0.0;
   }
   else
   {
      // C              --i OBSERVING j
      if (spdi == 0.0)
      {
         // C                 --OBSERVER AT REST
         // obang(i,j) = 0.0;
         return 0.0;
      }
      else
      {
         // coba = dot(dxp(1,j,i),v(1,i))/(rngnow*spdi);
         coba = dxp2.DotProduct(v1) / (rngnow * spdi);
         // C                 --.99 COEFFICIENT ADDED TO ROUND-OFF THE MACHINE ERROR
         // obang(i,j) = arccos(coba*0.999999);
         return arccos(coba * 0.999999f);
      }
      // LBM - only calculating from "i" to "j" in this function
      ////C              --j OBSERVING i
      // if(spdj == 0.0)
      //{
      //    //C                 --OBSERVER AT REST
      //    obang(j,i) = 0.0;
      // }
      // else
      //{
      //    coba = dot(dxp(1,i,j),v(1,j))/(rngnow(i,j)*spdj);
      //    //C                 --.99 COEFFICIENT ADDED TO ROUND-OFF THE MACHINE ERROR
      //    obang(j,i) = arccos(coba*0.999999f);
      // }

      // TODO - implement RangeRate() instead of calculating these here
      ////C              --FIND RANGE RATE
      // call vsub(v(1,i),v(1,j),dv);
      // rdotpr(i,j) = dot(dxp(1,i,j),dv)/rngnow(i,j);
      // rdotpr(j,i) = rdotpr(i,j);
   }
   // if (mMIND.SmoothingTimeSeconds() != 0.0)
   //{
   //    oba_old(i,j) = wt*oba_old(i,j)+wtc*obang(i,j);
   //    oba_old(j,i) = wt*oba_old(j,i)+wtc*obang(j,i);
   // }
   //    }
   // }
   // 10       continue
   // 20    continue
   // return;
}

double BrawlerPlatform::ProjectedTimeDelta()
{
   double tproj3 = mMIND.TimeProjection(); // Start with default tproj

   // see lines 140 - 145 of modsel.f
   if (mMissileMode == AIMFIRE)
   {
      tproj3 = 2.0;
   }
   else
   {
      // Find the nearest hostile, this duplicates code in GetConsciousnessEventTime()
      std::vector<double> ranges(3, 1.0e+12);
      // WsfTrack* nearestThreat = GetPerceptionProcessor()->NearestThreat(GetSimulation()->GetTime());
      UtEntity* nearestThreat = GetThreat();
      if (nearestThreat != nullptr)
      {
         double threatRelativeWCS[3];
         double threatWCS[3];
         nearestThreat->GetLocationWCS(threatWCS);
         State().GetRelativeLocationWCS(threatWCS, threatRelativeWCS);
         ranges[0] = UtVec3d::Magnitude(threatRelativeWCS);
      }
      ////WsfTrack* nearestMissile = GetThreatProcessor()->GetNearestThreat();
      // UtEntity* nearestMissile = nearestThreat;
      // if (nearestMissile != NULL && nearestMissile != nearestThreat)
      //{
      //    double missileRelativeWCS[3];
      //    double missileWCS[3];
      //    nearestThreat->GetLocationWCS(missileWCS);
      //    State().GetRelativeLocationWCS(missileWCS, missileRelativeWCS);
      //    ranges[1] = UtVec3d::Magnitude(missileRelativeWCS);
      // }
      //  Find minimum range, feet
      double rangeMin = *min_element(ranges.begin(), ranges.end()) * UtMath::cFT_PER_M;

      tproj3 = mMIND.TimeProjection() + (10.0 - mMIND.TimeProjection()) * ramp(75.0e+3, rangeMin, 150.0e+3);
   }

   return tproj3;
}

// replaces use of "valeff"
double BrawlerPlatform::LastComputedEngagementValue(UtEntity& aTgt)
{
   // C  VALEFF   ARRAY (MM-IDX) OF (REAL) [NA] - Last computed value for engaging each
   // majud.f line 246
   // double psabg = ProbabilityOfSurvival(aTgt);  //TODO - calculate this
   // majud.f line 266
   // valeff(iach) = (valint(iach) + vkexp(iach)) * (psrv(iach)/psabg)   //LBM  - final term "(psrv(iach)/psabg)" is 1.0
   // with a 1v1 fight assumed
   double valeff = (AircraftValue(aTgt) + ExpectedValueDestroyed(aTgt, State())) * 1.0;
   return valeff;
}


// C#PURPOSE  ASSESS THE TARGET AND POTENTIAL THREAT OF AN OBSERVED MSL
// C#TYPE     SITUATION ASSESSMENT
// C#PARAMETER DESCRIPTIONS
// CIN  IMISL  MM-IDX - INDEX OF MISSILE
// C#TECHNICAL DESCRIPTION
// C  When there is no aimpt solution, and if the missile is pointing
// C  away from target, then presume that missile has passed by.  Thus
// C  set MISLTG to zero, and MISLTA to time (loc 8).
// C  - Has owner sent me a msg telling me what the target is?  If so
// C    then do not update the estimate; just update its arrival time.
// C  - Similarly, if its my own missile.
// C  * For other owned missile, first check if it's friendly-owned;
// C    if so then double value later computed.
// C  * If a particular aircraft was assessed as the missile's target
// C    on the last iteration, multiply value later computed by  1.25.
// C  * Remove target from consideration for steering error > 90 deg.
// C  - With the above three (*) considerations in mind, loop over possible
// C    targets of missile.  Values are computed for each based on the
// C    steering error (SE) and time to intercept (DT).
// C    The assessment for self is computed differently.  While DT is the
// C    same, angular rate is used rather than steering error. This is
// C    because the angular rate calculation is much less sensitive to
// C    errors in the perceived missile velocity as the range to the
// C    intercept point gets small.
// C  - Largest value wins.
// C  For one's own missile, when target is no longer in mind,
// C  set MISLTA to time+30.
// C#VARIABLE DESCRIPTIONS:  none
// C####
// replaces use of "semmsl"
double BrawlerPlatform::DegreeOfThreat(UtEntity& aTgt)
{
   // C     --local declarations
   // double tgtval; //(macmnd);
   // double timoa;  //(macmnd);
   // int tgtlst;    //(macmnd);
   bool lsoln = false;
   // bool frndly;
   // int iac;
   // int iacown;
   // int index;
   // int itgt;
   // int jacid;
   // int jtgt;
   // int ntgt;
   double dt    = 0;
   double dxmag = 0;
   double se    = 0;
   double spdac = 0;
   double spdm  = 0;
   // double spdtgt;
   // double vmx;
   double spdmef = 0;
   // double xvdot;
   // double dt_scale=5.0;
   double t1     = 0;
   double t2     = 0;
   double angtol = 0;
   double se2    = 0;
   // double six_pack[6][6];
   UtFortMat3d losrbe = UtFortMat3d();
   UtVec3d     aimp(0.0, 0.0, 0.0);
   UtVec3d     dx(0.0, 0.0, 0.0);
   UtVec3d     vclos(0.0, 0.0, 0.0);
   UtVec3d     los(0.0, 0.0, 0.0);

   spdm = GetSpeed(aTgt); // as1tgt.f : 170
   // as1tgt.f  lines 194-222
   spdac  = GetSpeed(); // spdnow(iac)
   spdmef = amin1(spdm, 2000.0);
   if (spdmef == 0.0)
   {
      return 0.0; // goto 40;
   }
   UtVec3d xp    = PositionNED();
   UtVec3d vp    = VelocityNED();
   UtVec3d mislx = PositionNED(aTgt);
   lsoln         = AimPoint(mislx, spdmef, xp, vp, spdac, aimp);
   if (!lsoln)
   {
      return 0.0; // goto 40;
   }
   dx            = aimp - mislx;
   dxmag         = dx.Magnitude();
   UtVec3d mislv = VelocityNED(aTgt);
   se            = xlimit(dx.DotProduct(mislv) / (dxmag * spdm), 1.0, -1.0);
   se            = arccos(se);
   if (se > 90.0 * rad)
   {
      return 0.0; // goto 40;
   }

   dt = dxmag / spdmef;
   // ntgt = ntgt+1;
   // tgtlst(ntgt) = iacidx(iac);
   // if (iac == me)
   if (true)
   {
      // C           --Ownship gets enhanced evaluation
      // LBM - don't need any of this tgtval stuff for now
      // tgtval(ntgt) = 0.6*(cauchy(se,.150)+cauchy(se,0.300))*(dt_scale/(dt+dt_scale));
      // call makex(dx,losrbe);
      losrbe = makex(dx);


      // LBM - don't try to solve for LOS angle sigma using covariance matrix, right now...
      ////TODO - replicate this somehow - covariance matrix & ecmsig
      ////this is a ridiculous solution:  mudpack & ecmsig.
      ////I'm ashamed that other software developers thought this was good code at one time
      ////double msrmdl[9][9];  //msrmdl(1,1,imisl) //TODO - how to get this?
      ////mudpack(six_pack,msrmdl,1,1);
      ////angtol =  max(ecmsig(losrbe,six_pack,3,dxmag),rad);
      ////LBM - try this for now, otherwise I give up for now
      // const UtMatrixd* cov = aTgtTrack->GetResidualCovarianceMatrix();
      // if (cov == 0)
      {
         angtol = rad;
      }
      // else
      //{
      //    angtol =  std::max( LOSangleSigma(losrbe, *cov, dxmag), rad );
      // }

      t1 = (1.0 + border(17.5 - dt, 2.5));
      // C           --ALTERNATIVE FORUMLATION BASED ON LOS RATE
      vclos      = vp - mislv;
      los        = mislx - xp;
      se2        = vclos.AngleWith(los); // sepa(vclos,los);
      t2         = border(angtol + 0.2 - se2, angtol * 1.5);
      double val = (t1 * t2 * 0.5);
      return val;
   }
   // LBM - don't need any of this tgtval stuff for now
   // else
   //{
   //    tgtval(ntgt) = 0.5*(cauchy(se,.100)+cauchy(se,.200))*(dt_scale/(dt+dt_scale));
   // }
   ////C        --ASSSESSMENT FOR MYSELF GIVES LARGER VALUE FOR SAME SE, DT:
   ////C        --TO ENCOURAGE BREAKUP EFFECT OF A SHOT
   // if(itgt == iacidx(iac))
   //{
   //    tgtval(ntgt) = tgtval(ntgt)*2.0;
   // }
   // if(jtgt == iacidx(iac))
   //{
   //    tgtval(ntgt) = tgtval(ntgt)*1.25;
   // }
   // timoa(ntgt) = dt;
   return 0.0;
}


bool BrawlerPlatform::Damaged(UtEntity& aRef)
{
   bool damaged = false;

   // LBM - TODO - virtual methods?

   //// Our logic is very simple. See damage_evt.f for BRAWLER handling
   //// It has a generic case, but also damages individual subsystems
   // if (aRef.GetDamageFactor() > 0)
   //{
   //    damaged = true;
   // }

   return damaged;
}

// replaces "ppmohr" - computed in aohor() from envgeo() from mslenv.f (ovrhor)
double BrawlerPlatform::TargetAngleAboveHorizon(UtEntity& aTgt)
{
   double ahor   = 0;
   double adown  = 0;
   double ovrhor = 0;
   bool   masked = false;
   BrawlerEvaluation::AngleAboveHorizon(PositionNED(), PositionNED(aTgt), ahor, adown, ovrhor, masked);
   return ovrhor;
}

////TODO - replace "relmx"
// double BrawlerPlatform::SensorGimbalElevationLimit()
//{
//    return 0.0;
// }
//
////TODO - replace "razmx"
// double BrawlerPlatform::SensorGimbalAzimuthLimit()
//{
//    return 0.0;
// }

// replaces "mislta" computed in as1tgt.f
double BrawlerPlatform::WeaponSimTimeOfIntercept(UtEntity& aThreat)
{
   // aThreat represents a threat weapon
   // replicated all lines of as1tgt() that pertain to calculating mislta[]
   double mislta = 0;

   // C     --local declarations
   // double tgtval[macmnd],timoa[macmnd];
   // int    tgtlst[macmnd];
   // double tgtval;
   // double timoa;
   // int    tgtlst;
   bool lsoln = false;
   // bool   frndly;
   // int    iac;
   // int    iacown;
   // int    index;
   // int    itgt;
   // int    jacid;
   // int    jtgt;
   // int    ntgt;
   double dt    = 0;
   double dxmag = 0;
   double se    = 0;
   double spdac = 0;
   double spdm  = 0;
   // double spdtgt;
   // double vmx;
   double spdmef = 0;
   // double xvdot;
   // double dt_scale = 5.0;
   // double t1;
   // double t2;
   // double ecmsig;
   // double angtol;
   // double se2;
   UtVec3d     aimp(0.0, 0.0, 0.0);
   UtVec3d     dx(0.0, 0.0, 0.0);
   UtVec3d     los(0.0, 0.0, 0.0);
   UtVec3d     vclos(0.0, 0.0, 0.0);
   UtFortMat3d losrbe = UtFortMat3d();
   UtMatrixd   six_pack(6, 6);

   // C*ENDDEC
   // C     --WHAT IF MISSILE IS ON GROUND?
   UtVec3d mislx = PositionNED(aThreat);
   // if(mislx(3,imisl) >= 0)
   if (mislx[3 - 1] >= 0)
   {
      return 0.0; // goto 9999;
   }

   // LBM - in this function, it is implicit that is an other-owned missile, skip down to the "10" label

   ////C     --HAS OWNER INFORMED ME OF TARGET?
   // if(!misltf(imisl))
   //{
   //    //C        --SELF-OWNED MISSILE?
   //    if(mislow(imisl) != iacid)
   //    {
   //       goto 10;
   //    }
   // }

   ////C     --MISSILE TARGET IS KNOWN WITH CERTAINTY SO DO NOT UPDATE ESTIMATE
   ////C     --HOWEVER, UPDATE ESTIMATES OF MISLTA
   // mislse(imisl) = 0.
   // jacid = misltg(imisl)
   // if(jacid.eq.0) goto 9999

   ////C     --WHAT IF TGT HAS BEEN KILLED?
   // iac = mmindx(jacid)
   // if(iac.eq.0 .or. lmatch(deadac,ndead,jacid))
   //{
   //    //C        --I believe this guy is DEAD
   //    misltg(imisl) = 0
   //    mislta(imisl) = amin1(mislta(imisl),time+30.)
   //    //C***         call match(midbvr,nbvr,index,mislid(imisl))
   //    //C***       if(index.ne.0) call nabort('AS1TGT...confused bvr list')
   //    goto 9999
   // }

   ////C     --TOO HIGH SPEED WILL GIVE EARLY TOA
   // spdm = xmag(mislv(1,imisl))
   // spdtgt = spdnow(iac)
   // spdmef = amin1(spdm,2000.0)
   // if(std::abs(dist(mislx(1,imisl),xp(1,me))).le.6000.0)
   //{
   //    spdmef = 2000.0
   // }
   // aimpt(mislx(1,imisl),spdmef,xp(1,iac),vp(1,iac),spdtgt,aimp,lsoln);
   // vsub(xp(1,iac),mislx(1,imisl),dx);
   // xvdot = dot(dx,mislv(1,imisl));
   // if(lsoln)
   //{
   //    if (xvdot > 0.0)
   //    {
   //       //C           --MISSILE IS GOING IN "RIGHT" DIRECTION
   //       mislta(imisl) = time + dist(mislx(1,imisl),aimp)/spdmef;
   //    }
   //    else
   //    {
   //       //C           --If this is a self protection missile, MISLTA might be 0.0 at
   //       //C           -- this point.
   //       gmisld(misltp(imisl),1);
   //       if ( lbit(mskmsl,bitspm) .and. mislta(imisl) == 0.0 )
   //       {
   //          mislta(imisl) = time + 5.0;
   //       }
   //       else
   //       {
   //          //C              --GIVE THE MISSILE A CHANCE TO CHANGE COURSE (HOBS)
   //          mislta(imisl) = amin1(time+15.,mislta(imisl));
   //       }
   //    }
   // }
   // else if (xvdot < 0.0)
   //{
   //    //C        --HERE NO SOLUTION AND TARGET BEHIND MISSILE
   //    misltg(imisl) = 0
   //    mislta(imisl) = amin1(mislta(imisl),time)
   //    //C        --If this is a self protection missile, MISLTA might be 0.0 at
   //    //C        -- this point.
   //    call gmisld(misltp(imisl),1)
   //    if ( lbit(mskmsl,bitspm) .and. mislta(imisl).eq.0.0 )
   //    {
   //       mislta(imisl) = time + 5.0
   //    }
   // }
   // else
   //{
   //    //C        --THIS CODE APPEARS TO EXIST FOR MISSILES JUST LAUNCHED FROM
   //    //C        --"SLOW" AIRCRAFT WITH THE HOPE THAT IT WILL BE REVISED LATER
   //    mislta(imisl) = time+1000.
   // }
   // goto 9999;

   // C
   // C     --OTHER-OWNED MISSILE
   // 10    continue
   // C     --TOO HIGH SPEED WILL GIVE EARLY TOA
   // spdm = xmag(mislv(1,imisl));
   UtVec3d mislv = VelocityNED(aThreat);
   // spdm = GetSpeed(aThreat);
   spdm = mislv.Magnitude();
   ////C     --SET UP TO ENHANCE TGT ID IF FRIENDLY LAUNCHED MISSILE
   // match(mslnum,mmisl,index,mislid(imisl));
   // if(mislow(imisl) <= 0)
   //{
   //    itgt = 0;
   // }
   // else
   //{
   //    jacid = mislow(imisl);
   //    iacown = mmindx(jacid);
   //    if(iacown == 0)
   //    {
   //       itgt = 0;
   //    }
   //    else if(!frndly(iacown))
   //    {
   //       itgt = 0;
   //    }
   //    else if(index == 0)
   //    {
   //       itgt = 0;
   //    }
   //    else
   //    {
   //       itgt = msltar(index);
   //    }
   // }
   // jtgt = misltg(imisl)
   // mislse(imisl) = 0.0;
   // ntgt = 0;

   // LBM - only one target right now (me)
   // do 40 iac=1,nspotd
   // for (iac = 1; iac <= nspotd; ++iac)
   {
      ////C        --EXCLUDE ACTUAL LAUNCHER:
      // if(index != 0 && iacidt(iac) == msll(index))
      //{
      //    continue;   //goto 40
      // }
      // spdac = spdnow(iac);
      spdac  = GetSpeed();
      spdmef = amin1(spdm, 2000.0);
      if (spdmef == 0.0)
      {
         return 0.0; // continue;   //goto 40
      }

      UtVec3d xp = PositionNED();
      UtVec3d vp = VelocityNED();
      // aimpt(mislx(1,imisl),spdmef,xp(1,iac),vp(1,iac),spdac,aimp,lsoln);
      lsoln = AimPoint(mislx, spdmef, xp, vp, spdac, aimp);
      if (!lsoln)
      {
         return 0.0; // continue;   //goto 40
      }

      // vsub(aimp,mislx(1,imisl),dx);
      dx = aimp - mislx;
      // dxmag = xmag(dx);
      dxmag = dx.Magnitude();
      se    = xlimit(dx.DotProduct(mislv) / (dxmag * spdm), 1.0, -1.0);
      se    = arccos(se);
      if (se > 90.0 * rad)
      {
         return 0.0; // continue;   //goto 40
      }
      dt = dxmag / spdmef;

      // ntgt = ntgt+1;
      // tgtlst(ntgt) = iacidx(iac);
      // if (iac == me)
      //{
      //    //C           --Ownship gets enhanced evaluation
      //    tgtval(ntgt) = 0.6*(cauchy(se,.150)+cauchy(se,.300))*(dt_scale/(dt+dt_scale));
      //    call makex(dx,losrbe);
      //    call mudpack(six_pack,6,6,msrmdl(1,1,imisl),1,1,9,9);
      //    angtol =  max(ecmsig(losrbe,six_pack,3,dxmag),rad);
      //    t1 = (1.+border(17.5-dt,2.5));
      //    //C           --ALTERNATIVE FORUMLATION BASED ON LOS RATE
      //    call vsub(vp(1,me),mislv(1,imisl),vclos);
      //    call vsub(mislx(1,imisl),xp(1,me),los);
      //    se2 = sepa(vclos,los);
      //    t2 = border(angtol+0.2-se,angtol*1.5);
      //    t2 = border(angtol+0.2-se2,angtol*1.5);
      //    mislse(imisl) =t1*t2*0.5;
      // }
      // else
      //{
      //    tgtval(ntgt) = 0.5*(cauchy(se,.100)+cauchy(se,.200))*(dt_scale/(dt+dt_scale));
      // }
      ////C        --ASSSESSMENT FOR MYSELF GIVES LARGER VALUE FOR SAME SE, DT:
      ////C        --TO ENCOURAGE BREAKUP EFFECT OF A SHOT
      // if(itgt == iacidx(iac))
      //{
      //    tgtval(ntgt) = tgtval(ntgt)*2.0;
      // }
      // if(jtgt.eq.iacidx(iac))
      //{
      //    tgtval(ntgt) = tgtval(ntgt)*1.25;
      // }
      // timoa(ntgt) = dt;
   }
   // 40    continue
   // if (ntgt == 0)
   //{
   //    misltg(imisl) = 0;
   // }
   // else
   {
      // call vmax(tgtval,ntgt,index,vmx);
      // misltg(imisl) = tgtlst(index);
      // mislta = GetTime() + timoa(index);
      mislta = GetTime() + dt;
   }

   return mislta;
}


// replaces "ppmtrk" (from envlvl_aim -> eventually computed in envgeo.f)
// target above horizon or missile can track looking down
bool BrawlerPlatform::WeaponVerticalTrackCheck(UtEntity& aTgt)
{
   UtVec3d xa     = PositionNED();
   UtVec3d xt     = PositionNED(aTgt);
   double  ahor   = 0;
   double  adown  = 0;
   double  ovrhor = 0;
   bool    masked = false;
   BrawlerEvaluation::AngleAboveHorizon(xa, xt, ahor, adown, ovrhor, masked);
   bool trkbl = LookDownShootCapable() || ovrhor > 0.0;
   return trkbl;
}

// replaces use of "ppmse" - computed in mslenv.f (se) --> envgeo.f (se)
double BrawlerPlatform::WeaponSteeringError(UtEntity& aTgt)
{
   // envgeo.f
   // INPUT:
   // integer jacid;  aTgt
   // double alpha;
   UtVec3d xa = PositionNED();
   UtVec3d va = VelocityNED();
   UtVec3d xt = PositionNED(aTgt);
   UtVec3d vt = VelocityNED(aTgt);
   // UtFortMat3d rbeatt;   //earth to body for ownship
   // UtFortMat3d rweatt;   //earth to wind for ownship
   // bool ext_mle;

   // OUTPUT:
   UtVec3d aimp(0.0, 0.0, 0.0);
   double  se = 0;
   // double aof;
   // double elev = 0;
   // double ovrhor;
   double range = 0;
   // double oba;
   // double oelv;
   // bool trkbl;
   // bool inenv;

   // C     --Local variables
   // int ii;
   UtVec3d dpos(0.0, 0.0, 0.0);
   UtVec3d dvel(0.0, 0.0, 0.0);
   UtVec3d raima(0.0, 0.0, 0.0);
   // char who[8][3];   //?
   // char atype[4][2]; //?
   // double adown;
   // double ahor;
   // double caof;
   // double rdot = 0;
   double s  = 0;
   double sa = 0;
   double st = 0;
   // double zeta;
   double vmisl = 3000.0;
   // double s_err_max;
   // double a_off_max;
   // double zeta_max;
   // double azang;
   // double elang;
   // double senocn;
   // UtFortMat3d rbecan, rwecan;
   // double dumy;
   // double dumy1;
   // double macha,macht;
   UtVec3d xtemp(0.0, 0.0, 0.0);
   // double bodypoint(3),zaxis(3),roba(3,3),bodylos(3);
   // double y;
   // double p;
   // double r;
   // double mag2;
   // bool laimp = false;
   bool lvast = false;

   // C     --SPEEDS
   sa = va.Magnitude();
   st = vt.Magnitude();
   // call getqm(sa,-xa(3),dumy1,macha);
   // call getqm(st,-xt(3),dumy1,macht);

   // C     --AIMPOINT
   s          = 0.25 * (sa - 1000.0) + vmisl;
   bool laimp = AimPoint(xa, s, xt, vt, st, aimp);
   if (!laimp)
   {
      // C        --IF AIMPOINT CALC FAILS
      // C        --LET ATKR ASSUME SPEED OF TARGET PLUS 200 FPS
      s     = st + 200.0;
      lvast = AimPoint(xa, s, xt, vt, st, aimp);
      if (!lvast)
      {
         // assert(lvast); //else nabort('envgeo...no aimpoint solution, ');
         nabort("envgeo...no aimpoint solution, ");
      }
   }
   // C     --COMPUTE ATTACK GEOMETRY
   dpos = xt - xa;
   dvel = vt - va;
   // C     -NOTE XMAG BELOW YIELDS sqrt(dpos1**2+dpos2**2)
   range = dpos.Magnitude();
   // elev = arcsin(-dpos[3 - 1] / range);
   // rdot = dpos.DotProduct(dvel) / range;
   // call vdir(xa,aimp,raima);
   raima = aimp - xa;
   raima.Normalize();
   // C     --AVERAGE POINT AND LEAD:
   // call vecinc(raima,1.0/range,dpos,raima);  //vecinc(aa,b,bb,cc) -> CC = AA + B*BB
   raima = raima + (1.0 / range) * dpos;
   raima.Normalize();
   // LBM - don't use canards
   // C     --CANARDS
   // call canang(jacid,macha,-xt(3),azang,elang);
   // call bodcan(rbeatt,rweatt,azang,elang,raima,rbecan,rwecan,dumy);
   // C     --COMPUTE STEERING ERROR - use fact that xmag(raima)=1
   // because of no canards, use rbeatt and rweatt (not rbecan and rwecan)
   // call vxfrmc(rbeatt,raima,xtemp,1);
   // xtemp = rbeatt * raima;
   xtemp = ConvertNEDtoECS(raima);
   // C     --If the missile has more than one seeker, find the largest values for
   // C     --steering error, angle off, and gimbal limits.  Use these to determine
   // C     --if the target is in the envelope.
   se = arccos(xtemp[1 - 1]);
   return se;
}


double BrawlerPlatform::RangeRate(UtEntity& aRef, UtEntity& aTgt)
{
   // replicates code in obangs.f and svpred.f

   UtVec3d x1 = PositionNED(aRef);
   UtVec3d v1 = VelocityNED(aRef);
   UtVec3d x2 = PositionNED(aTgt);
   UtVec3d v2 = VelocityNED(aTgt);

   // Calculate range between platforms
   UtVec3d dx     = x2 - x1;
   double  rngnow = dx.Magnitude();

   // Calculate difference in velocity
   UtVec3d dv = v2 - v1;

   return dx.DotProduct(dv) / rngnow;
}


// replaces "vkexp" calculated in majud.f
double BrawlerPlatform::ExpectedValueDestroyed(UtEntity& aRef, UtEntity& aTgt)
{
   // majud.f line 195 (we do not need sum, just 1v1 for now)
   // iac = killer / jac = victim
   // vkexp(iac) = vkexp(iac)+valint(jac)*pkil(iac,jac);
   double vkexp = AircraftValue(aTgt) * ProbabilityOfKill(aRef, aTgt);
   return vkexp;
}

// C#ABSTRACT EVALUATES PROB. THAT FRIENDLY A/C HAVE BEEN SEEN
// C#PURPOSE  EVALUATES THE PROBABILITY THAT EACH FRIENDLY AIRCRAFT HAS
// C  BEEN SEEN BY ANY OF THE HOSTILES (PSEEN ARRAY IN /MIND4/).
// C#TECHNICAL DESCRIPTION
// C     THIS SUBROUTINE'S METHODOLOGY IS DESCRIBED IN DETAIL IN THE
// C  SECTION ON PROBABILITIES OF DETECTION IN THE ANALYST'S MANUAL.
// C     THE LARGE DO LOOP 50 CONTAINS
// C  THE CODE THAT EVALUATES, FOR EACH HOSTILE, HIS LIKELY ACTIVITIES:
// C  MANEUVERING OR NON-MANEUVERING, TURNING TOWARDS FRIENDLIES,
// C  ALREADY POINTING AT FRIENDLIES, ETC.  THE MARKOV UPDATE IS
// C  PERFORMED NEXT (CODE BETWEEN LOCATIONS 50 AND 60), AND THEN A
// C  BAYESIAN UPDATE IS PERFORMED FOR THE FLIGHT AS A WHOLE (PDETFL) TO
// C  DETERMINE THE PROBABILITY THAT THE FLIGHT HAS BEEN DETECTED.  THE
// C  REMAINING CODE, FROM LOCATION 100, EVALUATES THE BAYESIAN UPDATE
// C  TO THE DETECTION PROBABILITY FOR INDIVIDUAL FLIGHT MEMBERS,
// C  FOLLOWING THE DESCRIPTION IN THE ANALYST MANUAL CLOSELY.
// C#VARIABLE DESCRIPTION
// C  IACIDT   ARRAY (MM-IDX) OF (AC-IND) [NA] - Actual aircraft ID.   Identical  to IACIDX.
// C  PDETFL   REAL [NA] - Probability flight is detected
// C  PSEEN    ARRAY (MM-IDX) OF (REAL) [NA] - Probability a friendly aircraft has been detected
// C####
// replaces "pseen" from pbseen.f

// LBM - not needed
////relationship of other aircraft
// bool inmyfl(int iac)
//{
//    return irel[iac-1] < 4 && irel[iac-1] > 0;
// }

double BrawlerPlatform::ProbabilitySeen(UtEntity& looker, UtEntity& seen)
{
   // iacf = seen
   // iach = looker
   //#define mfrnd 1
#define ngg 1
   //#define nbg 1

   double pseen = 0.0; // probability "seen" has been seen
   // LBM - friendlies = 1 (for this 1v1 demo)
   // oba(mac),doba(mac)
   // double oba[20],doba[20];
   double oba    = 0;
   double doba   = 0;
   double obamin = 0;
   double obamns = 0;
   double dobamn = 0;
   double denom  = 0;
   double vlarge = 0;
   double d5     = 0;
   double d20    = 0;
   double d50    = 0;
   double d55    = 0;
   double d90    = 0;
   double d120   = 0;
   double spdh   = 0;
   double spdold = 0;
   double delv   = 0;
   double factor = 0;
   double aobs   = 0;
   double atow   = 0;
   double atto   = 0;
   double wt     = 0;
   double dels0  = 0;
   double delv0  = 0;
   // int ibg;
   // int igg;
   // int iach;
   // int iacf;
   int ntowi = 0;
   int nobsi = 0;
   int nttoi = 0;
   // int ntowrd(mac),nobsrv(mac),nturnt(mac), ntow(mac),nobs(mac),ntto(mac);
   // int ntowrd[20],nobsrv[20],nturnt[20],ntow[20],nobs[20],ntto[20];
   int ntowrd = 0;
   int nobsrv = 0;
   int nturnt = 0;
   int ntow   = 0;
   int nobs   = 0;
   int ntto   = 0;
   // int ltowrd(mfrnd,mac),lobsrv(mfrnd,mac),lturnt(mfrnd,mac);
   // int ltowrd[mfrnd][20],lobsrv[mfrnd][20],lturnt[mfrnd][20];
   // int ltowrd,lobsrv,lturnt;
   // int i;
   int  ntowts = 0;
   int  nobsts = 0;
   int  nttots = 0;
   bool toward = false;
   bool away   = false;
   bool observ = false;
   bool trnawy = false;
   bool towj;
   bool awayj;
   bool turnto = false;
   bool manuvd = false;
   bool manj;
   bool nwevid = false; //,early;  //inmyfl;
   // C
   // int iac;
   // C
   // double    DIST  ,XMAG
   // C*ENDDEC

   // if(nbg == 0)
   //{
   //    goto 999;
   // }

   vlarge = xlarge / 1000.0;
   toward = false;
   away   = true;
   observ = false;
   manuvd = false;
   turnto = false;
   trnawy = false;
   delv0  = mMIND.MinTimeBetweenHostAssessments() * grav;
   dels0  = mMIND.MinTimeBetweenHostAssessments() * grav * 0.3;
   d5     = 5.0 * rad;
   d20    = 20.0 * rad;
   d50    = 50.0 * rad;
   d55    = 55.0 * rad;
   d90    = hafpi;
   d120   = 120.0 * rad;
   // LBM - no such thing as early
   // early = tseen+mMIND.MinTimeBetweenHostAssessments() > GetTime();
   nwevid = false;
   // C
   // C  THIS SECTION TRIES TO ASSESS, FOR EACH HOSTILE, HIS LIKELY ACTIVITY.
   // C  HE MAY POINT AT FRIENDLIES OR BE TURNING TOWARD FRIENDLIES (SOME
   // C  ACTIVITES ARE MUTUALLY EXCLUSIVE), HE MAY BE KEEPING FRIENDLIES IN
   // C  SIGHT, OR BE POINTING AWAY FROM FLIGHT, OR BE TURNING AWAY FROM
   // C  FLIGHT. IF EARLY, ONLY NEWLY SIGHTED A/C MAY BE CONSIDERED.
   // C  SOME GLOBAL CONDITIONS (TOWARD, AWAY, ETC.) ARE TRUE IF ANY
   // C  HOSTILE IS ENGAGED IN THEM, EXCEPT THAT AWAY REQUIRES ALL HOSTILES.
   // C

   // for (int ibg=1; ibg<=nbg; ++ibg)
   //{
   // do 50 ibg=1,nbg
   // iach = listh(ibg);
   // LBM - no such thing as early, each query needs computed (for now)
   ////C     IF EARLY CONSIDER ONLY NEW HOSTILES
   // if(early && fstobs < GetTime())
   //{
   //    continue;   //goto 50;
   // }
   nwevid = true;
   obamin = vlarge;
   obamns = vlarge;
   dobamn = vlarge;
   ntowi  = 0;
   nobsi  = 0;
   nttoi  = 0;
   spdh   = GetSpeed(); // spdnow(iach);
   // spdold = xmag(vpold(1,iach));
   // delv = dist(vp(1,iach),vpold(1,iach));
   delv   = mMIND.MinTimeBetweenHostAssessments() * AccelerationNED(looker).Magnitude();
   spdold = spdh + delv * mMIND.MinTimeBetweenHostAssessments();
   manj   = delv > delv0 || std::abs(spdh - spdold) > dels0;
   towj   = false;
   awayj  = true;

   // for(int igg=1; igg<=ngg; ++igg)
   //{
   // do 10 igg=1,ngg
   // iacf = listf(igg);
   oba = OffBoreSightAngle(looker, seen); // obang(iach,iacf);

   // compute obaold in place:
   // call vsub(xp(1,iach),xp(1,iacf),dx)  //dx = xp(iach) - xp(iacf)
   // obaold(iacf,iach) = sepa(dx,vp(1,iacf))
   // obaold(jspotd,i) = pi-sepa(dx,vp(1,jspotd))
   UtVec3d dx = PositionNED(looker) - PositionNED(seen);
   // UtVec3d vpf = VelocityNED(*seen);
   // double obaold_f2h = dx.AngleWith(vpf); //sepa(dx,vpf);
   UtVec3d vph        = VelocityNED(looker);
   double  obaold_h2f = pi - dx.AngleWith(vph); // sepa(dx,vph);

   doba = oba - obaold_h2f; // obaold(iach,iacf);

   if (doba < -d5)
   {
      obamns = amin1(obamns, oba);
   }

   obamin = amin1(obamin, oba);
   dobamn = amin1(dobamn, doba);
   // if(!inmyfl)
   //{
   //    continue;   //goto 10;
   // }

   towj  = towj || oba < d20;
   awayj = awayj && oba > d120;
   // 10       continue
   // }

   // for (int igg=1; igg<=ngg; ++igg)
   //{
   // do 40 igg=1,ngg
   // iacf = listf(igg)
   if (!(oba > d20 || oba > obamin + d5))
   {
      // C                   TOWARD IACF:
      ntowi = ntowi + 1;
      // ltowrd(ntowi,iach) = iacf;
   }
   else
   {
      // C                   OBSERV?
      if (oba <= d55)
      {
         nobsi = nobsi + 1;
         // lobsrv(nobsi,iach) = iacf;
         // observ = observ || inmyfl(iacf);  //LBM - no inmylf for now
      }
      // C                   TURNTO?
      if (!(oba > d90 || oba > obamns * 1.33 || obamin < d20 || doba > -d5 || !manj))
      {
         nttoi = nttoi + 1;
         // lturnt(nttoi,iach) = iacf;
         // turnto = turnto;  // || inmyfl(iacf);  //LBM - no inmyfl for now //self assignment
      }
   }
   // 40          continue
   // }

   ntowrd = ntowi;
   nobsrv = nobsi;
   nturnt = nttoi;
   // C     TURNING AWAY?
   trnawy = trnawy || (!awayj && manj && obamin > d50 && dobamn > d5);
   toward = toward || towj;
   away   = away && awayj;
   manuvd = manuvd || manj;
   // 45       continue
   // 50       continue
   // }

   // C
   if (!nwevid)
   {
      return pseen; // goto 999;
   }
   // C
   // C  MARKOV UPDATE:
   // C
   // if(early) //LBM - no such thing as early
   //{
   //    goto 60;
   // }

   wt = 1.0 - exp(-(GetTime() - tseen) * mMIND.DefaultHostileAssessmentRate());
   // LBM - no need to update pdetfl, for now
   // pdetfl = pdetfl+wt*(1.0-pdetfl);
   tseen = GetTime();

   // for(int igg=1; igg<=ngg; ++igg)
   //{
   // do 55 igg=1,ngg
   // iacf = listf(igg);
   pseen += wt * (1.0 - pseen);
   // 55
   // }

   // 60    continue
   // C
   // C  BAYESIAN UPDATE FOR FLT AS A WHOLE:
   // C     FACTOR IS THE LIKELIHOOD RATIO THAT THE ACTIVITY OCCURS WHEN
   // C     FRIENDLIES ARE IN A DETECTED VS NON-DETECTED STATE. ONLY THE
   // C     MOST SIGNIFICANT ACTIVITY IS COUNTED.
   // C
   factor = 0.5;
   if (away)
   {
      factor = 0.75;
   }
   if (manuvd)
   {
      factor = 1.25;
   }
   if (observ)
   {
      factor = 1.25;
   }
   if (trnawy)
   {
      factor = 1.50;
   }
   if (turnto)
   {
      factor = 3.00;
   }
   if (toward)
   {
      factor = 3.00;
   }

   // LBM - no need to update pdetfl, for now
   // pdetfl = pdetfl/(pdetfl+(1.0-pdetfl)/factor);
   // C
   // C  NEXT SECTION UPDATES PSEEN FOR INDIVIDUAL A/C
   // C
   if (!(observ || toward || turnto))
   {
      // C  UPDATE INDIVIDUALS AS A GROUP IF NO INDIVIDUAL EVIDENCE:
      // for(int igg=1; igg<=ngg; ++igg)
      //{
      // do 70 igg=1,ngg
      // iacf = listf(igg);
      pseen = pseen / (pseen + (1.0 - pseen) / factor);
      // 70    continue
      // }
      return pseen; // goto 990;
   }

   // C  UPDATE AS INDIVIDUALS WHEN OBSERV OR TOWARD OR TURNTO:
   // C     FIRST GENERATE XREF COUNTS, FOR EACH FRIENDLY, OF THE
   // C     NUMBER OF HOSTILES POINTING, TURNING TO, AND OBSERVING HIM.
   // C     ALSO GENERATE RMS VALUES (COMPROMISE OF AVERAGE AND MAX).
   // call xmit(-mac,0,ntow);
   // call xmit(-mac,0,nobs);
   // call xmit(-mac,0,ntto);
   ntow = nobs = ntto = 0;

   // for(int ibg=1; ibg<=nbg; ++ibg)
   //{
   // do 160 ibg=1,nbg
   // iach = listh(ibg);
   // if(!early || fstobs == GetTime())
   // LBM: fstobs == first observed time
   // LBM - this replication of fstobs capability may not work if perception processor updates separately
   if (FirstObservedTime(&seen) == GetTime()) // LBM - no such thing as early
   {
      if (ntowrd != 0)
      {
         ntow = ntow + 1;
      }
      if (nobsrv != 0)
      {
         nobs = nobs + 1;
      }
      if (nturnt != 0)
      {
         ntto = ntto + 1;
      }
   }
   // 160      continue
   // }

   ntowts = 0;
   nobsts = 0;
   nttots = 0;

   // for(int igg=1; igg<=ngg; ++igg)
   //{
   // do 170 igg=1,ngg
   // iacf = listf(igg);
   ntowts = ntowts + (int)pow((double)ntow, 2); // LBM - assume no int overflow  :/
   nttots = nttots + (int)pow((double)ntto, 2); // LBM - assume no int overflow  :/
   nobsts = nobsts + (int)pow((double)nobs, 2); // LBM - assume no int overflow  :/
   // 170      continue
   // }
   atow = sqrt(double(ntowts) / double(ngg));
   aobs = sqrt(double(nobsts) / double(ngg));
   atto = sqrt(double(nttots) / double(ngg));
   // C  NOW GENERATE LIKELIHOOD RATIO "FACTOR" FOR EACH FRIENDLY:

   // for(int igg=1; igg<=ngg; ++igg)
   //{
   // do 260 igg=1,ngg
   factor = 1.0;
   // iacf = listf(igg);
   if (ntow != 0)
   {
      factor = factor * amax1(1.5, 3.0 * ntow / atow);
   }
   else if (ntto != 0) // C     HERE NO ONE POINTING AT IACF
   {
      factor = factor * amax1(1.25, 2.0 * ntto / (atow + atto));
   }
   else if (nobs == 0) // C     HERE NO ONE TURNING TOWARD EITHER
   {
      factor = factor * 0.5 / amin1(atow + aobs + atto + 1.0, 4.0);
   }
   else // C     OBS ONLY - CONSIDER LIKELIHOOD OF TOW, TURNTO
   {
      denom  = amin1(3.0, atow + atto + 1.0);
      factor = factor * 1.25 / denom;
   }
   // 250      continue
   pseen = pseen / (pseen + (1.0 - pseen) / factor);
   // 260      continue
   // }

   // 990
   // 999
   // return 0.5;
   return pseen;
}

// replaces use of "psrv" or "psrvfl" calculated in majud.f
double BrawlerPlatform::ProbabilityOfSurvival(UtEntity& aRef, UtEntity* aTgt)
{
   // C  PSRV     ARRAY (MM-IDX) OF (REAL) [NA] - Probability aircraft will survive, not counting me killing him
   // C  PSRVFL   ARRAY (MM-IDX) OF (REAL) [NA] - Probability aircraft will survive
   // double psrv0 = 0.0;  //(mac);
   double psrv0f = 1.0; //(mac);
   double psrv   = 0;
   double psrvfl = 0;

   // lines 149-161
   if (aTgt == nullptr)
   {
      // no hostiles
      if (Damaged(aRef))
      {
         psrv = 0.5;
      }
      else
      {
         psrv = 1.0;
      }
      return psrv;
   }

   // C     --GENERATE SURVIVAL PROBS FOR HOSTILES
   // C     --GENERATE FRIENDLY MISC DATA
   psrv = pow(0.9, GetNumberThreats());
   if (Damaged(aRef))
   {
      psrv *= 0.5;
   }
   psrvfl = psrv;
   // do 20 jac=1,nspotd
   // for(int jac=1; jac<=nspotd; ++jac)
   //{
   psrvfl *= (1.0 - ProbabilityOfKill(*aTgt, aRef) * sqrt(psrv0f));
   // 20       continue
   // }
   return 1.0;
}


// LBM - simplified for 1v1 demo
// replaces "pkil"
double BrawlerPlatform::ProbabilityOfKill(UtEntity& killer, UtEntity& victim)
{
   // iac = killer
   // jac = victim
   // C  PKIL     ARRAY (MM-IDX,MM-IDX) OF (REAL) [NA] - Probability i will kill j
   // C  PATK     ARRAY (MM-IDX,MM-IDX) OF (REAL) [NA] - Probability i will attack j

   double pseen  = ProbabilitySeen(killer, victim);
   double pdetfl = pseen; // LBM - simplified for 1v1 (flight == me) //TODO fix

   // do 10 iac=1,nspotd
   // double sumsq = 0.0;
   // double vkexp = 0.0;   //vkexp(iac)
   // C        --IN LOOP 5, PATK IS A TEMP VARIABLE TO SAVE A REPEAT COMPUTATION
   // C        --IT IS NOT TO BE THOUGHT OF AS A FINAL PATK UNTIL RECOMPUTED
   // C        --IN LOOPS 7 AND 8
   // C        --IN LOOP BELOW, TEMP IS A SURROGATE PSEEN THAT FAVORS CLOSE RANGES
   // do 5 jac=1,nspotd
   // if (jac .ne. iac)
   //{
   UtVec3d dx     = RelativePositionNED(killer, victim);
   double  rngnow = dx.Magnitude();
   double  temp   = cauchy(rngnow, 15.e3);
   temp           = pseen + 0.5 * (1.0 - pseen) * temp;
   // patk relevant from killer to victim (iac to jac)
   double sem2 = BrawlerEvaluation::SelfEngagementMeasure(*this, killer, victim);
   double patk = (temp / (rngnow + 10.e3)) * pow((sem2 * (1.0 + ramp(hafpi, OffBoreSightAngle(killer, victim), 0.0))), 2);
   // patk(iac,jac)
   // }
   // else
   //{
   //    patk(jac,jac) = 0.0;
   // }
   // sumsq = sumsq+patk;  //(iac,jac)  //sumsq == patk
   // 5        continue

   // do 6 ibg=1,nbg
   //    jac = listh(ibg)
   //    patk(iac,jac) = patk(iac,jac)/sumsq
   ////6        continue

   // do 7 igg=1,ngg
   // jac = listf(igg)
   // patk(iac,jac) = pdetfl*(patk(iac,jac)/sumsq)
   patk = pdetfl * 1.0; //(patk/sumsq) == 1.0
   // 7        continue

   // do 8 jac=1,nspotd
   // if (jac .ne. iac)
   //{
   // double pkil = sem2(killer,victim) * patk;
   double pkil = BrawlerEvaluation::SelfEngagementMeasure(*this, killer, victim) * patk;
   //}
   // else
   //{
   // pkil(jac,jac) = 0.0;
   //}


   // LBM //TODO - save these?
   // vkexp(iac) = vkexp(iac)+valint(jac)*pkil(iac,jac);
   // psrv0f(jac) = psrv0f(jac) * (1.-pkil(iac,jac));

   // 8        continue
   // 10    continue

   // return 0.5;
   return pkil;
}

// virtual
bool BrawlerPlatform::HaveWeapons()
{
   // return wepons;
   return false;
}

bool BrawlerPlatform::Skip1v1()
{
   if (GetTarget() == nullptr)
   {
      return true;
   }
   else if (RelativePositionNED(*GetTarget()).Magnitude() * ftnmi <= 6.0) // target within 6 nm
   {
      return false;
   }
   else
   {
      // && dirspc == 0);  //dirspc always zero for now (no interactive pilot)
      return !(ValueVectoredFlight() == 0.0 && NoAim());
   }
}

bool BrawlerPlatform::NoAim()
{
   // C     --NOAIM IF MISSILE MODE NOT 1 OR 2 OR
   // C     --DISENGAGING AND NOT ABOUT TO FIRE A WPN REQ. AIMING
   // noaim = !(intrst||ready)||(diseng&&!(ready&&(ppmknd==kndgun||(!CommandGuidedWeapon()&&CurrentWeaponSeekerType(irskr)))));
   // LBM - no disengaging right now
   // C     --NO AIMING IF WEAPON IS A DEW...ANOTHER ALTERNATIVE AND MANEVUER
   // C     --COMPONENT TAKES CARE OF THAT
   return (!(InIntercept() || Ready()) || (WeaponType() == knddew));
}

bool BrawlerPlatform::InIntercept()
{
   // intrst = lbit(mslmd,1);
   return (mMissileMode == INTERCEPT);
}

bool BrawlerPlatform::Ready()
{
   // ready = lbit(mslmd,2);
   return (mMissileMode == AIMFIRE);
}

bool BrawlerPlatform::OneVsOneOp()
{
   // return l1opp;
   return true; // hard code to true for now, replicate "hutil.f" later
}


// similar to svpred.f
// TODO - call this once each update before quantum tasker runs (to setup mMyNom & mTgNom)
void BrawlerPlatform::NominalProjection(double dt)
{
   // update nominal states if possible:
   UtVec3d xeuan(0.0, 0.0, 0.0);
   // LBM - simpler nominal projection for self, here:
   // C     --Get my nominal sv - assume no acceleration or angular rates
   UtVec3d xp = PositionNED();
   UtVec3d vp = VelocityNED();
   xeuan      = xp + dt * vp;
   if (xeuan[3 - 1] >= 0.0)
   {
      xeuan[3 - 1] = -1.0;
   }
   mMyNom = mMyState; // everything but position stays the same in our nominal projection of self
   // mMyNom.SetLocationWCS(ConvertNEDtoWCS(xeuan).GetData());
   UtVec3d lla = ConvertNEDtoLLA(xeuan);
   mMyNom.SetLocationLLA(lla[0], lla[1], lla[2]);

   // LBM - targets only here:
   // C     --Get projected sv for target ac
   // C     --Project target assuming constant G in wind axes
   // C     --discard small accelerations:
   // WsfPlatform* tgt = GetTargetPlatform();
   UtEntity* tgt = GetTarget();
   if (tgt == nullptr)
   {
      return;
   }
   UtVec3d apTgt = AccelerationNED(*tgt);
   UtVec3d vpTgt = VelocityNED(*tgt);
   UtVec3d a_use(0.0, 0.0, 0.0);
   if (apTgt.Magnitude() > 0.5 * grav)
   {
      a_use = 0.5 * apTgt;
   }
   // C     --Make sure that acceleration a_use won't make the aircraft
   // C     --get a negative speed inside projw.  Do this by calling a_lim.
   UtVec3d xtemp_ispotd = PositionNED(*tgt);
   if (xtemp_ispotd[3 - 1] >= 0.0) // if below ground
   {
      xtemp_ispotd[3 - 1] = -1.0; // set to one unit above ground
   }
   // call atmos(-xtemp_ispotd[3-1],t,pres,dens,vs,pr,dr);
   double dr     = AtmosphereDensityRatio(-xtemp_ispotd[3 - 1]);
   double spdnow = GetSpeed(*tgt);
   if (spdnow > 0.1)
   {
      a_use = a_lim(xtemp_ispotd, vpTgt, apTgt, dt, 300.0 / sqrt(dr));
   }
   else
   {
      // call zero3(a_use);
      a_use.Set(0, 0, 0);
   }
   UtVec3d     xeut(0.0, 0.0, 0.0); // projected target position
   UtVec3d     veut(0.0, 0.0, 0.0); // projected target velocity
   UtVec3d     aout(0.0, 0.0, 0.0); // projected target accel
   UtFortMat3d rweut = UtFortMat3d();
   projw(xtemp_ispotd, vpTgt, a_use, dt, xeut, veut, aout, rweut);
   // keep forward extrapolation above ground - clamp it
   if (xeut[3 - 1] >= 0.0)
   {
      xeut[3 - 1] = -1.0;
   }
   double      alftgt = GetAlpha(*tgt) / 2.0;
   UtFortMat3d rbeut  = bodyax(rweut, alftgt);

   // update the targets nominal state
   mTgNom = *((UtEntity*)tgt); // starting point
   // mTgNom.SetLocationWCS(ConvertNEDtoWCS(xeut).GetData());
   lla = ConvertNEDtoLLA(xeut);
   mTgNom.SetLocationLLA(lla[0], lla[1], lla[2]);

   mTgNom.SetVelocityNED(veut.GetData());
   mTgNom.SetAccelerationNED(aout.GetData());
   // mTgNom.SetOrientationNED();
   double e2b[3][3] = {{rbeut[0], rbeut[3], rbeut[6]}, {rbeut[1], rbeut[4], rbeut[7]}, {rbeut[2], rbeut[5], rbeut[8]}};
   double heading, pitch, roll;
   UtEntity::ExtractEulerAngles(e2b, heading, pitch, roll); // see function header in UtEntity.cpp
   mTgNom.SetOrientationNED(heading, pitch, roll);          // TODO - test units???

   ////C     --Get nominal relative positions and velocities
   // UtVec3d dxeuan = RelativePositionNED(mMyNom, mTgNom);  //earth axes, nominal position delta
   // UtVec3d dveuan = RelativeVelocityNED(mMyNom, mTgNom); //earth axes, nominal velocity delta
   // UtVec3d dxwutn;   //dxeuan in target wind axes
   // UtVec3d dxwuan;   //dxeuan in my wind axes
   // UtVec3d dxbuan;   //dxeuan in my body axes
   // UtVec3d dvwutn;   //dveuan in target wind axes
   // UtVec3d dvwuan;   //dveuan in my wind axes
   // rngun(ispotd) = xmag(dxeuan(1,ispotd))
   // rngrun(ispotd) = dot(dxeuan(1,ispotd),dveuan(1,ispotd))/rngun(ispotd)
}


// C#ABSTRACT Limits acceleration vector to avoid neg speeds during projections
// C#PURPOSE  Created to avoid the perennial problem of negative speeds
// C  inside projw.
// C#TYPE     Utility
// C#PARAMETER DESCRIPTIONS
// CIN  X_IN      (3-VEC) - Position of entity that is going to be projected.
// C              EARTH COORDINATES.
// CIN  V_IN      (3-VEC) - Velocity of entity that is going to be projected.
// C              EARTH COORDINATES.
// C              --must not be all zeros--
// CIN  A_IN      (3-VEC) - Acceleration of entity that is going to be projected.
// C              EARTH COORDINATES.
// CIN  DT        (REAL) - Time interval for upcoming projection.
// CIN  MIN_SPEED (REAL) - Minimum allowable speed in the X-direction in
// C              the maneuver plane.
// COUT A_USE     (3-VEC) - Limited acceleration.
// C#TECHNICAL DESCRIPTION
// C  The goal here is to make sure that acceleration a_in won't make
// C the aircraft in question get a negative speed during a projection
// C (probably using projw).  In order to do this, a minimum allowable
// C speed in the X-direction in maneuver plane, min_speed, is used to
// C compute the X acceleration in maneuver plane that will
// C produce it.  This number is used to limit the X-component of the
// C acceleration in maneuver plane, which is then transformed back to
// C ratrh coordinates.
// C
// C The maneuver plane is defined by a coordinate system with X-Z axes
// C in maneuver plane, i.e., with the X-axis aligned with initial velocity
// C and the Z-axis with acceleration.
// C#VARIABLE DESCRIPTIONS
// C !                  !
// C####
// returns a_use
// static
UtVec3d BrawlerPlatform::a_lim(const UtVec3d& x_in, const UtVec3d& v_in, const UtVec3d& a_in, double dt, double min_speed)
{
   UtVec3d a_use(0.0, 0.0, 0.0);
   // C  --local declarations:
   UtFortMat3d r_maneuv = UtFortMat3d();
   UtVec3d     vcrosa(0.0, 0.0, 0.0);
   UtVec3d     a_maneuv(0.0, 0.0, 0.0);
   double      speed_in = 0;
   // C*ENDDEC
   speed_in = v_in.Magnitude();
   a_use    = a_in;
   if (speed_in == 0.0 || dt == 0.0)
   {
      return a_use;
   }
   // C
   // call cross(v_in,a_use,vcrosa);
   vcrosa.CrossProduct(v_in, a_use);
   if (avnorm(vcrosa) > speed_in * 1.0e-4)
   {
      r_maneuv = makcsi(v_in, a_use);
   }
   else
   {
      // C        --here part of a normal to v_in is too small to effectively
      // C        --define the maneuver plane
      r_maneuv = makex(v_in);
   }
   // call vxfrmc(r_maneuv,a_use,a_maneuv,1);
   a_maneuv = r_maneuv * a_use;
   // C
   if (speed_in + a_maneuv[1 - 1] * 0.5 * dt >= min_speed)
   {
      // C        --no problem with too low speed across this interval
      a_use = a_in;
      return a_use;
   }
   if (speed_in < min_speed)
   {
      a_maneuv[1 - 1] = 0.0;
   }
   else
   {
      a_maneuv[1 - 1] = (min_speed - speed_in) / dt;
   }
   // call vxfrmc(r_maneuv,a_use,a_maneuv,2);
   a_use = r_maneuv.TransposeMultiply(a_maneuv);
   return a_use;
}


// C#ABSTRACT PROJECTS TRAJECTORY WITH CONSTANT LONG AND TRANSV ACCEL
// C#PURPOSE  PROJECTS TRAJECTORY ASSUMING CONSTANT LONGITUDINAL AND
// C  TRANSVERSE ACCELERATION.
// C#TYPE     POSITION PROJECTION
// C#PARAMETER DESCRIPTIONS
// CIN  XIN    3-VEC - INITIAL POSITION VECTOR
// CIN  VIN    3-VEC - INITIAL VELOCITY VECTOR
// CIN  A      3-VEC - INITIAL ACCELERATION VECTOR
// CIN  DT     REAL - TIME STEP
// COUT XOUT   3-VEC - FINAL POSITION VECTOR
// COUT VOUT   3-VEC - FINAL VELOCITY VECTOR
// COUT AOUT   3-VEC - FINAL ACCELERATION VECTOR
// COUT RWOUT  ORIENT - UPDATED WIND AXES
// C#TECHNICAL DESCRIPTION
// C     CIRCULAR MOTION IN MANEUVER PLANE WITH RADIUS AND ANGULAR
// C  VELOCITY CHARACTERIZED BY AVERAGE SPEED OVER TIME INTERVAL AND
// C  TRANSVERSE COMPONENT OF ACCELERATION.  MANEUVER PLANE DEFINED BY
// C  ACCELERATION A AND VIN.  AVERAGE SPEED COMPUTED FROM LONGITUDINAL
// C  COMPONENT OF ACCELERATION, DT, AND INITIAL SPEED.
// C     THE COMPUTATION IS MADE EASIER BY TRANSFORMING TO A FRAME IN
// C  WHICH THE X-AXIS IS ALIGNED WITH THE INITIAL VELOCITY AND THE
// C  Z-AXIS WITH THE INITIAL TRANSVERSE ACCELERATION.  THUS ALL MOTION
// C  IN THIS FRAME IS TWO-DIMENSIONAL, WITH NO Y-COMPONENT INVOLVEMENT.
// C  THE ORIENTATION RWIN DESCRIBES THIS FRAME.
// C#VARIABLE DESCRIPTIONS
// C  SPDAVG  REAL - AVERAGE SPEED DURING PROJECTION
// C  RADIUS  REAL - AVERAGE TURN RADIUS DURING PROJECTION
// C  THETA   REAL - ANGLE TURNED THRU DURING PROJECTION
// C####
// static
void BrawlerPlatform::projw(const UtVec3d& xin,
                            const UtVec3d& vin,
                            const UtVec3d& a,
                            double         dt,
                            UtVec3d&       xout,
                            UtVec3d&       vout,
                            UtVec3d&       aout,
                            UtFortMat3d&   rwout)
{
   // c  --local declarations:
   UtFortMat3d rwin = UtFortMat3d();
   UtVec3d     aw(0.0, 0.0, 0.0);
   UtVec3d     alift(0.0, 0.0, 0.0);
   UtVec3d     vcrosa(0.0, 0.0, 0.0);
   double      ct     = 0;
   double      radius = 0;
   double      spdavg = 0;
   double      spdin  = 0;
   double      spdout = 0;
   double      st     = 0;
   double      theta  = 0;
   // C*ENDDEC
   if (avnorm(a) <= 0.02)
   {
      // C        --for zero acceleration
      xout = xin + dt * vin;
      vout = vin;
      aout = a;
      if (avnorm(vout) > 0.02)
      {
         rwout = makex(vout);
      }
      else
      {
         // C           --Object has no velocity and no acceleration
         // C           --Definition of wind axis is meaningless
         // call xmit(9,unit3,rwout);
         rwout    = UtFortMat3d();
         rwout[0] = rwout[4] = rwout[8] = 1.0; // identity matrix
      }
      return;
   }
   spdin = vin.Magnitude();
   // C     --CONSTRUCT COORDINATE SYSTEM WITH X-Z AXES IN MANEUVER PLANE
   // C     --X-AXIS ALIGNED WITH INITIAL VELOCITY, Z-AXIS WITH ACCELERATION.
   // C     --CALLS MAKCSI UNLESS VIN AND A ARE PARALLEL, IN WHICH CASE MAKEX
   // C     --IS CALLED.
   // call cross(vin,a,vcrosa);
   vcrosa.CrossProduct(vin, a);
   if (avnorm(vcrosa) > spdin * 1.0e-4)
   {
      rwin = makcsi(vin, a);
   }
   else
   {
      // C        --here part of a normal to vin is too small to effectively
      // C        --define the maneuver plane
      rwin = makex(vin);
   }
   // C     --IN THIS COORDINATE SYSTEM AW(1) IS LONGITUDINAL ACCEL AND
   // C     --AW(3) IS TRANSVERSE ACCEL
   // call vxfrmc(rwin,a,aw,1);
   aw = rwin * a;
   // C     --APPROXIMATE PROJECTION BY CIRCLE WITH RADIUS CHARACTERISTIC OF
   // C     --AVERAGE VELOCITY
   spdavg = spdin + aw[1 - 1] * 0.5 * dt;
   // assert(spdavg > 0.0);   //else nabort("1000");
   if (spdavg <= 0.0)
   {
      nabort("1000");
   }

   if (std::abs(aw[3 - 1]) * dt < spdavg * 1.e-4)
   {
      // C        --too little transverse acceleration to bother with:
      // call vecinc(xin,dt*(spdavg/spdin),vin,xout);
      xout = xin + (dt * (spdavg / spdin)) * vin;
      // call vmult(1.0+aw(1)*dt/spdin,vin,vout);
      vout = (1.0 + aw[1 - 1] * dt / spdin) * vin;
      if (vout.Magnitude() != 0.0)
      {
         rwout = makex(vout);
      }
      else
      {
         // call xmit(9,unit3,rwout);
         rwout    = UtFortMat3d();
         rwout[0] = rwout[4] = rwout[8] = 1.0; // identity matrix
      }
      // call xmit(3,a,aout);
      aout = a;
      return;
   }
   radius = spdavg * spdavg / aw[3 - 1];
   theta  = aw[3 - 1] * dt / spdavg;
   ct     = cos(theta);
   st     = sin(theta);
   // C     --GET DX IN MANUEVER AXES, CONVERT, AND ADD TO XIN
   xout[1 - 1] = radius * st;
   xout[2 - 1] = 0.0;
   xout[3 - 1] = radius * (1.0 - ct);
   // call vxfrmc(rwin,xout,xout,2);
   xout = rwin.TransposeMultiply(xout);
   // call vsum(xin,xout,xout);
   xout += xin;
   // C     --GET VELOCITY IN MANUEVER AXES AND CONVERT
   // call vxfrmc(rwin,vin,vout,1);
   vout = rwin * vin;
   // C     --ABOVE STATEMENT GETS Y-COMPONENT, AN INVARIANT
   spdout      = spdin + aw[1 - 1] * dt;
   vout[1 - 1] = spdout * ct;
   vout[3 - 1] = spdout * st;
   // call vxfrmc(rwin,vout,vout,2);
   vout = rwin.TransposeMultiply(vout);
   // C     --GET GET FINAL ACCEL IN MANEUVER AXES, AND ROTATE.
   aout[1 - 1] = aw[1 - 1] * ct - aw[3 - 1] * st;
   aout[2 - 1] = 0.0;
   aout[3 - 1] = aw[1 - 1] * st + aw[3 - 1] * ct;
   // call vxfrmc(rwin,aout,aout,2);
   aout = rwin.TransposeMultiply(aout);
   // C     --REMOVE GRAVITY TO GET ACCEL DUE TO LIFT (AND THRUST, DRAG)
   // C     --USE THIS TO CONSTRUCT FINAL WIND AXES
   // call vsub(aout,gravec,alift);
   UtVec3d gravec(0, 0, grav);
   alift = aout - gravec;
   rwout = makcsi(vout, alift);
   return;
}


void BrawlerPlatform::SetTargetRoutepointLLA(int aLat, int aLon, int aAlt_m)
{
   rtepp[0] = aLat;
   rtepp[1] = aLon;
   rtepp[2] = aAlt_m * UtMath::cFT_PER_M;
}

void BrawlerPlatform::SetTargetRoutepointLLA(UtVec3d aLLA)
{
   rtepp[0] = aLLA[0];
   rtepp[1] = aLLA[1];
   rtepp[2] = aLLA[2] * UtMath::cFT_PER_M;
}

// replicates a portion of gldmx()
// virtual
double BrawlerPlatform::MaxLimitedInstantaneousGLoad_g()
{
   // double tbl1;
   // int idir;
   // int kacid;
   // double dyp;
   // double fm;
   double gmax = 0;
   double wate = CurrentWeight_lbs();
   // double gliftm;
   double gcap_use = 0;


   // if(idir.eq.1) then //if positive G load (yes)
   if (wate > DesignWeightForMaxG_lbs())
   {
      gcap_use = StructuralGLimitAtDesignWeight_g(); // tbl1(fm,gcap2,mchval,nmach1)
   }
   else
   {
      gcap_use = CurrentStructuralGLimit_g(); // tbl1(fm,gcap,mchval,nmach1)
   }
   gmax = amin1(gcap_use, MaxInstantaneousGLoad_g());
   //   elseif(idir.eq.2) then
   //   if (wate .gt. miss_mass*grav) then
   //      gcap_use = tbl1(fm,gcap2(1,2),mchval,nmach1)
   //   else
   //   gcap_use = tbl1(fm,gcap(1,2),mchval,nmach1)
   //   endif
   //   gmax = amax1(gcap_use,-gliftm(fm,dyp,wate,kacid))
   // else
   // call nabort('GLDMX...illegal value of idir')
   // endif
   // gldmx = gmax
   return gmax;
}

void BrawlerPlatform::SetFlightAggressiveness(double aFactor)
{
   aggfac = aFactor;
}
void BrawlerPlatform::SetCombatEffectiveness(double aEffect)
{
   cmbeff = aEffect;
}
void BrawlerPlatform::SetForceRatio(double aRatio)
{
   fratio = aRatio;
}


// void WsfBrawlerPlatform::hutil()
//{
//    /*
//    //C#ABSTRACT CONSTRUCTS HIGH-UTILITY-OF-ENGAGEMENT LIST, LHUTIL
//    //C#PURPOSE  CALLED BY MAJUD AND MINUD TO UPDATE LHUTIL
//    //C#TYPE     DECISION
//    //C#TECHNICAL DESCRIPTION
//    //C  AIRCRAFT IAC IS TESTED AS A CANDIDATE FOR THE LUATK AND LUEVD LISTS.
//    //C  IT IS A HIGH UTILITY OF ENGAGEMENT IF IT IS ON EITHER LIST.
//    //C  PROVISIONS ARE MADE TO PREVENT RUNNING AWAY DURING A BVR ATTACK MODE.
//    //C  FRIENDLIES DO NOT GET ON THIS LIST.
//    //C  1V1 MODE IS DETERMINED BY THE LENGTH OF THE HUTIL LIST AND THE RANGE TO THE TARGET ON THE LIST.
//    //C#VARIABLE DESCRIPTIONS
//    //C  43 TGT1V1 INT [NA] - Tail number of aircraft that the attacker is in 1v1 against.
//    //C  IACIDT  ARRAY (MM-IDX) OF (AC-IND) [NA] - Actual aircraft ID.   Identical  to IACIDX.
//    //C  ME      INT - Index of self in mental model. (1)
//    //C  LHUTIL  ARRAY DIM(MACMND) OF (MM-IND) [NA] - List of aircraft on high utility list
//    //C  LHUTIL  ARRAY DIM(NHUTIL) OF INT - LIST OF GOOD TARGETS OR
//    //C          DANGEROUS PERSUERS
//    //C  NHUTIL  INT   - NUMBER IN THE LIST
//    //C  ONLIST  LOG - .TRUE. IF TARGET QUALIFIES FOR LHUTIL
//    //C####
//    //#include "par.fi"
//    //#include "mind2.fi"
//    //#include "mind3.fi"
//    //#include "mind4.fi"
//    //#include "mindlc.fi"
//    //#include "opmode.fi"
//    int iac,iatak,ievade;
//    int ilevel,kalt,icall,lcall;
//    double cutoff,defcut,fcutfo,fcutfd,offcut,rng1v1,runawy;
//    bool onlist,was1op;
//    int IBG;
//    rng1v1=20000.0;
//    runawy=60000.0;
//    fcutfo = 0.5;
//    fcutfd = 0.75;
//    //CENDDEC
//    //C*ENDDEC
//    was1op = l1opp;
//    nuatk = 0;
//    nuevd = 0;
//    nhutil = 0;
//    if (nbg==0)
//    {
//       //goto 900
//    }
//    call vmax(uatk,nspotd,iatak,offscl);
//    call vmax(uevd,nspotd,ievade,defcut);
//    cutoff = 0.1f*amax1(defcut,offscl);
//    offcut = offscl*fcutfo;
//    defcut = defcut*fcutfd;
//    offcut = amax1(offcut,cutoff);
//    defcut = amax1(defcut,cutoff);
//
//    //do 100 ibg=1,nbg
//    for (int ibg=1; ibg <= nbg; ++ibg)
//    {
//       iac = listh(ibg);
//       onlist = false;
//       if(uatk(iac) >= offcut)
//       {
//          nuatk = nuatk+1;
//          onlist = true;
//          luatk(nuatk) = iac;
//       }
//
//       if(uevd(iac) >= defcut)
//       {
//          //C           --FIND THE CURRENT MISSION POSTURE KALT
//          call indupk(cactn(3,4),ilevel,kalt,icall,lcall);
//          if (rngnow(me,iac) <= runawy && kalt != 5)
//          {
//             nuevd = nuevd+1;
//             luevd(nuevd) = iac;
//             onlist = true;
//          }
//       }
//
//       if(!onlist)
//       {
//          continue; //goto 100;
//       }
//       nhutil = nhutil+1;
//       lhutil(nhutil) = iac;
//    }
////100      continue
//
//   //C
//   //C     --print and return
////900   continue
//   //C     --next block found 2/7/85 by rmk: not associated with a MODIFIED line
//   //C     --DON'T RELINQUISH 1V1 STATUS IF TARGET IS WITHIN RNG1V1.
//   if (tgt1v1 != 0 && mmindt(tgt1v1) != 0)
//   {
//      if(l1opp && rngnow(me,tgt1v1) < rng1v1)
//      {
//         lhutil(1) = mmindt(tgt1v1);
//         nhutil = 1;
//      }
//   }
//   //C     --L1OPP IS TRUE IF A GENUINE 1V1 SITUATION EXISTS
//   l1opp = nhutil == 1 && rngnow(me,lhutil(1)) < rng1v1;
//   l1opp = l1opp && pur1v1;
//   if (l1opp && !was1op)
//   {
//      tgt1v1= iacidt(lhutil(1));
//   }
//   if (!l1opp && was1op)
//   {
//      tgt1v1 = 0;
//   }
//   return;
//   */
//}
