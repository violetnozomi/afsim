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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "BrawlerMover.hpp"
//#include "Brawler.hpp"
#include "BrawlerCoordinateConversion.hpp"
#include "BrawlerUtil.hpp"
#include "UtInput.hpp"
//#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

// Bring in BRAWLER constants, enumerations, and util functions
using namespace BRAWLER;

BrawlerMover::BrawlerMover()
   : mAngularRates(0, 0, 0)
   , mAxialAccel(0, 0, 0)
   , mPlane(0, 0, 0)
   , mVelocityVector(0, 0, 0)
   , mHeading(0)
   , mAltitude(0)
   , mSpeed(0)
   , mThrottle(0)
   , mMaxGees(0)
   , mMaxClimb(0)
   , mCommandType(RATES)
   , mThrottleFlag(false)
   , gravec(0, 0, grav)
   , tsv(0.0)
   , pos()
   , vel()
   , acc()
   , rbea()
   , rwea()
   , wnow()
   , alfa(0.0)
   , alfdot(0.0) // initac.f
   , amass(0.0)
   , spd(0.0)
   , turn(0.0) // initac.f
   , wsqr(0.0)
   , rate(0.0)
   , fm(0.0) // call getqm()
   , bvrmch(0.0)
   , dyp(0.0)            // call getqm()
   , omegp(0.5)          // initac.f
   , omegq(0.5)          // initac.f
   , pmx(1.0)            // initac.f
   , qmxpos(0.2)         // initac.f
   , rlthrs(180.0 * rad) // statni.f
                         //, rolmx(10.0*rad)    //statni.f  //LBM - this gets overwritten
   , rolmx(xlarge)       // prdexe : 210 (rolmx = maxrol(kacid) == rlimit = rolmax;  only changed by production rules)
   , rarea(0.0)
   , gas(13448.2 / grav) //(6100 kg in slugs)
   , tset(2.0)
   , ttset(0.0)
   , fggsv(1.0)
   , fabsv(0.0)
   , trpdsv(0.0)
   , ddpdsv(0.0)     // initac.f
   , mxexrl(xlarge)  // statni.f
   , mnonrl(0.0)     // statni.f
   , tmexrl(-xlarge) // statni.f
   , tmonrl(xlarge)  // statni.f
   , tmacex(-xlarge) // statni.f
   , ld2(true)       // statni.f
   , over(false)     // statni.f
                     // extst
   , svtime(0.0)
   , barems(0.0)
   , crnrv(0.0)
   // prform
   , fmmax(0.0)
   , casmax(0.0)
   , trefgg(0.0)
   , trmla(0.0)
   , abdt(0.0)
   , dtggup(0.0)
   , dtggdn(0.0)
   , tdlim(false)
   , tmlim(false)
   , tablim(false)
   , trmm(0.0)
   , trtd(0.0)
   , alpha0(0.0)
   , aoa_tab_type(reflective)
   , nfm1(0)
   , nfm2(0)
   , nfm3(0)
   //, fmvec1[     mfm];
   //, fmvec2[     mfm];
   //, fmvec3[     mfm];
   //, clmax[      mfm];
   , nalph1(0)
   , nalph2(0)
   //, alfv1[     malpha];  //AOA values for clift[]
   //, alfv2[     malpha];  //AOA values for cdrag[]
   //, clift[ mfm*malpha];  //coefficient lift(mach,aoa)
   //, cdrag[ mfm*malpha];  //coefficient drag(mach,aoa)
   , nmach1(0)
   , nhts(0)
   //, mchval[mmach1];      //mach
   //, htvec [       mhts]; //alt
   //, tidle [mmach1*mhts]; //thrust idle(mach,alt)
   //, tmil  [mmach1*mhts]; //thrust mil (mach,alt)
   //, thrmx [mmach1*mhts]; //thrust max (mach,alt)
   , abcorl(0.0)
   //, gcap [2][mmach1];
   //, gcap2[2][mmach1];
   //, rfarea[msweep];
   //, floidl[mmach1*mhts];
   //, flomil[mmach1*mhts];
   //, flomax[mmach1*mhts];
   , ff_adj(1.0)
   //, dragvl[ndatbl];
   //, aoavl[ndatbl];
   //, drgaoa[ndtbl2];
   , aw()
   , gwind()
   , almax(0.0)
   , almin(0.0)
   , gmxin(0.0)
   , thrmin(0.0)
   , thrnow(0.0)
   , thrmax(0.0)
   , drgnow(0.0)
   , gmxsu(0.0)
   , almaxg(0.0)
   , alming(0.0)
   , amxin(0.0)
   , drgsu(0.0)
   , wmax(0.0)
   , wate(0.0)
   , gmnin(0.0)
   , gmxsut(0.0)
   , psubs(0.0)
   , cornrv(0.0)
   , tempme(0.0)
   , presme(0.0)
   , densme(0.0)
   , vsme(0.0)
   , prme(0.0)
   , drme(0.0)
   , radmnp(0.0)
   , aoamx(0.0)
   , aoamn(0.0)
   , epdmin(0.0)
   , epdnow(0.0)
   , epdmax(0.0)
   //, thrstl[5];
   // bugger
   , throtm(3.0)      // reg_w_tb.f
   , altmin(0.0)      // reg_w_tb.f
   , altmax(500000.0) // reg_w_tb.f
   , casmin(300.0)    // reg_w_tb.f
                      // acphys
   , miss_mass(0.0)
   //, parea[3];
   , deswt(0.0)
   , ac_fuel_cap(0.0)
   // typdat
   , arotyp(aero1)
   , thrst_adj(1.0)  // rdtype.f : 104
                     // acstat
   , num_ac_acd_m(0) // call get_cd_machs()
                     //, ac_acd_mach[2*max_acd_mach];
                     //, ac_acd[2*max_acd_mach];
   , throtl(0.0)
   , aeroLoaded(false)
   , mBrawlerPlatformPtr(nullptr)
   , erate(0.0)
{
   memset(fmvec1, 0, sizeof(fmvec1));
   memset(fmvec2, 0, sizeof(fmvec2));
   memset(fmvec3, 0, sizeof(fmvec3));
   memset(clmax, 0, sizeof(clmax));
   memset(alfv1, 0, sizeof(alfv1));
   memset(alfv2, 0, sizeof(alfv2));
   memset(clift, 0, sizeof(clift));
   memset(cdrag, 0, sizeof(cdrag));
   memset(mchval, 0, sizeof(mchval));
   memset(htvec, 0, sizeof(htvec));
   memset(tidle, 0, sizeof(tidle));
   memset(tmil, 0, sizeof(tmil));
   memset(thrmx, 0, sizeof(thrmx));
   memset(gcap, 0, sizeof(gcap));
   memset(gcap2, 0, sizeof(gcap2));
   memset(rfarea, 0, sizeof(rfarea));
   memset(floidl, 0, sizeof(floidl));
   memset(flomil, 0, sizeof(flomil));
   memset(flomax, 0, sizeof(flomax));

   for (int i = 0; i < ndatbl; ++i)
   {
      dragvl[i] = aoavl[i] = 0.0;
   }
   for (int i = 0; i < ndtbl2; ++i)
   {
      drgaoa[i] = 0.0;
   }
   for (int i = 0; i < 5; ++i)
   {
      thrstl[i] = 0.0;
   }

   // hedreq[0] = 1.0;
   // hedreq[1] = 0.0;
   parea[0] = parea[1] = parea[2] = 0.0;

   // LBM - no additive drag from stores right now (assume expendables that don't drag or something)
   // TODO - replicate get_cd_machs() and gcdtab_calc() later
   num_ac_acd_m   = 2;   // setext.f : 241
   ac_acd_mach[0] = 0.0; // setext.f : 242
   ac_acd_mach[1] = 3.0; // setext.f : 243
   ac_acd[0]      = 0.0; // gcdtab.f : 167
   ac_acd[1]      = 0.0; // gcdtab.f : 167
}

BrawlerMover::BrawlerMover(const BrawlerMover& aSrc)
   : mAngularRates(aSrc.mAngularRates)
   , mAxialAccel(aSrc.mAxialAccel)
   , mPlane(aSrc.mPlane)
   , mVelocityVector(aSrc.mVelocityVector)
   , mHeading(aSrc.mHeading)
   , mAltitude(aSrc.mAltitude)
   , mSpeed(aSrc.mSpeed)
   , mThrottle(aSrc.mThrottle)
   , mMaxGees(aSrc.mMaxGees)
   , mMaxClimb(aSrc.mMaxClimb)
   , mCommandType(aSrc.mCommandType)
   , mThrottleFlag(aSrc.mThrottleFlag)
   , gravec(aSrc.gravec)
   , tsv(aSrc.tsv)
   , pos(aSrc.pos)
   , vel(aSrc.vel)
   , acc(aSrc.acc)
   , rbea(aSrc.rbea)
   , rwea(aSrc.rwea)
   , wnow(aSrc.wnow)
   , alfa(aSrc.alfa)
   , alfdot(aSrc.alfdot)
   , amass(aSrc.amass)
   , spd(aSrc.spd)
   , turn(aSrc.turn)
   , wsqr(aSrc.wsqr)
   , rate(aSrc.rate)
   , fm(aSrc.fm)
   , bvrmch(aSrc.bvrmch)
   , dyp(aSrc.dyp)
   , omegp(aSrc.omegp)
   , omegq(aSrc.omegq)
   , pmx(aSrc.pmx)
   , qmxpos(aSrc.qmxpos)
   , rlthrs(aSrc.rlthrs)
   , rolmx(aSrc.rolmx)
   , rarea(aSrc.rarea)
   , gas(aSrc.gas)
   , tset(aSrc.tset)
   , ttset(aSrc.ttset)
   , fggsv(aSrc.fggsv)
   , fabsv(aSrc.fabsv)
   , trpdsv(aSrc.trpdsv)
   , ddpdsv(aSrc.ddpdsv)
   , mxexrl(aSrc.mxexrl)
   , mnonrl(aSrc.mnonrl)
   , tmexrl(aSrc.tmexrl)
   , tmonrl(aSrc.tmonrl)
   , tmacex(aSrc.tmacex)
   , ld2(aSrc.ld2)
   , over(aSrc.over)
   , svtime(aSrc.svtime)
   , barems(aSrc.barems)
   , crnrv(aSrc.crnrv)
   , fmmax(aSrc.fmmax)
   , casmax(aSrc.casmax)
   , trefgg(aSrc.trefgg)
   , trmla(aSrc.trmla)
   , abdt(aSrc.abdt)
   , dtggup(aSrc.dtggup)
   , dtggdn(aSrc.dtggdn)
   , tdlim(aSrc.tdlim)
   , tmlim(aSrc.tmlim)
   , tablim(aSrc.tablim)
   , trmm(aSrc.trmm)
   , trtd(aSrc.trtd)
   , alpha0(aSrc.alpha0)
   , aoa_tab_type(aSrc.aoa_tab_type)
   , nfm1(aSrc.nfm1)
   , nfm2(aSrc.nfm2)
   , nfm3(aSrc.nfm3)
   //, fmvec1[     mfm];
   //, fmvec2[     mfm];
   //, fmvec3[     mfm];
   //, clmax[      mfm];
   , nalph1(aSrc.nalph1)
   , nalph2(aSrc.nalph2)
   //, alfv1[     malpha];  //AOA values for clift[]
   //, alfv2[     malpha];  //AOA values for cdrag[]
   //, clift[ mfm*malpha];  //coefficient lift(mach,aoa)
   //, cdrag[ mfm*malpha];  //coefficient drag(mach,aoa)
   , nmach1(aSrc.nmach1)
   , nhts(aSrc.nhts)
   //, mchval[mmach1];      //mach
   //, htvec [       mhts]; //alt
   //, tidle [mmach1*mhts]; //thrust idle(mach,alt)
   //, tmil  [mmach1*mhts]; //thrust mil (mach,alt)
   //, thrmx [mmach1*mhts]; //thrust max (mach,alt)
   , abcorl(aSrc.abcorl)
   //, gcap [2][mmach1];
   //, gcap2[2][mmach1];
   //, rfarea[msweep];
   //, floidl[mmach1*mhts];
   //, flomil[mmach1*mhts];
   //, flomax[mmach1*mhts];
   , ff_adj(aSrc.ff_adj)
   //, dragvl[ndatbl];
   //, aoavl[ndatbl];
   //, drgaoa[ndtbl2];
   , aw(aSrc.aw)
   , gwind(aSrc.gwind)
   , almax(aSrc.almax)
   , almin(aSrc.almin)
   , gmxin(aSrc.gmxin)
   , thrmin(aSrc.thrmin)
   , thrnow(aSrc.thrnow)
   , thrmax(aSrc.thrmax)
   , drgnow(aSrc.drgnow)
   , gmxsu(aSrc.gmxsu)
   , almaxg(aSrc.almaxg)
   , alming(aSrc.alming)
   , amxin(aSrc.amxin)
   , drgsu(aSrc.drgsu)
   , wmax(aSrc.wmax)
   , wate(aSrc.wate)
   , gmnin(aSrc.gmnin)
   , gmxsut(aSrc.gmxsut)
   , psubs(aSrc.psubs)
   , cornrv(aSrc.cornrv)
   , tempme(aSrc.tempme)
   , presme(aSrc.presme)
   , densme(aSrc.densme)
   , vsme(aSrc.vsme)
   , prme(aSrc.prme)
   , drme(aSrc.drme)
   , radmnp(aSrc.radmnp)
   , aoamx(aSrc.aoamx)
   , aoamn(aSrc.aoamn)
   , epdmin(aSrc.epdmin)
   , epdnow(aSrc.epdnow)
   , epdmax(aSrc.epdmax)
   //, thrstl[5];
   , throtm(aSrc.throtm)
   , altmin(aSrc.altmin)
   , altmax(aSrc.altmax)
   , casmin(aSrc.casmin)
   , miss_mass(aSrc.miss_mass)
   //, parea[3];
   , deswt(aSrc.deswt)
   , ac_fuel_cap(aSrc.ac_fuel_cap)
   , arotyp(aSrc.arotyp)
   , thrst_adj(aSrc.thrst_adj)
   , num_ac_acd_m(aSrc.num_ac_acd_m) // call get_cd_machs()
                                     //, ac_acd_mach[2*max_acd_mach];
                                     //, ac_acd[2*max_acd_mach];
   , throtl(aSrc.throtl)
   , aeroLoaded(aSrc.aeroLoaded)
   , mBrawlerPlatformPtr(nullptr)
   , erate(0.0)
{
   for (int i = 0; i < ndatbl; ++i)
   {
      dragvl[i] = aSrc.dragvl[i];
      aoavl[i]  = aSrc.aoavl[i];
   }
   for (int i = 0; i < ndtbl2; ++i)
   {
      drgaoa[i] = aSrc.drgaoa[i];
   }
   for (int i = 0; i < 5; ++i)
   {
      thrstl[i] = aSrc.thrstl[i];
   }

   // hedreq[0] = aSrc.hedreq[0];
   // hedreq[1] = aSrc.hedreq[1];
   parea[0] = aSrc.parea[0];
   parea[1] = aSrc.parea[1];
   parea[2] = aSrc.parea[2];

   for (int i = 0; i < nfm1; ++i)
   {
      fmvec1[i] = aSrc.fmvec1[i];
      clmax[i]  = aSrc.clmax[i];
   }
   for (int i = 0; i < nfm2; ++i)
   {
      fmvec2[i] = aSrc.fmvec2[i];
   }
   for (int i = 0; i < nfm3; ++i)
   {
      fmvec3[i] = aSrc.fmvec3[i];
   }
   for (int i = 0; i < nalph1; ++i)
   {
      alfv1[i] = aSrc.alfv1[i];
   }
   int len = nfm2 * nalph1;
   for (int i = 0; i < len; ++i)
   {
      clift[i] = aSrc.clift[i];
   }
   for (int i = 0; i < nalph2; ++i)
   {
      alfv2[i] = aSrc.alfv2[i];
   }
   len = nfm3 * nalph2;
   for (int i = 0; i < len; ++i)
   {
      cdrag[i] = aSrc.cdrag[i];
   }
   for (int i = 0; i < nmach1; ++i)
   {
      mchval[i] = aSrc.mchval[i];
   }
   for (int i = 0; i < nhts; ++i)
   {
      htvec[i] = aSrc.htvec[i];
   }
   len = nmach1 * nhts;
   for (int i = 0; i < len; ++i)
   {
      tidle[i]  = aSrc.tidle[i];
      tmil[i]   = aSrc.tmil[i];
      thrmx[i]  = aSrc.thrmx[i];
      floidl[i] = aSrc.floidl[i];
      flomil[i] = aSrc.flomil[i];
      flomax[i] = aSrc.flomax[i];
   }
   for (int j = 0; j < 2; ++j)
   {
      for (int i = 0; i < nmach1; ++i)
      {
         gcap[j][i]  = aSrc.gcap[j][i];
         gcap2[j][i] = aSrc.gcap2[j][i];
      }
   }
   for (int i = 0; i < msweep; ++i)
   {
      rfarea[i] = aSrc.rfarea[i];
   }
   for (int i = 0; i < num_ac_acd_m; ++i)
   {
      ac_acd_mach[i] = aSrc.ac_acd_mach[i];
      ac_acd[i]      = aSrc.ac_acd[i];
   }
}

// virtual
BrawlerMover::~BrawlerMover() {}

// virtual
bool BrawlerMover::Initialize(double aCreationTime, UtEntity& aStartingState, BrawlerPlatform* aPlatformPtr)
{
   mBrawlerPlatformPtr = aPlatformPtr;
   // if (mBrawlerPlatformPtr == 0)
   //{
   //    ut::log::warning() << "brawler platform or processor not found, choices for gees used are agnostic of targets,
   //    threats, or weapons!";
   // }
   if (!aeroLoaded)
   {
      ut::log::error() << "Brawler aero_file not loaded, initialization fail!";
      return false;
   }
   tsv = aCreationTime;
   pos = BrawlerCoordinateConversion::GetPositionNED(aStartingState);
   vel = BrawlerCoordinateConversion::GetVelocityNED(aStartingState);
   acc = BrawlerCoordinateConversion::GetAccelerationNED(aStartingState);
   // UtVec3d ori = BrawlerCoordinateConversion::GetOrientationNED(startingState);
   UtFortMat3d rbea = NEDToECSRotation(aStartingState);
   rwea             = rbea;
   alfa             = 0.0;
   spd              = vel.Magnitude();
   if (spd < 1)
   {
      spd = 829.3963; // mach 0.8 (at 20k ft alt) in units: feet/sec
      UtVec3d vEcs(spd, 0, 0);
      vel = BrawlerPlatform::ConvertECStoNED(aStartingState, vEcs);
      // startingState.SetVelocityNED(vel.GetData());  //todo convert to m/s
   }
   getqm(Speed(), dyp, fm);
   amass = barems + gas;

   // set initial command (maneuver)

   // mCommandType = HEADING_SPEED_ALTITUDE;
   // mHeading = ori[0];
   // mSpeed = spd;
   // mAltitude = -pos[2];
   // mMaxGees = 6.0;
   // mMaxClimb = spd*cos(30*rad);  //30 deg climb if necessary (shouldn't be any, match current altitude)

   mCommandType = VECTOR_GEES;
   UtVec3d vEcs(spd, 0, 0);
   mVelocityVector = BrawlerPlatform::ConvertECStoNED(aStartingState, vEcs);
   mMaxGees        = 6.0;
   mThrottleFlag   = false;
   mSpeed          = spd;
   bvrmch          = fm;

   return true;
}

// virtual
bool BrawlerMover::UpdateState(double toSimTime)
{
   perfrm();
   flyac(toSimTime);
   return true;
}

// virtual
UtEntity BrawlerMover::State(double aSimTime)
{
   UtEntity state;

   double dt  = aSimTime - tsv;
   double hdt = 0.5 * dt;

   UtFortMat3d rweaTemp = rwea;
   udrbe(rweaTemp, AngularRates(), wsqr, rate, rate * dt);
   double      pitchrate = AngularRates()[1];
   double      alpha     = alfa + pitchrate * dt;
   UtFortMat3d rbeaTemp  = bodyax(rweaTemp, alpha);

   double yaw   = 0.0;
   double pitch = 0.0;
   double roll  = 0.0;
   orient(rbeaTemp, yaw, pitch, roll);
   state.SetOrientationNED(yaw, pitch, roll);

   UtVec3d posTemp = pos;
   posTemp += dt * (vel + hdt * acc);
   UtVec3d lla = BrawlerCoordinateConversion::ConvertNEDtoLLA(posTemp);
   state.SetLocationLLA(lla[0], lla[1], lla[2]);

   UtVec3d vel2 = vel;
   vel2 += dt * acc;
   vel2 = vel2.Magnitude() * UtVec3d{rweaTemp[0], rweaTemp[3], rweaTemp[6]}; // { (1,1), (1,2), (1,3) }
   vel2 *= UtMath::cM_PER_FT;
   state.SetVelocityNED(vel2.GetData());

   UtVec3d acc2 = acc;
   acc2 *= UtMath::cM_PER_FT;
   state.SetAccelerationNED(acc2.GetData());

   return state;
}

// virtual
void BrawlerMover::UpdateFuel(double aTimeStep, double aInitialFlow, double aThrottle, double& aFuelQuantity, double& aVehicleMass)
{
   double flow = 0.0;
   if (aFuelQuantity > 0)
   {
      flow = fflo(aThrottle);
   }

   if (flow == undnum)
   {
      // C           --FUEL FLOW UNDEFINED HERE
      // write(tmpstr,6004) kacid,fm,-pos[2],tset
      // nabort(tmpstr(1:rspace(tmpstr)));
      // nabort("fuel flow undefined!");
      ut::log::warning() << "Fuels flow undefined.";
      DeleteMover();
      return;
   }

   double flouse = (aInitialFlow + flow) / 2.0;
   aFuelQuantity -= flouse * aTimeStep;
   aVehicleMass -= flouse * aTimeStep;
}


bool BrawlerMover::LoadAeroConfig(const std::string& absolutePathFile)
{
   UtInput aInput;
   aInput.PushInput(ut::make_unique<UtInputFile>(absolutePathFile));
   // aInput->SetFileOpenCallback(UtInput::FileOpenCallback(&WsfScenario::FileOpenCallback, this));
   // FileOpenCallback(aFileName);

   // LoadFromStreamP(*aInput);   //copied contents below:
   std::string command;
   bool        foundEndComments = false;
   while (BRAWLER::TryReadCommand(aInput, command))
   {
      if (foundEndComments == false)
      {
         if (command == "END" && BRAWLER::TryReadCommand(aInput, command))
         {
            if (command == "COMMENTS")
            {
               foundEndComments = true;
            }
         }
      }
      else if (command == "PERF")
      {
         while (BRAWLER::TryReadCommand(aInput, command))
         {
            if (command == "ENDBLOCK")
            {
               if (BRAWLER::TryReadCommand(aInput, command) && command == "PERF")
               {
                  break;
               }
            }
            else if (command == "AROTYP")
            {
               int atype;
               BRAWLER::ReadValue(aInput, atype);
               if (atype != 1)
               {
                  throw UtInput::BadValue(aInput, "Aero model type is unsupported, only use AROTYP 1.");
               }
            }
            else if (command == "UNSWEPT")
            {
               // good, this is only supported configuration
            }
            else if (command == "MIDSWEEP")
            {
               ut::log::warning() << "MIDSWEEP mode is unsupported, only use UNSWEPT.";
               // throw UtInput::BadValue(*aInput, " mode is unsupported, only use UNSWEPT");
            }
            else if (command == "FULLSWEEP")
            {
               ut::log::warning() << "FULLSWEEP mode is unsupported, only use UNSWEPT.";
               // throw UtInput::BadValue(*aInput, " mode is unsupported, only use UNSWEPT");
            }
            else if (command == "AOA_TABLE")
            {
               std::string tableType;
               BRAWLER::TryReadCommand(aInput, tableType);
               if (tableType == "REFLECTIVE")
               {
                  aoa_tab_type = reflective;
               }
               // else if (tableType == "FULL")
               //{
               //    aoa_tab_type = full;
               // }
               else
               {
                  throw UtInput::BadValue(aInput, "Table type is unsupported, only use AOA_TABLE REFLECTIVE");
               }
            }
            else if (command == "NFM1")
            {
               ReadValue(aInput, nfm1);
            }
            else if (command == "FMVEC1")
            {
               for (int i = 0; i < nfm1; ++i)
               {
                  BRAWLER::ReadValue(aInput, fmvec1[i]);
               }
            }
            else if (command == "CLMAX")
            {
               for (int i = 0; i < nfm1; ++i)
               {
                  BRAWLER::ReadValue(aInput, clmax[i]);
               }
            }
            else if (command == "ALPHA0")
            {
               BRAWLER::ReadValue(aInput, alpha0);
            }
            else if (command == "NFM2")
            {
               BRAWLER::ReadValue(aInput, nfm2);
            }
            else if (command == "FMVEC2")
            {
               for (int i = 0; i < nfm2; ++i)
               {
                  BRAWLER::ReadValue(aInput, fmvec2[i]);
               }
            }
            else if (command == "NALPH1")
            {
               BRAWLER::ReadValue(aInput, nalph1);
            }
            else if (command == "ALFV1")
            {
               for (int i = 0; i < nalph1; ++i)
               {
                  BRAWLER::ReadValue(aInput, alfv1[i]);
               }
            }
            else if (command == "CLIFT")
            {
               int len = nfm2 * nalph1;
               for (int i = 0; i < len; ++i)
               {
                  BRAWLER::ReadValue(aInput, clift[i]);
               }
            }
            else if (command == "NFM3")
            {
               BRAWLER::ReadValue(aInput, nfm3);
            }
            else if (command == "FMVEC3")
            {
               for (int i = 0; i < nfm3; ++i)
               {
                  BRAWLER::ReadValue(aInput, fmvec3[i]);
               }
            }
            else if (command == "NALPH2")
            {
               BRAWLER::ReadValue(aInput, nalph2);
            }
            else if (command == "ALFV2")
            {
               for (int i = 0; i < nalph2; ++i)
               {
                  BRAWLER::ReadValue(aInput, alfv2[i]);
               }
            }
            else if (command == "CDRAG")
            {
               int len = nfm3 * nalph2;
               for (int i = 0; i < len; ++i)
               {
                  BRAWLER::ReadValue(aInput, cdrag[i]);
               }
            }
            else if (command == "RFAREA")
            {
               for (int i = 0; i < msweep; ++i)
               {
                  BRAWLER::ReadValue(aInput, rfarea[i]);
               }
               rarea = rfarea[0]; // always (& only) UNSWEPT position
            }
            else if (command == "SWEEP_MACHS")
            {
               // for (int i=0; i<(msweep-1); ++i)
               //{
               //    BRAWLER::ReadValue(aInput, sweep_machs[i]);
               // }
               ut::log::warning() << "Not using SWEEP_MACHS values. Only UNSWEPT configuration used.";
            }
            else if (command == "NMACH1")
            {
               BRAWLER::ReadValue(aInput, nmach1);
            }
            else if (command == "MCHVAL")
            {
               for (int i = 0; i < nmach1; ++i)
               {
                  BRAWLER::ReadValue(aInput, mchval[i]);
               }
            }
            else if (command == "NHTS")
            {
               BRAWLER::ReadValue(aInput, nhts);
            }
            else if (command == "HTVEC")
            {
               for (int i = 0; i < nhts; ++i)
               {
                  BRAWLER::ReadValue(aInput, htvec[i]);
               }
            }
            else if (command == "TIDLE")
            {
               int len = nmach1 * nhts;
               for (int i = 0; i < len; ++i)
               {
                  BRAWLER::ReadValue(aInput, tidle[i]);
               }
            }
            else if (command == "TMIL")
            {
               int len = nmach1 * nhts;
               for (int i = 0; i < len; ++i)
               {
                  BRAWLER::ReadValue(aInput, tmil[i]);
               }
            }
            else if (command == "THRMX")
            {
               int len = nmach1 * nhts;
               for (int i = 0; i < len; ++i)
               {
                  BRAWLER::ReadValue(aInput, thrmx[i]);
               }
            }
            else if (command == "FLOIDL")
            {
               int len = nmach1 * nhts;
               for (int i = 0; i < len; ++i)
               {
                  BRAWLER::ReadValue(aInput, floidl[i]);
               }
            }
            else if (command == "FLOMIL")
            {
               int len = nmach1 * nhts;
               for (int i = 0; i < len; ++i)
               {
                  BRAWLER::ReadValue(aInput, flomil[i]);
               }
            }
            else if (command == "FLOMAX")
            {
               int len = nmach1 * nhts;
               for (int i = 0; i < len; ++i)
               {
                  BRAWLER::ReadValue(aInput, flomax[i]);
               }
            }
            else if (command == "GCAP")
            {
               for (int j = 0; j < 2; ++j)
               {
                  for (int i = 0; i < nmach1; ++i)
                  {
                     BRAWLER::ReadValue(aInput, gcap[j][i]);
                  }
               }
            }
            else if (command == "GCAP2")
            {
               for (int j = 0; j < 2; ++j)
               {
                  for (int i = 0; i < nmach1; ++i)
                  {
                     BRAWLER::ReadValue(aInput, gcap2[j][i]);
                  }
               }
            }
            else if (command == "FMMAX")
            {
               BRAWLER::ReadValue(aInput, fmmax);
            }
            else if (command == "CASMAX")
            {
               BRAWLER::ReadValue(aInput, casmax);
            }
            else if (command == "ABDT")
            {
               BRAWLER::ReadValue(aInput, abdt);
            }
            else if (command == "DTGGUP")
            {
               BRAWLER::ReadValue(aInput, dtggup);
            }
            else if (command == "DTGGDN")
            {
               BRAWLER::ReadValue(aInput, dtggdn);
            }
            else if (command == "ABCORl")
            {
               BRAWLER::ReadValue(aInput, abcorl);
            }
            else if (command == "GTIDLE")
            {
               ut::log::warning() << "Not using GTIDLE table. No gross thrust effects.";
            }
            else if (command == "GTMIL")
            {
               ut::log::warning() << "Not using GTMIL table. No gross thrust effects.";
            }
            else if (command == "GTHRMX")
            {
               ut::log::warning() << "Not using GTHRMX table. No gross thrust effects.";
            }
            else if (command == "TREFGG")
            {
               BRAWLER::ReadValue(aInput, trefgg);
            }
            else if (command == "TRMLA")
            {
               BRAWLER::ReadValue(aInput, trmla);
            }
            else if (command == "TDLIM")
            {
               std::string temp;
               BRAWLER::TryReadCommand(aInput, temp);
               if (temp == "T")
               {
                  tdlim = true;
               }
               else
               {
                  tdlim = false;
               }
            }
            else if (command == "TMLIM")
            {
               std::string temp;
               BRAWLER::TryReadCommand(aInput, temp);
               if (temp == "T")
               {
                  tmlim = true;
               }
               else
               {
                  tmlim = false;
               }
            }
            else if (command == "TABLIM")
            {
               std::string temp;
               BRAWLER::TryReadCommand(aInput, temp);
               if (temp == "T")
               {
                  tablim = true;
               }
               else
               {
                  tablim = false;
               }
            }
            else if (command == "TRMM")
            {
               BRAWLER::ReadValue(aInput, trmm);
            }
            else if (command == "TRTD")
            {
               BRAWLER::ReadValue(aInput, trtd);
            }
            else
            {
               // what command is this?
               // cout << "WARNING, did not recognize the " << command << " command!" << endl;
            }
         }
      }
      else if (command == "PHYS")
      {
         while (BRAWLER::TryReadCommand(aInput, command))
         {
            if (command == "ENDBLOCK")
            {
               if (BRAWLER::TryReadCommand(aInput, command) && command == "PHYS")
               {
                  break;
               }
            }
            else if (command == "PAREA")
            {
               for (int i = 0; i < 3; ++i)
               {
                  BRAWLER::ReadValue(aInput, parea[i]);
               }
            }
            else if (command == "DESWT")
            {
               BRAWLER::ReadValue(aInput, deswt);
               barems = deswt;
            }
            else if (command == "MISS_MASS")
            {
               BRAWLER::ReadValue(aInput, miss_mass);
            }
            else if (command == "AC_FUEL_CAP")
            {
               BRAWLER::ReadValue(aInput, ac_fuel_cap);
               if (ac_fuel_cap < 1.e+10)
               {
                  gas = ac_fuel_cap / grav;
               }
            }
            // else
            //{
            //    //do not care about this command for now
            //    command = command; //self assignment
            // }
         }
      }
      else if (command == "RF")
      {
         while (BRAWLER::TryReadCommand(aInput, command))
         {
            if (command == "ENDBLOCK")
            {
               if (BRAWLER::TryReadCommand(aInput, command) && command == "RF")
               {
                  break;
               }
            }
            else
            {
               // do not care about this command for now
            }
         }
      }
      else if (command == "IR")
      {
         while (BRAWLER::TryReadCommand(aInput, command))
         {
            if (command == "ENDBLOCK")
            {
               if (BRAWLER::TryReadCommand(aInput, command) && command == "IR")
               {
                  break;
               }
            }
            else
            {
               // do not care about this command for now
            }
         }
      }
      else if (command == "END") // check for "END AIRCRAFT"
      {
         if (BRAWLER::TryReadCommand(aInput, command) && command == "AIRCRAFT")
         {
            while (BRAWLER::TryReadCommand(aInput, command)) // strip off rest of input file
            {
            }
            break;
         }
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   aeroLoaded = true;
   return true;
}


// virtual
double BrawlerMover::CoefficientOfLift(double mach, double alpha)
{
   return 0.0;
}


// virtual
double BrawlerMover::CoefficientOfDrag(double mach, double alpha)
{
   return tbl2e(mach, alpha, &cdrag[0], &fmvec3[0], nfm3, &alfv2[0], nalph2);
}


// virtual
UtVec3d BrawlerMover::VelocityWindAxes()
{
   UtVec3d wVel = rwea * vel;
   // vxfrmc(rwea,vel,wVel,1);
   return wVel;
}


// virtual
UtVec3d BrawlerMover::GravityWindAxes()
{
   UtVec3d wGrav = rwea * gravec;
   // vxfrmc(rwea,gravec,wGrav,1);
   return wGrav;
}


// virtual
UtVec3d BrawlerMover::AccelerationWindAxes()
{
   UtVec3d wAccel = rwea * acc;
   // vxfrmc(rwea,acc,wAccel,1);
   return wAccel;
}


void BrawlerMover::CommandRates(UtVec3d& aAngularRates, UtVec3d& aLongitudinalAccel)
{
   mCommandType  = RATES;
   mAngularRates = aAngularRates;
   mAxialAccel   = aLongitudinalAccel;
}

void BrawlerMover::CommandHeadingSpeedAltitude(double aHeading, double aSpeed, double aAltitude, double aMaxGees, double aMaxClimb)
{
   mCommandType  = HEADING_SPEED_ALTITUDE;
   mHeading      = aHeading;
   mSpeed        = aSpeed;
   mThrottleFlag = false;
   mAltitude     = aAltitude;
   mMaxGees      = aMaxGees;
   mMaxClimb     = aMaxClimb;
}

void BrawlerMover::CommandPlaneManeuver(const UtVec3d& aPlane, double aGees, double aSpeed, bool aThrottleFlag)
{
   mCommandType  = PLANE_GEES_SPEED;
   mPlane        = aPlane;
   mMaxGees      = aGees;
   mThrottleFlag = aThrottleFlag;
   if (mThrottleFlag)
   {
      mThrottle = aSpeed;
   }
   else
   {
      mSpeed = aSpeed;
   }
}

void BrawlerMover::CommandVector(UtVec3d& aVelocityVector, double aMaxGees, double aSpeed, bool aThrottleFlag)
{
   mCommandType    = VECTOR_GEES;
   mVelocityVector = aVelocityVector;
   mMaxGees        = aMaxGees;
   mThrottleFlag   = aThrottleFlag;
   if (mThrottleFlag)
   {
      mThrottle = aSpeed;
   }
   else
   {
      mSpeed = aSpeed;
   }
}

void BrawlerMover::CommandLevelTurn(double aHeading, double aMach, double aGees)
{
   double geedes;
   if (aGees == 0)
   {
      geedes = gmxsut;
   }
   else
   {
      geedes = aGees;
   }
   double longturn = 30;
   prlevl(geedes, longturn, 0, aHeading, aMach);
}

void BrawlerMover::prlevl(double aGees, double aTurn, double aRoc, double aHeading, double aMach)
{
   double trate, hdgnow, rolds, dhead, geedes;
   geedes = aGees;
   hdgnow = headeg(Velocity());
   aHeading += 90; // puts heading of 0 north
   dhead = aHeading - hdgnow;
   if (dhead > 180)
   {
      dhead = dhead - 360;
   }
   else if (dhead < -180)
   {
      dhead = dhead + 360;
   }
   if (std::abs(dhead) < aTurn)
   {
      forcev(aMach, "MACH", aHeading, aRoc * 60);
   }
   else
   {
      rolds = 75 + 30 * ramp(-30, -aRoc - Velocity()[2], 30);
      rolds = sign(rolds, dhead);
      trate = 1843 * geedes / Speed();
      if (trate * 7.5 > std::abs(dhead))
      {
         geedes = Speed() * std::abs(dhead) / (1843 * 7.5);
      }
      dirct(aMach, "MACH", rolds, "EARTH", geedes);
   }
}

void BrawlerMover::CommandSlice(double aHeading, double aAltitude, double aRunMach)
{
   double      roll, pitch, yaw, dhead, myalt, roc, dspd, bank, rolds, frcmch;
   double      gdes, hdgnow, gnow, maxbank;
   bool        ninety;
   UtFortMat3d rbep = EarthToBodyRotationMatrix(); // earth to body
   aHeading += 90;                                 // puts heading of zero north
   myalt = Altitude();
   orient(rbep, yaw, pitch, roll);
   hdgnow        = headeg(Velocity());
   UtVec3d aload = aw - gravec;
   gnow          = aload.Magnitude() / grav;
   dhead         = aHeading - hdgnow;
   if (dhead > 180)
   {
      dhead = dhead - 360;
   }
   else if (dhead < -180)
   {
      dhead = dhead + 360;
   }
   ninety = (dhead < 90);
   if (myalt < 5000 || std::abs(dhead) < 15)
   {
      bank = 0;
      gdes = gmxsut;
      if (fm < 0.8)
      {
         roc = -60 * (myalt - 5000) / 10;
         roc = xlimit(roc, 10000, -10000);
      }
      else
      {
         roc = -60 * (myalt - aAltitude) / 10;
         roc = xlimit(roc, 20000, -20000);
      }
      frcmch = std::min(fm * 1.1, bvrmch);
      if (aRunMach != 0)
      {
         frcmch = aRunMach;
      }
      forcev(frcmch, "MACH", aHeading, roc);
   }
   else
   {
      dspd    = 3.0;
      maxbank = 90 + 45 * ramp(-30 * rad, pitch, 0);
      if (!ninety)
      {
         bank = maxbank;
         gdes = gmxsut + 3 * ramp(0.85, fm, 1.2);
      }
      else
      {
         if (pitch < -20 * rad)
         {
            bank = amin1(110 * std::abs(dhead) / 90, maxbank);
         }
         else
         {
            bank = maxbank;
         }
         gdes = gmxsut + 2 * ramp(0.85, fm, maxbank);
      }
      gdes          = gnow + (gdes - gnow) * .5;
      double y_comp = std::sin(bank * rad) * gdes;
      double z_comp = std::cos(bank * rad) * gdes - 1;
      rolds         = (1 / rad) * atan2(y_comp, z_comp) * sign(1.0, dhead);
      dirct(dspd, "THROTTLE", rolds, "EARTH", gdes);
   }
}

void BrawlerMover::forcev(double spd, std::string spdmod, double headng, double rclimb)
{
   double  hlsped, angle, casnow;
   UtVec3d hlvect = UtVec3d();
   angle          = rad * headng - hafpi;
   hlsped         = 0;
   if (spdmod == "FPS")
   {
      hlsped = spd;
   }
   else if (spdmod == "MACH")
   {
      hlsped = Speed() * (spd / fm);
   }
   else if (spdmod == "KTS")
   {
      hlsped = spd / (UtMath::cFT_PER_NM * 3600);
   }
   else if (spdmod == "CAS")
   {
      casnow = cas(Speed()) * (UtMath::cFT_PER_NM * 3600);
      hlsped = Speed() * (spd / casnow);
   }
   hlvect[0] = std::cos(angle) * hlsped;
   hlvect[1] = sin(angle) * hlsped;
   hlvect[2] = -rclimb / 60;
   hlvect.Normalize();
   CommandVector(hlvect, gmxsut, hlsped, false);
}

void BrawlerMover::dirct(double spddes, std::string spdmd, double rolds, std::string rolmd, double geedes)
{
   double      threq, epd, spd0, dspdx, vs;
   double      fm, aoades, caoa, saoa, dr, xlft;
   double      roldes, gdesn, adesn, axreq, dsptau;
   double      acmasp = amass;
   std::string errtxt, rolmod, gdesu;
   UtFortMat3d rwind  = UtFortMat3d();
   UtFortMat3d rmanuv = UtFortMat3d();
   UtVec3d     gmanuv = UtVec3d();
   UtVec3d     amanuv = UtVec3d();
   dsptau             = 5; // This is what Brawler sets this value to
   dspdx              = 0;

   getqm(Velocity().Magnitude(), dyp, fm);
   vs = Velocity().Magnitude() / fm;

   gdesn = geedes;
   if (gdesn >= 0)
   {
      roldes = rolds;
   }
   else
   {
      gdesn  = -gdesn;
      roldes = rolds + 180;
   }
   adesn = gdesn * grav;

   if (rolmd == "EARTH")
   {
      rwind = makex(Velocity());
   }
   else if (rolmd == "DELTA")
   {
      UtVec3d aload = Acceleration() - gravec;
      rwind         = makcsi(Velocity(), aload);
   }

   rmanuv = mrotx(rwind, roldes * rad);
   gmanuv = rmanuv * gravec;

   if (std::abs(adesn) < std::abs(gmanuv[1]))
   {
      adesn = sign(gmanuv[1], adesn);
   }
   amanuv[1] = 0;
   amanuv[2] = -std::sqrt(adesn * adesn - gmanuv[1] * gmanuv[1]) + gmanuv[2];

   xlft   = adesn * acmasp;
   aoades = aoa(xlft, false);
   caoa   = std::cos(aoades);
   saoa   = std::sin(aoades);

   // NOT CURRENTLY CHECKING SPMODE EXCEPT THROTTLE, MAY NEED TO IN FUTURE
   dr   = 0;
   spd0 = 0;
   if (spdmd == "MACH")
   {
      dspdx = vs * spddes - Speed();
   }
   if (spdmd == "THROTTLE")
   {
      effth2(spddes, threq, epd);
      spd0   = xlimit(spddes, 3, -1);
      xlft   = xlft - threq * saoa;
      aoades = aoa(xlft, false);
      caoa   = std::cos(aoades);
      dr     = drag(xlft) + epd;
   }
   else
   {
      axreq = dspdx / dsptau;
      dr    = drag(xlft);
      grthr(axreq, dr, caoa, gmanuv[0], threq, epd);
      spd0 = throt(threq);
   }

   amanuv[0]  = (threq * caoa - dr - epd) / acmasp + gmanuv[0];
   UtVec3d a0 = rmanuv.TransposeMultiply(UtVec3d(0, 1, 0));
   if (amanuv[2] > 0)
   {
      a0 *= -1;
   }
   double gmx = std::abs(amanuv[2]);

   double yaw, pitch, roll;
   orient(rmanuv, yaw, pitch, roll);
   /*
   std::cout << "DIRCT... ORIENTATION OF MANUEVER PLANE:" << std::endl;
   std::cout << "YAW  : " << yaw << " deg" << std::endl;
   std::cout << "PITCH: " << pitch << " deg" << std::endl;
   std::cout << "ROLL : " << roll << " deg" << std::endl;
   std::cout << "DIRCT... AMANUV VECTOR:" << std::endl;
   std::cout << "X-COMP: " << amanuv[0] << std::endl <<
                "Y-COMP: " << amanuv[1] << std::endl << "Z-COMP: " << amanuv[2] << std::endl;
   std::cout << "DIRCT... A0 VECTOR:" << std::endl;
   std::cout << "X-COMP: " << a0[0] << std::endl <<
                "Y-COMP: " << a0[1] << std::endl << "Z-COMP: " << a0[2] << std::endl;
   */

   CommandPlaneManeuver(a0, gmx, spd0, true);
}
// C#ABSTRACT PROJECTS MANEUVER ALTERNATIVES
// C#PURPOSE  PROJECTS MANEUVER ALTERNATIVES
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS: none
// C#TECHNICAL DESCRIPTION
// C    Depends on the maneuver form.   All methods have in common the
// C  concept of the command not "taking" completely for a time tauar
// C  (0.75 seconds).  During this time the maneuver is an average of
// C  the old and new maneuvers.
// C
// C  JACTN = 1
// C    The desired rates case uses the requested angular rates
// C  (averaged with current angular rates for the initial period tauar)
// C  to give the projected orientation.  This also gives the direction
// C  of the projected velocity; its magnitude is computed using the
// C  original speed plus the specified longitudinal component of
// C  acceleration.  Finally, the displacement is estimated by using the
// C  average of the initial and final velocity vectors and the starting
// C  displacement.  The angular rates requested are limited to
// C  achievable values, considering load and gravity, and the
// C  longitudinal acceleration is also limited to achievable values
// C  using the allim subroutine.
// C
// C  JACTN = 2
// C    The desired altitude and heading case is determined by
// C  generating a desired velocity vector, projecting a short time
// C  ahead (at most 2 seconds), and then determining a new desired
// C  velocity vector.  This repeats until the projection time tproj3
// C  has elapsed.  The stepping is necessary to produce acceptable
// C  altitude-seeking behavior.  The vertical velocity component is
// C  generated on a "damped spring" basis, tied to the desired
// C  altitude, except that its magnitude is limited to clmbmx.  The
// C  horizontal velocity component is intended to achieve the desired
// C  heading.  As usual, accelerations are limited.  Certain
// C  peculiar-looking portions of the code are present when the heading
// C  discrepancy is large.  This is necessary because the algorithm
// C  used to compute acceleration from desired velocity has a tendency
// C  to make excessive use of the vertical when the heading discrepancy
// C  is close to 180 degrees.
// C
// C  JACTN = 3
// C    The desired acceleration case assumes the average of the old
// C  and desired accelerations for a period tauar, followed by the
// C  desired acceleration for the time tproj3-tauar.  The calls
// C  to the vsplit and laccel subroutines are used to limit the
// C  requested accelerations to achievable values.
// C
// C  JACTN = 4
// C    The desired velocity form is the most common maneuver command
// C  form.  Its treatment is very similar to that used for the
// C  route point maneuver form after the desired velocity vector is
// C  determined.  However, since in this case the desired vector does
// C  not vary over the projection interval it is not necessary to
// C  proceed in small time steps.
// C
// C    When the final position and velocity are determined, all
// C  maneuver forms execute the code at location 100, to compute values
// C  of various descriptive variables, including the projected body
// C  and wind axes, gees, and roll angle.  The do loop to location 120
// C  computes such things as range, and relative velocities, with
// C  respect to other aircraft.
// C#VARIABLE DESCRIPTIONS:
// C ATEMP_M   3-VEC - Acceleration in the maneuver frame
// C ATEMP_E   3-VEC - Acceleration in the earth frame
// C#ENTRY POINTS
// C  APROJ3   Normal entry called by pkactn
// C  APRJ3F   Final call from akshn3 uses this to inhibit dupalt
// C####
void BrawlerMover::aproj3(double timeDelta, UtEntity& proj)
{
   double tproj3 = timeDelta; // plat.ProjectedTimeDelta();

   // STATE
   UtVec3d     xp     = Position();     // plat.PositionNED();
   UtVec3d     vp     = Velocity();     // plat.VelocityNED();
   UtVec3d     ap     = Acceleration(); // plat.AccelerationNED();
   double      spdnow = vp.Magnitude();
   UtFortMat3d rbep   = EarthToBodyRotationMatrix(); // earth to body
   double      acmasp = amass;                       // current or projected aircraft mass?
   UtVec3d     wb     = AngularRates();              // current angular rates
   UtVec3d     aw     = AccelerationWindAxes();      // plat.ConvertECStoWind(plat.AccelerationECS());

   if ((mCommandType == PLANE_GEES_SPEED && !mThrottleFlag) || (mCommandType == HEADING_SPEED_ALTITUDE && mThrottleFlag))
   {
      nabort("aproj3, unsupported maneuver command: (type 3 + speed) or (type 2 + throttle)!");
   }

   // MANEUVER COMMANDS
   double  al0    = mAxialAccel[0];
   double  hgt0   = mAltitude;
   double  clmbmx = mMaxClimb;
   double  gmx    = mMaxGees;
   UtVec3d a0     = mAngularRates;
   UtVec3d dir0   = mVelocityVector;
   double  spd0   = (mThrottleFlag) ? (mThrottle) : (mSpeed);

   // OUTPUT:
   UtVec3d xeua(0.0, 0.0, 0.0);
   UtVec3d veua(0.0, 0.0, 0.0);

   // NOT USED
   // UtFortMat3d rwep;  //earth to wind - not used
   // UtVec3d abody; //acceleration in body coordinates  //not used
   // UtVec3d weua;  //wind in earth coordinates?  //not used
   // double gees = 0;  //projected gees - local variable //not used

   // LOCAL VARIABLES
   double      alfe  = 0;             // local variable
   UtFortMat3d rweua = UtFortMat3d(); // earth to wind - local variable
   UtFortMat3d rbeua = UtFortMat3d(); // earth to body - local variable
   erate             = 0;             // energy rate - class variable, reset
   // TODO_NOT_USED double spdua = 0; //projected speed - local variable
   // double hdng   = 0;//projected heading - local variable

   // c  --local declarations
   // LBM - no duplicate maneuver checks for now
   // bool dmactv = false; //true;
   // C*ENDDEC
   // bool dmanuv;
   // c  --external declarations
   // real dot,ramp,tauar,xlimit,xmag,tbl1b,param1, arccos, arcsin
   // c  --local declarations
   // int iac;
   // int jactn;
   int         ispd = 0;
   UtVec3d     xtemp(0.0, 0.0, 0.0);
   UtVec3d     vtemp(0.0, 0.0, 0.0);
   UtVec3d     atemp(0.0, 0.0, 0.0);
   UtVec3d     wavg(0.0, 0.0, 0.0);
   UtFortMat3d rmanuv = UtFortMat3d();
   UtFortMat3d rwtemp = UtFortMat3d();
   UtVec3d     dv(0.0, 0.0, 0.0);
   UtVec3d     vdes(0.0, 0.0, 0.0);
   UtVec3d     ades(0.0, 0.0, 0.0);
   UtVec3d     vavg(0.0, 0.0, 0.0);
   UtVec3d     atrans(0.0, 0.0, 0.0);
   UtVec3d     wdes(0.0, 0.0, 0.0);
   UtVec3d     aout(0.0, 0.0, 0.0);
   double      dt        = 0;
   double      dttrue    = 0;
   double      spd0x     = 0;
   double      tau_route = 0;
   double      aynet     = 0;
   double      atlsq     = 0;
   double      spdmin    = 0;
   double      almin_nom = -1.0e6;
   double      al        = 0;
   double      aldes     = 0;
   // double alt = 0;
   double amax = 0;
   // double aspdot;
   // double aspect;
   double at     = 0;
   double atload = 0;
   double grthr  = 0;
   double atmag  = 0;
   // double az;
   // double azdot;
   // double caspct;
   double cdhead = 0;
   double chi    = 0;
   double dspeed = 0;
   double dtmax  = 0;
   // double elv;
   // double elvdot;
   // double fpang = 0;
   // double psi;
   // double psidot;
   // double roll = 0;
   // double rp;
   double rzdot = 0;
   // double smach = 0;
   // double sp;
   double  spddes     = 0;
   double  spdfin     = 0;
   double  spdtmp     = 0;
   double  speedh     = 0;
   double  wavgsq     = 0;
   double  wavmag     = 0;
   double  vdesmg     = 0;
   double  althr      = 0;
   double  dragf      = 0;
   double  tgg        = 0;
   double  tab        = 0;
   double  id         = 0;
   double  frclon     = 0;
   double  spdwt      = 0;
   double  gx         = 0;
   double  almin_use  = 0;
   double  throt_des  = 0;
   double  atemp1_min = 0;
   UtVec3d gmanuv(0.0, 0.0, 0.0);
   UtVec3d atemp_e(0.0, 0.0, 0.0);
   UtVec3d atemp_m(0.0, 0.0, 0.0);
   UtVec3d atemp_avg(0.0, 0.0, 0.0);
   UtVec3d body_neg_z(0.0, 0.0, 0.0);
   UtVec3d orth_v(0.0, 0.0, 0.0);
   double  tauar = 0.75;
   // C*ENDDEC
   spdmin = casmin / sqrt(drme);
   // jactn = iactn;
   // if (jactn == 9)
   //{
   //    jactn=5;
   // }
   // LBM - no command type 5 for now
   // ckrngi(jactn,1,5,"jactn/iactn...aproj3_inr");
   // ckrngi(mCommandType,1,4,"jactn/iactn...aproj3_inr");  //see default switch case below:
   // goto (10,20,30,40,50),jactn //solved
   switch (mCommandType)
   {
   case RATES:
   {
      // C
      // C     --DESIRED RATES
      // C     --TRANSLATE INTO EQUIVALENT ACCELERATION
      // 10    continue
      // call xmit(3,a0,wdes);
      // wdes = a0;
      wdes        = mAngularRates;
      amax        = amxin;
      wdes[2 - 1] = xlimit(wdes[2 - 1], (amax - gwind[3 - 1]) / spdnow, (-amax - gwind[3 - 1]) / spdnow);
      // call veclin(0.5,wb(1,iacid),0.5,wdes,wavg);
      wavg   = 0.5 * wb + 0.5 * wdes;
      wavgsq = wavg.DotProduct(wavg); // dot(wavg,wavg);
      wavmag = sqrt(wavgsq);
      // rweua = rwep;  //xmit(9,rwep,rweua);
      rweua = EarthToWindRotationMatrix(); // TODO - verify?
      chi   = wavmag * tauar;
      udrbe(rweua, wavg, wavgsq, wavmag, chi); // TODO
      // C     --GET FINAL VELOCITY
      atload = wdes[2 - 1] * spdnow + gwind[3 - 1];
      alfe   = sign(tbl1b(std::abs(atload), aoavl, ndatbl, 0.0, amxin), atload);
      allim(al0, alfe, cos(alfe), gwind[1 - 1], acmasp, aldes, grthr);
      spdtmp = spdnow + 0.5 * tauar * (aw[1 - 1] + aldes);
      // call vxfrmc(rweua,vtemp,unitx,2);
      vtemp = rweua.TransposeMultiply(UtVec3d(1, 0, 0));
      // C     --VTEMP IS A VECTOR IN FINAL VELOCITY DIRECTION, EARTH COORDINATES
      // C     --GET DISPLACEMENT
      // call vmult(spdtmp,vtemp,vtemp);
      vtemp *= spdtmp;
      // call veclin(.5,vp(1,me),.5,vtemp,vavg);
      vavg = 0.5 * vp + 0.5 * vtemp;
      // call vecinc(xp(1,me),tauar,vavg,xtemp);
      xtemp = xp + tauar * vavg;
      // C     --REPEAT PROCESS FOR REMAINING PROJECTION TIME
      dt = tproj3 - tauar;
      // call vxfrmc(rweua,gravec,gmanuv,1);
      gmanuv = rweua * gravec;
      // call xmit(3,a0,wdes);
      wdes        = a0;
      wdes[2 - 1] = xlimit(wdes[2 - 1], (amax - gmanuv[3 - 1]) / spdtmp, (-amax - gmanuv[3 - 1]) / spdtmp);
      wavgsq      = wdes.DotProduct(wdes);
      wavmag      = sqrt(wavgsq);
      chi         = wavmag * dt;
      udrbe(rweua, wdes, wavgsq, wavmag, chi); // TODO
      atload = wdes[2 - 1] * spdtmp + gmanuv[3 - 1];
      alfe   = sign(tbl1b(std::abs(atload), aoavl, ndatbl, 0.0, amxin), atload);
      allim(al0, alfe, cos(alfe), gmanuv[1 - 1], acmasp, aldes, grthr);
      erate  = (aldes - gmanuv[1 - 1]) * spdtmp / grav;
      spdfin = spdtmp + dt * aldes;
      // call vxfrmc(rweua,veua,unitx,2);
      veua = rweua.TransposeMultiply(UtVec3d(1, 0, 0));
      // call vmult(spdfin,veua,veua);
      veua *= spdfin;
      // call veclin(.5,vtemp,.5,veua,vavg);
      vavg = 0.5 * vtemp + 0.5 * veua;
      // call vecinc(xtemp,dt,vavg,xeua);
      xeua         = xtemp + dt * vavg;
      atemp[1 - 1] = aldes;
      atemp[2 - 1] = wdes[3 - 1] * spdfin;
      atemp[3 - 1] = -wdes[2 - 1] * spdfin;
      // call vxfrmc(rweua,aout,atemp,2);
      aout = rweua.TransposeMultiply(atemp);
      // goto 100  //solved
   }
   break;

   case HEADING_SPEED_ALTITUDE:
   {
      // C
      // C     --DESIRED ALTITUDE, HDNG
      // 20    continue
      tau_route   = 10.0;
      vdes[3 - 1] = xlimit((-hgt0 - xp[3 - 1]) / tau_route, clmbmx, -clmbmx);
      speedh      = sqrt(pow(spd0, 2) - pow(vdes[3 - 1], 2));
      vdes[1 - 1] = speedh * dir0[1 - 1];
      vdes[2 - 1] = speedh * dir0[2 - 1];
      dv          = vorth(vdes, vp);
      cdhead      = (dir0[1 - 1] * vp[1 - 1] + dir0[2 - 1] * vp[2 - 1]) / sqrt(pow(vp[1 - 1], 2) + pow(vp[2 - 1], 2));
      if (cdhead < -0.75)
      {
         rzdot     = vdes[3 - 1];
         dv[3 - 1] = 0.0;
         // vmake(speedh,dv,vdes);
         vdes        = dv.GetNormal() * speedh;
         vdes[3 - 1] = rzdot;
         dv          = vorth(vdes, vp);
      }
      // C     --LONGITUDINAL ACCELERATION:
      spddes = vdes.Magnitude(); // xmag(vdes);
      dspeed = spddes - spdnow;
      aldes  = dspeed / tau_route;
      // C     --TRANSVERSE ACCELERATION - START WITH TRANSVERSE VEL ERROR
      cdhead = vdes.DotProduct(vp) / (spddes * spdnow);
      dv     = vorth(dv, vp);
      if (cdhead >= 0.0)
      {
         // call vmult(1.0/tau_route,dv,atrans);
         atrans = (1.0 / tau_route) * dv;
      }
      else
      {
         // call vmake((gmx+1.0)*grav,dv,atrans);
         atrans = dv.GetNormal() * ((gmx + 1.0) * grav);
      }
      laccel(aldes, almin_nom, atrans, vp, spdnow, rbep, acmasp, gmx * grav, atload, erate);
      // call vecinc(atrans,aldes/spdnow,vp(1,me),atemp);
      atemp = atrans + (aldes / spdnow) * vp;

      // call veclin(.5,ap(1,me),.5,atemp,atemp);
      atemp = 0.5 * ap + 0.5 * atemp;
      atemp = BrawlerPlatform::a_lim(xp, vp, atemp, tauar, spdmin);
      BrawlerPlatform::projw(xp, vp, atemp, tauar, xtemp, vtemp, aout, rwtemp);
      // C     --REPEAT FOR REMAINING PORTION OF TURN:
      dttrue = tproj3 - tauar;
      // C     --LOOP AT 2-SECOND MAX INTERVALS TO PREVENT WEIRD CLIMB RATES

      // 25    continue
      do
      {
         dt          = amin1(dttrue, 2.0);
         spdtmp      = vtemp.Magnitude();
         vdes[3 - 1] = xlimit((-hgt0 - xtemp[3 - 1]) / tau_route, clmbmx, -clmbmx);
         vdesmg      = vdes.Magnitude();
         dspeed      = vdesmg - spdtmp;
         aldes       = dspeed / tau_route;
         dv          = vorth(vdes, vtemp);
         cdhead      = vdes.DotProduct(vtemp) / (vdesmg * spdtmp);

         if (cdhead > 0.0)
         {
            // call vmult(1.0/tau_route,dv,atrans);
            atrans = (1.0 / tau_route) * dv;
         }
         else
         {
            // vmake((gmx+1.0)*grav,dv,atrans);
            atrans = dv.GetNormal() * ((gmx + 1.0) * grav);
         }
         almin_use = (spdmin - spdtmp) / dt;
         laccel(aldes, almin_use, atrans, vtemp, spdtmp, rbep, acmasp, gmx * grav, atload, erate);
         // call vecinc(atrans,aldes/spdtmp,vtemp,atemp);
         atemp = atrans + (aldes / spdtmp) * vtemp;
         atemp = BrawlerPlatform::a_lim(xtemp, vtemp, atemp, dt, spdmin);
         BrawlerPlatform::projw(xtemp, vtemp, atemp, dt, xeua, veua, aout, rweua);
         dttrue = dttrue - dt;
         if (dttrue >= 0.01)
         {
            // call xmit(3,xeua,xtemp);
            xtemp = xeua;
            // call xmit(3,veua,vtemp);
            vtemp = veua;
            // goto 25   //solved
         }
      } while (dttrue >= 0.01);

      // goto 100  //solved
   }
   break;

   case PLANE_GEES_SPEED:
   {
      // C
      // C     --DESIRED VECTOR ACCELERATION FORMAT
      // C     --A0   = normal to mnvr plane
      // C     --SPD0 = desired throttle, ft/sec
      // C     --GMX  = desired transverse acceleration (in ft/sec**2!)
      // 30    continue
      // C     --FIRST STEP IS AVERAGE ACCELERATION FOR DELAY TIME TAUAR
      rmanuv = makecs(vp, a0);
      // call vxfrmc(rmanuv,gravec,gmanuv,1);
      gmanuv    = rmanuv * gravec;
      throt_des = amin1(throtm, spd0);
      thrust(throt_des, tgg, tab, id);
      // C     --ACCELERATION DUE TO THRUST
      althr = (tgg + tab - id) / acmasp;
      // c     --compute transverse acceleration:
      // c     --NOMINAL VALUE WITHOUT LIMITING IS gmx:
      aynet = gmx;
      // c     --COMPUTE LOAD, atlsq, LIMITED IF NECESSARY
      // c     --Idea is to keep net accel in rmanuv +y direction.
      atlsq = pow(gmanuv[3 - 1], 2) + pow((aynet - gmanuv[2 - 1]), 2);
      if (atlsq > pow(amxin, 2))
      {
         // c        --adjust transverse accel aynet to keep load under gmxin
         aynet  = sqrt(amax1(pow(amxin, 2) - pow(gmanuv[3 - 1], 2), 0.0)) + gmanuv[2 - 1];
         atload = amxin;
      }
      else
      {
         atload = sqrt(atlsq);
      }
      // C     --DRAG ASSOCIATED WITH ADJUSTED LOAD
      dragf = tbl1b(atload, dragvl, ndatbl, 0.0, amxin);
      // C     --atemp_m: ACCELERATION VECTOR IN MANUV SYSTEM
      atemp_m[1 - 1] = althr - dragf / acmasp + gmanuv[1 - 1];
      atemp_m[2 - 1] = aynet;
      atemp_m[3 - 1] = 0.0;
      // C     --SAVE X-COMPONENT; MAY NEED TO LIMIT IT IN SECOND PART OF PROJECTION
      aldes = atemp_m[1 - 1];
      // C     --atemp_e IN EARTH FRAME
      // call vxfrmc(rmanuv,atemp_e,atemp_m,2);
      atemp_e = rmanuv.TransposeMultiply(atemp_m);
      // C     --WEIGHT WITH CURRENT ACC. FOR FIRST PART OF PROJ.
      // call veclin(0.5,ap(1,me),0.5,atemp_e,atemp_avg);
      atemp_avg = 0.5 * ap + 0.5 * atemp_e;
      dt        = amin1(tauar, tproj3);
      prjacc(xp, vp, atemp_avg, dt, xtemp, vtemp);
      // c     --PROJECT FOR REMAINING TIME PERIOD
      dt = tproj3 - dt;

      if (dt > 0.0)
      {
         // C        --FIND THE MINIMUM LOGNITUDINAL ACCELERATION FROM spdmin
         atemp1_min = (spdmin - vtemp.Magnitude()) / dt;
         // C        --GET atemp BACK TO MANUV SYSTEM
         // call vxfrmc(rmanuv,atemp_e,atemp_m,1);
         atemp_m = rmanuv * atemp_e;
         // C        --RECOMPUTE rmanuv BASED ON NEW VELOCITY.
         rmanuv = makecs(vtemp, a0);

         if (aldes < atemp1_min)
         {
            // C           --atemp BACK TO EARTH SYSTEM
            // call vxfrmc(rmanuv,atrans,atemp_m,2);
            atrans = rmanuv.TransposeMultiply(atemp_m);
            laccel(aldes, atemp1_min, atrans, vtemp, vtemp.Magnitude(), rbep, acmasp, amxin, atload, erate);
            // C           --RECOMPUTE DRAG
            dragf = tbl1b(atload, dragvl, ndatbl, 0.0, amxin);
         }
         // call vxfrmc(rmanuv,gravec,gmanuv,1);
         gmanuv         = rmanuv * gravec;
         atemp_m[1 - 1] = aldes;
         atemp_m[2 - 1] = atload;
         atemp_m[3 - 1] = 0.0;
         // call vxfrmc(rmanuv,atemp_e,atemp_m,2);
         atemp_e = rmanuv.TransposeMultiply(atemp_m);
         atemp_e = BrawlerPlatform::a_lim(xtemp, vtemp, atemp_e, dt, spdmin);
         BrawlerPlatform::projw(xtemp, vtemp, atemp_e, dt, xeua, veua, aout, rweua);
         erate = (atemp_m[1 - 1] - gmanuv[1 - 1]) * veua.Magnitude() / grav;
      }
      else
      {
         // call xmit(3,xtemp,xeua);
         xeua = xtemp;
         // call xmit(3,vtemp,veua);
         veua = vtemp;
         // call xmit(3,atemp_avg,aout);
         aout  = atemp_avg;
         rweua = makecs(vtemp, a0);
         // call vxfrmc(rmanuv,atemp_avg,atemp_m,1);
         atemp_m = rmanuv * atemp_avg;
         erate   = (atemp_m[1 - 1] - gmanuv[1 - 1]) * veua.Magnitude() / grav;
      }
      // goto 100  //solved
   }
   break;

   case VECTOR_GEES:
   {
      // C
      // C     --DESIRED DIRECTION, SPEED, MAX ACCELERATION
      // 40    continue
      dt = amin1(tauar, 0.5 * tproj3);
      // C     --DIR0 - UNNORMALIZED DIRECTION DESIRED
      // C     --GMX - MAX G'S
      if (!mThrottleFlag)
      {
         // C        --SPD0 IS DESIRED SPEED
         spd0x  = spd0 + 0.01;
         dspeed = spd0x - spdnow;
         // vmake(spd0x,dir0,vdes);
         vdes = dir0.GetNormal() * spd0x;
         spdtoa(spd0x, spdnow, dspeed, vdes, dt, gmx, vp, aldes, atrans);
      }
      else if (mThrottleFlag)
      {
         // C        --SPD0 IS A THROTTLE SETTING
         if (spd0 >= 3)
         {
            frclon = thrstl[3 - 1 + 2]; // FORTRAN had array index -1 to 3
         }
         else
         {
            ispd   = int(spd0) + 2;
            spdwt  = spd0 + 2 - ispd;
            frclon = param1(ispd, spdwt, thrstl);
         }
         thstoa(gwind[1 - 1], frclon, dir0, vp, spdnow, dt, gmx, aldes, atrans);
      }
      else
      {
         nabort("aproj3_inr...unknown spd0 description");
      }
      almin_use = (spdmin - spdnow) / dt;
      almin_use = amin1(almaxg, almin_use);
      laccel(aldes, almin_use, atrans, vp, spdnow, rbep, acmasp, gmx * grav, atload, erate);
      atmag = atrans.Magnitude();
      // call vecinc(atrans,aldes/spdnow,vp(1,me),ades);
      ades = atrans + (aldes / spdnow) * vp;
      // call veclin(.5,ap(1,me),.5,ades,atemp);
      atemp = 0.5 * ap + 0.5 * ades;
      aout  = vorth(atemp, vp);
      if (dt * aout.Magnitude() > 0.14 * spdnow)
      {
         // C        --0.14 approx. limits error in spd to 1%
         atemp = BrawlerPlatform::a_lim(xp, vp, atemp, dt, spdmin);
         BrawlerPlatform::projw(xp, vp, atemp, dt, xtemp, vtemp, aout, rwtemp);
      }
      else
      {
         prjacc(xp, vp, atemp, dt, xtemp, vtemp);
      }
      dttrue = tproj3 - dt;

      do
      {
         // 45    continue
         dtmax = 2.0 + 8.0 * ramp(4.0, atrans.Magnitude() / grav, 0.5);
         if (dtmax < dttrue)
         {
            // C        --Avoid breaking remaining time into 2 uneven pieces
            dt = amin1(dtmax, 0.5 * dttrue);
         }
         else
         {
            dt = dttrue;
         }
         spdtmp = vtemp.Magnitude();
         if (!mThrottleFlag)
         {
            dspeed = spd0x - spdtmp;
            spdtoa(spd0, spdtmp, dspeed, vdes, dt, gmx, vtemp, aldes, atrans);
         }
         else if (mThrottleFlag)
         {
            gx = gravec.DotProduct(vtemp) / spdtmp;
            thstoa(gx, frclon, dir0, vtemp, spdtmp, dt, gmx, aldes, atrans);
         }
         else
         {
            nabort("aproj3_inr...unknown spd0 declaration");
         }
         almin_use = (spdmin - spdtmp) / dt;
         almin_use = amin1(almaxg, almin_use);
         laccel(aldes, almin_use, atrans, vtemp, spdtmp, rbep, acmasp, gmx * grav, atload, erate);
         atmag = atrans.Magnitude();
         // call vecinc(atrans,aldes/spdtmp,vtemp,atemp);
         atemp = atrans + (aldes / spdtmp) * vtemp;
         if (dt * atmag > 0.14 * spdtmp)
         {
            // C        --0.14 approx. limits error in spd to 1%
            atemp = BrawlerPlatform::a_lim(xtemp, vtemp, atemp, dt, spdmin);
            BrawlerPlatform::projw(xtemp, vtemp, atemp, dt, xeua, veua, aout, rwtemp);
         }
         else
         {
            prjacc(xtemp, vtemp, atemp, dt, xeua, veua);
         }
         dttrue = dttrue - dt;
         if (dttrue >= 0.01)
         {
            // call xmit(3,xeua,xtemp);
            xtemp = xeua;
            // call xmit(3,veua,vtemp);
            vtemp = veua;
            // goto 45   //solved
         }
      } while (dttrue >= 0.01);

      // call xmit(3,atemp,aout);
      aout = atemp;
      // C     --compute rotation matrix for wind axes (rweua)
      // C     --note that aout must be FIRST arg to vorth here to
      // C     --avoid a zero divide when xmag(aout)=0.
      orth_v = vorth(aout, veua);
      if (orth_v.Magnitude() >= 0.001)
      {
         // C        --note that aout must be FIRST arg to vorth here to
         // C        --avoid a zero divide when xmag(aout)=0.
         manwnd(veua, aout, rweua, al, at);
      }
      else
      {
         // C        --no significant desired transverse acceleration, make Z point down.
         orth_v = vorth(veua, UtVec3d(0, 0, 1));
         if (orth_v.Magnitude() > 0.001)
         {
            rweua = makeh(veua);
         }
         else
         {
            // C           --velocity vector is straight down.  Define maneuver plane
            // C           --with the x-axis in the direction of the velocity vector
            // C           --and the z-axis aligned with the aircraft's negative z axis.
            // C           --This is a guess at the direction that he would maneuver in
            // C           --if he pulled straight up.
            // call vmult(-1.0,unitz,body_neg_z);
            body_neg_z = -1.0 * UtVec3d(0, 0, 1);
            // call vxfrmc(rbeua,body_neg_z,body_neg_z,2);
            body_neg_z = rbeua.TransposeMultiply(body_neg_z);
            rweua      = makecs(veua, body_neg_z);
         }
      }
      // goto 100  //solved
   }
   break;

      // case 5:
      //{
      // LBM - no tracking maneuver for now
      ////C
      ////50    continue
      ////C     --TRACKING MANEUVER:
      //   //C     --Control specifications: w0(1..3),time0,spd0,aztrk,eltrk
      //   aprj39(tauar,freqq(iacid),freqp(iacid),rratmx(iacid),pratmx(iacid),-pratmx(iacid),alrate(iacid),atload,aout);
      //   //C     --Tracking maneuver is never duplicated.  The dmanuv logic is not
      //   //C     --sharp enough to compare body orientations, which are critical here.
      //   dupalt = false;
      //   //C     --do not permit projections to produce a negative altitude.
      //   if (xeua[3-1] >= 0.0)
      //   {
      //      xeua[3-1] = -1.0;
      //   }
      //   goto 110
      //}
      // break;

   default:
   {
      nabort("aproj3 - unsupported maneuver command type!");
   }
   break;
   }

   // C
   // C     --HERE EXPECT THESE PRECOMPUTED: aout,atload,erate,xeua,veua,rweua
   // 100   continue
   // C     --do not permit projections to produce a negative altitude.
   if (xeua[3 - 1] >= 0.0)
   {
      xeua[3 - 1] = -1.0;
   }
   // TODO_NOT_USED spdua = veua.Magnitude();
   // if (dmactv)  //LBM - no duplicate maneuver checking for now
   //{
   //    dupalt = dmanuv(veua,spdua,altdsc)
   //    if(dupalt)
   //    {
   //       return;  //goto 999
   //    }
   // }
   // else
   //{
   //    bool dupalt = false; //TODO
   // }
   alfe  = sign(tbl1b(std::abs(atload), aoavl, ndatbl, 0.0, amxin), atload);
   rbeua = bodyax(rweua, alfe);


   // 110   continue
   // smach = spdua / vsme;
   ////vxfrmc(rbeua,aout,abody,1);
   // abody = rbeua * aout;
   ////cross(veua,aout,weua);
   // weua.CrossProduct(veua,aout);
   ////vmult(1.0/pow(spdua,2),weua,weua);
   // weua *= (1.0/pow(spdua,2));
   // alt = -xeua[3 - 1] / 1000.0;
   // gees = atload/grav;
   // if (veua[1-1] != 0.0 || veua[2-1] != 0.0)
   //{
   //    //hdng = atan2(veua[1-1],-veua[2-1]);
   // }
   // else
   //{
   //    //C        --veua is straight down.  Use negative z-axis of body system
   //    //C        --instead.  This will produce the heading that he would come to
   //    //C        --if he pulled straight up.
   //    ////vmult(-1.0,unitz,body_neg_z);
   //    //body_neg_z = -1.0 * UtVec3d(0,0,1);
   //    ////vxfrmc(rbeua,body_neg_z,body_neg_z,2);
   //    //body_neg_z = rbeua.TransposeMultiply(body_neg_z);
   //    //hdng = atan2(body_neg_z[1-1],-body_neg_z[2-1]);
   // }
   // if(hdng < 0.0)
   //{
   //    hdng=hdng+twopi;
   // }
   // fpang = arcsin(-veua[3 - 1] / spdua) / rad;
   // if (rbeua[8 - 1] != 0.0 || rbeua[9 - 1] != 0.0) //TODO - check indices
   //{
   //    roll = atan2(rbeua[8 - 1], rbeua[9 - 1]) / rad;
   // }
   // else
   //{
   //    roll = 0.0;
   // }

   // save off projected state (xeua, veua, & rbeua)
   ////////////////////////////////////////////////////////////////////////////
   UtVec3d lla = BrawlerCoordinateConversion::ConvertNEDtoLLA(xeua);
   proj.SetLocationLLA(lla[0], lla[1], lla[2]);
   proj.SetVelocityNED(veua.GetData());
   UtVec3d temp(0.0, 0.0, 0.0);
   proj.SetAccelerationNED(temp.GetData());
   double e2b[3][3] = {{rbeua[0], rbeua[3], rbeua[6]}, {rbeua[1], rbeua[4], rbeua[7]}, {rbeua[2], rbeua[5], rbeua[8]}};
   double h         = 0.0;
   double p         = 0.0;
   double r         = 0.0;
   UtEntity::ExtractEulerAngles(e2b, h, p, r); // see function header in UtEntity.cpp
   proj.SetOrientationNED(h, p, r);            // TODO - test units???

   double h2 = 0.0;
   double p2 = 0.0;
   double r2 = 0.0;
   orient(rbeua, h2, p2, r2);
   proj.SetOrientationNED(h2, p2, r2);

   ////////////////////////////////////////////////////////////////////////////

   ////C     --CONVERT ERATE TO KFT/MIN:
   // if (nspotd == 1)
   //{
   //    //goto 999
   // }
   // else
   //{
   //    //do 120 iac = 2,nspotd
   //    //for (iac = 2; iac <= nspotd; ++iac)
   //    {
   //       //subvec(xeut(1,iac),xeua,dxeua(1,iac));
   //       dxeua = xeut - xeua;
   //       //subvec(veut(1,iac),veua,dveua(1,iac));
   //       dveua = veut - veua;
   //       rng = dxeua.Magnitude();
   //       //rngr(iac) = dot(dxeua(1,iac),dveua(1,iac))/rng(iac);
   //       rngr = dxeua.DotProduct(dveua)/rng;
   //       //vxfrmc(rweut(1,iac),dxeua(1,iac),dxwut(1,iac),1);
   //       dxwut = rweut * dxeua;
   //       //vxfrmc(rweut(1,iac),dveua(1,iac),dvwut(1,iac),1);
   //       dvwut = rweut * dveua;
   //       //vxfrmc(rweua,dxeua(1,iac),dxwua(1,iac),1);
   //       dxwua = rweua * dxeua;
   //       //vxfrmc(rweua,dveua(1,iac),dvwua(1,iac),1);
   //       dvwua = rweua * dveua;
   //       //vxfrmc(rbeua,dxeua(1,iac),dxbua(1,iac),1);
   //       dxbua = rbeua * dxeua;
   //       //vxfrmc(rbeua,dveua(1,iac),dvbua(1,iac),1);
   //       dvbua = rbeua * dveua;
   //       //vxfrmc(rbeut(1,iac),dxeua(1,iac),dxbut(1,iac),1);
   //       dxbut = rbeut * dxeua;
   //       //vxfrmc(rbeut(1,iac),dveua(1,iac),dvbut(1,iac),1);
   //       dvbut = rbeut * dveua;
   //       //if (lprnt)
   //       //{
   //       //   if (jactn.ne.1)
   //       //   {
   //       //      call vxfrmc(rbeua,aout,aout,1)
   //       //      wdes[1-1] = 0.
   //       //      wdes[2-1] = -aout[3-1]/spdua
   //       //      wdes[3-1] = aout[2-1]/spdua
   //       //   }
   //       //   call vuangs(dxbua(1,iac),dvbua(1,iac),wdes,psi,psidot,elv,elvdot,az,azdot)
   //       //   caspct = dxbut(1,iac)/rng(iac)
   //       //   aspect = arccos(caspct)
   //       //   rp = sqrt(dxbut(2,iac)**2+dxbut(3,iac)**2)
   //       //   if(rp.ne.0.) sp = (dxbut(2,iac)*dvbut(2,iac) + dxbut(3,iac)*dvbut(3,iac))/rp
   //       //   if(rp.eq.0.) sp = sqrt(dvbut(2,iac)**2+dvbut(3,iac)**2)
   //       //   aspdot = (sp*dxbut(1,iac)-rp*dvbut(1,iac))/rng(iac)**2
   //       //   write(ioutp,2001)
   //       iacidt(iac),psi/rad,az/rad,elv/rad,aspect/rad,rng(iac),psidot/rad,azdot/rad,elvdot/rad,aspdot/rad,-rngr(iac)
   //       //}
   //    }  //120   continue
   // }
   // 999   continue
}


// C#ABSTRACT LIMITS LONGITUDINAL ACCELERATION TO ACHIEVABLE VALUES
// C#PURPOSE  LIMITS LONGITUDINAL ACCELERATION TO ACHIEVABLE VALUES,
// C  GIVEN DIRECTION AND TRANSVERSE LOAD.
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// CIN  ALDES  REAL - DESIRED LONGITUDINAL ACCELERATION
// CIN  ALPHA  REAL - ANGLE-OF-ATTACK
// CIN  COSALF REAL - COS(ALPHA)
// CIN  GX     REAL - COMPONENT OF GRAVITY ALONG LONGITUDINAL DIRECTION
// CIN  ACM    REAL - AIRCRAFT MASS
// COUT ALONG  REAL - REALISTICALLY LIMITED VERSION OF ALDES
// COUT THRUST REAL - ASSOCIATED GROSS THRUST
// C#TECHNICAL DESCRIPTION
// C    TABLE LOOKUP GETS DRAG FOR AOA ALPHA (CURRENT A/C SPEED,
// C  ALTITUDE ARE IMPLICIT IN TABLE).  INFORMATION NEEDED FOR GRTHR CALL
// C  IS THEN AVAILABLE, RESULTING IN THRUST BEING COMPUTED.  FINALLY, ALONG
// C  IS BACKED OUT USING THE LONGITUDINAL FORCE EQN.
// C#VARIABLE DESCRIPTIONS
// C  DRAGP   REAL - DRAG AT CURRENT SPEED, ALTITUD
// C####
void BrawlerMover::allim(double aldes, double alpha, double cosalf, double gx, double acm, double& along, double& thrust)
{
   // c  --local declarations
   double dragp = 0.0;
   double epd   = 0.0;
   // C*ENDDEC
   dragp = tbl1b(std::abs(alpha), drgaoa, ndtbl2, 0.0, aoamx);
   grthr(aldes, dragp, cosalf, gx, thrust, epd);
   along = (thrust * cosalf - dragp - epd) / acm + gx;
}


// C#ABSTRACT Computes gross thrust associated with a specified long accel,
// C  drag (excluding propulsion drag) and AOA.
// C#PURPOSE  Useful in recurring situation where gross thrust needs to be
// C  estimated from the information in /mypfrm/, plus AOA and drag
// C  information.
// C#TYPE     Aircraft Aerodynamics
// C#PARAMETER DESCRIPTIONS
// CIN  ALDES  REAL - Desired longitudinal acceleration
// CIN  DRAG   REAL - Drag excluding propulsion drag
// CIN  COSAOA REAL - Cosine of AOA
// CIN  GX     REAL - Component of gravity in wind x-direction
// COUT GROSST REAL - Gross thrust associated with aldes.  However, limits
// C           are applied if interval thrmin..thrmax is exceeded.
// COUT EPD    REAL - Propulsion drag associated with grosst
// C#TECHNICAL DESCRIPTION
// C Algorithm solves the following eqn's for grosst:
// C     aldes = gx + (1/acmasp)*(grosst*cosaoa-drag-p_drag)
// C     p_drag = pd0 + ((grosst-thr0)/(thr1-thr0))*(pd1-pd0)
// C The second eqn is a linear interpolation between three values in /mypfrm/:
// C     (thrmax,epdmax), (thrnow,epdnow), (thrmin,epdmin)
// C####
void BrawlerMover::grthr(double aldes, double drag, double cosaoa, double gx, double& grosst, double& epd)
{
   double acmasp = amass;
   // C  ---local declarations:
   double thr0   = 0.0;
   double thr1   = 0.0;
   double pd0    = 0.0;
   double pd1    = 0.0;
   double slope  = 0.0;
   double fxreqd = 0.0;
   // C*ENDDEC
   fxreqd = acmasp * (aldes - gx) + drag;
   // c     --determine which interval to use for interpolation.
   if (thrnow == thrmax || thrnow == thrmin)
   {
      // c        --really only 1 interval in these cases:
      thr0 = thrmin;
      pd0  = epdmin;
      thr1 = thrmax;
      pd1  = epdmax;
   }
   else if (fxreqd > (thrnow * cosaoa - epdnow))
   {
      thr0 = thrnow;
      pd0  = epdnow;
      thr1 = thrmax;
      pd1  = epdmax;
   }
   else
   {
      thr0 = thrmin;
      pd0  = epdmin;
      thr1 = thrnow;
      pd1  = epdnow;
   }
   if (thr1 != thr0)
   {
      slope = (pd1 - pd0) / (thr1 - thr0);
      if (std::abs(cosaoa - slope) < eps)
      {
         grosst = thrmax;
      }
      else
      {
         grosst = (fxreqd + pd0 - thr0 * slope) / (cosaoa - slope);
         grosst = xlimit(grosst, thrmax, thrmin);
      }
      epd = pd0 + (grosst - thr0) * slope;
   }
   else
   {
      // C        --This code handles the case where thr1=thr0.  This can legitimately
      // C        --occur in two ways: (1) thrust tables can have data that is the same
      // C        --for different throttle settings (this is probably an error unless an
      // C        --aircraft is not equipped with an afterburner) or (2) throtm is being
      // C        --used (this makes it appear that two throttle settings give the same
      // C        --thrust; this is not an error in TB).
      grosst = xlimit(thr0, thrmax, thrmin);
      epd    = pd0;
   }
}


// C#ABSTRACT LIMITS A/C ACCELERATION VECTOR TO ACHIEVABLE VALUE
// C#PURPOSE  LIMITS A/C ACCELERATION VECTOR TO ACHIEVABLE VALUE
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// CIO  ALONG  REAL - On input, desired longitudinal acceleration.
// C           On output, same, limited to achievable value.
// CIN  ALMIN_USE real - The minimum allowable longitudinal acceleration.  The
// C           system will reduce turn rate rather than decelerate excessively.
// CIO  ATRANS 3-VEC - DESIRED TRANSVERSE ACCELERATION.  Any portion
// C           parallel to V (below) will be removed.  Remainder will
// c           be limited.
// CIN  V      3-VEC - A/C VELOCITY
// CIN  SPD    REAL - SPEED
// CIN  RBE    ORIENT - Body-to-earth orientation matrix for aircraft.
// C           Only used in case where velocity is straight down and
// C           there is little to no desired transvers acceleration.
// CIN  ACM    REAL - A/C MASS
// CIN  AMAX   REAL - LOAD LIMIT IN ACCELERATION UNITS
// COUT ATLOAD REAL - LOAD IN ACCELERATION UNITS
// COUT ERATE  REAL - SPECIFIC ENERGY RATE
// C#TECHNICAL DESCRIPTION
// C    FIRST, ALOAD, THE DESIRED LOAD, IS COMPUTED BY SUBTRACTING THE
// C  TRANSVERSE COMPONENT OF GRAVITY FROM THE DESIRED TRANSVERSE
// C  ACCELERATION.  IF AMAX IS EXCEEDED THEN ATRANS IS ADJUSTED SO THAT
// C  THE LOAD WILL BE AMAX AND THE PLANE OF ACCELERATION IS UNALTERED.
// C    NEXT, THE ALLIM FUNCTION LIMITS LONGITUDINAL ACCELERATION
// C  TO AN ACHIEVABLE VALUE.  FINALLY, THE SPECIFIC ENERGY
// C  RATE ASSOCIATED WITH THE ACHIEVABLE ACCELERATION IS COMPUTED.
// C#VARIABLE DESCRIPTIONS
// C  ACMASP   REAL [SLUGS] - Aircraft mass.
// C  THRMAX   REAL [LB] - Maximum thrust available.
// C  ALOAD3  REAL - Z-COMP OF PART OF TRANSVERSE ACCEL ASSSOCIATED WITH LOAD
// C####
void BrawlerMover::laccel(double&            along,
                          double             almin_use,
                          UtVec3d&           atrans,
                          const UtVec3d&     v,
                          double             spd,
                          const UtFortMat3d& rbe,
                          double             acm,
                          double             amax,
                          double&            atload,
                          double&            erate)
{
   double acmasp = amass;
   // c  --local declarations
   double      drag_max = 0.0;
   double      p        = 0.0;
   double      lift_max = 0.0;
   double      aoa      = 0.0;
   int         ilow     = 0;
   UtFortMat3d rmanuv   = UtFortMat3d();
   UtVec3d     gmanuv(0.0, 0.0, 0.0);
   UtVec3d     anet(0.0, 0.0, 0.0);
   double      alpha, grthr;
   double      aload3, aluse;
   UtVec3d     r_orth(0.0, 0.0, 0.0);
   UtVec3d     body_neg_z(0.0, 0.0, 0.0);

   // C*ENDDEC
   // C     --FIRST LIMIT TRANSVERSE ACCELERATION
   // C     --define maneuver plane with x-axis in direction of velocity
   // C     --vector and z-axis aligned with desired transverse acceleration
   if (atrans.Magnitude() > 0.001)
   {
      rmanuv = makecs(v, atrans);
   }
   else
   {
      // C        --no significant desired transverse acceleration, make Z point down.
      r_orth = vorth(v, UtVec3d(0, 0, 1));
      if (r_orth.Magnitude() > 0.001)
      {
         rmanuv = makeh(v);
      }
      else
      {
         // C           --velocity vector is straight down.  Define maneuver plane
         // C           --with the x-axis in the direction of the velocity vector
         // C           --and the z-axis aligned with the aircraft's negative z axis.
         // C           --This is a guess at the direction that he would maneuver in
         // C           --if he pulled straight up.
         // call vmult(-1.0,unitz,body_neg_z);
         body_neg_z = -1.0 * UtVec3d(0, 0, 1);
         // call vxfrmc(rbe,body_neg_z,body_neg_z,2);
         body_neg_z = rbe.TransposeMultiply(body_neg_z);
         rmanuv     = makecs(v, body_neg_z);
      }
   }
   // call vxfrmc(rmanuv,gravec,gmanuv,1)
   gmanuv = rmanuv * gravec;

   // C     --Determine maximum load that won't exceed almin_use.  First, ignore
   // C     --cos(alpha) term that should multiply thrust.  Then make 1
   // C     --iteration to correct for AOA:
   drag_max = thrmax + (gmanuv[1 - 1] - almin_use) * acmasp;
   srch(dragvl, ndatbl, drag_max, ilow, p);

   if (ilow == 0 && drag_max < 0.0)
   {
      lift_max = 0.0;
      // goto 100  //solved
   }
   else
   {
      if (ilow == 0)
      {
         aoa = aoamx;
      }
      else
      {
         aoa = param1(ilow, p, aoavl);
      }

      drag_max = drag_max - thrmax * (1.0 - cos(aoa));
      srch(dragvl, ndatbl, drag_max, ilow, p);
      if (ilow == 0)
      {
         if (drag_max < 0.0)
         {
            lift_max = 0.0;
         }
         else
         {
            lift_max = amxin;
         }
      }
      else
      {
         lift_max = amxin * (ilow + p - 1.0) / ndatbl;
      }
      lift_max = amin1(lift_max, amax);
   }

   // 100   continue
   // C     --Compute anet, the net transverse acceleration in rmanuv coords,
   // C     --then use it to generate lift in rmanuv coords.
   // call vxfrmc(rmanuv,atrans,anet,1);
   anet        = rmanuv * atrans;
   anet[1 - 1] = 0.0;
   // C     --assert: anet(2) = 0.0 (from makecs definition)
   // C     --Set it to 0.0 explicitly, since vxfrmc may produce a very
   // C     --small value due to machine accuracy limitations
   anet[2 - 1] = 0.0;
   // C     --assert: aload(2) = -gmanuv(2) (from reqmt of desired plane = x-z)
   aload3 = anet[3 - 1] - gmanuv[3 - 1];
   atload = sqrt(pow(gmanuv[2 - 1], 2) + pow(aload3, 2));
   if (atload > lift_max)
   {
      // C        --limit to achievable value in same maneuver plane:
      atload = lift_max;
      if (std::abs(gmanuv[2 - 1]) < lift_max)
      {
         anet[3 - 1] = sqrt(pow(lift_max, 2) - pow(gmanuv[2 - 1], 2)) + gmanuv[3 - 1];
      }
      else
      {
         // C           --Do best possible to counter gmanuv(2) by placing all lift
         // C           --in y-direction:
         anet[2 - 1] = gmanuv[2 - 1] - sign(lift_max, gmanuv[2 - 1]);
         // C           --This leaves only gravity in the z-direction:
         anet[3 - 1] = gmanuv[3 - 1];
      }
   }
   // call vxfrmc(rmanuv,atrans,anet,2);
   atrans = rmanuv.TransposeMultiply(anet);
   // C     --NOW LIMIT LONGITUDINAL ACCELERATION
   alpha = tbl1b(atload, aoavl, ndatbl, 0.0, amxin);
   allim(amax1(along, almin_use), alpha, cos(alpha), gmanuv[1 - 1], acm, aluse, grthr); //
   along = aluse;
   erate = (along - gmanuv[1 - 1]) * spd / grav;
}


// C#ABSTRACT MAKES A CONSTANT ACCELERATION PROJECTION
// C#PURPOSE  MAKES A CONSTANT ACCELERATION PROJECTION.
// C#TYPE     POSITION PROJECTION
// C#PARAMETER DESCRIPTIONS
// CIN  XIN    3-VEC - STARTING POSITION
// CIN  VIN    3-VEC - STARTING VELOCITY
// CIN  A      3-VEC - ACCELERATION (CONSTANT FOR ENTIRE PROJECTION)
// CIN  DT     REAL - PROJECTION TIME
// COUT XOUT   3-VEC - PROJECTED POSITION VECTOR
// COUT VOUT   3-VEC - PROJECTED VELOCITY VECTOR
// C#TECHNICAL DESCRIPTION
// C    USES ELEMENTARY KINEMATICS INDEPENDENTLY FOR EACH CARTESIAN
// C  COMPONENT OF MOTION:
// C    X(T) = X0 + V0*T + .5*A*T**2
// C    V(T) = V0 + A*T
// C  NOTE THAT VOUT AND VIN MUST BE DIFFERENT MEMORY LOCATIONS.
// C#VARIABLE DESCRIPTIONS:  none
// C####
void BrawlerMover::prjacc(const UtVec3d& xin, const UtVec3d& vin, const UtVec3d& a, double dt, UtVec3d& xout, UtVec3d& vout)
{
   double hdt = 0.0;
   // C*ENDDEC
   hdt         = 0.5 * dt;
   vout[1 - 1] = vin[1 - 1] + dt * a[1 - 1];
   xout[1 - 1] = xin[1 - 1] + hdt * (vin[1 - 1] + vout[1 - 1]);
   vout[2 - 1] = vin[2 - 1] + dt * a[2 - 1];
   xout[2 - 1] = xin[2 - 1] + hdt * (vin[2 - 1] + vout[2 - 1]);
   vout[3 - 1] = vin[3 - 1] + dt * a[3 - 1];
   xout[3 - 1] = xin[3 - 1] + hdt * (vin[3 - 1] + vout[3 - 1]);
   // C     --PRJACC OF DOUBLE PRECISION POSITION
   // entry dprjacc (xdin,vin,a,dt,xdout,vout)
   // hdt = .5*dt
   // vout(1) = vin(1)+dt*a(1)
   // xdout(1) = xdin(1)+dble(hdt*(vin(1)+vout(1)))
   // vout(2) = vin(2)+dt*a(2)
   // xdout(2) = xdin(2)+dble(hdt*(vin(2)+vout(2)))
   // vout(3) = vin(3)+dt*a(3)
   // xdout(3) = xdin(3)+dble(hdt*(vin(3)+vout(3)))
   // return
}


// C#ABSTRACT Calculates a desired acceleration from a desired velocity
// C#PURPOSE  THIS CODE WAS BROKEN OUT OF APROJ3 FJOR IACTN = 4.  THIS
// C          CODE FINDS THE DESIRED LONGITUDINAL AND TRANSVERSE
// C          ACCELERATIONS FROM A DESIRED VELOCITY.
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// CIN   SPD0X    REAL - Desired speed
// CIN   SPDTMP   REAL - Current speed
// CIN   DSPEED   REAL - Difference between desired speed and current
// C              speed.
// CIN   VDES     ARRAY DIM(3-VEC) of REAL - Desired velocity vector.
// CIN   DT       REAL - Time step
// CIN   GMAX     REAL - Maximum G's for the alternative
// CIN   VTEMP    ARRAY DIM(3-VEC) of REAL - Current velocity vector
// COUT  ALDES    REAL - Desired longitudinal acceleration
// COUT  ATRANS   ARRAY DIM(3-VEC) of REAL - Transverse acceleration
// C              vector.
// C#TECHNICAL DESCRIPTION
// C This routine uses the desired direction, speed, and maximum
// C  acceleration to find the desired acceleration.
// C#VARIABLE DESCRIPTIONS
// C####
void BrawlerMover::spdtoa(double         spd0x,
                          double         spdtmp,
                          double         dspeed,
                          const UtVec3d& vdes,
                          double         dt,
                          double         gmax,
                          const UtVec3d& vtemp,
                          double&        aldes,
                          UtVec3d&       atrans)
{
   // C  --local declarations:
   double  cdhead = 0.0;
   double  coef   = 0.0;
   double  dvmag  = 0.0;
   UtVec3d dv(0.0, 0.0, 0.0);
   // int i;
   // C*ENDDEC
   aldes = dspeed / dt + 0.001;
   // C     --Remainder of code computes transverse acceleration.
   // C     --In a number of situations vdes and vtemp may be exactly aligned.
   // C     --Much of the remaining code is for coping with numerical
   // C     --problems that this can cause.
   cdhead = vdes.DotProduct(vtemp) / (spd0x * spdtmp);
   if (cdhead > 0.99999)
   {
      // C        --Aligned to within .26 deg: ignore transverse accel.
      // call zero3(atrans);
      atrans.Set(0, 0, 0);
   }
   else
   {
      dv = vorth(vdes, vtemp);
      // do 100 i=1,3
      for (int i = 1; i <= 3; ++i)
      {
         // C           --This loop avoids some numerical problems
         if (std::abs(dv[i - 1]) < 1.0e-15)
         {
            dv[i - 1] = 0.0;
         }
      } // 100      continue

      if (cdhead > 0.0)
      {
         // C           --For turns under 90 deg, approximate atrans with dv/dt:
         // call vmult(1./dt,dv,atrans)
         atrans = (1.0 / dt) * dv;
      }
      else
      {
         // C           --For turns over 90 degrees, turn at gmax, plus component of
         // C           --gravity in dv-direction:
         dvmag = dv.Magnitude();
         if (dvmag > eps)
         {
            coef = amax1(gmax + dv[3 - 1] / dvmag, 0.1);
            // call vmult(coef*grav/dvmag,dv,atrans);
            atrans = (coef * grav / dvmag) * dv;
         }
         else
         {
            // call zero3(atrans);
            atrans.Set(0, 0, 0);
         }
      }
   }
}


// C#ABSTRACT FINDS THE DESIRED ACCELERATION FROM A THROTTLE SETTING
// C#PURPOSE  CALCULATES DESIRED LONGITUDINAL AND TRANSVERSE ACCELERATION
// C          FROM A THROTTLE SETTING
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// CIN   GX       REAL - Gravity term in wind axes
// CIN   FRCLON   REAL - Longitudinal force
// CIN   DIR0     ARRAY DIM(3-VEC) of REAL - Desired direction
// CIN   VTEMP    ARRAY DIM(3-VEC) of REAL - Current velocity vector
// CIN   SPDTMP   REAL - Current speed
// CIN   DT       REAL - Time step
// CIN   GMAX     REAL - Max G's for this maneuver
// COUT  ALDES    REAL - Desired longitudinal acceleration
// COUT  ATRANS   ARRAY DIM(3-VEC) of REAL - Desired transverse
// C              acceleration
// C#TECHNICAL DESCRIPTION
// C The desired longitudinal acceleration is found as a function of
// C  the longitudinal force based in the throttle setting for the current
// C  speed and altitude, the drag, and GX.  The transverse acceleration is
// C  calculated from the current speed and time step.
// C#VARIABLE DESCRIPTIONS
// C !      !
// C####
void BrawlerMover::thstoa(double         gx,
                          double         frclon,
                          const UtVec3d& dir0,
                          const UtVec3d& vtemp,
                          double         spdtmp,
                          double         dt,
                          double         gmax,
                          double&        aldes,
                          UtVec3d&       atrans)
{
   double acmasp = amass;

   // C  ---local declarations:
   UtVec3d dv(0.0, 0.0, 0.0);
   double  cdhead = 0.0;
   double  dhead  = 0.0;
   double  accel  = 0.0;
   double  dragf  = 0.0;
   // int i;
   // real arccos
   // real arcsin
   // C*ENDDEC
   // C     --FIND THE ERROR
   cdhead = dir0.DotProduct(vtemp) / (spdtmp * dir0.Magnitude());
   // C     --CALCULATE TRANSVERSE ACCELERATION VECTOR
   if (cdhead > 0.99999)
   {
      // call xmit(-3,0.0,atrans)
      atrans.Set(0, 0, 0);
      // goto 100  //solved
   }
   else
   {
      dv = vorth(dir0, vtemp);
      if (dv.Magnitude() < 0.001)
      {
         // call cros1(UtVec3d(0,0,1),vtemp,dv);
         dv.CrossProduct(UtVec3d(0, 0, 1), vtemp);
         dv.Normalize();
         // call vmult(0.05,dv,dv);
         dv *= 0.05;
      }
      if (cdhead > 0.0)
      {
         dhead = arccos(cdhead);
         // call vmake(dhead*spdtmp/dt,dv,atrans);
         atrans = dv.GetNormal() * (dhead * spdtmp / dt);
      }
      else
      {
         // call vmake((gmax + 1.)*grav,dv,atrans);
         atrans = dv.GetNormal() * ((gmax + 1.0) * grav);
      }
   }
   // 100   continue
   accel = atrans.Magnitude();
   // C     --LIMIT THE ACCELERATION TO THE MAX ACCLERATION ACHIEVABLE
   if (accel > amxin)
   {
      accel = amxin;
   }

   // C     --FIND THE DRAG
   dragf = tbl1b(accel, dragvl, ndatbl, 0.0, amxin);
   // C     --FIND DESIRED ACCELERATION
   aldes = (frclon - dragf) / acmasp + gx;
}


// C#ABSTRACT CONSTRUCTS WIND AXES, GIVEN VELOCITY AND ACCELERATION
// C#PURPOSE  CONSTRUCTS WIND AXES, GIVEN VELOCITY AND ACCELERATION
// C#TYPE     MANEUVER
// C#PARAMETER DESCRIPTIONS
// CIN  V      3-VEC - VELOCITY
// CIN  A      3-VEC - NET ACCELERATION
// COUT RW     ORIENT - WIND AXIS SYSTEM OF OBJECT WITH V,A
// COUT AL     REAL - LONG ACCELERATION (NOT INCL. GRAVITY)
// COUT AT     REAL - TRANSVERSE ACCELERATION (NOT INCL. GRAVITY)
// C#TECHNICAL DESCRIPTION
// C    GRAVITY IS SUBTRACTED TO GET ACCEL DUE TO OTHER FACTORS, ASUP.
// C  THE MAKCSI ROUTINE THEN CONSTRUCTS RW USING V AND ASUP AS INPUTS.
// C  ASUP IS TRANSFORMED TO WIND AXES (AW) AND AL AND AT SET EQUAL TO
// C  THE X AND -Z COMPONENTS OF AW.
// C####
void BrawlerMover::manwnd(const UtVec3d& v, const UtVec3d& a, UtFortMat3d& rw, double& al, double& at)
{
   // dimension asup(3),aw(3)
   UtVec3d asup(0.0, 0.0, 0.0);
   UtVec3d aw(0.0, 0.0, 0.0);
   // C*ENDDEC
   // call vecinc(a,-grav,unitz,asup)
   asup = a + -grav * UtVec3d(0, 0, 1);
   // call makcsi(v,asup,rw)
   rw = makcsi(v, asup);
   // call vxfrmc(rw,asup,aw,1)
   aw = rw * asup;
   al = aw[1 - 1];
   at = -aw[3 - 1];
}


// C#ABSTRACT COMPUTES A/C PERF VARIABLES IN /MYPFRM/
// C#PURPOSE  COMPUTES A/C PERF VARIABLES IN /MYPFRM/
// C#TYPE     MANEUVER
// C#TECHNICAL DESCRIPTION
// C    THE COMPUTATION OF ALL VARIABLES EXCEPT GMXSU AND CORNRV ARE
// C  ROUTINE.  SINCE GMXSU IS INTENDED NOT AS THE TRUE MAX SUSTAINED
// C  GEE CAPABILITY, BUT RATHER AS THE MAX GEE'S THAT THE PILOT WILL
// C  USE IN ANY SITUATION EXCEPT EXTREME EMERGENCY, IT MUST BE
// C  SENSITIVE TO A NUMBER OF NON-AERODYNAMIC FACTORS.  FACTORS INCLUDE
// C  INTEREST IN FIRING A WEAPON AND RANGE TO NEAREST HOSTILE OR HOSTILE
// C  MISSILE.
// C    LOOP 10 COMPUTES DRAG-VERSUS-LIFT AND AOA-VERSUS-LIFT TABLES,
// C  GIVEN CURRENT SPEED, ALTITUDE.
// C#VARIABLE DESCRIPTIONS
// C   1   IACID    AC-IND [NA] - My aircraft index.
// C  ME       INT - Index of self in mental model. (1)
// C
// C  VP       ARRAY (MM-IDX) OF (3-VEC) [FT/SEC] - Current estimate of velocity.
// C  XP       ARRAY (MM-IDX) OF (3-VEC) [FT] - Current estimate of position.
// C  AMXIN    REAL [NA] - GMXIN/G.
// C  AOAMX    REAL [RAD] - Max allowable AOA
// C  AOAVL    ARRAY DIM(NDATBL) OF (REAL) [RAD] - Angle-of-attack
// C           versus lift table.
// C  DRAGVL   ARRAY DIM(NDATBL) OF (REAL) [LB] - Drag versus lift
// C           table.
// C  DRGAOA   ARRAY DIM(NDTBL2) of (REAL) [LB] - Drag vs AOA table
// C  GMXSU    REAL [NA] - Maximum load possible at maximum thrust.
// C  GMXSUT   REAL [NA] - Actual maximum sustained G's.
// C  THRSTL   ARRAY DIM(-1:3) of (REAL) [LB] - Longitudinal force as a
// C           function of throttle setting at current speed and current
// C           altitude.
// C  VSME     REAL [FT/SEC] - Velocity of sound.
// C 6115 GCAP     ARRAY DIM(2) OF (REAL) [NA] - Maximum g capacity in g's
// C               for the aircraft. gcap(1)=maximum positive g limit.
// C               gcap(2)=maximum negative g limit (GCAP(2) IS INPUT AS A
// C               NEGATIVE NUMBER!)
// C####
void BrawlerMover::perfrm()
{
   UtVec3d     xp     = Position();                  // plat.PositionNED();
   double      spdnow = Velocity().Magnitude();      // plat.GetSpeed();
   UtVec3d     ap     = Acceleration();              // plat.AccelerationNED();
   double      alphap = Alpha();                     // plat.GetAlpha();
   UtFortMat3d rwep   = EarthToWindRotationMatrix(); // TODO - verify?

   // c     --local declarations
   double xntbl1 = 0.0;
   double xntbl2 = 0.0;
   double tlift  = 0.0;
   double xx     = 0.0;
   double calpha = 0.0;
   double aoai   = 0.0;
   // double gmax;
   // double mymach;
   double tgglng = 0.0;
   double tablng = 0.0;
   double id     = 0.0;
   double thrtlx = 0.0;
   double tdd    = 0.0;
   int    ilift  = 0;
   int    ii     = 0;
   // int i;
   int ithrot = 0;
   // C*ENDDEC
   // call gtypd(iacid,3)   //no memory copies, use class member variables
   xntbl1 = float(ndatbl - 1);
   xntbl2 = float(ndtbl2 - 1);
   atmos(-xp[3 - 1], tempme, presme, densme, vsme, prme, drme);
   // double dyprsp = dyp; //dypres(iacid);
   double acmasp = amass;
   // double throtp = throtl;
   double fmachp = Mach();
   wate          = acmasp * grav;
   cornrv        = corner(-xp[3 - 1], vsme, wate);
   gmxin         = gldmx(wate, 1); // gldmx(fmachp,dyprsp,wate,1,iacid);
   amxin         = gmxin * grav;
   gmnin         = gldmx(wate, 2); // gldmx(fmachp,dyprsp,wate,2,iacid);
   thrtlx        = amin1(throtm, 3.0);
   if (fmachp == 0)
   {
      // C        --PRESUMABLY A SAM SITE
      gmxsut = 0.0;
      // call xmit(-ndatbl,0.0,dragvl);
      // call xmit(-ndatbl,0.0,aoavl);
      for (ilift = 1; ilift <= ndatbl; ++ilift)
      {
         dragvl[ilift - 1] = 0.0;
         aoavl[ilift - 1]  = 0.0;
      }
   }
   else
   {
      gmxsut = susgmx(wate, thrtlx);
      // do 10 ilift=1,ndatbl
      for (ilift = 1; ilift <= ndatbl; ++ilift)
      {
         tlift             = amxin * acmasp * float(ilift - 1) / xntbl1;
         dragvl[ilift - 1] = drag(tlift);       // drag(fmachp,dyprsp,tlift,iacid);
         aoavl[ilift - 1]  = aoa(tlift, false); // aoa(fmachp,dyprsp,tlift,iacid,false);
      }                                         // 10       continue
   }
   gmxsu = gmxsut;


   double rhst  = 0.0;
   double rhstm = 0.0;
   if (mBrawlerPlatformPtr != nullptr)
   {
      UtEntity* tgt = mBrawlerPlatformPtr->GetTarget();
      if (tgt != nullptr)
      {
         rhst = mBrawlerPlatformPtr->SlantRange(*tgt); // closest range to [hostile] target
      }
      UtEntity* threat = mBrawlerPlatformPtr->GetThreat();
      if (threat != nullptr)
      {
         // double rhstm = SlantRange(GetThreat());   // closest range to [hostile] missile
         rhstm = mBrawlerPlatformPtr->SlantRange(*threat); // closest range to [hostile] missile
      }
   }

   // if (iand(mslmd,3) != 0)
   if (mBrawlerPlatformPtr != nullptr && (mBrawlerPlatformPtr->MissileMode() == BRAWLER::INTERCEPT ||
                                          mBrawlerPlatformPtr->MissileMode() == BRAWLER::AIMFIRE))
   {
      // c        --allow max turns because firing interest
      gmxsu = gmxin;
   }
   else if (amin1(rhst, rhstm) * ftnmi > 20.0)
   {
      // c        --no need to turn hard at longer range
      gmxsu = gmxsut;
   }
   else if (fmachp > 1.0 || spdnow > cornrv + 100.0)
   {
      // c        --in vicinity of hostiles, fast enough to use gmxin
      gmxsu = gmxin;
   }
   else
   {
      // c        --in vicinity of hostiles, but too slow to exceed gmxsut
      // c        --note that this won't apply if weapons firing interest
      gmxsu = gmxsut;
   }

   if (fmachp == 0.0)
   {
      wmax   = 0.0;
      drgsu  = 0.0;
      radmnp = 0.0;
   }
   else
   {
      radmnp = fmachp * fmachp * vsme * vsme / (gmxsu * grav);
      wmax   = gmxsu * grav / spdnow;
      xx     = xntbl1 * gmxsu / gmxin + 1.0;
      if (xx >= ndatbl)
      {
         drgsu = dragvl[ndatbl - 1];
      }
      else
      {
         ii    = int(xx);
         xx    = xx - ii;
         drgsu = dragvl[ii - 1] * (1.0 - xx) + dragvl[ii + 1 - 1] * xx;
      }
   }

   // use:   fggsv, fabsv, trpdsv, ddpdsv
   effthr(fggsv, fabsv, trpdsv, ddpdsv, thrnow, epdnow);
   effthr(1.0, 1.0, trpdsv, ddpdsv, thrmax, epdmax);
   effthr(0.0, 0.0, trpdsv, ddpdsv, thrmin, epdmin);

   // call vxfrmc(rwep,ap(1,me),aw,1);
   aw     = rwep * ap;
   calpha = cos(alphap) / acmasp;
   drgnow = (thrnow * calpha - aw[1 - 1] - epdnow) * acmasp;
   // call vxfrmc(rwep,gravec,gwind,1);
   gwind = rwep * gravec;

   almaxg = (thrmax - dragvl[1 - 1] - epdmax) / acmasp;
   psubs  = almaxg * spdnow / grav;
   almax  = almaxg + gwind[1 - 1];
   alming = (thrmin - dragvl[ndatbl - 1] - epdmin) / acmasp;
   almin  = alming + gwind[1 - 1];
   // c     --compute drgaoa, aoamx, aoamn
   if (arotyp == aero1)
   {
      aoamx = aoavl[ndatbl - 1];
      aoamn = -aoamx;
   }
   // else if (arotyp.eq.aero2)   //LBM - not supporting aero2 type yet
   //{
   //    call aoamax(fmachp,thrnow,aoamx,aoamn);
   // }
   else
   {
      nabort("perfrm...unknown arotyp");
   }

   // do 20 ii=1,ndtbl2
   for (ii = 1; ii <= ndtbl2; ++ii)
   {
      aoai           = (aoamx / xntbl2) * float(ii - 1);
      drgaoa[ii - 1] = drag2(aoai); // drag2(fmachp,dyprsp,aoai,iacid);
   }                                // 20    continue

   // C     --COMPUTE LONGITUDINAL FORCE AS A FUNCTION OF THROTTLE SETTING
   // do 30 ithrot=-1,3,1
   for (ithrot = -1; ithrot <= 3; ++ithrot)
   {
      thrtlx = float(ithrot);
      thrust(thrtlx, tgglng, tablng, id);
      // LBM - no drag devices for now
      // if (ddmm > 0.0 && thrtlx < 0.0)
      //{
      //    //C           --DRAG DEVICES DEPLOYED
      //    tdd = fdddrg(fmachp,-xp(3,me),dyprsp,area(iacid), ddpd(iacid))*std::abs(thrtlx);
      // }
      // else
      {
         tdd = 0.0;
      }
      thrstl[ithrot - 1 + 2] = tgglng + tablng - tdd; // FORTRAN had array index -1 to 3
   }                                                  // 30    continue

   // if(lprnt) then
   //    mymach = xmag(vp(1,me))/vsme
   //    if (acmasp .gt. miss_mass) then
   //       gmax = tbl1(mymach,gcap2,mchval,nmach1)
   //    else
   //    gmax = tbl1(mymach,gcap,mchval,nmach1)
   //    endif
   //    write(ioutp,1000) iacid,-xp(3,me)/1000., mymach,gmax,vsme,cornrv/vsme
   //    write(ioutp,1100) (thrstl(i),i=-1,3)
   //    write(ioutp,1200) gmxsu,gmxsut
   //    write(ioutp,1201) amxin,aoamx,dragvl
   //    write(ioutp,1202) 'aoavl',aoavl
   //    write(ioutp,1202) 'drgaoa',drgaoa
   //    endif
}


// C#ABSTRACT COMPUTE CORNER VELOCITY
// C#PURPOSE  COMPUTE CORNER VELOCITY
// C#REWRITE  MCCLURE (AFSAA) 09-apr-1997
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  ALT    REAL [FT]     - Altitude.
// CIN  VS     REAL [FT/SEC] - Speed of sound at alt.
// CIN  WATE   REAL [SLUGS]  - A/C wate.
// CIN  JACID  INT  [N/A] AC-IDX - A/C for which corner velocity
// C                 is desired.
// COUT CORNER REAL [FT/SEC] - Corner velocity (ft/sec).
// C#TECHNICAL DESCRIPTION
// C  Solves for the maximum turn rate, which corresponds to corner
// C velocity.  This can be g-limited and is assumed to be sub-sonic.
// C At the first attempt, the algorithm breaks up the possible corner
// C speeds for the aircraft into n_step pieces from 0.1 to 1.0 Mach.
// C This corresponds to about 50 feet/second (30 knots) accuracy at
// C sea level.  Subsequent calls to corner use 1/2 the previous corner
// C calculation as the lower bound on the algorithm, so the accuracy
// C of the algorithm improves as a function of time.
// C
// C Turn rate is computed assuming a level turn by:
// C  TURN_RATE [RAD/SEC] = g*root(n**2-1) / V
// C
// C#VARIABLE DESCRIPTIONS
// C SPDA   REAL [FT/SEC] - Lowest evaluated speed
// C SPD    REAL [FT/SEC] - Speed for each turn rate computation
// C SPDB   REAL [FT/SEC] - Highest evaluated speed (the Mach)
// C N_STEP INT  [NA]     - Parameter defining the granularity of the
// C                        algorithm.
// C SPDINC REAL [FT/SEC] - Increment to add to current speed
// C CASB   REAL [FT/SEC] - CAS at the maximum defined Mach
// C GL     REAL [NA]     - Maximum G capability at SPD (including any
// C                         structural limits)
// C TURN_RATE REAL [RAD/SEC] -  Maximum turn rate at SPD
// C TRMAX  REAL [RAD/SEC] - Biggest turn rate
// C####
double BrawlerMover::corner(double alt, double vs, double wate)
{
   // C     --local declarations
   double spdmax = 0.0;
   double spd    = 0.0;
   double spda   = 0.0;
   double spdb   = 0.0;
   double spdinc;
   double gl        = 0.0;
   double turn_rate = 0.0;
   double trmax     = 0.0;
   double casb      = 0.0;
   double dypr      = 0.0;
   double mchdum    = 0.0;
   double gmax      = 0.0;
   int    step      = 0;
   int    n_steps   = 20;
   int    ifmmax    = 0;
   // C*ENDDEC

   // C     --USE SPDMAX THAT CORRESPONDS TO THE LAST NON-ZERO ENTRY OF THE
   // C     --CLMAX TABLE.  ALSO LIMIT WITH CAS LIMIT AT THIS ALTITUDE.
   ifmmax = nfm1;
   // 45    if (clmax(ifmmax).eq.0.) then
   //    ifmmax = ifmmax-1
   //    goto 45   //solved
   //    endif
   while (clmax[ifmmax - 1] ==
          0.0) // TODO - this assumes clmax eventually goes to zero (user input requirement? enforce on read in?)
   {
      --ifmmax; // ifmmax = ifmmax-1
   }
   spdmax = fmvec1[ifmmax - 1] * vs;
   casb   = cas(spdmax); // cas(spdmax,alt)
   if (casb > casmax)
   {
      spdmax = spdmax * (casmax / casb);
   }
   if (crnrv > 0.0) // crnrv(jacid)
   {
      // C        --IMPROVE GUESS AT LOWER BOUND
      spda = 0.5 * crnrv; // crnrv(jacid)
   }
   else
   {
      spda = 0.1 * vs;
   }
   // C     --ASSERT: CORNER VELOCITY LESS THAN THE SPEED OF SOUND.
   spdb   = amin1(spdmax, vs);
   spdinc = (spdb - spda) / n_steps;
   trmax  = -xlarge;

   // do 200 step=1,n_steps
   for (step = 1; step <= n_steps; ++step)
   {
      spdb = spda + (step - 1) * spdinc;
      // getqm(spdb,alt,dypr,mchdum);
      getqm(spdb, dypr, mchdum);

      // gl = gliftm(spdb/vs,dypr,wate,jacid);
      gl = gliftm(spdb / vs, dypr, wate);
      // C        --Find gcap at spdb, depending on whether above/below design weight.
      if (wate > miss_mass * grav)
      {
         gmax = tbl1(spdb / vs, &gcap2[0][0], mchval, nmach1);
      }
      else
      {
         gmax = tbl1(spdb / vs, &gcap[0][0], mchval, nmach1);
      }
      gl = amin1(gl, gmax);
      if (gl <= 1.0)
      {
         continue; // goto 200  //solved
      }
      // C        --FOR EFFICIENCY DONT INCLUDE THE "g" TERM
      turn_rate = sqrt(gl * gl - 1.0) / spdb;
      if (turn_rate > trmax)
      {
         trmax = turn_rate;
         spd   = spdb;
      }
   } // 200   continue

   // C     --Preserve result in crnrv(jacid) to seed next calculation
   crnrv = spd;
   return spd; // corner = spd;
}


// C#ABSTRACT FLYS AIRCRAFT OUT TO CURRENT TIME
// C#PURPOSE  DETAILED AIRCRAFT FLYOUT MAIN ROUTINE - FLYS A/C OUT TO
// C  CURRENT TIME, UPDATING /EXTST/ AND USING MANEUVER SPECIFIED IN
// C  /EXTST/.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  LACID  AC-IDX - Aircraft to fly out.
// C#TECHNICAL DESCRIPTION
// C    A detailed description is beyond a brief in-line documentation,
// C  but is covered in detail in the BRAWLER Programmers Manual.
// C  In brief, aircraft data is retrieved into the work area /svwork/
// C  by the gac call.  The loop from 10 to 100 is the main integration
// C  loop.  A variable time-step is obtained from the getdt routine.
// C  Rates computes actual rotational rates in wind axes, from desired
// C  rates, and computes the lift needed to obtain the actual pitch
// C  rate.  Average rotational rates over the step go into udrbe to
// C  update the aircraft orientation.  Angle of attack is recomputed
// C  from new lift.  New throttle setting and thrust are computed,
// C  leading to an update of position, velocity, and acceleration.
// C#VARIABLE DESCRIPTIONS
// C  TIME     REAL - the simulation time of the current event in seconds
// C  ACC      3-VEC [FT/SEC**2] - Inertial acceleration vector.
// C  ALFA     REAL [RAD] - Angle of attack.
// C  DDPDSV   REAL [NA] - Percentage of drag device deployment.
// C  FABSV    REAL [NA] - Afterburner fraction (0-1) Reflects actual
// C           afterburner state between min ab and full ab.
// C  FGGSV    REAL [NA] - Gas generator fraction (0-1) Reflects actual
// C           engine state between idle and mil power.
// C  GAS      REAL [SLUGS] - Fuel level.
// C  JACTON   INT [NA] - Alternative format.  Comes from /althld/iactn and
// C  KACID    AC-IDX [NA] - Index of current aircraft.
// C  POS      3-VEC [FT] - Inertial position vector.
// C  SPD      REAL [FT/SEC] - Speed.
// C  TRPDSV   REAL [NA] - Percentage of thrust reversal deployment.
// C  TSET     REAL [NA] - Cockpit throttle setting; acutal engine state may
// C           be different due to delays, time constants, etc. (1-3).
// C  TSV      REAL [SEC] - Time of the state vector.
// C  VEL      3-VEC [FT/SEC] - Inertial velocity vector.
// C  WDES     ARRAY DIM(3) OF (REAL) [RAD/SEC] - Desired angular rates.
// C  WNOW     ARRAY DIM(3) OF (REAL) [RAD/SEC] - Current angular rates.
// C  FAXIAL  REAL - Longitudinal force due to thrust, thrust reverser,
// C          and drag devices.  DOES NOT INCLUDES PROPULSION DRAG.
// C  VW      3-VEC - Velocity in wind axes.
// C  AW      3-VEC - Acceleration in wind axes.
// C  T       REAL - Thrust.
// C  XL      REAL - Lift.
// C  FLOW    REAL - Fuel flow.
// C  WATE    REAL - A/C weight.
// C  DELT    REAL - Integration time step.
// C  WNEXT   REAL - Rotational rates at end of time step, wind axes.
// C  D       REAL - Drag.
// C  ALFA    REAL - Angle-Of-Attack.
// C  ACCMX   REAL - Max acceleration along flight path.
// C  AWX     REAL - Actual accel along flight path.
// C  ACCX    REAL - Requested accel along flight path.
// C  TREQ    REAL - Thrust required to get accx.
// C  TSETR   REAL - Requested throttle setting.
// C  TSETMN  REAL - Throttle setting associated with direct maneuver.
// C          If -1<tsetmn<0, then throttle setting is mil power and
// C          std::abs(tsetmn) is treated as fraction of thrust reverser
// C          employed.
// C  TEND    REAL - Time at end of this projection (=/EVTIM/TIME)
// C  SHRTDT  LOG - .true. until the loop at $10 has been traversed at
// C          least once.  If still .true. after $100, this indicates a
// C          case where flact1 has not been called and many local
// C          variables, such as d, flow, faxial and xl are not defined.
// C  SPC_FLY Indicates that some method other than flyac has been used
// C          to update the aircraft state.  Used to determine if the
// C          aircraft trajectory files should be updated since this is
// C          done inside the integration loop 10.
// C####
///////////////////////////////////////////////////////////////////////////////
// flyac() is brawler's main platform update routine (not just aircraft?)
// so it includes other features beyond the aero model that we won't need:
//     data logging
//     special pdu output
//     dead platform processing
//     SAM site processing
//     simulator/confederation checks
//     weapon aero processing
//     terrain collision
//     check if platform is background track
//     saving platform state
//     EW emissions
//     additive signature from drag
// void BrawlerMover::flyac(double toTime, BrawlerPlatform& plat)
void BrawlerMover::flyac(double toTime)
{
   // C  --local declarations
   UtVec3d vw(0.0, 0.0, 0.0);
   UtVec3d gw(0.0, 0.0, 0.0);
   UtVec3d aw(0.0, 0.0, 0.0);
   UtVec3d wnext(0.0, 0.0, 0.0);
   double  accel  = 0.0;
   double  d      = 0.0;
   double  delt   = 0.0;
   double  flow   = 0.0;
   double  spddes = 0.0;
   double  faxial = 0.0;
   double  tend   = 0.0;
   double  wate   = 0.0;
   double  alfnow = 0.0;
   double  aldnow = 0.0;
   double  xl     = 0.0;
   double  efpd   = 0.0;
   double  ftime  = 0.0;
   // double gnow = 0.0;
   double gmax = 0.0;
   // double gmxin = 0.0;
   // C*ENDDEC
   // save_ac = lpac;
   // if (lpac != lacid)
   //{
   //    lpstac(lacid);
   // }
   // hit_ground = false;
   // spc_fly = false;
   // if (simltr == mansim)
   //{
   //    nabort("FLYAC...routine should not be called in simulator mode");
   // }
   // if (progrm == acprog)
   //{
   //    if ((ctrl_state(lacid) == tbmain_in_ctrl) || (ctrl_state(lacid) == waiting_for_xfer))
   //    {
   //       return;   //goto 9990;
   //    }
   // }
   // else
   //{
   //    if (ctrl_state(lacid) == acmain_in_ctrl)
   //    {
   //       flyac_acmain(lacid);
   //       spc_fly = true;
   //       return;   //goto 9990;
   //    }
   // }

   // TODO - no time logging for now
   // if(alltmr)
   //{
   //    tmstrt(63);
   // }

   // if(time == svtime)
   //{
   //    return;  //goto 9990;
   // }
   ////C     --SET STATE VECTOR TIME AND RETURN IF ENTITY IS A SAM SITE
   // if(samsit(lacid))
   //{
   //    svtime(lacid) = time;
   //    spc_fly = true;
   //    return;   //goto 9990;
   // }
   ////C     --TEST FOR DEAD AC
   // if (!alive(lacid))
   //{
   //    flyded(lacid);
   //    spc_fly = true;
   //    return;   //goto 9990;
   // }
   ////C     --ASSERT: AIRCRAFT IS ALIVE
   ////C     --TREAT BACKGROUND TRACKS
   // if (btrack(lacid))
   //{
   //    vecinc(xe(1,lacid),time-svtime(lacid),ve(1,lacid),xe(1,lacid));
   //    spc_fly = true;
   //    return;   //goto 9990;
   // }
   // C     --NEXT LINE REQUIRED FOR SIMULATOR APPLICATIONS SINCE OTHER PARTS
   // C     --OF BRAWLER MAY CHANGE BAREMS AND NOT ACMASS
   amass = barems + gas;
   // LBM - not getting/setting common block data!
   // gac(); //brawlers place where all platform state is copied from global common blocks

   // C     --TEST FOR CONSTANT VELOCITY TARGET
   // if (linear_fly)
   //{
   //    flyconst(lacid);
   //    spc_fly = true;
   //    return;  //goto 9990;
   // }
   // gtypd(lacid,3); //TODO - no mem copies necessary
   ////C     --CHECK FOR SURFACE-TO-SURFACE-MISSILES
   // if (is_ssm(lacid))
   //{
   //    fly_ssm(lacid);
   //    return;   //goto 9990;
   // }

   // LBM - do not flyefficient for now
   ////C     --CHECK FOR BYPASS OF 5-DOF
   // flyefficient(to500);
   // if (to500)
   //{
   //    //not doing EW considerations
   //    //label 500 is only EW stuff, so returning here is fine
   //    return;  //goto 500;
   // }

   // C     --COMPUTE INITIAL VALUES OF FAXIAL, XL, FLOW, EFPD
   // C     --NOTE THAT FAXIAL DOES NOT INCLUDE PROPULSION DRAG
   flacti(faxial, xl, flow, efpd);
   tend = toTime;
   // shrtdt = true;

   do // 10    continue
   {
      // C     --TOP OF INNER LOOP
      if (tsv > (tend - 0.01))
      {
         //         flyac_short(tend - tsv);
         wate = amass * grav;
         //         tsv = tend;
         break; // goto 100; //solved
      }
      // shrtdt = false;
      // C     --THIS SECTION COMPUTES VARIOUS STATE VARIABLES AT BEGINNING OF
      // C     --THE INTEGRATION STEP.  THEY ARE: vw,gw,aw
      // C     --Also, for jacton=1, accreq is defined.
      // flact0(vw,gw,aw,tend,xl);
      flact0(vw, gw, aw);
      // C     --SET REFERENCE AREA
      acarea();
      // C     --GET PRELIMINARY INTEGRATION STEP
      ftime = tend - tsv;
      // getdt(aw,wnow,wdes,ftime,jacton,delt);
      getdt(aw, AngularRates(), ftime, delt);
      // C
      // C     --COMPUTE WIND ANGULAR RATES, AOA, AOA-RATE AT END OF TIME STEP
      // C
      // if (mCommandType != ALSO_COMMAND_ALPHA) //TODO - not supporting command type 9 yet
      {
         // rates(xl,faxial,delt,tend,rarea,lacid,wnext,spddes,vw,gw,aw,arotyp,alfnow,aldnow);
         // rates(ftime, wnext);
         rates(xl, faxial, delt, tend, wnext, spddes, vw, gw, aw, arotyp, alfnow, aldnow);
      }
      // TODO - not supporting command type 9 yet
      // else
      //{
      //    //C        --HERE IS RATES CALL FOR COMMANDED AOA MANUEVER FORMAT
      //    rates9(xl,faxial,delt,rarea,wnext,spddes,vw,gw,arotyp,alfnow,aldnow);
      // }
      alfa   = alfnow;
      alfdot = aldnow;
      // C
      // C     --COMPUTE VALUES AT END OF TIME STEP.
      // C     --NOTE THAT FAXIAL DOES NOT INCLUDE PROPULSION DRAG
      flact1(wnext, d, delt, flow, spddes, faxial, wate, xl, efpd);
      ////C     --EXIT IF A/C HAS HIT GROUND:
      // if (terrain)
      //{
      //    xy_to_geo (pos[0],pos[1],vel[0],vel[1],lat,lon,latdot,londot);
      //    terr_lkup(sngl(lat),sngl(lon),onmap,gnd_ht);
      // }
      // else
      //{
      //    gnd_ht = 0.0;
      // }
      // if (speed[kacid-1] < 0.1 && -xe(3,kacid) < (gnd_ht+1.0))
      //{
      //    hit_ground = true;
      //    return;   //goto 9990
      // }
      tsv = tsv + delt;
      // C     --COMPUTE MINIMUM TURN RADIUS
      gmax = susgmx(wate, 3.0);
      // gnow = (vw[0] * wnext[1] + gw[2]) / grav - (faxial * sin(alfa) / wate); //local - unused
      accel = gmax * grav;               // local
      turn  = Speed() * Speed() / accel; // member
      gmxin = gldmx(wate, 1);            // local - unused
      // if(lprsum)write(ioutp,1810) tsv,xl/wate-1.
      // if (lprtrj2 .and. tsv.ge.0.) {
      //    upd_acf_file(lacid,tsv,pos,vel,acc,rbea,alfa,fm,faxial,d,efpd,wate,flow,turn,gnow,gmax,gmxin,fggsv,fabsv);
      // }

      // if (tsv < (tend-0.001))
      //{
      //    //goto 10;   //solved
      // }
   } while (tsv < (tend - 0.001)); // goto 10;   //solved

   // C
   // 100   continue
   // C     --WRITE SUMMARY FOR NORMAL FLYOUT
   // if(lprsum) {
   //    if(shrtdt) {
   //       //write(ioutp,1801) lacid,time,alfa,fuel,spd,wate
   //    }
   //    else
   //    {
   //       //write(ioutp,1800) lacid,time,d,faxial,xl,alfa,flow,fuel,spd,wate
   //    }
   // }

   // LBM - no getting/setting common blocks!
   // pac(); //brawlers place where all platform state is copied back to global common blocks

   // if (lprtrj && svtime(lacid) >= 0.0)
   //{
   //    if (spc_fly)
   //    {
   //       upd_acf_file(lacid,svtime(lacid),xe(1,lacid),ve(1,lacid),ae(1,lacid),rbe(1,lacid),alpha(lacid),0.,0.,0.,0.,amass(lacid)/grav,0.,1.,1.,1.,1.,1.,0.);
   //    }
   //    else
   //    {
   //       upd_acf_file(lacid,tsv,pos,vel,acc,rbea,alfa,fm,faxial,d,efpd,wate,flow,turn,gnow,gmax,gmxin,fggsv,fabsv);
   //    }
   // }
   // C     --Update RF blackboard listings for continuously emitting
   // C     --avionics devices.
   ////500   continue
   //   if (progrm != acprog)
   //   {
   //      acemt(lacid);
   //   }
   //   if(adhsby(lacid) && adsig(lacid,paero))
   //   {
   //      arosig(lacid);
   //   }
   // 9990  continue
   // if(simltr == confed)
   //{
   //   if (progrm == acprog)
   //   {
   //      //C           --Only send ES PDUs to the ICS write connection.
   //      conn_ptr = conn_ptrs(ics_write_conn);
   //   }
   //   else
   //   {
   //      //C           --Send ES PDUs to all connections.
   //      conn_ptr = 0;
   //   }
   //   if (hit_ground)
   //   {
   //      ent_pdu_deact(conn_ptr,lacid,entity(lacid));
   //   }
   //   else
   //   {
   //      entity_pdu_2(conn_ptr,lacid,entity(lacid));
   //   }
   //}
   // if (save_ac != lpac)
   //{
   //   lpstac(save_ac);
   //}
   // TODO - no time logging for now
   // if(alltmr)
   //{
   //   dummy = tmstop(63);
   //}
}


// C#ABSTRACT COMPUTES ACTUAL ANGULAR RATES AT TIME T+DT
// C#PURPOSE  COMPUTES ACTUAL ANGULAR RATES AT TIME T+DT
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIO  XL     REAL - Lift.
// CIN  THRST  REAL - Thrust.  (THIS IS A GROSS THRUST)
// CIO  DT     REAL - Time step size.
// CIN  TEND   REAL - Time at end of projection (=/EVTIM/TIME)
// CIN  AREA   REAL - Reference area.
// CIN  LACID  AC-IDX - Aircraft index.
// COUT WACT   3-VEC - Angular rates at time t+dt.
// COUT SPDDES REAL - Desired speed.
// CIN  VW     3-VEC - Velocity in wind axes.
// CIN  GW     3-VEC - Gravitational acceleration (ft/sec**2) in wind axes.
// CIN  AW     3-VEC - Acceleration in wind axes.
// CIN  AROTYP REAL - Arodynamics algorithm type.
// COUT AOAACT REAL - AOA at time t+dt.
// COUT ALDACT REAL - AOA rate at time t+dt.
// C#TECHNICAL DESCRIPTION
// C  There are two aerodynamic algorithims in BRAWLER.  The primary
// C  difference between them is their treatment of desired and actual
// C  angular rates.  The first model uses first order lag filters for
// C  both pitch and roll and has "hardwired" pitch and roll rate limits.
// C  This is the orginal BRAWLER algorithim.
// C  The second algorithim is largely data driven.  It uses a first order
// C  lag filter for roll but treats pitch as a second order system.
// C  Limits are imposed on roll and pitch frequencies, requested angular
// C  rates, roll acceleration and angle of attack acceleration.  All
// C  limits are data driven.  Once the roll and pitch are calculated,
// C  slight "turbulence" is added to the rates.  The motivation
// C  for this was originally to avoid certain underflow problems that
// C  occur when an a/c is allowed to settle down too closely to a
// C  specified heading.
// C    Next the yaw rate needed for coordinated flight is calculated,
// C  as is the lift needed to get the pitch rate wact(2) (remember the
// C  rates as defined here are wind-axis rates, so they are dependent
// C  only on forces and velocity, not on body moments).  If this lift
// C  exceeds specified limits the pitch rate is adjusted so that the
// C  lift does not exceed its limit.
// C#VARIABLE DESCRIPTIONS:  none
// C  OMEGP    REAL [RAD/SEC] - Roll rate control frequency.
// C           Roll rate control frequency for the first order lag filter
// C           used to model aircraft roll response.
// C           For aero type 1:
// C              Omegp is computed by the equation omegp = (4/3)*omegq,
// C  OMEGQ    REAL [RAD/SEC] - Pitch rate control frequency.
// C           For aero type 1:
// C            Omegq is computed by the equation omegq =
// C              7.5*dynamic_pressure*wing_ref_area/(speed*mass), limited
// C              to 10 rad/sec.  This is the pitch rate control frequency
// C              for the first order lag filter used to model aircraft
// C              pitch response.
// C           For aero type 2:
// C              The angle of attack rate control frequency looked up
// C              from table /prform/wspmh.  This is the control frequency
// C              used for the first order lag filter used to model aircraft
// C              angle-of-attack response.
// C  PMX      REAL [RAD/SEC] - Max roll rate.
// C  QMXPOS   REAL [RAD/SEC] - Max positive pitch rate.
// C  RATE     REAL [RAD/SEC] - |WNOW|.
// C  WDES     3-VEC OF (REAL) [RAD/SEC] - Desired angular rates.
// C  WNOW     3-VEC OF (REAL) [RAD/SEC] - Current angular rates.
// C####
void BrawlerMover::rates(double&   xl,
                         double    thrst,
                         double&   dt,
                         double    tend,
                         UtVec3d&  wact,
                         double&   spddes,
                         UtVec3d&  vw,
                         UtVec3d&  gw,
                         UtVec3d&  aw,
                         const int arotyp,
                         double&   aoaact,
                         double&   aldact)
{
   // C     --local declarations
   UtVec3d wnext(0.0, 0.0, 0.0);
   double  fac    = 0;
   double  gmax   = 0;
   double  preq   = 0;
   double  qmxneg = 0;
   double  qreq   = 0;
   double  tsalf  = 0;
   double  wate   = 0;
   double  aoades = 0;
   double  mapos  = 0;
   bool    newlft = false;
   bool    gavdfl = false;
   // unused: double maneg;
   // unused: double omgaoa;
   // C*ENDDEC
   // TODO - is this check necessary? (no)
   // if(lacid != kacid) {
   //    nabort("lacid != kacid");
   // }

   if (arotyp == aero1)
   {
      // C        --COMPUTE ROLL AND PITCH FILTER FREQUENCIES OMEGP, OMEGQ
      fac   = DynamicPressure() * ReferenceArea() / (Speed() * amass);
      omegp = amin1(7.5 * fac, 10.0);
      omegq = amin1(4.0 * omegp / 3.0, 10.0);
      // C        --COMPUTE MAXIMUM RATES PMX, QMXPOS, QMXNEG
      ratmx1(xl, amass, vw, gw, alfa, thrst, pmx, qmxpos, qmxneg, mapos);
      // C        --GET REQUESTED RATES AND LIMIT THEM
      gwreq(dt, thrst, tend, aw, wnext, spddes, aoades, gavdfl);
      preq = xlimit(wnext[0], pmx, -pmx);
      qreq = xlimit(wnext[1], qmxpos, qmxneg);
      // C        --NOW COMPUTE FILTERED ACTUAL RATES
      wact[0] = actxdt(AngularRates()[0], preq, omegp, dt);
      wact[1] = actxdt(AngularRates()[1], qreq, omegq, dt);
   }
   // else if (arotyp == aero2)
   //{
   ////TODO - not supporting arotype2 yet, put in later
   //   //C     --COMPUTE ROLL AND AOA FILTER FREQUENCIES
   //   omegp = pfreq(fm,-pos[2]);
   //   omgaoa = afreq(fm,-pos[2]);
   //   omegq = omgaoa;
   //   //C        --COMPUTE MAXIMUM RATES
   //   ratmx2(xl,amass,dyp,fm,alfa,thrst,vw,gw,kacid,pmx,qmxpos,mapos,maneg);
   //   //C        --GET REQUESTED RATES
   //   gwreq(dt,thrst,tend,aw,wnext,spddes,aoades,gavdfl);
   //   //C        --NOW COMPUTE FILTERED ACTUAL RATES
   //   wact[0] = pnow(wnext[0],pmx,-pmx,omegp,dt,thrst,fm,-pos[2],alfa,wnow[0],wdes[0]);
   //   aoanow(aoades,mapos,maneg,omgaoa,thrst,dt,aoaact,aldact);
   //   //C        --GET PITCH RATE FROM AOA
   //   xl = xlift(aoaact,fm,dyp,kacid);
   //   tsalf = thrst*sin(aoaact);
   //   wact[1] = (xl+tsalf-amass*gw[2]) / (amass*spd);
   //}
   else
   {
      nabort("RATES...unknown aerodynamic algorithm");
   }

   // TODO - no turbulence, at least not computed here
   ////C     --ADD TURBULENCE except if TRAP is running
   // if(progrm != trprog) {
   //    wact[0] = wact[0]+1.0e-3*(rnsq(0)-0.5);
   //    wact[1] = wact[1]+1.0e-3*(rnsq(0)-0.5);
   // }
   // C     --WE NOW HAVE ROLL AND PITCH RATES, NOW COMPUTE YAW RATE
   // C     --FOR COORDINATED FLIGHT
   wact[2] = aw[1] / vw[0];
   // C     --COMPUTE LIFT NEEDED FOR DESIRED PITCH RESPONSE
   // C     --note that change in thrust term due to changing alpha is
   // c     --neglected
   tsalf = thrst * sin(alfa);
   xl    = amass * (vw[0] * wact[1] + gw[2]) - tsalf;
   // C     --CHECK LIFT LIMIT AND MODIFY PITCH RATE IF NECESSARY
   wate = amass * grav;
   gmax = gldmx(wate, 1);
   if ((mCommandType == HEADING_SPEED_ALTITUDE || mCommandType == VECTOR_GEES) && !gavdfl)
   {
      gmax = amin1(gmax, mMaxGees);
   }
   if (std::abs(xl / wate) > gmax)
   {
      // C        --HERE REQUIRED LIFT EXEEDS MAX LIFT - MODIFY PITCH RATE
      xl      = wate * sign(gmax, xl);
      wact[1] = (xl + tsalf - amass * gw[2]) / (amass * vw[0]);
      newlft  = true;
   }
   else
   {
      newlft = false;
   }
   if (arotyp == aero1 || newlft)
   {
      // C        --COMPUTE AOA AND AOA RATE
      // TODO - not supporting arotype2 yet, put in later
      // if(arotyp == aero2 && alfa >= aoamxcl(fm,kacid)) {
      //    aoaact = aoa(fm,dyp,xl,kacid,true);
      // } else
      {
         aoaact = aoa(xl, false);
      }
      aldact = (aoaact - alfa) / dt;
   }
   // xmit(3,wnext,wdes);
   // wdes = wnext;
   mAngularRates = wnext;
}


// C#ABSTRACT DETERMINES NEW REQUESTED ANGULAR RATES
// C#PURPOSE  DETERMINES NEW REQUESTED ANGULAR RATES
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  DT     REAL - TIME STEP - MAY BE INCREASED BY GWREQ IN VERY
// C           SLOWLY VARYING SITUATIONS
// CIN  THRST  REAL - CURRENT THRUST
// CIN  TEND   REAL - Time at the end of this projection (=/EVTIM/TIME)
// CIN  AW     3-VEC - Acceleration in wind axis
// COUT WNEXT  3-VEC - DESIRED WIND-AXIS ANGULAR RATES
// COUT SPDDES REAL - DESIRED SPEED
// COUT AOADES REAL - DESIRED ANGLE-OF-ATTACK (RADIANS)
// COUT GAVDFL LOG - .true. if in a ground avoidance situation (as
// C           determined by adjal)
// C#TECHNICAL DESCRIPTION
// C    THE COMMAND MODE 1 DIRECTLY SPECIFIES REQUESTED ANGULAR RATES.
// C  THESE GO DIRECTLY TO WNEXT.
// C
// C    COMMAND MODE 2 (FOR ROUTEPOINT FOLLOWING) USES AS PRIMARY INPUT
// C  A REQUESTED HEADING (HEDREQ), ALTITUDE (ALTREQ), AND HORIZONTAL
// C  SPEED (SPDREQ).  THESE HAVE TO BE CONVERTED INTO A DESIRED
// C  INSTANTANEOUS VELOCITY, WHICH IS IN TURN CONVERTED TO DESIRED
// C  ANGULAR RATES.  AN ATTEMPT IS MADE TO SEPARATE HORIZONTAL AND
// C  VERTICAL MOTION.  FIRST RCDES, A DESIRED RATE-OF-CLIMB SENSITIVE
// C  TO RESPONSE TIMES, IS COMPUTED.  THEN A REQUIRED HORIZONTAL TURN
// C  IS DETERMINED (CDHEAD = COSINE OF REQUIRED TURN). IF THE ANGLE IS
// C  LESS THAN 45 DEGREES THE DESIRED VELOCITY IS SPDREQ ALONG THE
// C  DESIRED HEADING HORIZONTALLY, AND RCDES VERTICALLY.
// C    WHEN THE ANGLE IS GREATER THAN 45 DEGREES AN ACCELERATION VECTOR
// C  IS CONSTRUCTED WITH THE HORIZONTAL COMPONENT NORMAL TO THE CURRENT
// C  VELOCITY IN THE DIRECTION OF THE DESIRED HEADING AND A VERTICAL
// C  ACCELERATION GIVEN BY ((-RCDES)-VEL(3))/5.0 (HERE 5.0 PLAYS THE
// C  ROLE OF A RELAXATION TIME).  THE PART OF THIS PERPENDICULAR TO THE
// C  CURRENT VELOCITY IS COMPUTED AND GIVEN A MAGNITUDE OF SPDDES.
// C  AFTER COMPUTING SOME ANGULAR VARIABLES (DESANGS CALL) PROCESSING
// C  CONTINUES AT LABEL 500 (SEE BELOW).
// C
// C    FOR COMMAND MODE 3 (DESIRED ACCELERATION) LITTLE IS DONE BEFORE
// C  LABEL 500 EXCEPT TO CALL DESANGS
// C
// C    FOR COMMAND MODE 4 SOME "AUTOMATIC" GROUND AVOIDANCE IS ADDED
// C  WHEN ALL OF THE FOLLOWING CONDITIONS ARE MET. (1) ALTITUDE IS
// C  BELOW 2000 FEET, (2) EITHER CURRENT OR REQUESTED VERITICAL
// C  VELOCITY IS DOWNWARD, (3) EITHER BELOW 100 FEET OR THE
// C  ACCELERATION NEEDED TO AVOID THE GROUND EXCEEDS 1 GEE.  WHEN
// C  GROUND AVOIDANCE IS REQUIRED THE GEES NEEDED TO PULL OUT AT 100
// C  FEET IS COMPUTED AND USED HEURISTICALLY TO ADJUST THE DESIRED
// C  DIRECTION VECTOR'S Z-COMPONENT.
// C
// C    AT LABEL 500 COMMAND MODES 2-4 ARE ALL IN AN EFFECTIVE DESIRED
// C  VELOCITY FORM.  THIS, AND THE MAX G'S ARE THEN CONVERTED TO A
// C  DESIRED PITCH AND ROLL RATE.
// C#VARIABLE DESCRIPTIONS:
// C  GREQ     REAL [NA] - GREQ defines max G's to use in manuever for
// C           Manuever formats 2 and 4 (jacton=2,4), or it is the
// C           requested G's for manuever format 3 (jacton=3).  GREQ
// C           is thought if in the sense of transverse acceleration,
// C           in the combined Y and Z directions of the wind-axis
// C           (coordinate system that lift and drag act in).  This
// C           magnitude may be modified for other reasons such as
// C           ground avoidance, or to specify an acceleration floor.
// C  JACTON   INT [NA] - Alternative format.  Comes from /althld/iactn and
// C  PMX      REAL [RAD/SEC] - Max roll rate.
// C  ROCREQ   REAL [FT/SEC] - Max ROC for route maneuvers (jacton=2).
// C  SPD      REAL [FT/SEC] - Speed.
// C SPDRMD    INT [NA] - Mode flag for spdreq, the desired speed. (desspd =
// C           desired speed, thrttl = desired throttle setting)
// C  VEL      3-VEC [FT/SEC] - Inertial velocity vector.
// C  WNOW     ARRAY DIM(3) OF (REAL) [RAD/SEC] - Current angular rates.
// C  OMEGP  REAL - ROLL RATE CONTROL FREQUENCY (RAD/SEC)
// C  OMEGQ  REAL - PITCH RATE CONTROL FREQUENCY (RAD/SEC)
// C  PMX    REAL - MAX ROLL RATE
// C  QMXPOS REAL - MAX POSITIVE PITCH RATE
// C####
//     wnext[3] populated with {roll, pitch, yaw} rates.
void BrawlerMover::gwreq(double&  dt, // Note that it states this can be changed but is labeled as IN only??
                         double   thrst,
                         double   tend,
                         UtVec3d& aw,
                         UtVec3d& wnext,
                         double&  spddes,
                         double&  aoades,
                         bool&    gavdfl)
{
   // C     --local declarations
   UtFortMat3d rot = UtFortMat3d();
   UtVec3d     ver(0.0, 0.0, 0.0);
   UtVec3d     gw(0.0, 0.0, 0.0);
   UtVec3d     vwr(0.0, 0.0, 0.0);
   UtVec3d     toup(0.0, 0.0, 0.0);
   UtVec3d     temp(0.0, 0.0, 0.0);
   UtVec3d     awr(0.0, 0.0, 0.0);
   double      al      = 0;
   double      aly     = 0;
   double      alz     = 0;
   double      alzmax  = 0;
   double      atrans  = 0;
   double      aznext  = 0;
   double      bord    = 0;
   double      calpha  = 0;
   double      cphi    = 0;
   double      dpsidt  = 0;
   double      dtpsi   = 0;
   double      dtrats  = 0;
   double      dtroll  = 0;
   double      gmxpos  = 0;
   double      greqm   = 0;
   double      gsprng  = 0;
   double      omega   = 0;
   double      lift    = 0;
   double      pmxuse  = 0;
   double      psi     = 0;
   double      psidot  = 0;
   double      rcdes   = 0;
   double      rcnow   = 0;
   double      rolreq  = 0;
   double      salpha  = 0;
   double      sphi    = 0;
   double      taup    = 0;
   double      tauq    = 0;
   double      taurc   = 0;
   double      cdhead  = 0;
   double      psi_use = 0;
   // double droll; //unused
   // double pitch; //unused
   // double rollr; //unused
   // double headng; //unused
   // double dpitch; //unused
   // double pitchr; //unused
   // double roll; //unused
   // double rpitch; //unused
   // double rroll; //unused
   // bool save_lprnt; //unused
   bool   low_spd     = false;
   double rollmx      = 0.0873;
   double ptchmx      = 0.0873;
   double taupuz      = 0.0;
   double g_trans_max = 0.0;
   // char stri[10];   //character stri*10

   taup = 1 / omegp;
   tauq = 1 / omegq;
   // C     --/SVWORK/GREQ IS MAX G'S TO USE (FORMAT 2,4) OR REQUESTED ACCELERATION
   // C     --(FORMAT 3).  USE A MIN OF 1.25 TO PRECLUDE PROBLEMS WITH
   // C     --UNACHIEVABLE MANEUVER PLANES BECAUSE LIFT < GRAVITY
   max_asubt(g_trans_max, low_spd);

   if (mCommandType != PLANE_GEES_SPEED)
   {
      if (g_trans_max != 0)
      {
         greqm = amin1(mMaxGees, g_trans_max);
      }
      else
      {
         greqm = mMaxGees;
      }
   }
   else
   {
      // C        --FOR DIRECT FORMAT, AREQTM IS DESIRED TRANSVERSE ACCEL DUE TO LIFT:
      if (g_trans_max != 0)
      {
         // greqm = amin1(areqtm/grav,g_trans_max);
         greqm = amin1(mMaxGees / grav, g_trans_max);
      }
      else
      {
         // greqm = areqtm/grav;
         greqm = mMaxGees / grav;
      }
   }
   // C     --MAKE SURE GREQM IS NOT LESS THEN 1.25 G'S
   greqm = amax1(1.25, greqm);
   // vxfrmc(rwea,unitz,gw,1);
   UtVec3d unitz(0, 0, 1);
   gw = rwea * unitz;
   ckrngi(mCommandType, 1, 4, "mCommandType..ckrngi: maneuver iactn value");

   // this local variable facilitates the replication of goto statements
   int go_to = 500;
   // this switch replicates the goto on line 179 of gwreq.f
   switch (mCommandType)
   {
   case RATES:
   {
      // C     --COMMAND MODE 1: direct request of angular rates
      // xmit(3,wdes,wnext);
      // wnext = wdes;
      wnext = mAngularRates;
      // C     --NEXT LINES ARE AN APPROXIMATION, SINCE CURRENT AOA, ALFA, != AOADES
      lift   = amass * (wnext[2 - 1] * Speed() + grav * gw[3 - 1]) - thrst * sin(alfa);
      aoades = aoa(lift, false);
      // goto 610  //solved
      go_to = 610;
   }
   break;

   case HEADING_SPEED_ALTITUDE:
   {
      // C     --COMMAND MODE 2: Routepoint maneuver
      // C     --do not allow less than +.25g loading; a pilot will not pull
      // C     --negative gee's for a routepoint maneuver.  Remember z-axis down!
      alzmax = -0.25 * grav;
      // C     --COMPUTE DESIRED ROC RCDES.
      rcnow = -vel[3 - 1];
      taurc = amax1(11.8468 * 200 / mMaxClimb, std::abs(mMaxClimb) / (greqm * grav));
      // desired rate of climb
      rcdes = dsxdot(mAltitude, Altitude(), rcnow, mMaxClimb, taurc);
      // C     --NEXT SECTION COMPUTES DESIRED DIRECTION VER

      // cdhead = (hedreq[0]*vel[0]+hedreq[1]*vel[1])/sqrt(pow(vel[0],2)+pow(vel[1],2));
      double hedreq0 = cos(mHeading * UtMath::cRAD_PER_DEG);
      double hedreq1 = sin(mHeading * UtMath::cRAD_PER_DEG);
      cdhead         = (hedreq0 * vel[1 - 1] + hedreq1 * vel[2 - 1]) / sqrt(pow(vel[1 - 1], 2) + pow(vel[2 - 1], 2));
      if (cdhead < 1 / root2)
      {
         // C        --TURN REQUIRED > 45 DEG.  GENERATE VER AT 90 DEG TO CURRENT
         // C        --HEADING, IN PROPER DIRECTION.  LEFT OR RIGHT DETERMINED BY
         // C        --SIGN OF TEMP[1] IN HEADING COORDINATE SYSTEM ROT.
         // makeh(vel,rot);
         rot         = makeh(vel);
         temp[1 - 1] = hedreq0;
         temp[2 - 1] = hedreq1;
         temp[3 - 1] = 0;
         // vxfrmc(rot,temp,temp,1);
         temp       = rot * temp;
         ver[1 - 1] = 0;
         ver[2 - 1] = sign(greqm * grav, temp[2 - 1]);
         ver[3 - 1] = (-rcdes - vel[3 - 1]) / 5.0;
         spddes     = sqrt(pow(mSpeed, 2) + pow(rcdes, 2));
         greqm      = ver.Magnitude() / grav;
         // vxfrmc(rot,ver,ver,2);
         ver = rot.TransposeMultiply(ver);
      }
      else
      {
         // C        --TURN REQUIRED < 45 DEG.
         ver[1 - 1] = mSpeed * hedreq0;
         ver[2 - 1] = mSpeed * hedreq1;
         ver[3 - 1] = -rcdes;
         spddes     = ver.Magnitude();
      }
      // vxfrmc(rwea,ver,vwr,1);
      vwr = rwea * ver;
      desang(vwr, AngularRates(), cphi, sphi, psidot, psi);
      // goto 500  //solved
      go_to = 500;
   }
   break;

   case PLANE_GEES_SPEED:
   {
      // C     --COMMAND MODE 3: Direct maneuver
      // C     --DO NOT ALLOW LESS THAN -2.0G LOADING
      alzmax = +2.0 * grav;
      // chkrng(greqm,1.0,15,":GWREQ - IMPROPER SCALING OF AREQTM DETECTED:");
      // if (greqm > 15.0)
      //{
      //    cout << "Warning :GWREQ() - IMPROPER SCALING OF AREQTM DETECTED:" << endl;
      // }
      // vxfrmc(rwea,accreq,awr,1);
      // awr = rwea * accreq;
      awr = rwea * mAxialAccel;
      desang(awr, AngularRates(), cphi, sphi, psidot, psi);
      atrans = sqrt(pow(awr[2 - 1], 2) + pow(awr[3 - 1], 2));
      // if (! toltst(atrans,areqtm,1.e-4)) {
      if (!toltst(atrans, mMaxGees, 1.e-4))
      {
         std::string error = "Inconsistency between atrans and areqtm.  gwreq...assumption error for direct maneuver";
         nabort(error);
      }
      dpsidt = -atrans / Speed();
      aly    = awr[2 - 1] - gw[2 - 1] * grav;
      alz    = awr[3 - 1] - gw[3 - 1] * grav;
      // goto 510  //solved
      go_to = 510;
   }
   break;

   case VECTOR_GEES:
   {
      // C     --COMMAND MODE 4: Desired vector velocity form, max g's to use in
      // C     --getting there
      // C     --DO NOT ALLOW LESS THAN -2.0 G LOADING
      alzmax = 2.0 * grav;
      greqm  = amax1(1.25, mMaxGees);
      // vxfrmc(rwea,mVelocityVector,vwr,1);
      vwr = rwea * mVelocityVector;
      // C     --ADJUST VWR FOR ALTITUDE LIMITS SPECIFIED THROUGH RULES
      altlim(vwr);
      // C     --SET SPDDES BASED ON IF LOW_SPD .TRUE. AND mThrottleFlag
      if (low_spd)
      {
         // C        --HAVE A LOW_SPD SITUATION
         if (mThrottleFlag)
         {
            // spddes = throtm[kacid-1];
            spddes = throtm;
         }
         else // if (!mThrottleFlag)
         {
            spddes = Speed() + 100;
         }
      }
      else
      {
         spddes = mSpeed;
      }
      desang(vwr, AngularRates(), cphi, sphi, psidot, psi);
      // goto 500  //solved
      go_to = 500;
   }
   break;
   case ALSO_COMMAND_ALPHA:
   {
      ut::log::warning() << "GWREQ() - ALSO COMMAND ALPHA is unsupported.";
      go_to = 500;
   }
   break;
   };

   // no break statements in this switch, each section flows down to the next
   switch (go_to)
   {
   // 500   continue
   case 500:
   {
      // C     --PROCESSING FOR MODES 2 & 4 COMMON HERE
      // C     --PICK OMEGA SO THAT MAX OF 3 G'S OR GREQM IS REQUIRED FOR A 22.5 DEGREE
      // C     --HEADING CHANGE
      gsprng = amax1(3.0, greqm);
      omega  = sqrt(4 * gsprng * grav / (pi * Speed() * tauq));
      // C     --GET DESIRED RATE ON PSIDOT AND RESULTING ACCELERATIONS
      // C     --Next block reduces urgency to correct small errors:
      dpsidt = -2 * omega * (psidot + omega * psi) * tauq + psidot;
      atrans = -dpsidt * Speed();
      if (std::abs(psi) > 5.0 * rad)
      {
         psi_use = psi;
      }
      else
      {
         psi_use = psi * psi / (5.0 * rad);
         gsprng  = 2.0;
         omega   = sqrt(4 * gsprng * grav / (pi * Speed() * tauq));
         dpsidt  = -2 * omega * (psidot + omega * psi_use) * tauq + psidot;
         atrans  = -dpsidt * Speed();
      }
      alz = atrans * cphi - gw[3 - 1] * grav;
      aly = atrans * sphi - gw[2 - 1] * grav;
      al  = sqrt(pow(aly, 2) + pow(alz, 2));
      // C     --AL IS LOAD-INDUCED ACCELERATION. CHECK THAT IT DOESN'T EXCEED GREQM*GRAV
      if (al > greqm * grav)
      {
         // C        --HERE EXCEEDS LIMIT. SPECIFY MAX LOAD IN DESIRED MNVR PLANE INSTEAD
         salpha = (gw[3 - 1] * sphi - gw[2 - 1] * cphi) / greqm;
         calpha = sqrt(1 - pow(salpha, 2));
         al     = greqm * grav;
         alz    = al * (cphi * calpha - sphi * salpha);
         aly    = al * (sphi * calpha + cphi * salpha);
      }
   }

   // 510   continue
   case 510:
   {
      // C     --MODE mCommandType = 3 JOINS IN HERE
      // C     --MODIFY ALY, ALZ FOR GROUND AVOIDANCE
      adjal(aly, alz, al, greqm, toup, gavdfl);
      // C     --COMPUTE DESIRED ROLL RATE
      rolreq = atan2(aly, -alz);
      // C     --CHECK FOR USER-SPECIFIED ROLL LIMITATIONS
      rollim(gw, rolreq, aly, alz, psi);
      // C     --RECALCULATE DPSIDT IN CASE ALY, ALZ MODIFIED BY MAX GEES OR
      // C     --ROLLIM.  Note that rollim should not override adjal in an
      // C     --important way because the requested upwards acceleration from
      // C     --adjal would not imply a roll.
      atrans = sqrt(pow((aly + gw[2 - 1] * grav), 2) + pow((alz + gw[3 - 1] * grav), 2));
      dpsidt = -atrans / Speed();
      // C     --ADJUST MAX ROLL RATE (PMX) FOR URGENCY:
      if (std::abs(rolreq) < 1)
      {
         // C        --FOR SMALL ROLREQ (LESS THAN 1 RADIAN) URGENCY DEPENDS ON
         // C        --DPSIDT AND GREQM
         bord   = std::abs(dpsidt) - 15 * rad;
         bord   = border(bord, 5 * rad);
         pmxuse = pmx * amax1(std::abs(rolreq), 0.75 + 0.25 * bord);
         gmxpos = gw[3 - 1] + qmxpos * Speed() / grav;
         pmxuse = amin1(pmxuse, pmx * (greqm / gmxpos));
         taupuz = 2 * taup;
      }
      else
      {
         pmxuse = pmx;
         taupuz = taup;
      }
      if (mCommandType == HEADING_SPEED_ALTITUDE)
      {
         wnext[1 - 1] = dsxdot(rolreq, 0, AngularRates()[1 - 1], pmxuse, taup + taup);
      }
      else // mCommandType == 3 || mCommandType == 4
      {
         wnext[1 - 1] = dsxdot(rolreq, 0, AngularRates()[1 - 1], pmxuse, taupuz);
      }
      // C     --GET DESIRED PITCH RATE FROM COMPONENT OF PSIDOT
      alz    = amin1(alz, alzmax);
      aznext = alz + gw[3 - 1] * grav;
      if (gavdfl)
      {
         if (std::abs(aznext) <= 1.5 * grav)
         {
            aznext = sign(1.5 * grav, toup[3 - 1]);
            alz    = aznext - gw[3 - 1] * grav;
         }
      }
      wnext[2 - 1] = -aznext / Speed() + 1.e-10;
      wnext[3 - 1] = 0;
      // C     --DETERMINE THE AOA ASSOCIATED WITH THE REQUIRED LIFT.
      aoades = aoa(-alz * amass - thrst * sin(alfa), false);
      // C     --ENSURE DPSIDT IS NOT IDENTICALLY ZERO
      if (dpsidt == 0)
      {
         dpsidt = sign(1.0e-07, -psi);
      }
      // C     --CHECK TIME STEPS
      dtroll = stept(rolreq, AngularRates()[1 - 1], rollmx, wnext[1 - 1], taup);
      dtpsi  = stept(-psi, psidot, ptchmx, dpsidt, tauq);
      dtrats = amin1(dtroll, dtpsi);
      if (dt > dtrats && dtrats != 0)
      {
         dt = dtrats;
      }
   }

   // 610   continue
   case 610:
   default:
   {
      // C     --COMMAND MODE 1 (commanded rates) JOINS IN HERE
      // NOTE - no debug output printing or logging: empty function
   }
   };
}


// C#ABSTRACT Computes values at the end of an aero time step.
// C#PURPOSE  Called by flyac after final angular rates and AOA are
// C  determined to compute the rest of the state variables.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  WNEXT  REAL - Rotational rates at end of time step, wind axes.
// COUT D      REAL - Drag.  DOES NOT INCLUDE PROPULSION DRAG.
// CIN  DELT   REAL - Integration time step.
// CIO  FLOW   REAL - Fuel flow.
// CIN  SPDDES REAL - Desired speed.  Defined unless jacton=3.
// COUT FAXIAL REAL - Longitudinal force due to thrust, thrust reverser,
// C           and drag devices. (DOES NOT INCLUDE PROPULSION DRAG)
// COUT WATE   REAL - A/C weight.
// COUT XL     REAL - Lift.
// CIO  EFPD   REAL - Effective propulsion drag
// C#TECHNICAL DESCRIPTION
// C  Average rotational rates over the step go into udrbe to
// C  update the aircraft orientation.  Angle of attack is recomputed
// C  from new lift.  New throttle setting and thrust are computed,
// C  leading to an update of position, velocity, and acceleration.
// C#VARIABLE DESCRIPTIONS
// C  ALFA     REAL [RAD] - Angle of attack.
// C  GAS      REAL [SLUGS] - Fuel level.
// C  KACID    AC-IDX [NA] - Index of current aircraft.
// C  TSV      REAL [SEC] - Time of the state vector.
// C  WNOW     ARRAY DIM(3) OF (REAL) [RAD/SEC] - Current angular rates.
// C  CHI     REAL - Angle a/c rotates thru in time delt.
// C  ALFA    REAL - Angle-Of-Attack.
// C  FW      3-VEC - Forces in wind-axes, excluding gravity.
// C####
// updates the orientation matrices (based on angular rates & alpha)
//     rwea - earth to wind rotation matrix
//     rbea - earth to body rotation matrix
// calls seteng() - determines engine state
// calls effthr() - calculates forces due to thrust
// updates platform position, velocity, etc
// updates fuel
void BrawlerMover::flact1(UtVec3d& wnext,
                          double&  d /*drag*/,
                          double   delt,
                          double&  flow,
                          double   spddes,
                          double&  faxial,
                          double&  wate,
                          double&  xl,
                          double&  efpd)
{
   // c  --local declarations
   UtVec3d accbar(0.0, 0.0, 0.0);
   UtVec3d fw(0.0, 0.0, 0.0);
   UtVec3d f(0.0, 0.0, 0.0);
   UtVec3d accf(0.0, 0.0, 0.0);
   UtVec3d wavg(0.0, 0.0, 0.0);
   UtVec3d oldpos(0.0, 0.0, 0.0);
   double  acctrb /*,rmanuv[3][3]*/;
   double  chi  = 0.0;
   double  rmag = 0;
   double  rsqr = 0;
   // double gnd_ht = 0;
   /*int i,kr;*/
   /*double lat, lon, latdot, londot;*/
   // bool /*onmap, */clear_los = false;
   // C*ENDDEC
   // C     --COMPUTE VALUES AT END OF TIME STEP.
   // C     --SOME INTEGRATIONS ASSUME AVERAGES OF BEGINNING AND FINAL VALUES
   // C     --(E.G. ANGULAR RATES)
   // C     --RECOMPUTE LIFT BASED ON FINAL AOA, (BUT USING INITIAL MACH, DYP)
   xl = Lift(); // xlift(alfa,fm,dyp,kacid);

   // C     --UPDATE WIND AXES USING AVERAGE ANGULAR RATES OVER INTERVAL:
   for (int kr = 0; kr < 3; ++kr) // do 40 kr=1,3
   {
      wavg[kr] = 0.5 * (AngularRates()[kr] + wnext[kr]);
   } // 40    continue

   rsqr = wavg.DotProduct(wavg);
   rmag = sqrt(rsqr);
   chi  = rmag * delt;
   udrbe(rwea, wavg, rsqr, rmag, chi);
   // C     --UPDATE CURRENT ANGULAR RATES WNOW FOR END OF TIME STEP:
   wsqr = wnext.DotProduct(wnext);
   rate = sqrt(wsqr);
   // xmit(3,wnext,wnow);
   wnow = wnext; // AngularRates()
   d    = drag2(alfa);
   rbea = bodyax(rwea, alfa);
   // C     --SET CURRENT ENGINE STATE
   seteng(delt, d, spddes, mThrottle);
   // C     --COMPUTE NEW AERO FORCES IN WIND AXES AND EARTH COORDINATES:
   if (gas < 0.0)
   {
      // C        --ONLY PASS THROUGH HERE ONCE BY SETTING GAS TO ZERO
      gas    = 0.0;
      flow   = 0.0;
      faxial = 0.0;
      efpd   = 0.0;
   }
   else if (gas > 0.0)
   {
      effthr(fggsv, fabsv, trpdsv, ddpdsv, faxial, efpd);
   }
   else
   {
      faxial = 0.0;
      efpd   = 0.0;
   }
   fw[0] = faxial * cos(alfa) - d - efpd;
   fw[1] = 0.0;
   fw[2] = -faxial * sin(alfa) - xl;
   // C     --COMPUTE INERTIAL ACCELERATIONS AT NEW ORIENTATION
   // vxfrmc(rwea,f,fw,2);
   f = rwea.TransposeMultiply(fw);

   // C     --CONVERT FORCE TO ACCEL, AND ADD SMALL TURBULENCE TO PREVENT
   // C     --UNDERFLOWS LATER
   for (int i = 0; i < 3; ++i) // do 50 i=1,3
   {
      // if(progrm == trprog)
      //{
      //    accf[i]=f[i]/amass;
      // }
      // else
      {
         // LBM - do turbulence later
         // acctrb = (rnsq(0)-0.5)*0.1;
         acctrb  = 0.0; // LBM - placeholder
         accf[i] = f[i] / amass + acctrb;
      }
   } // 50    continue

   accf[2] = accf[2] + grav;
   // C     --UPDATE POSITION, VELOCITY AND ACCELERATION
   // veclin(0.5,acc,0.5,accf,accbar);  // veclin(a,aa,b,bb,cc) == cc(i) = a*aa(i)+b*bb(i)
   accbar = 0.5 * acc + 0.5 * accf;

   // xmit(3,accf,acc);
   acc = accf;
   // C     --UDXV UPDATES X,V.  V IS FORCED TO ALIGN WITH RWEQ X-AXIS
   oldpos = pos;
   udxv(accbar, pos, vel, delt, rwea);

   ////C     --TEST FOR GROUND COLLISION:
   // if (terrain)
   //{
   //    xy_to_geo(pos[0],pos[1],vel[0],vel[1],lat,lon,latdot,londot);
   //    terr_lkup(sngl(lat),sngl(lon),onmap,gnd_ht);
   //    clear_los = clr_los(oldpos,pos);
   // }
   // else
   //{
   //    gnd_ht = 0.0;
   //    clear_los = true;
   // }

   // if(-pos[2] <= gnd_ht || !clear_los)
   //{
   //    //C        --TREATMENT FOR AC CRASHING INTO GROUND WHILE ALIVE:
   //    xmit(2,pos,xe[kacid-1][0]);
   //    xe[kacid-1][2] = -gnd_ht;
   //    xmit(-3,0.0,ve[kacid-1][0]);
   //    speed[kacid-1] = 0.0;
   //    svtime[kacid-1] = time;
   //    killac(kacid,kacid);
   //    return;
   // }
   // C     --UPDATE OTHER SV PARAMETERS

   UpdateFuel(delt, flow, tset, gas, amass);


   spd  = vel.Magnitude();
   wate = amass * grav;
   getqm(Speed(), dyp, fm);
   // C
   // 1300  format(' FLACT1...COMPUTED ALFA,DRAG',2(2x,e12.6))
   // 6000  format(' AC ',i2,' OUT OF GAS AT TIME',f8.3)
   // 6001  format(' AC ',i2,' IMPACTED GROUND.  FUEL LEVEL =',f8.1)
   // 6004  format(' FLACT1...undefined region of fuel flow ',
   // 1       'table accessed (flow2).  About to abort: ',
   // 2       'a/c,Mach,alt,throttle ',I10,F8.3,F11.1,F8.3)
   // 6005  format(1x,'FLACT1...after rates, new values: ',/t10,'LIFT',
   // 1 t20,F12.5,t40,'DRAG ',t50,F12.5,/t10,'WNOW',t20,F12.5,
   // 2 t40,F12.5,T60,F12.5)
   // end
}


// C#ABSTRACT Determines engine state during a/c flyout
// C#PURPOSE  Determines engine state during a/c flyout,sets variables in
// C  /svwork/ which describe the engine state. (ie. fggsv, fabsv, tset
// C  trpdsv, ttset)
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  DELT   REAL - Integration time step.
// CIN  DRG    REAL - Drag.  DOES NOT INCLUDE PROPULSION DRAG.
// CIN  SPDDES REAL - Desired speed.
// CIN  TSMN   REAL - Throttle setting if current maneuver is direct
// C           maneuver or if manuever is desired velocity and a throttle
// C           setting has been specified..
// C#TECHNICAL DESCRIPTION
// C  If the aircraft is out of fuel, the engine state is set to reflect
// C zero thrust and returns.  Otherwise, desaxf is called to
// C calculate the desired axial force based on maneuver specified.
// C DESENG is then called to calculate the desired engine state based
// C on the desired axial force.  STENST is then called to set the engine
// C state based on the desired settings calculated by deseng.
// C  For each maneuver, either the desired speed or the throttle setting
// C will be meaningful, but not both.
// C#VARIABLE DESCRIPTIONS
// C  THROTM   ARRAY DIM(MAC) OF (REAL) - Maximum allowed throttle setting
// C           for each aircraft
// C  KACID    AC-IDX [NA] - Index of current aircraft.
// C KACID  AC-IDX - Aircraft current in /SVWORK/
// C TSET   REAL - Cockpit throttle setting
// C TTSET  REAL - If non-zero, time since afterburner delay entered.
// C####
// calls desaxf() - finds required force for desired accleration (interprets maneuver commands)
// calls deseng() - finds engine settings given required force (thrust)
// limits engine settings inside of possible values
// calls stenst() - sets engine state
void BrawlerMover::seteng(double delt, double drg, double spddes, double tsmn)
{
   // c  --local declarations
   double axfreq = 0;
   double fggdes = 0;
   double fabdes = 0;
   double ddpdes = 0;
   double trpdes = 0;
   double fabmax = 0;
   double fggmax = 0;
   // C*ENDDEC
   if (gas <= 0.0)
   {
      // C        --OUT OF GAS, SHUT DOWN ENGINE
      // C        --SET TSET TO BE CONSISTENT WITH FGGSV AND FABSV
      tset   = 1.0;
      ttset  = 0.0;
      fggsv  = 0.0;
      fabsv  = 0.0;
      trpdsv = 0.0;
      ddpdsv = 0.0;
      return;
   }
   // C     --FIND DESIRED ENGINE STATE
   // C     --Compute desired axial force, along the wind x-axis.  This
   // c     --includes all propulsion effects, such as gross_thrust * cos (AOA),
   // c     --drag devices and thrust reversers, and propulsion drag.
   double accelXaxis; // unused in this scope, but highly useful
   desaxf(drg, spddes, tsmn, axfreq, accelXaxis);
   // C     --GET DESIRED ENGINE SETTINGS BASED ON THIS FORCE
   deseng(axfreq, fggdes, fabdes, ddpdes, trpdes);
   // C     --LIMIT ENGINE FRACTIONS TO WHAT IS ACHIEVABLE
   // fabmax = throtm[kacid-1]-2.0;
   fabmax = throtm - 2.0;
   fabdes = amin1(fabdes, fabmax);
   fabdes = amax1(0.0, fabdes);
   // fggmax = throtm[kacid-1]-1.0;
   fggmax = throtm - 1.0;
   fggdes = amin1(fggdes, fggmax);
   fggdes = amax1(0.0, fggdes);
   // C     --SET ACTUAL STATE OF ENGINE
   stenst(fggdes, fabdes, ddpdes, trpdes, delt);
}


// C#ABSTRACT COMPUTES DESIRED AXIAL FORCE FOR EACH MANEUVER TYPE.
// C#PURPOSE  USED DURING A FLY EVENT TO COMPUTE A DESIRED AXIAL
// C   FORCE FROM A MANEUVER SPECIFICATION.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  DRG    REAL - Drag (not including propulsion drag).
// CIN  SPDDES REAL - Desired speed.  Used only for jacton=2 (route
// C           maneuver), jacton=4 (desired velocity and spdmod=desspd),
// C           and jacton=9
// C           (commanded aoa)
// CIN  TSMN   REAL - Throttle setting.  Used only for jacton=1
// C           (desired roll/pitch rates and spdmod=thrttl), jacton=3 (direct
// C           maneuver), and jacton=4(desired velocity and spdmod=thrttl)
// COUT AXFREQ REAL - Desired axial force based on maneuver specified.
// C           Includes gross thrust and (potentially) thrust reverser,
// C           drag devices, AND propulsion drag.  That is, the net wind
// C           x-axis force associated with propulsion forces is computed.
// C#TECHNICAL DESCRIPTION
// C For each maneuver type (jacton value), a longitudinal acceleration
// C is calculated (ACCX). If the aircraft is currently exceeding
// C aerodynamic limits on speed, ACCX is adjusted to reduce the speed
// C to the limiting value.  This longitudinal accleration is then
// C used to calculate a desired axial force.
// C#VARIABLE DESCRIPTIONS: none
// C 6654 TRMLA    REAL [FT/SEC**2] - Minimum (signed) longitudinal
// C               acceleration  (excluding that due to gravity) that the
// C               aircraft can tolerate. If exceeded, thrust reversal is
// C               reduced. A zero value conventionally indicates no thrust
// C               reversal capablity.  If non-zero, this should be a negative
// C               number.
// C  AMASS    REAL [SLUGS] - Aircraft mass.
// C  SPD      REAL [FT/SEC] - Speed.
// C####
// computes required acceleration (along X axis) to fulfill the current maneuver command
// calculates required force to achieve that acceleration
void BrawlerMover::desaxf(double drg, double spddes, double tsmn, double& axfreq, double& accx)
{
   // c  --local declarations
   UtVec3d gw(0.0, 0.0, 0.0);
   double  gwx    = 0;
   double  calf   = 0;
   double  faxial = 0;
   double  taua   = 0;
   // double accx = 0;
   double accmx  = 0;
   double excess = 0;
   double casnow = 0;
   double efpd   = 0;

   // C     --GET LONG. COMPONENT OF GRAVITY IN WIND SYSTEM
   // vxfrmc(rwea,gravec,gw,1);
   gw   = rwea * gravec;
   gwx  = gw[0];
   calf = cos(alfa);
   if (mCommandType == RATES) // && (!mThrottleFlag)) {  //LBM - commanding rates never controls throttle
   {
      // C        --DESIRED ANG. RATES and LONG. ACCEL.
      // accx = accreq[0];
      accx = mAxialAccel[0];
      // C        --These are needed for later lprnts:
      efpd   = 0.0;
      faxial = 0.0;
   }
   else if (mCommandType == HEADING_SPEED_ALTITUDE || (mCommandType == VECTOR_GEES && !mThrottleFlag) ||
            mCommandType == ALSO_COMMAND_ALPHA)
   {
      // C        --ROUTE MANEUVER (JACTON=2)
      // C        --DESIRED VECTOR (JACTON=4) GIVEN A DESIRED SPEED
      // C        -- ALSO COMMANDED AOA (JACTON=9)
      effth2(3.0, faxial, efpd);
      accmx = gwx + (faxial * calf - drg - efpd) / amass;
      taua  = 3.0;
      accx  = (spddes - Speed()) / taua;
      if (accx > accmx)
      {
         accx = accmx;
      }
   }
   else if (mCommandType == PLANE_GEES_SPEED ||
            (mCommandType == VECTOR_GEES && mThrottleFlag)) //|| (mCommandType == RATES && mThrottleFlag)) { //LBM -
                                                            // commanding rates never controls throttle or speed
   {
      // C        --DIRECT MANEUVER (JACTON=3): THRUST IS SENSITIVE TO
      // C        -- NEGATIVE THROTTLE SETTINGS FOR THRUST REVERSAL
      effth2(tsmn, faxial, efpd);
      accx = gwx + (faxial * calf - drg - efpd) / amass;
   }
   else
   {
      nabort("DESAXF..unknown jacton value"); // stri(jacton))
   }

   if (Mach() >= fmmax)
   {
      // C        --LIMIT ACCX IF FMMAX EXCEEDED:
      excess = Speed() * (fmmax - Mach()) / Mach();
      accx   = excess;
   }
   else
   {
      // C        --LIMIT ACCX IF CASMAX EXCEEDED:
      casnow = cas(Speed());
      if (casnow >= casmax)
      {
         excess = Speed() * (casmax - casnow) / casnow;
         accx   = excess;
      }
   }
   // C     --LIMIT ACCX IF MAX ALLOWABLE LONG DECEL LIMIT EXCEEDED:
   if (trefgg > 0.0 && tdlim && (accx - gwx) < trmla)
   {
      accx = trmla + gwx;
   }
   // C     --REQUIRED FORCE
   axfreq = drg + amass * (accx - gwx);
}


// C#ABSTRACT Computes some aero parameters at start of flyac
// C#PURPOSE  Part of flyac.  Called before the main flyac loop
// C  to compute aero parameters needed by rates, and which are not
// C  computed by flact0 because flact1 also computes them.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// COUT FAXIAL REAL - Longitudinal force due to thrust, thrust reverser,
// C           and drag devices.  DOES NOT INLCUDE PROPULSION DRAG.
// COUT XL     REAL - Lift.
// COUT FLOW   REAL - Fuel flow.
// COUT EFPD   REAL - Propulsion drag
// C#TECHNICAL DESCRIPTION
// C  Computes using basic formulas or by calling utility subroutines to
// C  perform the indicated computations.
// C#VARIABLE DESCRIPTIONS
// C####
void BrawlerMover::flacti(double& faxial, double& xl, double& flow, double& efpd)
{
   // c  --external declarations
   // int rspace;  //function?
   // double xmag;
   // C*ENDDEC
   // c     --get current thrust,lift,and fuel flow
   xl = Lift(); // xlift(alfa,fm,dyp,kacid);

   if (gas > 0.0)
   {
      // flow = fflo(fm,-pos[2],tset,kacid);
      // flow = fflo(Mach(), Altitude(), tset, kacid);
      flow = fflo(tset);
      if (flow == undnum)
      {
         // c           --fuel flow undefined here
         // write(outstr,6003) kacid,fm,-pos(3),tset
         // nabort(outstr(1:rspace(outstr)));
         nabort("flacti() - fuel flow undefined");
      }
      effthr(fggsv, fabsv, trpdsv, ddpdsv, faxial, efpd);
   }
   else
   {
      flow   = 0.0;
      faxial = 0.0;
      efpd   = 0.0;
   }
   // TODO - not supporting command type 9 yet
   ////C     --if commanded AOA maneuver, set up rtrack, wdmag, and xte
   // if (mCommandType == 9)
   //{
   //    xte(1) = cos(eltrac);
   //    xte(2) = -xte(1)*cos(aztrac);
   //    xte(1) = xte(1)*sin(aztrac);
   //    xte(3) = sin(-eltrac);
   //    makecs(xte,wdes,rtrack);
   //    wdmag = xmag(wdes);
   // }
}


// see comments for flyac()
void BrawlerMover::flyac_short(double dt)
{
   // double xmag;
   double hdt       = 0;
   double pitchrate = 0;
   // c*enddec
   if (dt == 0.0)
   {
      return;
   }
   udrbe(rwea, AngularRates(), wsqr, rate, rate * dt);
   pitchrate = AngularRates()[1];
   alfa      = alfa + pitchrate * dt;
   rbea      = bodyax(rwea, alfa);
   hdt       = 0.5 * dt;
   pos[0]    = pos[0] + dt * (vel[0] + hdt * acc[0]);
   pos[1]    = pos[1] + dt * (vel[1] + hdt * acc[1]);
   pos[2]    = pos[2] + dt * (vel[2] + hdt * acc[2]);
   vel[0]    = vel[0] + dt * acc[0];
   vel[1]    = vel[1] + dt * acc[1];
   vel[2]    = vel[2] + dt * acc[2];
   spd       = vel.Magnitude();
   // C     --Re-orient vel to align with x-axis of rwea
   // vel[0] = Speed()*rwea[0][0];//(1,1);
   // vel[1] = Speed()*rwea[1][0];//(1,2);
   // vel[2] = Speed()*rwea[2][0];//(1,3);
   vel[0] = Speed() * rwea[0]; //(1,1);
   vel[1] = Speed() * rwea[3]; //(1,2);
   vel[2] = Speed() * rwea[6]; //(1,3);
}


// C#ABSTRACT Computes some aero parameters at start of time step
// C#PURPOSE  Part of flyac.  Called at the top of each integration step
// C  loop to compute aero parameters needed to determine values at the
// C  end of the current integration step.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// COUT VW     3-VEC - Velocity in wind axes.
// COUT GW     3-VEC - Gravity in wind axes (ft/sec^2)
// COUT AW     3-VEC - Acceleration in wind axes.
// CIN  TEND   REAL - Time at end of this projection (=/EVTIM/TIME)
// CIN  XL     REAL - Lift.
// C#TECHNICAL DESCRIPTION
// C  Computes using basic formulas or by calling utility subroutines to
// C  perform the indicated computations.
// C#VARIABLE DESCRIPTIONS
// C  RBEA     ORIENT [NA] - Earth-to-body rotation matrix.
// C  RWEA     ORIENT [NA] - Earth-to-wind rotation matrix.
// C####
// compute velocity, gravity, acceleration in wind axis
void BrawlerMover::flact0(UtVec3d& vw, UtVec3d& gw, UtVec3d& aw)
{
   // vxfrmc(EarthToWindRotationMatrix(), Velocity(), vw, 1);
   vw = EarthToWindRotationMatrix() * Velocity();
   // vxfrmc(EarthToWindRotationMatrix(), gravec, gw, 1);
   gw = EarthToWindRotationMatrix() * gravec;
   // vxfrmc(EarthToWindRotationMatrix(), Acceleration(), aw, 1);
   aw = EarthToWindRotationMatrix() * Acceleration();
   if (mCommandType == PLANE_GEES_SPEED)
   {
      // c        --construct current desired transverse acceleration, accreq:
      UtFortMat3d rmanuv = UtFortMat3d();
      // makecs(Velocity(), mPlane.GetData(), rmanuv);   //areqt = mPlane
      rmanuv         = makecs(Velocity(), mPlane); // areqt = mPlane
      mAxialAccel[0] = 0.0;
      mAxialAccel[1] = mMaxGees; // areqtm = mMaxGees
      mAxialAccel[2] = 0.0;
      // vxfrmc(rmanuv, mAxialAccel, mAxialAccel, 2);
      mAxialAccel = rmanuv.TransposeMultiply(mAxialAccel);
   }
}


void BrawlerMover::acarea()
{
   // LBM - not using different wing sweep configurations
   rarea = rfarea[0]; // UNSWEPT

   // int isweep;
   ////double acrefa;
   ////C*ENDDEC
   ////gtypd(iac,2);  //TODO - not necessary
   // if(sweep0 == 0) {
   //    //C        --PILOT NOT CONTROLLING SWEEP
   //    if(Mach() < sweep_machs[0]) {
   //       isweep = 1;
   //    } else if(Mach() < sweep_machs[1]) {
   //       isweep = 2;
   //    } else {
   //       isweep = 3;
   //    }
   //    //C        --HAS SWEEP CHANGED
   //    if(wing_sweep != isweep) {
   //       //C           --ASSUME /PRFORM/ IS CURRENT
   //       gsweepc(isweep);  //TODO - I think this is an ERROR - it doesn't update the proper place of memory to
   //       acquire the rfarea[] from PRFORM common block
   //       //C           --UPDATE /SVWORK/
   //       rarea = rfarea[isweep-1];
   //       //wsweep = isweep;
   //       //C           --UPDATE /EXTST/
   //       //area = rarea;
   //       wing_sweep = isweep;
   //    }
   // }
}


// C#ABSTRACT DETERMINES AERO INTEGRATION TIME STEP SIZE
// C#PURPOSE  DETERMINES AERO INTEGRATION TIME STEP SIZE.  A VARIABLE
// C  TIME STEP IS USED TO AVOID MANY INTEGRATION STEPS WHEN THE
// C  A/C IS IN A MORE-OR-LESS STEADY STATE.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  AW     3-VEC - ACCELERATION IN WIND AXES
// CIN  WNOW   3-VEC - CURRENT ANGULAR RATES (BODY AXES)
// CIN  FTIM   REAL - DIFFERENCE BETWEEN FINAL UPDATE TIME AND CURRENT
// C           TIME
// CIN  JACTON INTEGER - MANEUVER TYPE
// COUT DT     REAL - INTEGRATION TIME STEP
// C#TECHNICAL DESCRIPTION
// C  Uses limits on angle rolled through, angle pitched through, and
// C  speed change, in any time step.
// C  Furthermore, a minimum of 0.1 seconds is imposed.
// C  Finally, a maximum of the remaining time to the upper limit of the
// C  time integral is imposed.
// C#VARIABLE DESCRIPTIONS:  none
// C####
void BrawlerMover::getdt(UtVec3d& aw, UtVec3d& wnow, double ftim, double& dt)
{
   double rollmx = 20.0 * rad;
   double ptchmx = 5.0 * rad;
   double dspdmx = 25.0;
   double dtmax  = 0;
   double spdt   = 0;
   double rollt  = 0;
   double ptcht  = 0;
   double dtlim  = 0;
   int    nsteps = 0;
   // C*ENDDEC
   // C     --SET DTMAX = 0.1 FOR COMMANDED AOA MANEUVERS, ALL OTHERS = 1.0
   // if (mCommandType != ALSO_COMMAND_ALPHA) {  //TODO - not supporting command type 9 yet
   dtmax = 1.0;
   //} else {
   //   dtmax = 0.1;
   //}
   // C     --dt should be no longer than the time required to roll, pitch, or
   // C     --accelerate by rollmx, ptchmx, dspdmx, respectively.  It also
   // C     --precludes a step > dtmax:
   rollt = rollmx / (std::abs(wnow[0]) + eps);
   dtlim = rollt;
   ptcht = ptchmx / (std::abs(wnow[1]) + eps);
   if (ptcht < dtlim)
   {
      dtlim = ptcht;
   }
   spdt = dspdmx / (std::abs(aw[0]) + eps);
   if (spdt < dtlim)
   {
      dtlim = spdt;
   }
   if (dtmax < dtlim)
   {
      dtlim = dtmax;
   }
   // C     --Above code, aside from diagnostic info, is equivalent to this line:
   // C**   dtlim = amin1(rollt, ptcht, spdt, dtmax)
   // C     --This code regularizes dt to even steps over the remaining
   // C     --integration time.  It may reduce dt from dtlim, but won't increase it.
   nsteps = std::max(int(ftim / dtlim + 0.9999), 1);
   dt     = ftim / nsteps;
}


// C#ABSTRACT COMPUTES MAXIMUM ANGULAR RATES FOR AERO ALGORITHM 1
// C#PURPOSE  USED DURING AN AIRCRAFT FLY EVENT TO COMPUTE
// C  MAXIMUM ROLL AND PITCH RATES FOR AERO ALGORITHM 1.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  XL       REAL - Lift.
// CIN  ACMASS   REAL - Aircraft mass.
// CIN  AREA     REAL - Aircraft reference area.
// CIN  DYPRES   REAL - Dynamic pressure.
// CIN  MACH     REAL - Mach.
// CIN  AVW      3-VEC - Aircraft velocity in WIND AXES.
// CIN  AGW      3-VEC - Gravity accel. expressed in aircraft WIND AXES.
// CIN  AOAOLD   REAL - Old AOA.
// CIN  THRST    REAL - Thrust.
// CIN  JACID    AC-IDX - Aircraft ID.
// COUT MAXP     REAL - Maximum roll rate.
// COUT QMXPOS   REAL - Maximum positive pitch rate.
// COUT QMXNEG   REAL - Maximum negative pitch rate.
// COUT AOAMX    REAL - Maximum positive AOA.
// C#TECHNICAL DESCRIPTION
// C Computes maximum allowable pitch and roll rates.  Also max pos AOA.
// C#VARIABLE DESCRIPTIONS:  none
// C####
void BrawlerMover::ratmx1(double   xl,
                          double   acmass,
                          UtVec3d& avw,
                          UtVec3d& agw,
                          double   aoaold,
                          double   thrst,
                          double&  maxp,
                          double&  qmxpos,
                          double&  qmxneg,
                          double&  aoamx)
{
   // C  ---local declarations:
   double wate = 0;
   double gees = 0;
   // C*ENDDEC
   // C     --COMPUTE MAX ALLOWABLE ROLL RATE
   wate = acmass * grav;
   gees = xl / wate;
   maxp = pmax(wate, gees);
   // C     --COMPUTE MAX ALLOWABLE PITCH RATE
   qmxpos = qmax(avw[0], wate, agw[2], 1);
   qmxneg = qmax(avw[0], wate, agw[2], 2);
   // C     --COMPUTE MAX ALLOWABLE POSITIVE AOA
   aoamx1(acmass, aoamx);
}


double BrawlerMover::pmax(double wate, double glift)
{
   double pclmax = glift / gliftm(Mach(), DynamicPressure(), wate);
   return (pi * (1.0278 - pclmax * pclmax));
}


double BrawlerMover::gliftm(double mach, double dypr, double wate)
{
   // real fliftm  //function
   // integer rspace
   // character*240 outstr
   // character*20 idstr,mstr
   // C*ENDDEC
   // gtypd(kacid,3); //TODO - no mem copies necessary
   double gliftm_val = fliftm(mach, dypr, wate);
   if (gliftm_val == undnum)
   {
      nabort("GLIFTM...undefined region of clmax table accessed.");
   }
   // C
   return gliftm_val;
}


double BrawlerMover::fliftm(double mach, double dypr, double wate)
{
   double clmx = tbl1(mach, clmax, fmvec1, nfm1);
   // C     --DETERMINE IF UNDEFINED REGION OF TABLE ACCESSED
   if (clmx < -1.E6)
   {
      return undnum;
   }
   else
   {
      return (dypr * ReferenceArea() * clmx / wate);
   }
}


// C#ABSTRACT COMPUTES MAX AVAILABLE PITCH RATE FOR AERO ALGORITHM 1
// C#PURPOSE  COMPUTES MAX AVAILABLE PITCH RATE FOR AERO ALGORITHM 1
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  U      REAL - Speed.
// CIN  FM     REAL - Mach number.
// CIN  DYP    REAL - Dynamic pressure.
// CIN  WATE   REAL - A/C weight (NOT mass).
// CIN  AWZ    REAL - Wind-axis z-component of gravity.
// CIN  IDIR   INT - 1=> pos limit desired, 2=> neg limit desired.
// CIN  KACID  AC-IDX - Aircraft index for aircraft calculations.
// COUT QMAX   REAL - Pitch rate limit (negative for IDIR=2).
// C#TECHNICAL DESCRIPTION
// C    It is important to note that qmax is the max pitch rate of the
// C  wind-axes, not the body axes.  Thus, the rate of change is due
// C  only to the circular motion induced by forces, not by rigid-body
// C  moments.  Qmax thus operates by adding together the max lift and
// C  the z-component of gravity and using the sum in the centripetal
// C  acceleration formula:
// C    <ANGULAR-RATE> = <CENTRIPETAL-ACCELERATION>/<SPEED>
// C####
double BrawlerMover::qmax(double u, double wate, double awz, int idir)
{
   // double tbl1;  //function
   // double gmin,gpos,gwz,gliftm,gmax;
   double gmin = 0;
   double gpos = 0;
   double gwz  = 0;
   double gmax = 0;
   // C*ENDDEC
   gwz = awz / grav;
   // gtypd(kacid,3); //TODO - no mem copies necessary
   if (idir == 2)
   {
      if (wate > miss_mass * grav)
      {
         // gmin = tbl1(fm,gcap2(1,2),mchval,nmach1)
         gmin = tbl1(Mach(), &gcap2[1][0], mchval, nmach1);
      }
      else
      {
         // gmin = tbl1(fm,gcap(1,2),mchval,nmach1);
         gmin = tbl1(Mach(), &gcap[1][0], mchval, nmach1);
      }
      return (grav * (gmin - gwz) / u);
   }
   else if (idir == 1)
   {
      if (wate > miss_mass * grav)
      {
         gmax = tbl1(Mach(), &gcap2[0][0], mchval, nmach1);
      }
      else
      {
         gmax = tbl1(Mach(), &gcap[0][0], mchval, nmach1);
      }
      gpos = amin1(gmax, gliftm(Mach(), DynamicPressure(), wate));
      return (grav * (gpos - gwz) / u);
   }
   else
   {
      nabort("QMAX...illegal value of idir");
      return 0.0;
   }
}


// C#ABSTRACT COMPUTES MAX AOA FOR AERO ALGORITHM 1
// C#PURPOSE  USED DURING AN AIRCRAFT FLY EVENT TO COMPUTE
// C  MAXIMUM AOA  AERO ALGORITHM 1.  ALSO USED TO TEST AOA LIMIT
// C  FOR LAUNCHING MISSILES.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  ACMASS   REAL - Aircraft mass.
// CIN  AREA     REAL - Aircraft reference area.
// CIN  DYPRES   REAL - Dynamic pressure.
// CIN  MACH     REAL - Mach.
// COUT AOAMX    REAL - Maximum positive AOA.
// C#TECHNICAL DESCRIPTION
// C  Computes max AOA via table lookup on clmax and then clift.
// C  Used for iactn=9 by rates9 routine.
// C  Structural limits are then applied here.
// C  Broken out from ratmx1 to allow code that calculates AOA max
// C  to be called from aslct7 for test for missile launch.
// C#VARIABLE DESCRIPTIONS:  none
// C####
void BrawlerMover::aoamx1(double acmass, double& aoamx)
{
   // C  ---local declarations:
   double wate   = 0;
   double clmx   = 0;
   double clmxst = 0;
   double pm     = 0;
   double pa     = 0;
   // double  gc_fact = 0;
   double gmax = 0;
   int    im   = 0;
   int    ia   = 0;
   // C*ENDDEC
   wate = acmass * grav;
   // C      gc_fact = amin1(1.0,(miss_mass*grav)/wate)
   if (acmass > miss_mass)
   {
      gmax = tbl1(Mach(), &gcap2[0][0], mchval, nmach1);
   }
   else
   {
      gmax = tbl1(Mach(), &gcap[0][0], mchval, nmach1);
   }
   // C     --COMPUTE MAX ALLOWABLE POSITIVE AOA
   clmx = tbl1(Mach(), clmax, fmvec1, nfm1);
   // C     --COMPUTE CLMAX AT STRUCTURAL LIMIT - CLMXST
   clmxst = (gmax * wate / (DynamicPressure() * ReferenceArea()));
   clmx   = amin1(clmx, clmxst);
   srch(fmvec2, nfm2, Mach(), im, pm);
   if (im == 0)
   {
      // C        --THE VALUE TO BE INTERPOLATED IS OUTSIDE THE LIMITS OF TABLE
      if (Mach() < fmvec2[0])
      {
         // C           --OFF THE LOW END
         im = 1;
         pm = 0.0;
      }
      else
      {
         // C           --OFF THE HIGH END
         im = nfm2 - 1;
         pm = 1.0;
      }
   }

   if (aoa_tab_type == reflective)
   {
      invtb2(clift, clmx, nfm2, im, pm, false, nalph1, ia, pa);
      aoamx = param1(ia, pa, alfv1);
   }
   // else if (aoa_tab_type == full)
   //{
   //    aoamx = param1(im,pm,alpha_clmax);
   // }
   else
   {
      nabort("aoamx1...unknown Cl table type");
   }
}


// C#ABSTRACT RETURNS ACTUAL ANGULAR RATES FROM REQUESTED RATES
// C#PURPOSE  RETURNS ACTUAL ANGULAR RATES FROM REQUESTED RATES
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  XDOTA0 REAL - ACTUAL RATE AT TIME T-DT
// CIN  XDOTR  REAL - REQUESTED RATE AT TIME T
// CIN  OMEG   REAL - FILTER FREQUENCY
// CIN  DT     REAL - TIME STEP
// COUT ACTXDT REAL - ACTUAL RATE AT TIME T
// C#TECHNICAL DESCRIPTION
// C    THE FORMULA IS BASED ON A FIRST-ORDER LAG FILTER.  THAT IS, THE
// C  ACTUAL VALUE IS ASSUMED TO RELAX TO A REQUESTED VALUE ACCORDING
// C  TO THE EQUATION D(XDOT)/DT = OMEG*(XDOTR-ACTXDT).  IF THIS
// C  IS INTEGRATED THE FORMULA USED IN THE CODE RESULTS.
// C  (FAC IS A NUMERICAL APPROXIMATION TO EXP(-OMEG*DT)).
// C####
double BrawlerMover::actxdt(double xdota0, double xdotr, double omeg, double dt)
{
   double chi = 0;
   double fac = 0;
   // C*ENDDEC
   chi           = omeg * dt;
   fac           = 1.0 / (1.0 + chi * (1.0 + 0.5 * chi));
   double actxdt = fac * xdota0 + (1. - fac) * xdotr;
   return actxdt;
}


// C#ABSTRACT COMPUTES MAX AVAILABLE LOADING IN G'S
// C#PURPOSE  COMPUTES MAX AVAILABLE LOADING IN G'S
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach number.
// CIN  DYP    REAL - Dynamic pressure.
// CIN  WATE   REAL - Weight (not mass) of aircraft.
// CIN  IDIR   INT - 1=>Positive load limit desired .
// C           2=>Negative limit desired.
// CIN  KACID  AC-IDX - The a/c whose limit is desired.
// COUT GLDMX  REAL - Max load in g's.
// C#TECHNICAL DESCRIPTION
// C    THE GLIFTM FUNCTION IS USED TO GET MAX LIMIT DUE TO AERO
// C  CONSIDERATIONS.  THIS IS COMBINED WITH GCAP, THE STRUCTURAL LIMIT,
// C  TO OBTAIN THE DESIRED VALUE.
// C####
double BrawlerMover::gldmx(double wate, const int idir)
{
   // double tbl1;
   // int idir;
   // int kacid;
   // double dyp;
   // double fm;
   double gmax = 0;
   // double wate;
   // double gliftm;
   double gcap_use = 0;
   // C*ENDDEC
   // call gtypd(kacid,3)
   if (idir == 1)
   {
      if (wate > miss_mass * grav)
      {
         gcap_use = tbl1(fm, &gcap2[0][0], mchval, nmach1);
      }
      else
      {
         gcap_use = tbl1(fm, &gcap[0][0], mchval, nmach1);
      }
      gmax = amin1(gcap_use, gliftm(Mach(), DynamicPressure(), wate));
   }
   else if (idir == 2)
   {
      if (wate > miss_mass * grav)
      {
         // gcap_use = tbl1(fm,gcap2(1,2),mchval,nmach1);
         gcap_use = tbl1(fm, &gcap2[1][0], mchval, nmach1);
      }
      else
      {
         // gcap_use = tbl1(fm,gcap(1,2),mchval,nmach1);
         gcap_use = tbl1(fm, &gcap[1][0], mchval, nmach1);
      }
      gmax = amax1(gcap_use, -gliftm(Mach(), DynamicPressure(), wate));
   }
   else
   {
      nabort("GLDMX...illegal value of idir");
   }
   double gldmx = gmax;
   return gldmx;
}


// TODO
double BrawlerMover::drag(double tlift)
{
   return 0.0;
}


// C#ABSTRACT Returns drag on an aircraft
// C#PURPOSE  Returns the drag on an aircraft
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach number.
// CIN  DYP    REAL - Dynamic pressure.
// CIN  XLFT   REAL - Lift. - drag entry only
// CIN  XAOA   REAL - AOA - drag2 entry only
// CIN  KACID  AC-IDX - The aircraft tail number.
// COUT DRAG   REAL - Drag on the a/c.
// C#TECHNICAL DESCRIPTION
// C  Makes /ACPHYS/ and /PRFORM/ current and calls fdrag to get drag.
// C  See technical description of function fdrag.  An abort is called if
// C  fdrag returns /par/undnum/.
// C####
double BrawlerMover::drag2(double xaoa)
{
   // gtypd(kacid,3); //no mem copies necessary
   double drag2 = fdrag2(xaoa, num_ac_acd_m, &ac_acd_mach[0], &ac_acd[0]);
   if (drag2 == undnum)
   {
      // call dragck(1,kacid,fm,dyp,xaoa)  //TODO - use nabort directly for now
      nabort("drag2 == undnum");
   }
   return drag2;
}


// see comment for drag2()
double BrawlerMover::fdrag2(double xaoa, const int num_acd_m, double* acd_mach, double* acd)
{
   // C     --local declarations
   double aoa = 0;
   // C*ENDDEC
   if (DynamicPressure() == 0.0)
   {
      return 0.0;
   }
   aoa = std::abs(xaoa);
   return fdrag_inner(aoa, num_acd_m, acd_mach, acd);
}


// see comment for drag2()
double BrawlerMover::fdrag_inner(double aoa, const int num_acd_m, double* acd_mach, double* acd)
{
   // C     --local declarations
   // double adterm;
   double cdastr      = 0;
   double cdbase      = 0;
   double drag        = 0;
   double bayterm     = 0;
   double fdrag_inner = 0.0; // returned value
   // C*ENDDEC
   // cdbase = tbl2e(Mach(),aoa,&cdrag[0],&fmvec3[0],nfm3,&alfv2[0],nalph2);
   cdbase = CoefficientOfDrag(Mach(), aoa);
   gcdadd(num_acd_m, acd_mach, acd, cdastr);
   drag = DynamicPressure() * ((cdastr + cdbase) * ReferenceArea());
   // if ( adactv ) { //TODO - should never be true?
   //    adterm = drgad();
   //    fdrag_inner = drag + adterm;
   // } else {
   // adterm = 0.0;
   fdrag_inner = drag;
   //}
   bayterm = 0.0;
   // LBM - dont worry about bay doors right now
   // if(has_bay_door) {
   //    if(bd != bay_closed) {
   //       bayterm = bay_drag;
   //    }
   // }
   fdrag_inner = fdrag_inner + bayterm;
   if (fdrag_inner < -1.E6)
   {
      fdrag_inner = undnum;
   }
   return fdrag_inner;
}

double BrawlerMover::Drag(double alpha, double speed, double altitude)
{
   double aoa = std::abs(alpha);
   double t, pres, dens, vs, pr, dr;
   atmos(altitude, t, pres, dens, vs, pr, dr);
   double dypp = 0.5 * dens * speed * speed;
   double mach = speed / vs;

   // cdbase = tbl2e(Mach(),aoa,&cdrag[0],&fmvec3[0],nfm3,&alfv2[0],nalph2);
   double cdbase = CoefficientOfDrag(mach, aoa);
   double cdastr = 0;
   gcdadd(num_ac_acd_m, &ac_acd_mach[0], &ac_acd[0], cdastr);
   double drag = dypp * ((cdastr + cdbase) * ReferenceArea());
   // if ( adactv ) { //TODO - should never be true?
   //    drag += drgad();
   // }
   // LBM - do not worry about bay doors right now
   // double bayterm = 0.0;
   // if(has_bay_door) {
   //    if(bd != bay_closed) {
   //       drag += bay_drag;
   //    }
   // }
   if (drag < -1.E6)
   {
      ut::log::info() << "Drag(alpha,speed,altitude) -> computed drag < -1.E6, so zero returned.";
      return 0.0;
   }
   return drag;
}

void BrawlerMover::gcdadd(const int num_mach, double* machs, double* cds, double& cdadd)
{
   // C*ENDDEC
   cdadd = tbl1(Mach(), cds, machs, num_mach);
}


double BrawlerMover::drgad()
{
   nabort("DRGAD....should not be called for doors");
   return 0.0;
}


// C#ABSTRACT COMPUTES MAX SUSTAINED G CAPABILITY
// C#PURPOSE  COMPUTES MAX SUSTAINED G CAPABILITY
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach.
// CIN  DYP    REAL - Dynamic pressure.
// CIN  ALT    REAL - Altitude.
// CIN  WATE   REAL - Weight (not mass) of a/c.
// CIN  KACID  AC-IDX - a/c desired.
// CIN  THRTLX REAL - Throttle setting
// COUT SUSGMX REAL - Max g's for sustained turn.
// C#TECHNICAL DESCRIPTION
// C    Thrust is set to max ab.  First a test is made on exb, the
// C  excess longitudinal force available at max instantaneous G and
// C  full ab.  If exb is positive, then we are load limited and the max
// C  inst G is returned.
// C    Otherwise, it is necessary to iterate to determine the angle of
// C  attack at which drag and thrust balance.  Iteration stops when drag
// C  and the longitudinal component of thrust are balanced to within 10
// C  lbs, and the net turning force due to lift and thrust*sin(aoa) at
// C  this aoa is returned.
// C#VARIABLE DESCRIPTIONS
// C  AOAA    REAL - Angle of attack at lower end of iteration.
// C  AOAB    REAL - Angle of attack at upper end of iteration.
// C  EXA     REAL - Excess longitudinal force at aoaa.
// C  EXB     REAL - Excess longitudinal force at aoab.
// C  AOAT    REAL - Guess at aoa for which excess long. force is zero.
// C####
double BrawlerMover::susgmx(double wate, double thrtlx)
{
   // C  --locals
   int    icnt   = 0;
   int    mcount = 20;
   double aoaa   = 0;
   double aoab   = 0;
   double aoat   = 0;
   double da     = 0;
   double db     = 0;
   double dt     = 0;
   double id     = 0;
   double exa    = 0;
   double exb    = 0;
   double ext    = 0;
   double slope  = 0;
   double t      = 0;
   double xlb    = 0;
   double xlt    = 0;
   double tgg    = 0;
   double tab    = 0;
   double aoaa2  = 0;
   double exa2   = 0;
   double aoab2  = 0;
   double exb2   = 0;
   double aoax   = 0;
   double exx    = 0;
   double dx     = 0;
   double ma     = 0;
   double mb     = 0;
   // character*6 subnam
   std::string subnam = "";
   bool        zprnt  = false;
   // C*ENDDEC
   subnam = "SUSGMX";
   zprnt  = false;
   icnt   = 0;

   // use of while, go_to, and switch to account for nasty goto control statements
   int go_to = 10;
   while (go_to < 50)
   {
      switch (go_to)
      {
      case 10:
      {
         // 10    continue
         aoaa2 = -1000.0;
         aoab2 = -1000.0;
         // C     --CHECK THAT A VALID CALCULATION HAS BEEN PERFORMED
         thrust(thrtlx, tgg, tab, id);
         t    = tgg + tab;
         aoab = aoamcl(wate);
         db   = drag2(aoab) + id;
         exb  = t * cos(aoab) - db;

         if (exb >= 0.0)
         {
            // C        --HERE LIMITED BY AVAILABLE G'S
            xlb = LiftFromAlpha(aoab); // xlift(aoab,fm,dyp,kacid);
            // susgmx = (xlb+t*sin(aoab))/wate
            // return
            return (xlb + t * sin(aoab)) / wate;
         }
         // C     --HERE LIMITED BY DRAG=THRUST*COS(AOA)
         aoaa = alpha0;
         da   = drag2(aoaa) + id;
         exa  = t * cos(aoaa) - da;
         if (exa <= 0.0)
         {
            // susgmx = 1.1
            // return
            return 1.1;
         }
      } // NOTE - no break, processing continues on to case 20

      case 20:
      {
         // C  SOLVE FOR EX=0
         // 20    continue
         if (maxcnt(icnt, zprnt, mcount, subnam))
         {
            // goto 10;  //solved
            go_to = 10;
            break;
         }
         // C     --Following block improves convergence speed by avoiding
         // C     --interpolation until bracketing errors on the same order:
         if (std::abs(exb) > amax1(200.0, 5.0 * std::abs(exa)))
         {
            // C        --deliberately conservative wts (aggressive wt on aoaa is 5/6)
            // C        --to avoid overshooting 0 point
            go_to = 30;
            break;
         }
         else if (std::abs(exa) > amax1(200.0, 5.0 * std::abs(exb)))
         {
            // C        --deliberately conservative wts (aggressive wt on aoab is 5/6)
            // C        --to avoid overshooting 0 point
            go_to = 40;
            break;
         }
         go_to = 50;
      }
      break;

      case 30:
      {
         // 30       continue
         if (maxcnt(icnt, zprnt, mcount, subnam))
         {
            // goto 10;  //solved
            go_to = 10;
            break;
         }
         aoat = 0.7 * aoaa + 0.3 * aoab;
         dt   = drag2(aoat) + id;
         ext  = t * cos(aoat) - dt;
         if (sign(1.0, exb) != sign(1.0, ext))
         {
            // C              --Well, we overshot the 0 point anyway. Want to bracket solution.
            // C              --Back off towards aoab and update aoaa:
            aoaa2 = aoaa;
            exa2  = exa;
            aoaa  = aoat;
            exa   = ext;
            // goto 30;  //solved
            go_to = 30;
            break;
         }
         else
         {
            aoab2 = aoab;
            exb2  = exb;
            aoab  = aoat;
            exb   = ext;
            if (std::abs(exa) > 100.0)
            {
               // goto 20;  //solved
               go_to = 20;
               break;
            }
         }
         go_to = 50;
      }
      break;

      case 40:
      {
         // 40       continue
         if (maxcnt(icnt, zprnt, mcount, subnam))
         {
            // goto 10;  //solved
            go_to = 10;
            break;
         }
         aoat = 0.7 * aoab + 0.3 * aoaa;
         dt   = drag2(aoat) + id;
         ext  = t * cos(aoat) - dt;

         if (sign(1.0, exa) != sign(1.0, ext))
         {
            // C              --Well, we overshot the 0 point anyway. Want to bracket solution.
            // C              --Back off towards aoaa and update aoab:
            aoab2 = aoab;
            exb2  = exb;
            aoab  = aoat;
            exb   = ext;
            // goto 40;  //solved
            go_to = 40;
            break;
         }
         else
         {
            aoaa2 = aoaa;
            exa2  = exa;
            aoaa  = aoat;
            exa   = ext;
            if (std::abs(exb) > 100.0)
            {
               // goto 20;  //solved
               go_to = 20;
               break;
            }
         }
         go_to = 50;
      }
      break;

      default:
      {
         go_to = 50;
      }
      break;
      }
   }

   // C     --ASSERT: Pts A and B bracket solution and errors (ex's) are within a
   // C     --factor of 5 of each other, or largest is less than 200 lbs.

   // 50    continue
   while (true)
   {
      if (aoaa2 != -1000.0 && aoab2 != -1000.0)
      {
         // C        --The next section addresses a problem where slow convergence
         // C        --is due to a break in the interpolation table occurring right
         // C        --near the solution point.  Getting aoaa and aoab on the same
         // C        --side of the break enormously speeds convergence.  Aoax is an
         // C        --estimate of where the break point falls.  Visualize a
         // C        --piecewise linear ex(aoa) table, with aoab, aoab2 on 1 segment
         // C        --and aoaa, aoaa2 on the other.
         mb   = (exb - exb2) / (aoab - aoab2);
         ma   = (exa - exa2) / (aoaa - aoaa2);
         aoax = (exa - exb + aoab * mb - aoaa * ma) / (mb - ma);
         if (aoax > aoaa && aoax < aoab)
         {
            dx  = drag2(aoax) + id;
            exx = t * cos(aoax) - dx;
            if (exx > 0.0)
            {
               aoaa2 = aoaa;
               exa2  = exa;
               aoaa  = aoax;
               exa   = exx;
            }
            else
            {
               aoab2 = aoab;
               exb2  = exb;
               aoab  = aoax;
               exb   = exx;
            }
         }
      }

      slope = (exb - exa) / (aoab - aoaa);
      aoat  = aoab - exb / slope;
      dt    = drag2(aoat) + id;
      ext   = t * cos(aoat) - dt;
      // C     --CONVERGENCE CRITERION IS 10 LBS:

      if (std::abs(ext) <= 10.0)
      {
         xlt = LiftFromAlpha(aoat); // xlift(aoat,fm,dyp,kacid);
         // susgmx = (xlt+t*sin(aoat))/wate
         // return
         return (xlt + t * sin(aoat)) / wate;
      }
      else if (ext >= 0.0)
      {
         // C        --REPLACE "A" VERSION
         aoaa2 = aoaa;
         exa2  = exa;
         exa   = ext;
         aoaa  = aoat;
      }
      else
      {
         // C        --REPLACE "B" VERSION
         aoab2 = aoab;
         exb2  = exb;
         exb   = ext;
         aoab  = aoat;
      }
   } // goto 50;  //solved
}

// C#ABSTRACT Calculates angle of attack at max instantaneous lift
// C#PURPOSE  To overcome some inconsistencies found in the CLMAX and CL
// C          tables whereby the routine (AOA) which searches for the angle
// C          of attack associated with a CL would return incorrect results
// C          if the CL being searched for (CLMAX in this case) was greater
// C          than any entry (at that mach).
// C#TYPE     Aircraft aerodynamics
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - A/C MACH
// CIN  DYP    REAL - Dynamic pressure
// CIN  WATE   REAL - Aircraft weight
// CIN  KACID  AC-IDX - A/c tail number
// C#TECHNICAL DESCRIPTION
// C The routine was created to deal with inconsistencies in the CLMAX(MACH)
// C and CL(MACH,ALPHA) tables which caused problems when an attempt was
// C made to find the AOA at CLMAX.  The difficulty occurs when the CLMAX
// C entry at a particular MACH cannot be found in the CL(MACH,ALPHA) table
// C because it is too large (usually slightly too large).  This routine
// C detects this condition by calling AOA to get the angle of attack at
// C CLMAX and then recalculates the lift.  If this value disagress with
// C the original lift calculation, the routine assumes problems with the
// C table and searches (at the fixed mach) down the table until the
// C maximum CL is found.  The value of AOA at that value is then used.
// C#VARIABLE DESCRIPTIONS
// C XL     real - Lift calculated from CLMAX
// C XL2    real - Lift calculated from AOA at CLMAX, using CL table.
// C####
double BrawlerMover::aoamcl(double wate)
{
   // C  ---local declarations:
   double g     = 0;
   double xl    = 0;
   double xl2   = 0;
   double pm    = 0;
   double clow  = 0;
   double chigh = 0;
   double pa    = 0;
   // double gldmx;
   // double aoa;
   // double fxlift;
   double aoaclm = 0;
   int    im     = 0;
   // int im1 = 0;
   int j  = 0;
   int ia = 0;
   int i1 = 0;
   int i2 = 0;
   // C*ENDDEC
   // call gtypd(kacid,3)
   g      = gldmx(wate, 1);
   xl     = g * wate;
   aoaclm = aoa(xl, false);
   // C     --TEST FOR CORRECT CALCULATION
   xl2 = fxlift(aoaclm);
   if (xl2 < 0.99 * xl)
   {
      // C        --FIND MAXIMUM CL MANUALLY
      srch(fmvec2, nfm2, fm, im, pm);
      if (im == 0)
      {
         // C           --VALUE TO BE INTERPOLATED IS OUTSIDE THE LIMITS OF THE DATA TABLE
         if (fm < fmvec2[1 - 1])
         {
            // C              --OFF THE LOW END
            im = 1;
            pm = 0.0;
         }
         else
         {
            // C              --OFF THE HIGH END
            im = nfm2 - 1;
            pm = 1.0;
         }
      }
      // im1 = im + 1;

      // do 200 j=1,nalph1-1
      for (j = 1; j < nalph1; ++j)
      {
         if (j > 1)
         {
            clow = chigh;
         }
         else
         {
            i1   = im + (j - 1) * nfm2;
            i2   = i1 + 1;
            clow = clift[i1 - 1] + pm * (clift[i2 - 1] - clift[i1 - 1]);
         }
         i1    = im + j * nfm2;
         i2    = i1 + 1;
         chigh = clift[i1 - 1] + pm * (clift[i2 - 1] - clift[i1 - 1]);
         if (clow > chigh)
         {
            // C              --TABLE HAS A MAX, USE IT
            ia = j;
            pa = 0.0;
            // goto 9990;   //solved
            // 9990     continue
            aoaclm = alfv1[ia - 1] + pa * (alfv1[ia + 1 - 1] - alfv1[ia - 1]);
            //}
            return aoaclm;
         }
      } // 200      continue
      // C        --assert: max at largest table value; use last point:
      ia = nalph1 - 1;
      pa = 1.0;
      // 9990     continue
      aoaclm = alfv1[ia - 1] + pa * (alfv1[ia + 1 - 1] - alfv1[ia - 1]);
   }
   return aoaclm;
}


// C#ABSTRACT RETURNS DESIRED X-DOT FOR A RATE-COMMANDED CONTROL SYSTEM
// C#PURPOSE  RETURNS DESIRED X-DOT FOR A RATE-COMMANDED SECOND-ORDER
// C  CONTROL SYSTEM THAT PLACES LIMITS ON COMMANDED RATES.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  XDES   REAL - DESIRED VALUE OF VARIABLE 'X'
// CIN  X      REAL - PRESENT VALUE OF X
// CIN  XDOT   REAL - TIME DERIVATIVE OF X
// CIN  XDOTMX REAL - MAXIMUM ALLOWED XDOT
// CIN  TAU    REAL - SYSTEM CHARACTERISTIC TIME CONSTANT
// COUT DSXDOT REAL - "OPTIMAL" COMMAND XDOT
// COUT DT     REAL - TIME BEFORE WHICH DSXDOT (OR DXDOT2) SHOULD BE CALLED
// C           AGAIN TO GIVE REASONABLE APPROXIMATION TO CONTINUOUS CALLING.
// C#TECHNICAL DESCRIPTION
// C    DERIVATION DETAILS ARE TO BE FOUND IN APPENDIX C OF THE ANALYST
// C  MANUAL TO BRAWLER.  IT IS BASED ON A RATE-
// C  COMMANDED SYSTEM WITH FIRST-ORDER LAG TO THE COMMANDED RATE WITH
// C  TIME CONSTANT TAU.  CONTINUOUS APPLICATION OF THE FORMULA WILL
// C  LEAD TO A SLIGHTLY UNDER-DAMPED RELAXATION OF X TO XDES.
// C     DXDOT2 IS LIKE DSXDOT, BUT ALSO RETURNS A MAX RECONSIDERATION TIME.
// C  CALLS AT THIS INTERVAL OR LESS WILL APPROXIMATE CONTINUOUS CALLING.
// C#VARIABLE DESCRIPTIONS
// C  CHI     REAL - USED WHEN NEAR CONVERGENCE TO ALLOW LONGER RECONSIDERATION
// C         TIMES, I.E., NEARER TO TAU.
// C         COMPARED TO XDOT, CHI IS NEAR 1
// C  XC      REAL - THE DISTANCE X-CRITICAL AT WHICH THE DAMPED OSCILLATOR EQN
// C         WILL UNLIMIT," I.E., RETURN A
// C####
double BrawlerMover::dsxdot(double xdes, double x, double xdot, double xdotmx, double tau)
{
   // C**   dsxdot = 11.8468*(xdes-x)/tau - 3.8676*xdot
   // C     --The above eqn is modified to the one below because the derivation of the
   // C     --original gains assumed continuous update of the output variable.  In
   // C     --fact, the system is not stable if the refresh interval is longer than
   // C     --0.4*tau.  The revised gains yield stability if the interval is 0.8*tau.
   // C     --There appears to be little loss in terms of achieving speedy response.
   double dsxdot = 5.019 * (xdes - x) / tau - 2.168 * xdot;
   if (std::abs(dsxdot) > xdotmx)
   {
      dsxdot = sign(xdotmx, dsxdot);
   }
   return dsxdot;
   // C     ------------------------------------------------------------------
   // entry dxdot2(xdes,x,xdot,xdotmx,tau,dt)
   // C     --WORKS LIKE DSXDOT, BUT ALSO RETURNS DT, A RECONSIDERATION TIME
   // dx = x-xdes
   // C**   dsxdot = -11.8468*dx/tau-3.8678*xdot
   // dsxdot = 5.019*(xdes-x)/tau - 2.168*xdot
   // if(std::abs(dsxdot).gt.xdotmx) goto 10
   //    dt = .25*tau
   //    if(std::abs(dsxdot).gt..5*xdotmx .or.
   //       1 std::abs(xdot).gt..5*xdotmx) return
   //       dt = tau
   //       chi = -1.5*xdot/(dsxdot+1.e-35)
   //       if(chi < 0. .or. chi.gt.1.) return
   //          dt = amax1(chi,.5)*tau
   //          return
   //          10    dsxdot = sign(xdotmx,dsxdot)
   //          xc = .41088*xdotmx*tau
   //          if(std::abs(dx).gt.xc) goto 20
   //             dt = .2*tau
   //             return
   //             20    xc = sign(xc,dx)
   //             dt = .1*tau + (xc-dx+tau*(dsxdot-xdot))/dsxdot
   //             if(dt <= 0.) dt = .2*tau
   //                return
}


// C#ABSTRACT ADJUSTS INTEGRATION STEP TIME FOR A/C FLYOUT
// C#PURPOSE  ALLOWS VARIABLE INTEGRATION STEPS FOR A/C WHEN THE ROTATION
// C  WILL BE SMALL OVER TIME.  IN THIS CASE A LONGER STEP IS APPROPRIATE.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  ADES   REAL - DESIRED CHANGE IN ANGLE
// CIN  W      REAL - CURRENT ANGULAR RATE
// CIN  ANGMAX REAL - MAX ALLOWED ANGULAR DISPLACEMENT
// CIN  WDES   REAL - DESIRED ANGULAR RATE
// CIN  TAU    REAL - ANGULAR RATE RELAXATION TIME CONSTANT
// COUT STEPT  REAL - APPROPRIATE INTEGRATION STEP TIME
// C#TECHNICAL DESCRIPTION
// C     --THIS IS THE TECH DESCRIPTION OF THE ALGORITHM USED BY STEPT
// C     --This is the tech description of the quadratic algorithm:
// C  Assuming that there is exponential relaxation of w to wdes with a
// C  time constant of tau, the angular displacement after a time t is given by
// C  w(t)=wdes+(w-wdes)*exp(-t/tau).  Integration gives:
// C  theta=wdes*t+tau*(w-wdes)*(1-exp(-t/tau)).  Fitting this solution to 3 points
// C  at t=0, t=tau, t=2*tau, yields a quadratic fit.  Setting theta = ades and
// C  solving this for t (the qroots call) yields estimates of the time it will
// C  take to reach the desired value.  These are used as the basis of the stept
// C  return value.
// C     --THIS IS THE TECHNICAL DESCRIPTION OF THE Piecewise-Linear ALGORITHM
// C The step time is based on the smaller of the maximum allowed
// C angular displacement and .75 times the desired change in angle.
// C .75 is used to avoid unnecessarily small step times.  Assuming that
// C the angular rate will relax from w to wdes with a time constant tau,
// C it is possible to approximate angle as a function of time by a constant
// C rate of w for time less than tau and a constant rate wdes for time
// C greater than tau.  Knowing the angle, time is calculated and
// C compared to 0.1 and the larger is returned as stept.
// C#VARIABLE DESCRIPTION
// C####
double BrawlerMover::stept(double  ades,
                           double  w,
                           double  angmax,
                           double& wdes, // TODO_AWK lists as IN only but is either local or modified here, need brawler
                                         // source to verify??
                           double tau)
{
   double ang          = 0;
   double acoeff       = 0;
   double bcoeff       = 0;
   double roots[2]     = {0, 0};
   double min_pos_root = 0;
   bool   soln         = false;
   // C*ENDDEC
   if (wdes == 0.0)
   {
      // C        --STEPT CONTAINS TWO ALGORITHMS.  WE WAN'T TO FORCE THE PROPER
      // C        --ONE TO EXECUTE DEPENDING ON THE RELATIVE SIGNS OF w AND ades.
      // C        --WE CAN'T SELECT THE ZERO CROSSING ALGORITHM IF ades AND w
      // C        --HAVE DIFFERENT SIGNS; OTHERWISE WE WILL ABORT; I.E.
      // C        --SHOULD HAVE OPPOSITE SIGN FROM ades AND OPPOSITE SIGN FROM
      // C        --w.  SO w AND ades BETTER HAVE SAME SIGN!
      if (sign(1.0, ades) == sign(1.0, w))
      {
         // C           --FORCE THE ZERO CROSSING ALGORITHM (WHICH FOLLOWS FIRST)
         wdes = 1.e-5 * sign(1.0, -w);
      }
      else
      {
         // C           --FORCE THE QUADRATIC
         wdes = 1.e-5 * sign(1.0, ades);
      }
   }
   if (sign(1.0, ades) != sign(1.0, wdes))
   {
      // C        --PRESUMABLY w IS THE SAME SIGN AS ades, AND SIGN OF wdes DIFFERS
      // C        --TO PREVENT OVERSHOOT; HENCE THE NEXT TEST CHECKS THAT ASSUMPTION.
      if (sign(1.0, w) == sign(1.0, wdes))
      {
         // write(ioutp,6001) ades,w,angmax,wdes,tau
         // write(6    ,6001) ades,w,angmax,wdes,tau
         nabort("stept...assertion error...see IOUT or LOG");
      }
      // C        --Moving away from ades, ultimately.  wdes is presumably in the
      // C        --opposite direction because of damping on w.  In this case
      // C        --associate stept with the time it takes to zero w.
      // C        --FOLLOWING IS SOLUTION OF FOLLOWING FOR w(t)=0.  wdes and w ARE OPPOSITE
      // C        --SIGNS, SO THERE WILL BE A ZERO CROSSING.
      // C        --w(t) = w(0) + (wdes-w(0))*(1-exp(-t/tau))
      return tau * log(1.0 - w / wdes);
   }
   // C     --Moving towards ADES, ultimately. That is, ADES and WDES have the same
   // C     --sign; however current rate W may have either sign.
   // C     --Associate stept with time to reach the smaller (in magnitude) of ades, angmax.
   acoeff = 0.1998 * (wdes - w) / tau;
   bcoeff = 0.8319 * w + 0.1681 * wdes;
   ang    = amin1(std::abs(ades * 0.75), angmax);
   ang    = sign(ang, ades);
   qroots(acoeff, bcoeff, -ang, roots[0], roots[1], soln);
   double stept = 0.0;
   if (soln)
   {
      // C        --extract smallest positive root
      if (roots[0] < 0.0)
      {
         min_pos_root = roots[1];
      }
      else if (roots[1] < 0.0)
      {
         min_pos_root = roots[0];
      }
      else
      {
         min_pos_root = amin1(roots[0], roots[1]);
      }
      if (min_pos_root < 0.0)
      {
         // write(ioutp,6002) ades,w,angmax,wdes,tau,roots[0],roots[1]
         // write(6    ,6002) ades,w,angmax,wdes,tau,roots[0],roots[1]
         nabort("stept...only negative roots when sign(wdes) =sign(ades)...see IOUT or LOG");
      }
      stept = amin1(min_pos_root, 1.5 * tau);
   }
   else
   {
      // c        --This can occur due to the quadratic approximation if both w
      // c        --and wdes have the same sign and w is the larger in magnitude.
      // c        --In this case the actual solution exists but is larger than
      // c        --2*tau.  We don't want to return a stept bigger than that anyway.
      if (sign(1.0, w) != sign(1.0, wdes) || std::abs(w) < std::abs(wdes))
      {
         // write(ioutp,6003) ades,w,angmax,wdes,tau,acoeff,bcoeff
         // write(6    ,6003) ades,w,angmax,wdes,tau,acoeff,bcoeff
         nabort("stept...unexpected case...see IOUT or LOG");
      }
      stept = 1.5 * tau;
   }
   stept = amax1(stept, 0.1);
   return stept;
}


// C#ABSTRACT FINDS REAL ROOTS OF A QUADRATIC EQUATION
// C#PURPOSE  FINDS REAL ROOTS OF A QUADRATIC EQUATION
// C#TYPE     MATHEMATICS UTILITY
// C#PARAMETER DESCRIPTIONS
// CIN  A      REAL - COEFF OF X**2 IN A*X**2+B*X+C=0
// CIN  B      REAL - COEFF OF X
// CIN  C      REAL - CONSTANT TERM
// COUT X1     REAL - ROOT WITH POSITIVE DISCRIMINANT SQRT
// COUT X2     REAL - THE OTHER ROOT
// COUT SOLUTN LOG - .TRUE. IMPLIES REAL ROOTS
// C           WHEN .FALSE., X1, X2 UNDEFINED
// C#TECHNICAL DESCRIPTION
// C  Uses standard formula for quadratic equation
// C
// C  However, a negative discriminant attributable to roundoff errors
// C  is set to zero.  For this to happen, the residual must be much smaller
// C  than b*b.
// C
// C  The case of a zero A coefficient is also detected.  In this case, the
// C  solution of the resulting linear eqn is returned.
// C####
void BrawlerMover::qroots(double a, double b, double c, double& x1, double& x2, bool& solutn)
{
   double d  = 0;
   double a2 = 0;
   // C*ENDDEC
   if (a == 0.0)
   {
      x1     = -c / b;
      x2     = x1;
      solutn = true;
      return;
   }
   d = b * b - 4.0 * a * c;
   if (d > 0.0)
   {
      solutn = true;
      d      = sqrt(d);
      a2     = a + a;
      x1     = (-b + d) / a2;
      x2     = (-b - d) / a2;
   }
   else
   {
      if ((-d < 1.0e-5 * (b * b)) && (-d < pow((1.0e-3 * a), 2)))
      {
         // C           --ASSUME DISCRIMINANT=0, WON'T MAKE MUCH DIFFERENCE
         solutn = true;
         x1     = -b / (a + a);
         x2     = x1;
      }
      else
      {
         solutn = false;
      }
   }
}


// C#ABSTRACT GETS ANGLE OF ATTACK
// C#PURPOSE  Returns angle of attack given an aircraft's aerodynamic state.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach number.
// CIN  DYP    REAL - Dynamic pressure.
// CIN  XL     REAL - Lift.
// CIN  KACID  AC-IDX - The aircraft whose aoa is needed.
// CIN  POSTALL LOGICAL - True if looking for post stall AOA.
// COUT AOA    REAL - Angle of attack of aircraft kacid.
// C#TECHNICAL DESCRIPTION
// C  Makes /ACPHYS/ and /PRFORM/ current and calls faoa.  An abort is called
// C  if AOA = /PAR/UNDNUM.
// C####
double BrawlerMover::aoa(double xl, bool postall)
{
   // character*20 mstr,idstr
   // C*ENDDEC
   // gtypd(kacid,3); //no mem copies necessary
   double aoa = faoa(xl, postall);
   if (aoa == undnum)
   {
      nabort("aoa undefined error, bad table reference");
   }
   // C
   return aoa;
}


// C#ABSTRACT Gets angle of attack
// C#PURPOSE  Returns the angle of attack given the current state of an aircraft
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach number.
// CIN  DYP    REAL - Dynamic pressure.
// CIN  XL     REAL - Lift.
// CIN  AREA   REAL - A/C reference area.
// CIN  POSTALL LOGICAL - True if post stall solution desired.
// COUT FAOA   REAL - Angle of attack of kacid.  FAOA is /par/undnum if
// C           the differential lift computed is less than -1,000,000,
// C           indicating that undefined data has been accessed.
// C#TECHNICAL DESCRIPTION
// C  Assumes /ACPHYS/ and /PRFORM/ are current.
// C
// C  The FXLIFT routine gives the lift at zero angle of attack.  The
// C  differential lift combines with CLA, the coefficient of lift vs
// C  angle of attack, to yield the angle of attack.  If the differential
// C  lift (DCL) is less than -1,000,000 FAOA is returned as /PAR/UNDNUM
// C  as an indication that undefined data was accessed.
// C
// C  To obtain the post stall solution, the postall argument must be true
// C  and aoa_tab_type must be full.
// C#VARIABLE DESCRIPTION
// C 95  ALFV1    ARRAY DIM(MALPHA) OF (REAL) [RAD] - Angle of attack values
// C              in radians for CLIFT.
// C 62  ALPHA0   REAL [RAD] - Angle of attack at which there is zero lift.
// C              Lift is assumed to be symmetric about this AOA. Read in
// C              radians.
// C  FULL     INT - Full aero table specification. To be used with
// C           AOA_TAB_TYPE.
// C  REFLECTIVE INT - Aero table reflective through zero intercept.  To
// C           be used with AOA_TAB_TYPE.
// C####
double BrawlerMover::faoa(double xl, bool postall)
{
   double dcl = 0;
   int    im  = 0;
   int    ia  = 0;
   double pm  = 0;
   double pa  = 0;
   // double clmxm;
   bool revrse = false;
   // C*ENDDEC
   double faoa = 0.0;

   srch(fmvec2, nfm2, Mach(), im, pm);
   if (im == 0)
   {
      // C     --THE VALUE TO BE INTERPOLATED IS OUTSIDE THE LIMITS OF THE TABLE
      if (Mach() < fmvec2[0])
      {
         // C           --OFF THE LOW END
         im = 1;
         pm = 0.0;
      }
      else
      {
         // C           --OFF THE HIGH END
         im = nfm2 - 1;
         pm = 1.0;
      }
   }
   dcl = xl / (DynamicPressure() * ReferenceArea());
   if (dcl < -1.E6)
   {
      // C        --FAOA UNDEFINED HERE
      faoa = undnum;
      return faoa;
   }
   else
   {
      // C        --FAOA DEFINED HERE
      // if (aoa_tab_type == full)
      //{
      //    clmxm = param1(im, pm, clmax);
      //    if(dcl >= clmxm) {
      //       faoa = param1(im, pm, alpha_clmax);
      //    } else {
      //       invtb2(clift, dcl, nfm2, im, pm, postall, nalph1, ia, pa);
      //       faoa = param1(ia, pa, alfv1);
      //    }
      // } else
      if (aoa_tab_type == reflective)
      {
         if (postall)
         {
            nabort("faoa... post stall for reflective table");
         }
         if (dcl >= 0.0)
         {
            revrse = false;
         }
         else
         {
            dcl    = -dcl;
            revrse = true;
         }
         invtb2(clift, dcl, nfm2, im, pm, false, nalph1, ia, pa);
         faoa = param1(ia, pa, alfv1);
         if (revrse)
         {
            faoa = -faoa;
         }
         faoa = faoa + alpha0;
      }
      else
      {
         nabort("faoa... unknown aoa table type");
      }
   }
   return faoa;
}


bool BrawlerMover::toltst(double xf, double x0, double tol)
{
   chkrng(tol, 0.0, 1.0, "tol...in toltst");
   return ((2.0 * std::abs(xf - x0)) < (tol * std::abs(xf + x0)));
}


// C#ABSTRACT Corrects requested transverse accelerations for ground avoidance
// C#PURPOSE  Called by gwreq to add automatic ground avoidance in at a low
// C  level.
// C#TYPE     AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIO  ALY    REAL - Y-COMPONENT OF REQUESTED ACCEL IN WIND AXES
// CIO  ALZ    REAL - Z-COMPONENT OF REQUESTED ACCEL IN WIND AXES
// CIO  AL     REAL - MAGNITUDE OF TRANSVERSE ACCEL = SQRT(ALY**2+ALZ**2)
// CIN  GREQM  REAL - MAX REQUESTED TRANSVERSE ACCEL IN G'S.  THIS IS NOT
// C           necessarily a physical limit, and al need not equal
// C           greqm*grav.
// COUT TOUP   3-VED - Direction closest to the vertical but still
// C           normal to velocity.
// COUT GAVDFL LOG - Ground avoidance flag; TRUE if ground avoidance
// C           is necessary.
// C#TECHNICAL DESCRIPTION
// C  Considers ground avoidance situation to be in effect whenever one of
// C  the following conditions holds: Below 100 ft or will descend below 50
// C  ft in less than 10 seconds.  In these situations, the requested lift
// C  forces aly, alz are adjusted to correct the situation.
// C
// C  Correction is defined in terms of a desired minimum ROC, rcdes.
// C  Rcdes in turn implies a minimum accel alupmn in the direction to_up,
// C  which is imposed on aly and alz.  Toup is the direction normal to the
// C  current velocity that is closest to the (upwards) vertical.
// C#VARIABLE DESCRIPTIONS
// C  VEL      3-VEC [FT/SEC] - Inertial velocity vector.
// C alupmn REAL - min acceptable acceleration in direction toup
// C rcmin  REAL - min acceptable ROC.  Used to compute alupmn
// C####
void BrawlerMover::adjal(double& aly, double& alz, double& al, double greqm, UtVec3d& toup, bool& gavdfl)
{
   double      alup   = 0;
   double      alupmn = 0;
   double      rcmin  = 0;
   UtVec3d     alwind(0.0, 0.0, 0.0);
   UtVec3d     alrot(0.0, 0.0, 0.0);
   UtFortMat3d rot   = UtFortMat3d();
   double      anorm = 0;
   // C*ENDDEC
   if (Altitude() < 100.0)
   {
      // c        --Below 100 feet now.  Demand min climb rate of 10 ft/sec.
      rcmin  = 10.0;
      gavdfl = true;
   }
   else if (vel[2] * 10.0 - Altitude() > -50.0)
   {
      // c        --Will descend below 50 feet in less that 10 seconds.
      // c        --Set rcmin to vel that descends to 50 ft in 10 seconds.
      rcmin  = (50.0 - Altitude()) / 10.0;
      gavdfl = true;
   }
   else
   {
      // c        --no ground avoidance problem
      gavdfl = false;
      return;
   }
   // c     --Form toup, the vector in wind y-z plane closest to vertical.
   // c     --This direction must have a min accel associated with it.  The
   // c     --min accel, alupmn, will achieve rcmin in 2 seconds.
   toup[0] = 0.0;
   toup[1] = 0.0;
   toup[2] = -1.0;
   // vxfrmc(rwea,toup,toup,1);
   toup = rwea * toup;
   // c     --next term is part of alupmn needed to counteract gravity:
   alupmn = grav / sqrt(1.0 - pow(toup[0], 2));
   // c     --compute new toup = unit vector projection of old toup onto y-z plane
   toup[0] = 0.0;
   // vnorm(toup,toup);
   toup.Normalize();
   alup = aly * toup[1] + alz * toup[2];
   // c     --add in part of alupmn that corrects vel to -rcmin in 2 seconds
   alupmn = alupmn + (rcmin + vel[2]) / 2.0;
   if (alup < alupmn)
   {
      // c        --adjust component of (0,aly,alz) parallel to toup
      if (greqm * grav <= alupmn)
      {
         // c           --ignore perpendicular component, greqm already exceeded
         // c           --assert: toup is a unit vector
         aly = toup[1] * alupmn;
         alz = toup[2] * alupmn;
         al  = alupmn;
      }
      else
      {
         // c           --Requirement alupmn is not so large as to preclude a perpendicular
         // c           --(turning) component of accel.  Express al in coords where toup is
         // c           --x-axis, al in x-z plane.  Then set parallel component alrot[0] to
         // c           --alupmn but leave perp component alrot[2] alone unless greqm
         // c           --exceeded.
         alwind[0] = 0.0;
         alwind[1] = aly;
         alwind[2] = alz;
         rot       = makecs(toup, alwind);
         // vxfrmc(rot,alwind,alrot,1);
         alrot    = rot * alwind;
         alrot[0] = alupmn;
         anorm    = alrot.Magnitude();
         if (anorm > greqm * grav)
         {
            // c              --need to adjust magnitude of accel down to greqm g's
            al       = greqm * grav;
            alrot[2] = sqrt(pow(al, 2) - pow(alupmn, 2));
         }
         else
         {
            al = anorm;
         }
         // c           --rotate back to wind axes
         // vxfrmc(rot,alwind,alrot,2);
         alwind = rot.TransposeMultiply(alrot);
         aly    = alwind[1];
         alz    = alwind[2];
      }
   }
   else
   {
      // no print
   }
}


// C#ABSTRACT Revise desired velocity vector based on altitude considerations
// C#PURPOSE  Based on altitude limits from production rules, change
// C          desired vector velocity in order to avoid violating
// C          altitude limits.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIO  vwr    3-vec - desired velocity direction in current wind system
// C#TECHNICAL DESCRIPTION
// C  The code here was fashioned after the algorithm in gwreq for ground
// C  avoidance.  Both the code for minimum altitude and maximum altitude
// C  constraints are identical except for the sense of the tests in the
// C  code.  Hence only the minimum altitude test will be discussed.
// C  Two cases are considered.
// C
// C  Case 1: Already below minimum altitude
// C
// C    Vwr is adjusted to achieve a minimum pitch angle = arcsin(.05)
// C
// C  Case 2:  Above minimum altitude but will drop more than
// C    50 ft under minimum altitude in 10 seconds.
// C
// C    Select descent rate that will allow drop to 50 ft under minimum in
// C    10 seconds.  Adjust vwr for this descent rate.
// C
// C#VARIABLE DESCRIPTIONS
// C  KACID    AC-IDX [NA] - Index of current aircraft.
// C  TSV      REAL [SEC] - Time of the state vector.
// C  drop   real - used in several contexts to mean a worst case descent
// C         rate or desired descent rate.
// C####
void BrawlerMover::altlim(UtVec3d& vwr)
{
   double  altmn = 0;
   double  altmx = 0;
   double  drop  = 0;
   UtVec3d diruse(0.0, 0.0, 0.0);
   // C*ENDDEC
   // C     --IGNORE ZERO AS MINIMUM SINCE GROUND AVOIDANCE TAKEN CARE OF
   // C     --IN GWREQ

   // if (altmin(kacid) == 0.0) goto 50 //solved
   if (altmin != 0.0)
   {
      altmn = altmin;
      if (Altitude() < altmn)
      {
         // C        --ALREADY BELOW MINIMUM ALTITUDE
         diruse[2] = amin1(mVelocityVector[2], -0.05);
         vmake2(sqrt(1.0 - pow(diruse[2], 2)), mVelocityVector, diruse);
         // vxfrmc(rwea,diruse,vwr,1);
         vwr = rwea * diruse;
         return; // goto 900  //solved
      }
      else
      {
         // C        --CURRENTLY ABOVE MIN. ALTITUDE
         // C        --DON'T ALLOW DESCENT BELOW ALTMIN-50 IN LESS THAN 10 SEC.
         drop = amax1(vel[2], Speed() * mVelocityVector[2]);
         if ((drop * 10.0 - Altitude()) > -(altmn - 50.0))
         {
            // C           --WILL GET BELOW ALTMN-50 FT. IN LESS THAN 10 SECONDS
            // C           --COMPUTE DROP=MAX SAFE DESCENT RATE
            // C           --LET YOURSELF DROP DISTANCE TO ALTMN-50 IN NEXT 10 SEC.
            drop      = ((Altitude()) - (altmn - 50.0)) / 10.0;
            diruse[2] = drop / Speed();
            vmake2(sqrt(1.0 - pow(diruse[2], 2)), mVelocityVector, diruse);
            // vxfrmc(rwea,diruse,vwr,1);
            vwr = rwea * diruse;
            return; // goto 900  //solved
         }
      }
   }
   // 50    continue
   altmx = altmax;
   if (Altitude() > altmx)
   {
      // C        --ALREADY ABOVE CEILING
      diruse[2] = amax1(mVelocityVector[2], +0.05);
      vmake2(sqrt(1.0 - pow(diruse[2], 2)), mVelocityVector, diruse);
      // vxfrmc(rwea,diruse,vwr,1);
      vwr = rwea * diruse;
      return; // goto 900  //solved
   }
   else
   {
      // C        --BELOW ALTMX.
      // C        --DON'T ALLOW ASCENT ABOVE ALTMX+50 IN LESS THAN 10 SECONDS
      drop = amax1(vel[2], Speed() * mVelocityVector[2]);
      if ((drop * 10.0 - Altitude()) < -(altmx + 50.0))
      {
         // C           --WILL GET ABOVE ALTMX+50 FT. IN LESS THAN 10 SECONDS
         // C           --COMPUTE DROP=MAX SAFE DESCENT RATE
         // C           --DROP = (-50.-POS[2])/10.
         // C           --LET YOURSELF DROP DISTANCE TO ALTMN+50 IN NEXT 10 SEC.
         drop      = -(altmx - Altitude() + 50.0) / 10.0;
         diruse[2] = drop / Speed();
         vmake2(sqrt(1.0 - pow(diruse[2], 2)), mVelocityVector, diruse);
         // vxfrmc(rwea,diruse,vwr,1);
         vwr = rwea * diruse;
         return; // goto 900  //solved
      }
   }
   // 900   continue
}


void BrawlerMover::vmake2(double a, UtVec3d& vin, UtVec3d& vout)
{
   double b = 0;
   // C*ENDDEC
   b       = a / sqrt(1.0e-35 + vin[0] * vin[0] + vin[1] * vin[1]);
   vout[0] = b * vin[0];
   vout[1] = b * vin[1];
}


void BrawlerMover::chkrng(double x, double xmin, double xmax, std::string text)
{
   // C  --Local declarations:
   // int nstart, nstop;
   // character delim*(*)
   // character*500 chtext
   // parameter (delim=':=~-+''"_')
   // C*ENDDEC
   if (xmin <= x && x <= xmax)
   {
      return;
   }
   // nstop = len( text );
   // nstart = 1;
   // if (text(1:1) == text(nstop:nstop)) {
   //    if (index(delim,text(1:1))  !=  0) {
   //       nstop = nstop - 1
   //       nstart = 2
   //    }
   // }
   // write(chtext,1000) text(nstart:nstop),x,xmin,xmax
   // call nabort(chtext)
   nabort(text);
}


void BrawlerMover::ckrngi(const int k, const int kmin, const int kmax, const std::string& text)
{
   // C  --Local declarations:
   // int nstart, nstop;
   // character delim*(*)
   // character*500 chtext
   // parameter (delim=':=~-+''"_')
   // c*enddec
   if (kmin <= k && k <= kmax)
   {
      return;
   }
   // nstop = len( text )
   // nstart = 1
   // if (text(1:1) == text(nstop:nstop)) {
   //    if (index(delim,text(1:1))  !=  0) {
   //       nstop = nstop - 1
   //       nstart = 2
   //    }
   // }
   // write(chtext,1001) text(nstart:nstop),k,kmin,kmax
   // call nabort(chtext)
   nabort(text);
}


// C#ABSTRACT GETS ANGLE OFF, ITS RATE, AND ITS "ROLL ANGLE" SIN & COS
// C#PURPOSE  GETS ANGLE OFF, ITS RATE, AND ITS "ROLL ANGLE" SIN & COS
// C  OF A VECTOR, EXPRESSED IN A ROTATING COORDINATE SYSTEM
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  VW     3-VEC - THE VECTOR, REPRESENTED IN COORDINATE SYSTEM
// CIN  WNOW   3-VEC - THE ROTATIONAL RATES OF THE COORDINATE SYSTEM,
// C           EXPRESSED IN COORDINATE SYSTEM.  THAT IS, WNOW(1) = ROLL
// C           RATE, WNOW(2) = PITCH RATE, WNOW(3) = YAW RATE
// COUT CPHI   REAL - COS OF PHI, THE ANGLE THE PROJECTION OF VW ONTO
// C           THE Y-Z PLANE MAKES WITH THE Z-AXIS
// COUT SPHI   REAL - SIN OF PHI
// COUT PSIDOT REAL - THE TIME DERIVATIVE OF PSI, DUE TO WNOW
// COUT PSI    REAL - THE ANGLE VW MAKES WITH THE X-AXIS
// C#TECHNICAL DESCRIPTION
// C    FORMULAS FOR PSI, CPHI, AND SPHI ARE ELEMENTARY TRIG.  THE
// C  FORMULA FOR PSIDOT MAY BE DERIVED BY TAKING THE TIME DERIVATIVE OF
// C  THE EQUATION
// C    COS(PSI) = DOT(VW,UNITX)/XMAG(VW)
// C  TREATING VW AS FIXED AND THE X-AXIS AS HAVING A TIME DERIVATIVE
// C  GIVEN BY
// C    D(UNITX)/DT = WNOW(3)*UNITY-WNOW(2)*UNITZ
// C####
void BrawlerMover::desang(UtVec3d& vw, UtVec3d& wnow, double& cphi, double& sphi, double& psidot, double& psi)
{
   double vtrans = 0;
   // double val;
   // C*ENDDEC
   vtrans = sqrt(vw[2 - 1] * vw[2 - 1] + vw[3 - 1] * vw[3 - 1]);
   if (vtrans == 0.0)
   {
      psi    = 0.0;
      psidot = sqrt(wnow[2 - 1] * wnow[2 - 1] + wnow[3 - 1] * wnow[3 - 1]);
      if (psidot != 0.0)
      {
         cphi = wnow[2 - 1] / psidot;
         sphi = -wnow[3 - 1] / psidot;
      }
      else
      {
         // projection of vw onto Y-Z plane is just the origin, so assume its pointed slightly up or slightly down???
         sphi = 0.0;  // slightly up or down
         cphi = -1.0; // slightly up
         // cphi =  1.0;   //slightly down
      }
   }
   else
   {
      psi    = atan2(vtrans, vw[1 - 1]);
      cphi   = vw[3 - 1] / vtrans;
      sphi   = vw[2 - 1] / vtrans;
      psidot = wnow[2 - 1] * cphi - wnow[3 - 1] * sphi;
   }
}


// C#ABSTRACT Finds max tranverse acceleration from min long. acceleration
// C#PURPOSE  Called by GWREQ
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// COUT G_TRANS_MAX   REAL - Maximum transverse acceleration in gees
// COUT LOW_SPD       LOG - .true. if speed is low
// C#TECHNICAL DESCRIPTION
// C If the current speed (spd) is less than spdmin (300. ft/sec) then
// C find the max transverse accleration in gees at which the aircraft
// C could decclerate. G_TRANS_MAX is used in GWREQ to set GREQM.  The
// C following equation was used:
// C     almin ==> g_max_trans
// C
// C     amass*almin = t*cos(aoa) - D(aoa) + amass*gwind(1)
// C
// C  This equation is solved iteratively with cos(aoa) = 1 the first time
// C  through.  Solve for the drag (D) and find aoa_zero.  AOA_ZERO is then
// C  used to find drag_one and a reverse interpolation is done to find
// C  both aoa_zero and aoa_one.  Once aoa_one is determined the lift is
// C  found with the function, xlift.
// C#VARIABLE DESCRIPTIONS
// C  CASMIN   ARRAY DIM(MAC) of REAL - Minimum calibrated airspeed allowed.
// C           This is used by gwreq to set max gees and throttle setting for
// C           maneuvers of jacton=4.
// C  KACID    AC-IDX [NA] - Index of current aircraft.
// C  SPD      REAL [FT/SEC] - Speed.
// C CASMIN    REAL - Minimum calibrated airspeed allowed
// C CASNOW    REAL - Current calibrated airspeed
// C####
void BrawlerMover::max_asubt(double& g_trans_max, bool& low_spd)
{
   // C  --local declarations:
   double  spdmin         = 0;
   double  almin          = 0;
   double  casnow         = 0;
   double  tgg            = 0;
   double  tab            = 0;
   double  id             = 0;
   double  t              = 0;
   double  aoab           = 0;
   double  almax_unloaded = 0;
   double  db             = 0;
   UtVec3d gwind(0.0, 0.0, 0.0);
   double  xlb       = 0;
   double  drag_zero = 0;
   double  aoa_zero  = 0;
   double  drag_one  = 0;
   double  aoa_one   = 0;
   // C*ENDDEC
   casnow = cas(Speed());
   spdmin = Speed() * casmin / casnow;
   if (Speed() > spdmin)
   {
      g_trans_max = 100.0;
      low_spd     = false;
      return;
   }
   // C     --FIND MAX UNLOADED LONGITUDINAL ACCELERATION
   low_spd = true;
   thrust(throtm, tgg, tab, id);
   t              = tgg + tab;
   aoab           = alpha0;
   db             = drag2(aoab) + id;
   almax_unloaded = (t - db) / amass;
   // C     --FIND MIN LONGITUDINAL ACCELERATION
   almin = amin1((spdmin - Speed()) / 5.0, almax_unloaded);
   // C     --FIND MAX TRANSVERSE ACCLERATIONM ITERATIVELY
   // C     --ASSUME COS(ALPHA)=1 AND SOLVE FOR DRAG_ZERO
   // vxfrmc(rwea,gravec,gwind,1);
   gwind       = rwea * gravec;
   drag_zero   = amass * (gwind[0] - almin) + t - id;
   aoa_zero    = AngleOfAttackFromDrag(drag_zero);
   drag_one    = amass * (gwind[0] - almin) + t * cos(aoa_zero) - id;
   aoa_one     = AngleOfAttackFromDrag(drag_one);
   xlb         = LiftFromAlpha(aoa_one); // xlift(aoa_one,fm,dyp,kacid);
   g_trans_max = xlb / (amass * grav);
}


// C#ABSTRACT RETURNS LIFT FROM ANGLE OF ATTACK AND MACH
// C#PURPOSE  RETURNS LIFT FROM ANGLE OF ATTACK AND MACH
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  ALFA   REAL - Angle of attack.
// CIN  FM     REAL - Mach.
// CIN  DYP    REAL - Dynamic pressure.
// CIN  KACID  AC-IDX - Desired A/C.
// COUT XLIFT  REAL - Lift.
// COUT XLIFT0 REAL - Lift at zero angle of attack.
// C#TECHNICAL DESCRIPTION
// C  Makes /PRFORM/ and /ACPHYS/ current, and then uses function fxlift.
// C  An abort is called if fxlift returns /par/undnum.
// C#VARIABLE DESCRIPTIONS
// C####
double BrawlerMover::xlift(double alfa)
{
   // C      --LOCALS
   double xl = 0;
   // character*240 outstr
   // C*ENDDEC
   // gtypd(kacid,3); //memory copies not necessary
   xl = fxlift(alfa);
   if (xl == undnum)
   {
      nabort("xl == undnum");
   }
   return xl;
}


// C#ABSTRACT Returns lift from angle of attack and mach
// C#PURPOSE  Returns lift given the angle of attack and mach of an aircraft
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  ALFA   REAL - Angle of attack.
// CIN  FM     REAL - Mach.
// CIN  DYP    REAL - Dynamic pressure.
// CIN  AREA   REAL - A/C reference area.
// COUT FXLIFT REAL - Lift.  If the calculated lift is less than -1.E7,
// C           fxlift is returned as /par/undnum.
// C#TECHNICAL DESCRIPTION
// C
// C  Assumes PRFORM/ is current.
// C
// C  Uses the table CLIFT which contains the coefficient of lift as a
// C  function of Mach and std::abs(alpha-alpha0), where alpha0 is the angle of
// C  attack at which Cl is zero.   A linear interpolation is
// C  performed and the formula:  LIFT = DYP*AREA*CL
// C  is used.  In the above, CL2is the result of interpolating the
// C  CLIFT table.  If the lift calculated is less than
// C  -10,000,000, fxlift is returned as /PAR/UNDNUM as an indication that
// C  undefined data was accessed.
// C#VARIABLE DESCRIPTIONS
// C  CL2     REAL - Coefficient of lift due to angle-of-attack
// C####
double BrawlerMover::fxlift(double alfa)
{
   // double tbl2, tbl1, xlimit;
   // C     --local declarations
   double fxl  = 0;
   double cl2  = 0;
   double clmx = 0;
   double cl   = 0;
   // double clmn;
   // C*ENDDEC
   if (Mach() < 0.0)
   {
      nabort("FXLIFT...trying to get lift for aircraft with Mach < 0.0");
   }
   if (DynamicPressure() < 0.0)
   {
      nabort("FXLIFT...trying to get lift for aircraft with dynamic pressure < 0.0");
   }
   if (ReferenceArea() <= 0.0)
   {
      nabort("FXLIFT...trying to get lift for aircraft with area <= 0.0");
   }
   clmx = tbl1(Mach(), clmax, fmvec1, nfm1);
   // C     ---CL2 IS CLIFT(FM,ALFA)
   if (aoa_tab_type == reflective)
   {
      cl2 = tbl2(Mach(), std::abs(alfa - alpha0), clift, fmvec2, nfm2, alfv1, nalph1);
      cl  = xlimit(sign(cl2, (alfa - alpha0)), clmx, -clmx);
   }
   // else if(aoa_tab_type == full)
   //{
   //    cl2 = tbl2(Mach(),alfa,clift,fmvec2,nfm2,alfv1,nalph1);
   //    clmn = tbl1(Mach(),clmin,fmvec1,nfm1);
   //    cl = xlimit(cl2,clmx,clmn);
   // }
   else
   {
      nabort("FXLIFT...unknown aoa_tab_type");
   }
   fxl = DynamicPressure() * ReferenceArea() * cl;
   // C     --TEST FOR UNDEFINED DATA
   if (fxl < -1.E7)
   {
      return undnum;
   }
   return fxl;
}


double BrawlerMover::Lift(double alpha, double speed, double altitude)
{
   double t, pres, dens, vs, pr, dr;
   atmos(altitude, t, pres, dens, vs, pr, dr);
   double dypp = 0.5 * dens * speed * speed;
   double mach = speed / vs;

   double fxl  = 0;
   double cl2  = 0;
   double clmx = 0;
   double cl   = 0;
   if (mach < 0.0)
   {
      ut::log::info() << "Lift(alpha,speed,altitude) -> mach < 0.0, so zero returned.";
      return 0.0;
   }
   if (dypp < 0.0)
   {
      ut::log::info() << "Lift(alpha,speed,altitude) -> dynamic pressure < 0.0, so zero returned.";
      return 0.0;
   }
   if (ReferenceArea() <= 0.0)
   {
      ut::log::info() << "Lift(alpha,speed,altitude) -> area <= 0.0, so zero returned.";
      return 0.0;
   }
   clmx = tbl1(mach, clmax, fmvec1, nfm1);
   // C     ---CL2 IS CLIFT(FM,ALFA)
   if (aoa_tab_type == reflective)
   {
      cl2 = tbl2(mach, std::abs(alpha - alpha0), clift, fmvec2, nfm2, alfv1, nalph1);
      cl  = xlimit(sign(cl2, (alpha - alpha0)), clmx, -clmx);
   }
   // else if(aoa_tab_type == full)
   //{
   //    cl2 = tbl2(Mach(),alfa,clift,fmvec2,nfm2,alfv1,nalph1);
   //    clmn = tbl1(Mach(),clmin,fmvec1,nfm1);
   //    cl = xlimit(cl2,clmx,clmn);
   // }
   else
   {
      ut::log::info() << "Lift(alpha,speed,altitude) -> unknown aoa_tab_type, for now it must be: reflective.";
      return 0.0;
   }
   fxl = dypp * ReferenceArea() * cl;
   // C     --TEST FOR UNDEFINED DATA
   if (fxl < -1.E7)
   {
      ut::log::info() << "Lift(alpha,speed,altitude) -> computed lift < -1.E7, so zero returned.";
      return 0.0;
   }
   return fxl;
}


double BrawlerMover::get_aoa(double drag)
{
   // C     --PURPOSE IS TO DO REVERSE INTERPOLATION FOR AOA
   // C  --local declarations
   double coef_drag = 0;
   double pm        = 0;
   double pa        = 0;
   int    im        = 0;
   int    ia        = 0;
   // C*ENDDEC
   coef_drag = drag / (DynamicPressure() * ReferenceArea());
   // C     --DO A REVERSE INTERPOLATION TO GET AOA_ZERO
   srch2(fmvec3, nfm3, Mach(), im, pm);
   if (im == 0)
   {
      // C        --THE VALUE TO BE INTERPOLATED IS OUTSIDE DATA TABLE LIMITS
      if (Mach() < fmvec3[0])
      {
         // C           --OFF THE LOW END
         im = 1;
         pm = 0.0;
      }
      else
      {
         // C           --OFF THE HIGH END
         im = nfm3 - 1;
         pm = 1.0;
      }
   }
   invtb2(cdrag, coef_drag, nfm3, im, pm, false, nalph2, ia, pa);
   return param1(ia, pa, alfv2);
}


void BrawlerMover::srch2(double* x, const int nx, double xint, int& ilow, double& p)
{
   int i     = 0;
   int ihigh = 0;
   int mx    = 0;
   // C*ENDDEC

   // entry srch2(x,nx,xint,ilow,p)
   if (nx <= 0)
   {
      nabort("SRCH2...nx <= 0");
   }
   // do 100 mx=nx,1,-1
   for (mx = nx; mx >= 1; mx += -1)
   {
      if (x[mx - 1] != undnum)
      {
         // goto 105  //solved
         break; // lbm - added
      }
   } // 100   continue

   // goto 60
   // 60
   if (mx == 0 || x[mx - 1] == undnum) // lbm - added
   {
      ilow = 0;
      return;
   }

   // 105   continue
   if (x[0] < x[mx - 1])
   {
      // C        --MONOTONICALLY INCREASING ARRAY
      // goto 5
      // 5     continue
      ilow  = 1;
      ihigh = mx;
      if (xint < x[ilow - 1] || xint > x[ihigh - 1])
      {
         ilow = 0;
         return;
      }

      while (true)
      {
         // 10    continue
         i = (ihigh + ilow) / 2;
         if ((xint - x[i - 1]) < 0)
         {
            // goto 20
            // 20    continue
            ihigh = i;
            if (ihigh == ilow + 1)
            {
               p = (xint - x[ilow - 1]) / (x[ihigh - 1] - x[ilow - 1]);
               return;
            }
            // goto 10;
         }
         else if ((xint - x[i - 1]) == 0)
         {
            // goto 30
            // 30    continue
            ilow = i;
            p    = 0.0;
            return;
         }
         else // (xint-x[i-1]) > 0
         {
            // goto 40
            // 40    continue
            ilow = i;
            if (ihigh == ilow + 1)
            {
               p = (xint - x[ilow - 1]) / (x[ihigh - 1] - x[ilow - 1]);
               return;
            }
            // goto 10;
         }
      }
   }
   // C     --MONOTONICALLY DECREASING ARRAY
   ilow  = 1;
   ihigh = mx;
   if (xint > x[ilow - 1] || xint < x[ihigh - 1])
   {
      // goto 160
      // 160   continue
      ilow = 0;
      return;
   }

   // 110   continue
   while (true)
   {
      i = (ihigh + ilow) / 2;
      // if(xint-x[i-1]) { //140,130,120
      if ((xint - x[i - 1]) < 0)
      {
         // 140   continue
         ilow = i;
         if (ihigh == ilow + 1)
         {
            // goto 150
            // 150   continue
            p = (xint - x[ilow - 1]) / (x[ihigh - 1] - x[ilow - 1]);
            return;
         }
         // goto 110
      }
      else if ((xint - x[i - 1]) == 0)
      {
         // 130   continue
         ilow = i;
         p    = 0.0;
         return;
      }
      else //(xint-x[i-1]) > 0
      {
         // 120   continue
         ihigh = i;
         if (ihigh == ilow + 1)
         {
            // goto 150
            // 150   continue
            p = (xint - x[ilow - 1]) / (x[ihigh - 1] - x[ilow - 1]);
            return;
         }
         // goto 110
      }
   }
   if (nx <= 0)
   {
      nabort("SRCH...nx <= 0");
   }
}


// C#ABSTRACT Returns calibrated airspeed in FT/SEC
// C#PURPOSE  Returns calibrated airspeed in FT/SEC
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  SPEED  REAL - ABSOLUTE SPEED
// CIN  ALT    REAL - ALTITUDE
// COUT CAS    REAL - CALIBRATED AIRSPEED
// C#TECHNICAL DESCRIPTION
// C  Looks up calibrated airspeed as a function of mach and altitude.
// C  This data was read off the charts found in T.O. 1F-16A-1-1, pages C1-18/19
// C  Calibrated airspeed is indicated airspeed corrected of instrumentation
// C  errors, and is independent of airframe.  Brawler only uses this speed
// C  to reference certain placarded speed limits.
// C#VARIABLE DESCRIPTIONS
// C  PR      REAL - PRESSURE RATIO
// C####
double BrawlerMover::cas(double speed)
{
   // C     --external declarations
   // double tbl2e;
   // C     --local declarations
   double dr   = 0;
   double p    = 0;
   double pr   = 0;
   double t    = 0;
   double vs   = 0;
   double rho  = 0;
   double h    = 0;
   int    nalt = 13;
   int    nmch = 3;
   // parameter (nalt=13, nmch=3)
   double cas_alt[13] =
      {0.0, 5000.0, 10000.0, 15000.0, 20000.0, 25000.0, 30000.0, 35000.0, 40000.0, 45000.0, 50000.0, 55000.0, 60000.0};
   double cas_mach[3] = {0.4, 1.0, 2.0};
   // real cas_tbl(nmch,nalt)/
   // double cas_tbl[3][13] = {
   //    { 438.82, 405.07, 371.31, 337.56, 303.80, 273.42, 246.42, 219.41, 194.09, 172.15, 151.90, 131.65, 118.14},
   //    { 1120.68, 1037.98, 962.03, 879.33, 806.76, 734.18, 664.98, 592.41, 529.96, 470.89, 421.94, 371.31, 330.80},
   //    { 2214.36, 2079.34, 1908.88, 1767.10, 1611.83, 1468.37, 1324.91, 1206.76, 1097.06, 992.41, 894.52, 810.13, 724.06}
   // };
   // LBM - FORTRAN stores double arrays differently than c++
   // transpose it for this replication, because:
   // most double array operations in brawler us the pointer and sum of indexes
   double cas_tbl[13][3] = {{438.82, 1120.68, 2214.36},
                            {405.07, 1037.98, 2079.34},
                            {371.31, 962.03, 1908.88},
                            {337.56, 879.33, 1767.10},
                            {303.80, 806.76, 1611.83},
                            {273.42, 734.18, 1468.37},
                            {246.42, 664.98, 1324.91},
                            {219.41, 592.41, 1206.76},
                            {194.09, 529.96, 1097.06},
                            {172.15, 470.89, 992.41},
                            {151.90, 421.94, 894.52},
                            {131.65, 371.31, 810.13},
                            {118.14, 330.80, 724.06}};

   // C*ENDDEC
   h = Altitude();
   if (h < 0.0)
   {
      if (h < -trkr_neg_alt)
      {
         // nabort("h < -trkr_neg_alt");
         DeleteMover();
         return 0;
      }
      else
      {
         h = 1.0;
      }
   }
   if (speed < 0.0)
   {
      nabort("speed < 0.0");
   }
   atmos(h, t, p, rho, vs, pr, dr);
   return tbl2e(speed / vs, h, &cas_tbl[0][0], cas_mach, nmch, cas_alt, nalt);
}


// C#ABSTRACT Returns the values of atmospheric variables given altitude
// C#PURPOSE  Returns the values of some aerodynamically important
// C atmospheric variables given an altitude
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  ALT    REAL - ALTITUDE
// COUT T      REAL - TEMPERATURE IN DEGREES RANKINE
// COUT PRES   REAL - PRESSURE
// COUT DENS   REAL - DENSITY
// COUT VS     REAL - SPEED OF SOUND
// COUT PR     REAL - PRESSURE RATIO
// COUT DR     REAL - DENSITY RATIO
// C#TECHNICAL DESCRIPTION
// C    Standard approximation formulas are used to obtain the output values.
// C####
void BrawlerMover::atmos(double alt, double& t, double& pres, double& dens, double& vs, double& pr, double& dr)
{
   // C     --local declarations
   double h    = 0;
   double fac  = 0;
   double tr   = 0;
   double c1   = 6.87535e-6;
   double c2   = 5.2561;
   double c3   = -4.80634e-5;
   double c6   = 5.4864e-04;
   double c7   = 353.97;
   double c8   = -34.1743;
   double c9   = -13.20114;
   double c10  = 11.20114;
   double alt1 = 36089.24;
   double alt2 = 65616.8;
   double alt3 = 104987.0;
   double alt4 = 154199.0;
   double alt5 = 167323.0;
   double alt6 = 232940.0;
   bool   warn = true;
   // save warn //TODO - does this make "warn" a static variable???
   // C*ENDDEC
   h = alt;
   if (h < 0.0)
   {
      if (h < -trkr_neg_alt)
      {
         // nabort("h < -trkr_neg_alt");
         ut::log::info() << "h < -trkr_neg_alt";
         DeleteMover();
         return;
      }
      else
      {
         h = 1.0;
      }
   }
   if (h <= alt1)
   {
      // C        --TROPOSPHERE CALCULATION
      fac = 1.0 - c1 * h;
      pr  = pow(fac, c2);
      dr  = pow(fac, (c2 - 1.0));
      tr  = fac;
   }
   else if (h <= alt2)
   {
      // C        --STRATOSPHERE CALCULATION
      h   = h - alt1;
      fac = exp(c3 * h);
      pr  = 0.223358 * fac;
      dr  = 0.29708 * fac;
      tr  = 0.75187;
   }
   else if (h <= alt3)
   {
      // C        --MESOSPHERE CALCULATION
      fac = (c6 * h + c7) * 0.0025643;
      tr  = fac * 0.751865;
      pr  = 0.054033 * pow(fac, (c8));
      dr  = 0.071867 * pow(fac, (c8 - 1.0));
   }
   else if (h <= alt4)
   {
      tr = 0.482561 + h / 337634.0;
      dr = pow((0.857003 + h / 190115.0), (c9));
      pr = pow((0.898309 + h / 181373.0), (c9 + 1));
   }
   else if (h <= alt5)
   {
      // C        --Thermoshpere
      tr  = 0.939268;
      fac = exp((h - alt4) / -25992.0);
      dr  = 0.00116533 * fac;
      pr  = 0.00109456 * fac;
   }
   else
   {
      // C        --CHECK TO SEE IF THE LIMITS OF THE ALGORITHM HAVE BEEN EXCEEDED
      if (h > alt6 && warn)
      {
         // write(*,*)'   ************** '
         // write(*,*)'ATMOS... EXCEEDED DEFINITION OF ATMOSPHERE AT ',h
         // write(*,*)'   no further warnings will be printed'
         // write(*,*)'   ************** '
         warn = false;
      }
      tr = 1.434843 - h / 337634.0;
      dr = pow((0.79899 - h / 606330.0), (c10));
      pr = pow((0.838263 - h / 577922.0), (c10 + 1));
   }
   // C     --Compute Atmosphere Terms
   t    = tsl * tr;
   pres = psl * pr;
   dens = dsl * dr;
   vs   = 49.020576 * sqrt(t);
}


// C#ABSTRACT RETURNS THRUST IN AIRCRAFT BODY AXES IN LBS BASED ON
// C   THROTTLE OR ENGINE STATE
// C#PURPOSE  RETURNS THRUST IN AIRCRAFT BODY AXES DUE TO GAS GENERATOR
// C   AND AFTERBURNER IN LBS BASED ON THROTTLE SETTING OR ENGINE STATE.
// C   THIS ROUTINE IS INTENDED TO BE USED IN SITUATIONS WHERE IT IS
// C   DESIRED TO COMPUTE THE THRUST FROM THE ENGINE.
// C   WITHOUT CONSIDERING THRUST REVERSAL OR DRAG DEVICES.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach.
// CIN  ALT    REAL - Altitude.
// CIN  THRTLX REAL - Throttle setting, 1.0=idle, 2.0=mil, 3.0=full ab.
// C           Value 0.0 implies a/c damaged, hence thrust set to small
// C           negative value.
// C           If -1.0 < thrtlx < 0.0, then thrust reversal desired.
// CIN  KACID  AC-IDX - Indicated a/c.
// COUT TGG    REAL - Gross thrust due to the gas generator.
// C           (Defined to act along aircraft body x-axis).
// C           If the thrust calculated is less than -1,000,000,000,
// C           tgg is returned as /par/undnum.
// COUT TAB    REAL - Gross thrust due to the afterburner.
// C           (Defined to act along aircraft body x-axis).
// C           If the thrust calculated is less than -1,000,000,000,
// C           tab is returned as /par/undnum.
// COUT ID     REAL - Inlet drag.
// C           (Defined to act along aircraft wind x-axis).
// CIN  FGG    REAL - Gas generator fraction. (THRST2 ENTRY)
// CIN  FAB    REAL - Afterburner fraction. (THRST2 ENTRY)
// C#TECHNICAL DESCRIPTION
// C  Makes /prform/ and /acphys/ current and calls function fthrst.
// C  See discussion in function fthrst for physics.
// C  NOTE: Drag devices are not considered.
// C####
void BrawlerMover::thrust(double thrtlx, double& tgg, double& tab, double& id)
{
   // C  --local declarations
   double fgg    = 0;
   double fab    = 0;
   double thrt   = 0;
   double trpd   = 0;
   bool   destrv = false;
   // C*ENDDEC
   if (thrtlx == 0.0)
   {
      // C        --NEGATIVE THRUST FOR THRLT=0 (ENGINE FAILURE) TO SIMULATE ADDED DRAG
      tgg = -1000.0 * (pow(Mach(), 2) + 1.0);
      tab = 0.0;
      id  = 0.0;
      return;
   }
   thrt = thrtlx;
   // gtypd(kacid,3); //no mem copies necessary
   if (trefgg > 0.0)
   {
      // C        --CAPABLE OF THRUST REVERSAL
      destrv = (thrt < 0.0);
   }
   else
   {
      // C        --NOT CAPABLE, RESET THROTTLE IF NECESSARY
      destrv = false;
      if (thrt < 0.0)
      {
         thrt = 1.0;
      }
   }
   thrdec(thrt, fgg, fab, trpd);
   thrst3(fgg, fab, tgg, tab, id, trpd, destrv);
}


double BrawlerMover::Thrust(double throttle, double speed, double altitude)
{
   if (throttle < 1.0 || throttle > 3.0)
   {
      ut::log::info() << "Thrust(throttle, speed, altitude) -> throttle outside of bounds [1,3], so returning zero.";
      return 0.0;
   }
   if (gas <= 0.0)
   {
      ut::log::info() << "Thrust(throttle, speed, altitude) -> no thrust because gas <= 0.0.";
      return 0.0;
   }
   double t, pres, dens, vs, pr, dr;
   atmos(altitude, t, pres, dens, vs, pr, dr);
   // double dypp = 0.5 * dens * speed * speed;
   double mach = speed / vs;
   if (mach < 0.0)
   {
      ut::log::info() << "Thrust(throttle, speed, altitude) -> mach < 0.0, so zero returned.";
      return 0.0;
   }
   // thrust()
   double fgg  = 0;
   double fab  = 0;
   double trpd = 0;
   thrdec(throttle, fgg, fab, trpd); // TODO - should we expand this here & use our own altitude & speed???
   if (fgg < 0.0 || fgg > 1.0 || fab < 0.0 || fab > 1.0)
   {
      ut::log::info() << "Thrust(throttle, speed, altitude) -> fgg or fab out of bounds [0,1], so returning zero.";
      return 0.0;
   }
   // thrst3()
   // fthrst()
   // fntthr()  //LBM - just using net thrust values (never have aero tables for gross thrust anyways)
   double GasGeneratorThrust = 0.0;
   double AfterBurnerThrust  = 0.0;
   double t1                 = 0;
   double t2                 = 0;
   double t3                 = 0;
   t1                        = tbl2(mach, altitude, tidle, mchval, nmach1, htvec, nhts);
   t2                        = tbl2(mach, altitude, tmil, mchval, nmach1, htvec, nhts);
   GasGeneratorThrust        = t1 + fgg * (t2 - t1);
   if (GasGeneratorThrust <= -1.0E9)
   {
      ut::log::info() << "Thrust(throttle, speed, altitude) -> GasGeneratorThrust <= -1.0E9, so returning zero.";
      return 0.0;
   }
   GasGeneratorThrust *= thrst_adj;
   if (fab > 0.0)
   {
      t3                = tbl2(mach, altitude, thrmx, mchval, nmach1, htvec, nhts);
      AfterBurnerThrust = fab * (t3 - t2) * ((1.0 - abcorl) + abcorl * fgg);
      if (AfterBurnerThrust <= -1.0e9)
      {
         ut::log::info() << "Thrust(throttle, speed, altitude) -> AfterBurnerThrust <= -1.0E9, so returning zero.";
         return 0.0;
      }
   }
   AfterBurnerThrust *= thrst_adj;
   // end fntthr()
   // end fthrst()
   // end thrst3()
   // end thrust()
   return (GasGeneratorThrust + AfterBurnerThrust);
}


double BrawlerMover::Throttle()
{
   // if (mThrottleFlag == true)
   //{
   //    return mThrottle;
   // }
   // else
   //{
   // }
   return tset; // actual current throttle for engine state
}


// C#ABSTRACT TRANSLATES A THROTTLE SETTING INTO EXPLICIT ENGINE SETTINGS.
// C#PURPOSE  TRANSLATES A THROTTLE SETTING INTO A GAS GENERATOR FRACTION,
// C  AN AFTERBURNER FRACTION, AND A PERCENT DEPLOYMENT FOR THRUST
// C  REVERSAL.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  THRTLX REAL - Throttle setting, 1.0=idle, 2.0=mil, 3.0=full ab.
// C           If -1.0 < thrtlx < 0.0, then assume mil power (thrtlx=2),
// C           and std::abs(thrtlx)= percentage of thrust reversal employed.
// COUT FGGC   REAL - Gas generator fraction.
// COUT FABC   REAL - Afterburner fraction.
// COUT TRPDC  REAL - Percentage of thrust reversal deployment.
// C#TECHNICAL DESCRIPTION
// C  Used the definition of a throttle setting to define the outputs.
// C#VARIABLE DESCRIPTIONS:  none
// C####
void BrawlerMover::thrdec(double thrtlx, double& fgg, double& fab, double& trpd)
{
   double throtl = 0;
   // C*ENDDEC
   throtl = thrtlx;
   if (throtl > 0.0)
   {
      chkrng(throtl, 1.0, 3.0, "throtl in thrdec");
      trpd = 0.0;
   }
   else
   {
      trpd   = std::abs(throtl);
      throtl = 2.0;
      chkrng(trpd, 0.0, 1.0, "trpd in thrdec");
   }
   if (throtl > 2.0)
   {
      fab = throtl - 2.0;
      fgg = 1.0;
   }
   else
   {
      fab = 0.0;
      fgg = throtl - 1.0;
   }
   // C
}


// see comment for thrust()
void BrawlerMover::thrst3(double fgg, double fab, double& tgg, double& tab, double& id, double trpd, bool destrv)
{
   // C  --local declarations
   double ftr = 0;
   // c*enddec
   chkrng(fgg, 0.0, 1.0, "fgg in thrst3");
   chkrng(fab, 0.0, 1.0, "fab in thrst3");
   fthrst(fgg, fab, tgg, tab, id);
   // C     --CHECK FOR CONTRIBUTION FROM THRUST REVERSAL
   if (destrv)
   {
      // C        --THRUST REVERSAL IS IN EFFECT
      ftr = trpd * trefgg * tgg;
      tgg = (1.0 - trpd) * tgg - ftr;
   }
   if (tgg == undnum || tab == undnum)
   {
      nabort("tgg == undnum || tab == undnum");
   }
}


// C#ABSTRACT CALCULATES GROSS THRUST DUE TO GAS GENERATOR, GROSS THRUST DUE
// C  TO AFTERBURNER AND INLET DRAG
// C#PURPOSE  CALCULATES GROSS THRUST DUE TO GAS GENERATOR (ALONG THE AIRCRAFT
// C  BODY X-AXIS), GROSS THRUST DUE TO AFTERBURNER (ALONG THE AIRCRAFT
// C  BODY X-AXIS) AND INLET DRAG (ALONG AIRCRAFT WIND X-AXIS).
// C  WITHOUT CONSIDERING THRUST REVERSAL OR DRAG DEVICES.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach.
// CIN  ALT    REAL - Altitude.
// CIN  FGG    REAL - Afterburner fraction.
// CIN  FAB    REAL - Gas generator fraction.
// COUT TGG    REAL - Gross thrust due to the gas generator.
// C           (Defined to act along aircraft body x-axis).
// C           If the thrust calculated is less than -1,000,000,000,
// C           tgg is returned as /par/undnum.
// COUT TAB    REAL - Gross thrust due to the afterburner.
// C           (Defined to act along aircraft body x-axis).
// C           If the thrust calculated is less than -1,000,000,000,
// C           tab is returned as /par/undnum.
// COUT ID     REAL - Inlet drag.
// C           (Defined to act along aircraft wind x-axis).
// C#TECHNICAL DESCRIPTION
// C  Gross thrust and net thrust are given as functions of Mach and
// C  altitude for throttle settings of idle, mil and maximum AB.
// C  Three way linear interpolations of the gross and net thrust tables
// C  are done to determine the magnitude of gross and net thrust.
// C  The inlet drag is calculated as the magnitude of gross thrust
// C  minus net thrust.
// C
// C  NOTE: Thrust reversal and drag devices are not considered.
// C        Use subroutine thrdec to decompose a throttle setting into
// C        fgg, fab, and trpd before calling this routine if necessary.
// C
// C  /Prform/ is assumed to be current.
// C  Table mach intervals are in /prform/mchval
// C  Table altitude intervals are constant; held in HTVAL so that tbl2
// C  can be used.
// C
// C#VARIABLE DESCRIPTIONS:  none
// C####
void BrawlerMover::fthrst(double fgg, double fab, double& tgg, double& tab, double& id)
{
   // c  --local declarations
   double tngg = 0;
   double tnab = 0;
   // C*ENDDEC
   chkrng(fgg, 0.0, 1.0, "fgg in fthrst");
   chkrng(fab, 0.0, 1.0, "fab in fthrst");
   // C     --GET NET THRUST
   fntthr(fgg, fab, tngg, tnab);

   ////C     --GET GROSS THRUST
   // fgrthr(fgg,fab,tgg,tab);
   ////C     --CALCUALTE INLET DRAG
   // id = (tgg-tngg) + (tab-tnab);
   ////C

   // LBM - no gross thrust stuff, just match net thrust values
   tgg = tngg;
   tab = tnab;
   id  = 0.0;
}


// C#ABSTRACT DOES A TABLE LOOKUP TO FIND THE MAGNITUDE OF NET THRUST DUE TO
// C  GAS GENERATOR AND AFTERBURNER
// C#PURPOSE  DOES A TABLE LOOKUP TO FIND THE MAGNITUDE OF NET THRUST DUE TO
// C  GAS GENERATOR AND AFTERBURNER
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach.
// CIN  ALT    REAL - Altitude.
// CIN  FGG    REAL - Gas generator fraction.
// CIN  FAB    REAL - Afterburner fraction.
// COUT TGG    REAL - Net thrust in the aircraft body axes from the
// C           gas generator.
// C           If the thrust calculated is less than -1,000,000,000,
// C           tgg is returned as /par/undnum.
// COUT TAB    REAL - Net thrust in the aircraft body axes from the
// C           afterburner.
// C           If the thrust calculated is less than -1,000,000,000,
// C           tab is returned as /par/undnum.
// C#TECHNICAL DESCRIPTION
// C  Thrust is given as a function of Mach and altitude for throttle
// C  settings of idle, mil power, and full ab.  This subroutine does a
// C  three-way linear interpolation on this table.  /Prform/ is assumed to
// C  be current.
// C  NOTE: Use subroutine thrdec to decompose a throttle setting into
// C        fgg, fab, and trpd before calling this routine if necessary.
// C
// C  Table mach intervals are in /prform/mchval
// C
// C  THIS CODE USED TO BE IN FTHRST.
// C#VARIABLE DESCRIPTIONS
// C  ITHROT  INT - Throttle interpolation index.
// C  DIF     REAL - Throttle interpolation difference.
// C  NM      INT - Mach interpolation index.
// C  XX      REAL - Mach interpolation difference.
// C  NA      INT - Altitude interpolation index.
// C  YY      REAL - Altitude interpolation difference.
// C####
void BrawlerMover::fntthr(double fgg, double fab, double& tgg, double& tab)
{
   // C  --local declarations
   double t1 = 0;
   double t2 = 0;
   double t3 = 0;
   // C*ENDDEC
   chkrng(fgg, 0.0, 1.0, "fgg in fntthr");
   chkrng(fab, 0.0, 1.0, "fab in fntthr");
   t1  = tbl2(Mach(), Altitude(), tidle, mchval, nmach1, htvec, nhts);
   t2  = tbl2(Mach(), Altitude(), tmil, mchval, nmach1, htvec, nhts);
   tgg = t1 + fgg * (t2 - t1);
   if (tgg <= -1.0E9)
   {
      tgg = undnum;
   }
   else
   {
      tgg = tgg * thrst_adj;
   }
   if (fab > 0.0)
   {
      t3  = tbl2(Mach(), Altitude(), thrmx, mchval, nmach1, htvec, nhts);
      tab = fab * (t3 - t2) * ((1.0 - abcorl) + abcorl * fgg);
   }
   else
   {
      tab = 0.0;
   }
   if (tab <= -1.0e9)
   {
      tab = undnum;
   }
   else
   {
      tab = tab * thrst_adj;
   }
}


void BrawlerMover::orient(UtFortMat3d& dircos, double& yaw, double& pitch, double& roll)
{
   // pitch = -asin(dircos[0][2]);
   pitch = -asin(dircos[6]);
   // if(std::abs(dircos[0][2]) == 1.0) {
   if (std::abs(dircos[6]) == 1.0)
   {
      // goto 10
      // 10    continue
      // C     --HERE PITCHED +- PI/2 - RESOLVE AMBIGUITY FOR ROLL (YAW=0)
      yaw = 0.0;
      // roll = atan2(sign(dircos[1][0], dircos[0][2]), dircos[1][1]);
      roll = atan2(sign(dircos[1], dircos[6]), dircos[4]);
      return;
   }
   // roll = atan2(dircos[1][2], dircos[2][2]);
   // yaw  = atan2(dircos[0][1], dircos[0][0]);
   roll = atan2(dircos[7], dircos[8]);
   yaw  = atan2(dircos[3], dircos[0]);
}


// C#ABSTRACT Adjusts roll and gees for p.r.-specified roll limits
// C#PURPOSE  To provide user control of the maximum roll angle of a/c
// C#TYPE     Aircraft Aerodynamics
// C#PARAMETER DESCRIPTIONS
// CIN  GW     3-VEC - Gravity vector in wind, in units of gees
// C           hence xmag(gw)=1.0.
// CIO  ROLDES REAL - on input, desired roll angle in wind system,
// C           on output adjusted roll angle.  Assumed between -180 and +180
// CIO  ALY    REAL - on input, desired y-component of wing loading in wind
// C           system. On output new desired y component of wing loading in
// C           wind.  Adjusted only if roll angle limit encountered.
// CIO  ALZ    REAL - on input, desired z-component of wing loading in wind
// C           system. On output new desired z component of wing loading in
// C           wind.  Adjusted only if roll angle limit encountered.
// CIN  PSI    REAL - Angle between current velocity vector and desired
// C           velocity vector.
// CIN  ROLLIM_A  real - additional roll limit based on maneuver urgency
// C#TECHNICAL DESCRIPTION
// C  First a check is performed to see if roll limits need to be applied.
// C  Then the desired roll angle in the earth system is calculated from
// C  the current roll angle (determined from gw) and the relative required
// C  roll (roldes).  If the desired roll angle exceeds the maximum allowed
// C  value then two cases are evaluated.
// C
// C  1) A net acceleration can be achieved in the original desired
// C     direction by rolling to the roll limit and applying reduced positive
// C     load.
// C
// C  2) A net acceleration can be achieved in the original desired
// C     direction by applying negative load.  This requires a roll in the
// C     opposite direction.  In this case the desired load need not be
// C     reduced.
// C
// C#VARIABLE DESCRIPTIONS
// C  KACID    AC-IDX [NA] - Index of current aircraft.
// C  rolle  real - Original desired roll in earth system; later becomes
// C         adjusted desired roll in earth system subject to rolmx_use limit
// C  rolnow real - Current roll angle in earth system
// C####
void BrawlerMover::rollim(UtVec3d& gw, double& roldes, double& aly, double& alz, double psi)
{
   // C  --local declarations
   double  rolle = 0;
   double  rollw = 0;
   UtVec3d yman(0.0, 0.0, 0.0);
   UtVec3d zman(0.0, 0.0, 0.0);
   UtVec3d uloadw(0.0, 0.0, 0.0);
   double  almag     = 0;
   double  rolnow    = 0;
   double  rolmx_use = 0;
   bool    limit     = false;

   // data yman[0]/0./,zman[0]/0./

   // C*ENDDEC

   rolmx_use = rolmx;
   if (rolmx_use >= pi)
   {
      ld2  = true;
      over = false;
      return;
   }
   // C     --GET CURRENT ROLL ORIENTATION IN EARTH SYSTEM
   rolnow = atan2(gw[1], gw[2]);
   limchk(rolnow, rolmx_use, mxexrl, mnonrl, tsv, tmacex, tmexrl, tmonrl, ld2, over, limit);
   if (!limit && psi >= rlthrs)
   {
      // C        --LIMITS MAY BE EXCEEDED AND WE WANT TO EXCEED THEM
      // C        --DO NOT APPLY LIMITS
      return;
   }
   // C     --HERE EITHER ROLL LIMITS ARE ENABLED OR THEY ARE DISABLED
   // C     --AND PSI<RLTHRS.  IMPOSE LIMITS.
   // C     --COMPUTE TOTAL DESIRED ROLL IN EARTH FRAME
   rolle = rolnow + roldes;
   // C     --ADJUST TO -180 +180
   if (rolle < -pi)
   {
      rolle = rolle + twopi;
   }
   else if (rolle > pi)
   {
      rolle = rolle - twopi;
   }
   // C     --CHECK ROLL IN EARTH SYSTEM
   if (std::abs(rolle) < rolmx_use)
   {
      return;
   }
   // C     --ROLL LIMIT EXCEEDED!
   // C     --WE ALWAYS WANT TO GENERATE A NET ACCELERATION IN THE CORRECT
   // C     --DIRECTION.  IT MAY BE NECESSARY TO REDUCE ITS MAGNITUDE
   // C     --DUE TO THE ROLL LIMIT.
   if ((std::abs(rolle) + rad) > (pi - rolmx_use))
   {
      // C        --DESIRED NET ACCELERATION CAN BE ACHIEVED BY ROLL IN OPPOSITE
      // C        --DIRECTION AND APPLYING NEGATIVE LOAD. NOTE THAT ALY AND ALZ
      // C        --ARE NOT MODIFIED.
      rolle = rolle - sign(pi, rolle);
      rollw = rolle - rolnow;
      if (rollw < -pi)
      {
         rollw = rollw + twopi;
      }
      else if (rollw > pi)
      {
         rollw = rollw - twopi;
      }
   }
   else
   {
      // C        --A REDUCED NET ACCELERATION CAN BE ACHIEVED IN THE ORIGINAL
      // C        --DIRECTION BY ROLLING TO THE LIMIT AND REDUCING LIFT
      rolle = sign(rolmx_use, rolle);
      // C        --MOVE ROLL FROM EARTH TO WIND
      rollw = rolle - rolnow;
      if (rollw < -pi)
      {
         rollw = rollw + twopi;
      }
      else if (rollw > pi)
      {
         rollw = rollw - twopi;
      }
      // C        --NOW REDUCE ALY, ALZ TO OBTAIN NET ACCELERATION IN ORIGINAL
      // C        --DESIRED DIRECTION.
      // C        --VECTOR IN WIND SYS. ALONG DESIRED NET ACC.
      zman[1] = aly + gw[1] * grav;
      zman[2] = alz + gw[2] * grav;
      // C        --vector yman perpendicular to desired net accel:
      // C        --formula for almag below sets net accel in this direction = 0
      yman[1] = zman[2];
      yman[2] = -zman[1];
      // C        --UNIT VEC IN WIND ALONG NEW LOAD VEC
      uloadw[1] = sin(rollw);
      uloadw[2] = -cos(rollw);
      almag     = -grav * (gw[1] * yman[1] + gw[2] * yman[2]) / (uloadw[1] * yman[1] + uloadw[2] * yman[2]);
      aly       = almag * uloadw[1];
      alz       = almag * uloadw[2];
   }
   roldes = rollw;
}


void BrawlerMover::limchk(double  value,
                          double  limit,
                          double  maxovr,
                          double  minund,
                          double  timnow,
                          double& toact,
                          double& timovr,
                          double& timund,
                          bool&   ld2,
                          bool&   over,
                          bool    limon)
{
   // C  ---local declarations:
   bool wasovr = false;
   bool posx   = false;
   bool negx   = false;
   bool ld1    = false;
   // C*ENDDEC
   // C     --DETECT CROSSINGS OF LIMIT AND RESET TIMOVR, TIMUND, TOACT,
   // C     --AND LD2 AS REQUIRED
   wasovr = over;
   over   = (std::abs(value) > limit);
   ld1    = (timnow < (maxovr + timovr));
   ld2    = ((timnow > (minund + timund)) && !over);
   posx   = (!wasovr && over);
   negx   = (wasovr && !over);
   if (posx)
   {
      // C        --POSITIVE CROSSING DETECTED HERE
      if (ld2)
      {
         // C           --COUNT THIS POSITIVE CROSSING
         timovr = timnow;
         ld2    = false;
      }
      toact = timnow;
   }
   else if (negx)
   {
      // C        --NEGATIVE CROSSING DETECTED HERE
      if (timnow - toact > 0.25)
      {
         // C           --WE HAVE BEEN UNDER FOR AT LEAST 0.25 SECONDS
         // C           --COUNT THIS NEGATIVE CROSSING
         timund = timnow;
      }
   }
   // C     --SET LIMITS
   if (over)
   {
      // C        --IF CURRENTLY OVER, APPLY THE CRITERIA THAT WE ARE
      // C        --NOT ALLOWED TO EXCEED THE LIMIT FOR MORE THAN MAXOVR SECONDS
      limon = !ld1;
   }
   else
   {
      // C        --IF CURRENTLY UNDER, APPLY THE CRITERIA THAT WE ARE
      // C        --NOT ALLOWED TO EXCEED THE LIMIT UNLESS WE HAVE BEEN UNDER FOR
      // C        --AT LEAST MINUND SECONDS
      limon = !ld2;
   }
   // C
}


// C#ABSTRACT ROTATES MATRIX ABOUT ARBITRARY AXIS
// C#PURPOSE  ROTATES MATRIX ABOUT ARBITRARY AXIS.  BECAUSE W IS
// C  SPECIFIED IN RBE-COORDINATES THIS ROUTINE IS CONVENIENT FOR
// C  SIMULTANEOUS ROLL, PITCH, YAW MOTION OF AN OBJECT
// C#TYPE     3-VECTOR MANIPULATION
// C#PARAMETER DESCRIPTIONS
// CIO  RBE    ORIENT - MATRIX TO ROTATE
// CIN  W      3-VEC - AXIS TO ROTATE ABOUT, EXPRESSED IN COORD SYSTEM
// C           SPECIFIED BY RBE
// CIN  WSQ    REAL - SQUARE OF MAGNITUDE OF W
// CIN  WMAG   REAL - MAGNITUDE OF W
// CIN  CHI    REAL - DESIRED ANGLE OF ROTATION
// C#TECHNICAL DESCRIPTION
// C  Transformation derived by going to and from a representation in
// C  which W is aligned with a major axis.
// C#VARIABLE DESCRIPTIONS
// C  T       ARRAY DIM(3,3) OF (REAL) - MATRIX TO PRE-MULTIPLY RBE BY
// C  RBEF    ORIENT - TEMPORARY STORAGE TO ALLOW RBE TO BE IO
// C         VARIABLE.
// C####
void BrawlerMover::udrbe(UtFortMat3d& rbe, UtVec3d& w, double wsq, double wmag, double chi)
{
   // C  --local declarations
   // double t[3][3];
   // double rbef[3][3];
   UtFortMat3d t    = UtFortMat3d();
   UtFortMat3d rbef = UtFortMat3d();
   double      a    = 0;
   double      b    = 0;
   double      ca   = 0;
   double      cb   = 0;
   double      val  = 0;
   // C*ENDDEC
   if (wmag < 1.e-5)
   {
      return;
   }
   if (std::abs(chi) < 1.e-10)
   {
      return;
   }
   // C
   ca   = sin(chi) / wmag;
   val  = sin(chi / 2.0);
   cb   = 2.0 * (val * val) / wsq;
   t[0] = 1.0 - cb * (wsq - w[1 - 1] * w[1 - 1]); // t(1,1)
   t[4] = 1.0 - cb * (wsq - w[2 - 1] * w[2 - 1]); // t(2,2)
   t[8] = 1.0 - cb * (wsq - w[3 - 1] * w[3 - 1]); // t(3,3)
   a    = ca * w[3 - 1];
   b    = cb * w[1 - 1] * w[2 - 1];
   t[3] = a + b;  // t(1,2)
   t[1] = -a + b; // t(2,1)
   a    = ca * w[2 - 1];
   b    = cb * w[1 - 1] * w[3 - 1];
   t[6] = -a + b; // t(1,3)
   t[2] = a + b;  // t(3,1)
   a    = ca * w[1 - 1];
   b    = cb * w[2 - 1] * w[3 - 1];
   t[7] = a + b;  // t(2,3)
   t[5] = -a + b; // t(3,2)
   // call mlt333(t,rbe,rbef);
   rbef = t * rbe; // multiply operator matches mlt333() perfectly

   // C     --re-normalize to prevent accumulation of errors
   // call cros1(rbef(1,1),rbef(1,2),rbe(1,3)); //rbe[2] = (rbef[0] X rbef[1]).normalized()
   // call cros1(rbe(1,3),rbef(1,1),rbe(1,2));  //rbe[1] = (rbe[2]  X rbef[0]).normalized()
   // call vnorm(rbef(1,1),rbe(1,1));           //rbe[0] = rbef[0].normalize()
   UtVec3d rbef0(rbef[0], rbef[1], rbef[2]);
   UtVec3d rbef1(rbef[3], rbef[4], rbef[5]);
   UtVec3d rbe2(0.0, 0.0, 0.0);
   rbe2.CrossProduct(rbef0, rbef1);
   rbe2.Normalize();
   UtVec3d rbe1(0.0, 0.0, 0.0);
   rbe1.CrossProduct(rbe2, rbef0);
   rbe1.Normalize();
   UtVec3d rbe0 = rbef0;
   rbe0.Normalize();
   rbe[0] = rbe0[0];
   rbe[1] = rbe0[1];
   rbe[2] = rbe0[2];
   rbe[3] = rbe1[0];
   rbe[4] = rbe1[1];
   rbe[5] = rbe1[2];
   rbe[6] = rbe2[0];
   rbe[7] = rbe2[1];
   rbe[8] = rbe2[2];
}


// C#ABSTRACT UPDATES AIRCRAFT POSITION AND VELOCITY
// C#PURPOSE  UPDATES A/C POS AND VEL DURING INTEGRATION
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  ACCBAR 3-VEC - AVERAGE ACCELERATION DURING INTEGRATION INTERVAL
// CIO  POS    3-VEC - A/C POSITION AT START AND END OF INTERVAL
// CIO  VEL    3-VEC - A/C VELOCITY AT START AND END OF INTERVAL
// CIN  DELT   REAL - TIME STEP
// CIN  RWEA   ORIENT - FINAL WIND AXIS SYSTEM
// C#TECHNICAL DESCRIPTION
// C    The subroutine is not completely straightforward because there
// C  is an inconsistency between the velocity obtained by using intial
// C  velocity and average acceleration and the direction of velocity
// C  obtained by rotating the wind axis system with indicated roll,
// C  pitch, and yaw rates (in wind axes).
// C    It was decided to use the average acceleration ACCBAR to
// C  determine a final velocity VELF.  This velocity is used, along
// C  with initial velocity and position, to update position.  However,
// C  its direction is then discarded, and a new velocity constructed
// C  using the magnitude of VELF and the direction of the wind axes'
// C  X-axis.
// C#VARIABLE DESCRIPTIONS
// C  VWF     3-VEC - FINAL VELOCITY IN WIND AXES (ONLY X-COMPONENT
// C         NON-Z
// C  VELBAR  3-VEC - AVERAGE VELOCITY DURING INTEGRATION INTERVA
// C####
void BrawlerMover::udxv(UtVec3d& accbar, UtVec3d& pos, UtVec3d& vel, double delt, UtFortMat3d& rwea)
{
   UtVec3d velf(0.0, 0.0, 0.0);
   UtVec3d vwf(0.0, 0.0, 0.0);
   UtVec3d velbar(0.0, 0.0, 0.0);
   // real    ACCBAR,POS   ,RWEA  ,VEL   ,VELBAR,VELF  ,VWF   ,XMAG
   // data vwf/3*0./
   // C*ENDDEC
   // call vecinc(vel,delt,accbar,velf);
   velf = vel + delt * accbar;
   // vwf(1)=xmag(velf);
   vwf[0] = velf.Magnitude();
   // call veclin(0.5,vel,0.5,velf,velbar);
   velbar = 0.5 * vel + 0.5 * velf;
   // call vecinc(pos,delt,velbar,pos);
   pos = pos + delt * velbar;
   // call vxfrmc(rwea,vel,vwf,2);
   vel = rwea.TransposeMultiply(vwf);
}


// C#ABSTRACT COMPUTES FUEL FLOW
// C#PURPOSE  COMPUTES FUEL FLOW
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach.
// CIN  ALT    REAL - Altitude.
// CIN  THRTLX REAL - Cockpit throttle setting.
// CIN  KACID  AC-IDX - A/C of interest.
// COUT FFLO   REAL - Fuel flow.  If computed value is less than -1, FFLO
// C           is returned as /par/undnum.  Units are slugs/sec, BEWARE!
// C#TECHNICAL DESCRIPTION
// C    Table interpolation on a table giving fuel flow as a function of
// C  Mach and altitude, for each of three throttle settings (idle, mil,
// C  max a.b.).  A two-dim interpolation of Mach and altitude is done
// C  for each bounding throttle setting, followed by a linear
// C  interpolation in throttle setting.  Note that this underestimates
// C  fuel usage for min a.b. setting.
// C  Note that argument to this function is cockpit throttle setting, not
// C  Gas generator fraction (fgg) and afterburner fraction (fab).
// C  If fflo is less than -1, fflo is returned as /par/undnum
// C  as an indication that undefined data was accessed.
// C  Table mach intervals are in /prform/mchval
// C#VARIABLE DESCRIPTION
// C####
double BrawlerMover::fflo(double thrtlx)
{
   // C  --local declarations
   int    ithrot = 0;
   double dif    = 0;
   double flo1   = 0;
   double flo2   = 0;
   double thrtl  = 0;
   // double fflo_max;
   double fflo = 0; // return value
   // C*ENDDEC
   thrtl = thrtlx;
   if (thrtl == 0.0)
   {
      return 0.0;
   }
   // C     --CHECK FOR THRUST REVERSER EMPLOYMENT => mil power
   if (thrtl <= 0.0 && thrtl >= -1.0)
   {
      thrtl = 2.0;
   }
   // C     --Limiting throttle...if a change is made here, make it also
   // C     --in subroutine THRUST
   if (thrtl > throtm)
   {
      // thrtl = throtm(kacid);
      thrtl = throtm;
   }
   chkrng(thrtl, 1.0, 3.0, "-thrtl in fflo-");
   // call gtypd(kacid,2);
   ithrot = int(thrtl);
   dif    = thrtl - double(ithrot);
   if (ithrot == 1)
   {
      flo1 = tbl2(Mach(), Altitude(), floidl, mchval, nmach1, htvec, nhts);
      flo2 = tbl2(Mach(), Altitude(), flomil, mchval, nmach1, htvec, nhts);
      fflo = flo1 + dif * (flo2 - flo1);
   }
   else if (ithrot == 2)
   {
      flo1 = tbl2(Mach(), Altitude(), flomil, mchval, nmach1, htvec, nhts);
      flo2 = tbl2(Mach(), Altitude(), flomax, mchval, nmach1, htvec, nhts);
      fflo = flo1 + dif * (flo2 - flo1);
   }
   else
   {
      fflo = tbl2(Mach(), Altitude(), flomax, mchval, nmach1, htvec, nhts);
   }
   if (fflo < -1.0)
   {
      fflo = undnum;
   }
   else
   {
      fflo = fflo * ff_adj;
   }
   return fflo;
}


// C#ABSTRACT GETS DYNAMIC PRESSURE AND MACH
// C#PURPOSE  GETS DYNAMIC PRESSURE AND MACH
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  SPEED  REAL - SPEED
// CIN  ALT    REAL - ALTITUDE
// COUT DYP    REAL - DYNAMIC PRESSURE AT SPEED, ALT
// COUT FMACH  REAL - MACH NUMBER AT SPEED, ALT
// C#TECHNICAL DESCRIPTION
// C    USES ATMOS CALL TO OBTAIN DENSITY DENS AND SPEED OF SOUND VS.
// C  THEN APPLIES FORMULAS THAT DEFINE MACH AND DYNAMIC PRESSURE IN
// C  TERMS OF SPEED, DENSITY, AND SPEED OF SOUND.
// C####
// void BrawlerMover::getqm(double speed)
void BrawlerMover::getqm(double speed, double& dypp, double& fmach)
{
   double dens = 0;
   double p    = 0;
   double pr   = 0;
   double t    = 0;
   // C*ENDDEC
   atmos(Altitude(), t, p, dens, vsme, pr, drme);
   dypp  = 0.5 * dens * speed * speed;
   fmach = speed / vsme;
}


// C#ABSTRACT CALCULATES DESIRED ENGINE STATE AND DESIRED DRAG DEVICE EMPLOYMENT
// C#PURPOSE  BASED ON A DESIRED AXIAL FORCE, DESENG CALCULATES THE DESIRED
// C  ENGINE STATE, THE DESIRED PERCENTAGE OF THRUST REVERSAL, AND
// C  THE DESIRED PERCENTAGE OF DRAG DEVICE DEPLOYMENT.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FREQ   REAL - Desired axial force (wind x-axis) associated with propulsion
// C           effects.  Includes all propulsion-associated forces such as
// C           inlet drag.  Current AOA is assumed.
// COUT FGGDES REAL - Desired gas generator fraction to obtain FREQ.
// COUT FABDES REAL - Desired afterburner fraction to obtain FREQ.
// COUT DDPDES REAL - Desired percentage of drag device deployment.
// COUT TRPDES REAL - Desired percentage of thrust reversal deployment.
// C#TECHNICAL DESCRIPTION
// C  Based on the input value of the desired axial force, this routine
// C calculates the steady state value of the throttle setting,
// C gas generator fraction, afterburner fraction, drag device fraction,
// C and thrust reversal fraction required to obtain the axial force.
// C   If the idle thrust exceeds the desired axial force by more than
// C 3500 pounds(about .1g for a typical fighter aircraft), then thrust
// C reversal and drag devices are considered in that order.
// C Thrust reversal is assumed to be used at mil power
// C since it is considered to be a transient maneuver and the pilot
// C will likely desire to speed up later.
// C If thrust reversal cannot generate an axial force small enough,
// C then drag devices are employed.
// C#VARIABLE DESCRIPTIONS:  none
// C  DYP      REAL [LBS/FT**2] - Dynamic pressure at present aircraft
// C           state.
// C  FM       REAL [MACH] - Mach number.
// C  KACID    AC-IDX [NA] - Index of current aircraft.
// C RAREA     REAL [FT**2] - Aircraft reference area.  Computed as a
// C         function of mach from /acphys/rfarea
// C####
void BrawlerMover::deseng(double freq, double& fggdes, double& fabdes, double& ddpdes, double& trpdes)
{
   // c  --local declarations
   double fxcess = 0;
   // double drgmx;
   double dfmax = 0;
   double tgg   = 0;
   double tab   = 0;
   double thr   = 0;
   double thr_prev;
   double id            = 0;
   double calf          = 0;
   double icalf         = 0;
   double grthr         = 0;
   double pdrag         = 0;
   double grthr_des     = 0;
   double ac_wt         = 0;
   double grthr_old     = 0;
   double pdrag_old     = 0;
   double dpdrag_dgrthr = 0;
   int    count         = 0;
   // C*ENDDEC
   ac_wt = amass * grav;
   calf  = cos(alfa);
   icalf = 1.0 / calf;
   // C     --SINCE THROT TAKES A GROSS THRUST ARGUMENT, MUST
   // C     --ITERATE TO ACCOUNT FOR PROPULSION DRAG.  The eqn to solve for
   // C     --grthr is: freq = grthr*cos(alfa) - pdrag, remembering that pdrag
   // C     --is a function of grthr.

   // 10    continue
   count = 0;
   // C     --Start with throttle that would be obtained by ignoring pdrag in eqn.
   // C     --Then compute pdrag at that throttle setting and try again,
   // C     --ignoring the fact that pdrag varies:
   thr = throt(freq * icalf);
   // call effth2(fm,-pos(3),thr,kacid,grthr_old,pdrag_old)
   effth2(thr, grthr_old, pdrag_old);
   // C     --No need for iteration if no significant propulsion drag:
   if (std::abs(pdrag_old) >= 0.001 * ac_wt)
   {
      grthr_des = (freq + pdrag_old) * icalf;
      thr       = throt(grthr_des);
      // call effth2(fm,-pos(3),thr,kacid,grthr,pdrag)
      effth2(thr, grthr, pdrag);
      // c     --Also no need for further iteration if propulsion drag is constant:
      if (std::abs(pdrag - pdrag_old) >= 0.001 * ac_wt)
      {
         // C
         while (true)
         {
            // 100   continue
            // c     --We can now improve the estimate by approximating the variation
            // c     --of pdrag wrt grthr by a first order expansion: we have enough
            // c     --information to estimate the first derivative.  This first order
            // c     --approximation can be solved for grthr_des:
            dpdrag_dgrthr = (pdrag - pdrag_old) / (grthr - grthr_old);
            // C     --CROGHAN -- Limit the step taken to no more than 1/2 a throttle setting
            dpdrag_dgrthr = xlimit(dpdrag_dgrthr, 0.5, -0.5);
            grthr_des     = (freq + pdrag - dpdrag_dgrthr * grthr) / (calf - dpdrag_dgrthr);
            // C     --Terminate iteration if converged or if thr doesn't change.  The
            // C     --latter accounts for cases where desired thrust isn't achievable.
            if (std::abs(grthr_des - grthr) < 0.01 * ac_wt)
            {
               break; // goto 200
            }
            thr_prev = thr;
            thr      = throt(grthr_des);
            if (thr == thr_prev)
            {
               break; // goto 200
            }

            count = count + 1;
            if (count > 20)
            {
               // C        --Abort
               nabort("deseng...excessive iteration count");
            }
            grthr_old = grthr;
            pdrag_old = pdrag;
            effth2(thr, grthr, pdrag);
            // goto 100
         }
      }
      // else { goto 200; }
   }
   // else { goto 200; }

   // C
   // 200   continue
   // C     --NOTE THAT THROT ALWAYS RETURNS A POSITIVE THROTTLE SETTING
   thrdec(thr, fggdes, fabdes, trpdes);
   ddpdes = 0.0;
   // C     --ARE ADDITIONAL TECHNIQUES NEEDED?
   thrst2(fggdes, fabdes, tgg, tab, id);
   fxcess = (tgg + tab - id * calf) - freq;
   // C     --TRY THRUST REVERSAL
   // C     --FIRST TEST IF I HAVE CAPBILITY
   if (fxcess > 3500.0 && trefgg > 0.0)
   {
      // C        --I HAVE THRUST REVERSAL CAPABILITY
      // C        --AND I NEED AT LEAST 3500 POUNDS
      // C        --AM I TRAVELLING TOO FAST TO EMPLOY?
      if (!tmlim || (tmlim && Mach() < trmm))
      {
         if (fabdes != 0.0)
         {
            // C              --LOGIC ERROR
            nabort("DESENG...logic error, fabdes should be 0");
         }
         ddpdes = 0.0;
         fggdes = 1.0;
         // C           --SPOOL UP TO MIL AND TRY
         thrst2(1.0, 0.0, tgg, tab, id);
         fxcess = (tgg - id * calf) - freq;
         dfmax  = (1.00 + trefgg) * tgg;
         if (dfmax < fxcess)
         {
            fxcess = fxcess - dfmax;
            trpdes = 1.0;
         }
         else
         {
            trpdes = fxcess / dfmax;
            fxcess = 0.0;
         }
      }
   }
   // LBM - no drag devices, for now
   ////C     --UTILIZE DRAG DEVICES
   // if(fxcess > 3500.0 && fm <= ddmm && ddmm > 0.0)
   //{
   //    drgmx = fdddrg(fm,-pos(3),dyp,rarea,1.0);
   //    if (drgmx != 0.0)
   //    {
   //       ddpdes = amin1(1.0,fxcess/drgmx);
   //    }
   //    else
   //    {
   //       ddpdes = 0.0;
   //    }
   // }
   // else
   {
      ddpdes = 0.0;
   }
   // C
}


// C#ABSTRACT COMPUTES THROTTLE POSITION AS FUNCTION OF THRUST
// C#PURPOSE  CALLED IF THROTTLE POSITION IS NEEDED
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FM     REAL - Mach.
// CIN  ALT    REAL - Altitude.
// CIN  THRST  REAL - Required thrust.
// CIN  ALPHA  REAL - Angle of attack.
// CIN  KACID  AC-IDX - Indicated a/c.
// COUT THROT  REAL - Throttle level that will yield thrst.
// C#TECHNICAL DESCRIPTION
// C  Since thrust is a piecewise linear function of throttle setting,
// C  a search is made to determine which linear segment yields the
// C  required thrust.  Once this is determined the appropriate piece is
// C  linearly interpolated to get the throttle setting.  If the requested
// C  thrust is less than idle thrust THROT=1 (idle) is returned.
// C  Similarly, if it is greater than max ab THROT=3 is returned.
// C####
double BrawlerMover::throt(double thrst)
{
   int    i     = 0;
   double thi   = 0;
   double tlo   = 0;
   double tset  = 0;
   double xhi   = 0;
   double xlo   = 0;
   double tgghi = 0;
   double tabhi = 0;
   double tgglo = 0;
   double tablo = 0;
   double idhi  = 0;
   double idlo  = 0;
   double throt = 0; // return value
   // C*ENDDEC

   // do 10 i=2,3
   for (i = 2; i <= 3; ++i)
   {
      // C        --FIND LOWEST INTEGER THROTTLE SETTING GIVING THRUST ABOVE THRST
      xhi = float(i);
      thrust(xhi, tgghi, tabhi, idhi);
      // C        --ADD SMALL EXTRA TERM FOR INCOMPLETE (=0) THRUST DATA
      // C        --AVOIDS ZERO DIVIDE ON THI-TLO
      thi = tgghi + tabhi + xhi / 100.0;
      if (thrst <= thi)
      {
         // goto 20
         // 20    continue
         xlo = xhi - 1.0;
         thrust(xlo, tgglo, tablo, idlo);
         tlo = tgglo + tablo;
         // C     --NEXT LINE ASSUMES TLO<THI AND THRST<=THI.  THE SECOND ASSUMPTION
         // C     --IS ASSERTED TO BE TRUE BY THE LOOP $10 LOGIC; THE FIRST
         // C     --ASSUMPTION IS THE REASON FOR THE TSET>3 TEST BELOW.
         tset = xlo + (thrst - tlo) / (thi - tlo);
         if (tset < 1.0)
         {
            // C        --IN THIS CASE REQUESTED THRUST THRST IS LOWER THAN IDLE
            // C        --THRUST; JUST RETURN IDLE
            tset = 1.0;
         }
         else if (tset > 3.0)
         {
            // C        --THIS IS A PRESUMED BAD INTERPOLATION ON THRUST TABLE;
            // C        --WE PROBABLY HAVE A DATA POINT SOMEWHERE THAT SIMPLY IS NOT
            // C        --FILLED IN.  AT HIGHER MACHS/ALTS, WE'VE SEEN THE MAX-AB TABLE
            // C        --ALL FILLED OUT, WHEREAS MIL-THRUST TABLE ENTRIES ARE ALL ZERO
            // C        -- = = = REMOVE THIS WRITE STATEMENT AT YOUR OWN RISK = = =
            // C        --YOU ARE RUNNING WITH BAD AERO DATA - YOU'D BETTER FIX IT!!!
            tset = 3.0;
         }
         throt = amin1(tset, throtm);
         return throt;
      }
   } // 10    continue

   // C     --CAN'T GET THRST.  RETURN THE MAX SETTING AVAILABLE.
   throt = throtm;
   return throt;
}


// C#ABSTRACT COMPUTES ACTUAL STATE OF ENGINE, DRAG DEVICES, AND THRUST
// C          REVERSERS FROM DESIRED ENGINE STATE.
// C#PURPOSE  TO CALCULATE ACTUAL STATE OF GAS GENERATOR, AFTERBURNER,
// C          THRUST REVERSAL AND DRAG DEVICES FROM DESIRED STATES AND
// C          RESPONSE CHARACTERISTICS.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FGGDES REAL - Desired gas generator fraction.
// CIN  FABDES REAL - Desired afterburner fraction.
// CIN  DDPDES REAL - Desired percentage of drag device deployment.
// CIN  TRPDES REAL - Desired percentage of thrust reversal deployment.
// CIN  DELT   REAL - Time step.
// C#TECHNICAL DESCRIPTION
// C  --ENGINE
// C  The model of engine spool-up(down) assumes linear deployment
// C  to desired state using a time delay for spool-up, DTGGUP, and
// C  a time delay for spool-down, DTGGDN. Afterburner response is
// C  characterized by an afterburner delay time ABDT which
// C  is the time between initiating a/b and fuel ignition, and an
// C  afterburner correlation factor which determines the degree to
// C  which thrust due to a/b is correlated with the state of the
// C  gas generator.
// C  --DRAG DEVICES
// C  The fraction of drag device deployment is characterized
// C  by DDTD, the time to move from 0% to 100% deployment.  Deployment
// C  proceeds linearly towards the desired fraction of deployment.
// C  --THRUST REVERSERS
// C  The fraction of thrust reversers actually deployed is characterized
// C  by TRTD, the time to move from 0% to 100% deployment.  Deployment
// C  proceeds linearly towards the desired fraction of deployment.
// C#VARIABLE DESCRIPTIONS
// C  LOUT     INT - Terminal output
// C  DDPDSV   REAL [NA] - Percentage of drag device deployment.
// C  FABSV    REAL [NA] - Afterburner fraction (0-1) Reflects actual
// C           afterburner state between min ab and full ab.
// C  FGGSV    REAL [NA] - Gas generator fraction (0-1) Reflects actual
// C           engine state between idle and mil power.
// C  KACID    AC-IDX [NA] - Index of current aircraft.
// C  TRPDSV   REAL [NA] - Percentage of thrust reversal deployment.
// C  TSET     REAL [NA] - Cockpit throttle setting; acutal engine state may
// C           be different due to delays, time constants, etc. (1-3).
// C  TSV      REAL [SEC] - Time of the state vector.
// C  TDLY12  REAL - TIME TO GET FROM TSET=1 TO TSET=2
// C  TDLY23  REAL - TIME TO GET FROM MIN TO MAX A.B.
// C  TDLYAB  REAL - TIME TO TURN ON A.B.
// C  T0      REAL - TIME FROM IDLE VALUE CORRESPONDING TO TSET0.
// C  TMAX    REAL - TIME FROM IDLE REACHABLE IN DELT (INCREASING)
// C  TSETMX  REAL - THROTTLE SETTING CORRESPONDING TO TMAX
// C  TMIN    REAL - TIME FROM IDLE REACHABLE IN DELT (DESCENDING)
// C####
void BrawlerMover::stenst(double fggdes, double fabdes, double ddpdes, double trpdes, double delt)
{
   double fgg0  = 0;
   double fab0  = 0;
   double trpd0 = 0;
   // double ddpd0 = 0;
   // double faxreq;
   double ftreff = 0;
   // double fddeff;
   double dtgas = 0;
   // C*ENDDEC
   // C     --SAVE CURRENT ENGINE STATE
   fgg0  = fggsv;
   fab0  = fabsv;
   trpd0 = trpdsv;
   // ddpd0  = ddpdsv;
   // C     --SET /svwork/tset
   tset = 1.0 + fggdes + fabdes;
   // C     --GAS GENERATOR FRACTION ALWAYS DECAYS TO DESIRED VALUE
   if (fggdes > fgg0)
   {
      dtgas = dtggup;
   }
   else
   {
      dtgas = dtggdn;
   }
   if (fggdes == fgg0)
   {
      fggsv = fggdes;
   }
   else
   {
      fggsv = fgg0 + (fggdes - fgg0) * amin1(delt / (std::abs(fggdes - fgg0) * (dtgas + .001)), 1.0);
   }
   // C     --CALCULATE AFTERBURNER FRACTION
   if (fabdes == 0.0)
   {
      // C        --DON'T WANT AB
      fabsv = 0.0;
      ttset = 0.0;
   }
   else
   {
      // C        --WANT AB
      if (fab0 > 0.0)
      {
         // C           --ALREADY IN AB, DUMP FUEL INSTANTLY
         fabsv = fabdes;
      }
      else
      {
         // C           --WANT AB BUT NOT ON YET
         if ((delt + ttset) > abdt)
         {
            // C              --DELAY EXPIRES IN THIS INTERVAL
            ttset = 0.0;
            fabsv = fabdes;
         }
         else
         {
            // C              --INCREMENT AB TIMER
            ttset = ttset + delt;
            fabsv = 0.0;
         }
      }
   }

   // C     --THRUST REVERSAL
   if (trpdes == trpd0 || trtd == 0.0)
   {
      trpdsv = trpdes;
   }
   else
   {
      ftreff = amin1((delt / (std::abs(trpdes - trpd0) * trtd)), 1.0);
      trpdsv = trpd0 + (trpdes - trpd0) * ftreff;
   }
   // C     --CAN'T USE IF AFTERBURNER IS ON
   if (tablim && trefgg > 0.0 && fab0 > 0.0)
   {
      trpdsv = 0.0;
   }

   // LBM - no drag deployment devices, for now
   ddpdsv = 0.0; // LBM for now
   // if(ddpdes == ddpd0 || ddtd == 0.0)
   //{
   //    ddpdsv = ddpdes;
   // }
   // else
   //{
   //    fddeff = amin1(delt/(std::abs(ddpdes-ddpd0)*ddtd),1.0);
   //    ddpdsv = ddpd0 + (ddpdes-ddpd0)*fddeff;
   // }
}


// see comment for effthr()
void BrawlerMover::effth2(double thrtlx, double& efgrth, double& efpd)
{
   // C  --local declarations
   double fggc  = 0;
   double fabc  = 0;
   double trpdc = 0;
   // c*enddec
   thrdec(thrtlx, fggc, fabc, trpdc);
   effthr(fggc, fabc, trpdc, 0.0, efgrth, efpd);
}


// C#ABSTRACT CALCULATES EFFECTIVE GROSS THRUST AND EFFECTIVE PROPULSION
// C  DRAG DUE TO THRUST, INCLUDING THRUST REVERSAL AND DRAG DEV.
// C#PURPOSE ROUTINE CALCULATES EFFECTIVE GROSS THRUST AND EFFECTIVE
// C  PROPULSION DRAG AS FUNCTION OF CURRENT ENGINE STATE, DEPLOYMENT
// C  FRACTION FOR THRUST REVERSAL, AND DEPLOYMENT FRACTION FOR DRAG DEVICES.
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  FMACHC REAL - Aircraft Mach.
// CIN  ALT    REAL - Aircraft altitude.
// CIN  FGGC   REAL - Gas generator fraction.
// CIN  FABC   REAL - Afterburner fraction.
// CIN  TRPDC  REAL - Percentage of thrust reversal deployment.
// CIN  DDPDC  REAL - Percentage of drag device deployment.
// CIN  LACID  INT - Aircraft id.
// COUT EFGRTH REAL - Effective gross thrust.
// C           (Acts along aircraft body x-axis.)
// COUT EFPD   REAL - Effective propulsion drag.
// C           (Acts along aircraft wind x-axis.)
// C#TECHNICAL DESCRIPTION
// C  Calculates the effective gross thrust (body axes) taking into
// C  account the engine, thrust reversers, and use of drag devices.
// C  The effective gross thrust includes contributions from the
// C  gas generator, afterburner and thrust reversers.  Effective
// C  propulsion drag includes contributions from inlet drag and
// C  drag devices.
// C#VARIABLE DESCRIPTIONS:  none
// C####
void BrawlerMover::effthr(double fggc, double fabc, double trpdc, double ddpdc, double& efgrth, double& efpd)
{
   // C  --local declarations
   double ftr    = 0;
   double tgg    = 0;
   double fggi   = 0;
   double fabi   = 0;
   double fabmax = 0;
   double fggmax = 0;
   double tab    = 0;
   // double ddd;
   double id = 0;
   // c*enddec
   // C     --SET LIMITS TO FGGI AND FABI
   // C     --The following two lines assume 1.0 <= throtm <= 3.0.  A more careful
   // C     --(but slower) implementation would use xlimit instead of amax1 or amin1
   fabmax = amax1(throtm - 2.0, 0.0);
   fggmax = amin1(throtm - 1.0, 1.0);
   fabi   = xlimit(fabc, fabmax, 0.0);
   fggi   = xlimit(fggc, fggmax, 0.0);
   // C     --COMPUTE TGG AND TAB, THE THRUST COMPONENTS FOR THE GAS
   // C     --GENERATOR AND AFTERBURNER CONTRIBUTIONS, and the inlet drag.
   thrst2(fggi, fabi, tgg, tab, id);
   // C     --CONTRIBUTION FROM THRUST REVERSAL
   // C     --APPLY THRUST REVERSAL TO GROSS THRUST FROM GAS GENERATOR
   if (trefgg == 0.0 || trpdc == 0.0)
   {
      // C         --THRUST REVERSAL IS OFF OR EFFECTIVENESS IS ZERO
      efgrth = tgg + tab;
      ftr    = 0.0;
   }
   else
   {
      ftr    = trpdc * trefgg * tgg;
      efgrth = (1 - trpdc) * tgg + tab - ftr;
   }

   // LBM - no drag devices
   // C     --CONTRIBUTION FROM DRAG DEVICES
   // ddd  = fdddrg(ddpdc);
   // efpd = ddd + id;

   efpd = id;
}


// see comment for thrust()
void BrawlerMover::thrst2(double fgg, double fab, double& tgg, double& tab, double& id)
{
   // gtypd(kacid,3); //no mem copies necessary
   // C     --Call with request for no thrust reversal
   thrst3(fgg, fab, tgg, tab, id, 0.0, false);
}

void BrawlerMover::SetMaxThrottle(double aThrottle)
{
   throtm = aThrottle;
}
