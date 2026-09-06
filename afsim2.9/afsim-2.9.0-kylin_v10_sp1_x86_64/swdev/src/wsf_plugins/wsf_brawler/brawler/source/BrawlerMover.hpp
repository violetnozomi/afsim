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

#ifndef BRAWLERMOVER_HPP
#define BRAWLERMOVER_HPP

#include "brawler_export.h"

#include <algorithm>
#include <bitset>
#include <string>
#include <vector>

#include "BrawlerPlatform.hpp"
#include "UtEntity.hpp"
#include "UtFortMat3.hpp"
#include "UtLog.hpp"
#include "UtVec3.hpp"

// NOTE - these values are never initialized, they are for reference only
//#include "acphys.h"     //somebody else's conversion of acphys.fi
//#include "par.h"        //somebody else's conversion of par.fi
//#include "pcon.h"       //my initial attempt at converting pcon.fi (following style in par.h)
//#include "typdat.h"     //somebody else's conversion of typdat.fi
//#include "extst.h"      //somebody else's conversion of extst.fi
//#include "acstat.h"     //somebody else's conversion of acstat.fi
//#include "svwork.h"   //somebody else's conversion of svwork.fi

/******************************************************************************
 *  OOP representation of a Brawler aircraft mover.
 *  Specifically to replicate this call tree:
 *  NOTE: methods that get/set/update common blocks are left out
 *        all values stored in the class member variables now
 *     perfrm()
 *     flyac()
 *        flyefficient()    //for straight flight
 *        flacti()
 *        flyac_short()     //for short time steps
 *        flact0()
 *        acarea()          //computes aircraft reference area
 *        getdt()           //timestep for integration of equations of motion
 *        rates()           //computes new yaw, pitch, & roll rates
 *           ratmx1()       //aero algorithm type 1
 *           gwreq()        //converts commanded angular rates
 *           aoa()          //computes current alpha
 *        flact1()
 *           xlift()        //computes current lift
 *           udrbe()        //updates orientation
 *           drag2()
 *           seteng()       //computes new engine settings
 *              desaxf()    //convert commanded axial forces
 *              deseng()    //translates desired forces into engine settings
 *              stenst()    //computes actual engine settings from desired
 *           effthr()       //computes effective thrust & drag
 *              thrdec()    //translate throttle settings into engine settings
 *              fthrst()    //thrust
 *                 fntthr() //net thrust
 *           udxv()            //updates aircraft position/velocity
 *           fflo()            //computes current rate of fuel flow
 *     aproj3()             //projects a maneuver forward in time for alternative evaluation
 *        allim()
 *        laccel()
 *        prjacc()
 *        spdtoa()
 *        thstoa()
 *        manwnd()
 ******************************************************************************/
class BRAWLER_EXPORT BrawlerMover
{
protected:
   BrawlerMover(const BrawlerMover& aSrc);

public:
   BrawlerMover();
   BrawlerMover& operator=(const BrawlerMover&) = delete;
   virtual ~BrawlerMover();
   virtual bool     LoadAeroConfig(const std::string& absolutePathFile);
   virtual bool     Initialize(double aCreationTime, UtEntity& aStartingState, BrawlerPlatform* aPlatformPtr);
   virtual bool     UpdateState(double toSimTime); // calls perfrm() and flyac()
   virtual UtEntity State(double aSimTime);

   virtual void DeleteMover() = 0;

   // Update the amount of fuel used un a timestep based on the throttle
   // and the flow at the beginning of the timestep
   virtual void UpdateFuel(double aTimeStep, double aInitialFlow, double aThrottle, double& aFuelQuantity, double& aVehicleMass);

   // virtual void UpdateLocal(double toSimTime, UtVc3d& AngularRates, UtVec3d& AxialAccel, double Speed) {
   //    UpdateOrientation(toSimTime, AngularRates);
   //    UpdateTranslation(toSimTime, AxialAccel, Speed);
   // }
   // virtual void UpdateOrientation(double deltaTime, UtVec3d& AngularRates);            //1st part of flact1() BEFORE
   // it calls seteng() virtual void UpdateTranslation(double deltaTime, UtVec3d& AxialAccel, double Speed); //2nd part
   // of flact1() AFTER  it calls seteng()

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   // C++ OOP versions of brawler values:
   ////////////////////////////////////////////////////////////////////////////

   // direct queries of constants, limits, or state
   // svwork
   virtual double       PlatformTime() { return tsv; } // seconds?
   virtual UtVec3d&     Position() { return pos; }     // pos[3] nautical miles NED ???
   virtual double       Altitude() { return -Position()[2]; }
   virtual UtVec3d&     Velocity() { return vel; }                   // vel[3]  units???
   virtual UtVec3d&     Acceleration() { return acc; }               // acc[3]  units???
   virtual UtFortMat3d& EarthToBodyRotationMatrix() { return rbea; } // units?
   // virtual double* EarthToWindRotationMatrix() { return &rwea[0][0]; }   //units?
   virtual UtFortMat3d& EarthToWindRotationMatrix() { return rwea; } // units?
   // virtual double* AngularRates() { return wnow; }  //wnow[3]  units???
   virtual UtVec3d& AngularRates() { return wnow; }
   virtual double   Alpha() { return alfa; }      // radians
   virtual double   AlphaCL0() { return alpha0; } // radians
   virtual double   AngleOfAttackRate() { return alfdot; }
   virtual double   AircraftMass() { return amass; } // total mass, including fuel in slugs
   virtual double   EmptyMass() { return barems; }   // slugs
   virtual double   Speed() { return spd; }
   virtual double   TurnRadius() { return turn; } // units???
   // double wsqr;       //magnitude of axis to rotate about - squared
   // double rate;       //magnitude of axis to rotate about
   virtual double Mach() { return fm; } // mach number
   virtual double DynamicPressure() { return dyp; }
   virtual double LagFilterFreqPitchRate() { return omegp; } // 1st order lag filter frequency for pitch rates
   virtual double LagFilterFreqRollRate() { return omegq; }  // 1st order lag filter frequency for roll rates
   virtual double RollRateMax() { return pmx; }              // max roll RATE limit
   virtual double PitchRatePositiveMax() { return qmxpos; }  // max positive pitch RATE limit
   // double rlthrs;     //some large roll threshold for limit checking (180*rad ???)
   virtual double RollMax() { return rolmx; }       // max allowed roll limit
   virtual double ReferenceArea() { return rarea; } // aircraft reference area (sq feet)
   virtual double Gas() { return gas; }             // fuel mass, slugs

   // queries for calculated values:
   virtual double MaxAllowedThrottle() { return throtm; } // value 1-3 ?
   virtual double CoefficientOfLift(double mach, double alpha);
   virtual double CoefficientOfDrag(double mach, double alpha);
   virtual double Drag2() { return drag2(alpha0); }                  // difference between this & Drag()?
   virtual double Drag(double alpha, double speed, double altitude); // deg, ft/sec, ft
   virtual double Lift() { return xlift(Alpha()); }
   virtual double Lift(double alpha, double speed, double altitude); // deg, ft/sec, ft
   virtual double LiftFromAlpha(double alpha) { return xlift(alpha); }
   virtual double AngleOfAttackFromDrag(double drag)
   {
      return get_aoa(drag);
   }; // reverse lookup from CD table using given drag and current mach
   virtual double AngleOfAttackFromLift(double lift) { return aoa(lift, false); };
   // virtual double AngleOfAttackFromLift(double lift) { return faoa(lift, false); };
   virtual double CalibratedAirSpeed() { return cas(Speed()); }
   //! Returns specific energy rate calculated in aproj3
   virtual double SpecificEnergyRate() { return erate; }
   // RETURNS THRUST IN AIRCRAFT BODY AXES IN LBS (DUE TO GAS GENERATOR AND AFTERBURNER) BASED ON THROTTLE OR ENGINE
   // STATE DOES NOT CONSIDER THRUST REVERSAL OR DRAG DEVICES.
   virtual double Thrust(double throttle) // throttle setting in [1,3]
   {
      double GasGeneratorThrust, AfterBurnerThrust, InletDrag;
      thrust(throttle, GasGeneratorThrust, AfterBurnerThrust, InletDrag);
      return (GasGeneratorThrust + AfterBurnerThrust);
   }
   virtual double Thrust(double throttle,  // throttle setting only in [1,3] (no thrust reverser or engine failure
                                           // supported in this query)
                         double speed,     // ft/sec
                         double altitude); // ft
   virtual double Throttle();              // current throttle setting (tricky if commanded maneuver doesn't specify)

   // RETURNS Longitudinal force due to thrust, thrust reverser, and drag devices.
   // DOES NOT INLCUDE PROPULSION DRAG.
   //(Effective gross thrust acting along aircraft body x-axis)
   virtual double AxialForceX()
   {
      double faxial, xl, flow, efpd;
      flacti(faxial, xl, flow, efpd); // go deeper? call effthr() instead?
      return faxial;
   }
   virtual double InletDrag(double throttle) // going to be zero, not using gross thrust calculations (usually same as net thrust)
   {
      double GasGeneratorThrust, AfterBurnerThrust, InletDrag;
      thrust(throttle, GasGeneratorThrust, AfterBurnerThrust, InletDrag);
      return InletDrag;
   }
   virtual UtVec3d VelocityWindAxes(void);
   virtual UtVec3d GravityWindAxes(void);
   virtual UtVec3d AccelerationWindAxes(void);
   ////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   // maneuver command processing
   ////////////////////////////////////////////////////////////////////////////
   enum CommandType
   {
      RATES                  = 1,
      HEADING_SPEED_ALTITUDE = 2,
      PLANE_GEES_SPEED       = 3,
      VECTOR_GEES            = 4,
      ALSO_COMMAND_ALPHA     = 9
   };

   // replicate pieces of the flyac() logic to compute command rates/forces
   // bool GetCommandedRates(double aSimTime, UtVec3d& AngularRates, UtVec3d& AxialAccel, double& Speed); //calls
   // gwreq() and desaxf()

   void CommandRates(UtVec3d& aAngularRates, UtVec3d& aLongitudinalAccel);
   void CommandHeadingSpeedAltitude(double aHeading,   // degrees
                                    double aSpeed,     // feet/sec
                                    double aAltitude,  // feet
                                    double aMaxGees,   // Gs or ft/sec^2 ???
                                    double aMaxClimb); // feet/sec
   void CommandPlaneManeuver(const UtVec3d& aPlane, double aGees, double aSpeed, bool aThrottleFlag = false);
   void CommandVector(UtVec3d& aVelocityVector,
                      double   aMaxGees,
                      double   aSpeed, // feet/sec
                      bool     aThrottleFlag = false);
   void CommandLevelTurn(double aHeading, double aMach, double aGees);
   void CommandSlice(double aHeading, double aAltitude, double aRunMach);

   // C++ equivalents of svwork.fi COMMAND values (e.g.  wdes, spdreq, greq, etc...)
   //     (the equivalents of svwork.fi STATE values are further below)
   //                             //SVWORK:         //ALTHLD:
   UtVec3d     mAngularRates;   // wdes            //a0
   UtVec3d     mAxialAccel;     // accreq          //al0
   UtVec3d     mPlane;          // areqt           //a0
   UtVec3d     mVelocityVector; // direq           //dir0
   double      mHeading;        // hedreq          //dir0
   double      mAltitude;       // altreq          //hgt0
   double      mSpeed;          // spdreq          //spd0         (only use if can mThrottleFlag == false), ft/s
   double      mThrottle;       // tsetmn          //al0 / spd0   (only use if can mThrottleFlag == true )
   double      mMaxGees;        // greq / areqtm   //gmx
   double      mMaxClimb;       // rocreq          //clmbmx
   CommandType mCommandType;    // jacton          //iactn
   bool        mThrottleFlag;   // spdrmd          //accmod / spdmod

   ////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////
   //#include "svwork.fi"
   ////////////////////////////////////////////////////////////////////////////
   // desired high level aero commands:
   // int jacton;       //action type (aero command type) //use: mCommandType
   // command type: jacton == 1
   // int spdrmd;       //speed/throttle flag
   // UtVec3d wdes;    //desired angular rates [roll,pitch,yaw] //use: mAngularRates
   // double tsetmn;     //throttle  //use: mThrottle
   // UtVec3d accreq;  //acceleration required (axial force?)  //use: mAxialAccel
   // command type: jacton == 2
   // double hedreq[2];  //heading required (north/east vectors???)
   // double spdreq;     //speed required
   // double altreq;     //altitude required
   // double rocreq;     //max rate of climb
   // double greq;       //max gees  //use: mMaxGees
   // command type: jacton == 3
   // double areqt[3];   //maneuver plane  //use: mPlane
   // double areqtm;     //max gees //use: mMaxGees
   // double tsetmn;     //above
   // command type: jacton == 4
   // UtVec3d direq;   //use: mVelocityVector (direction vector, world coordinates (NED brawler))
   // double greq;       //above
   // int spdrmd;       //above
   // double tsetmn;     //above
   // double spdreq;     //above
   ////////////////////////////////////////////////////////////////////////////
   void prlevl(double aGees, double aTurn, double aRoc, double aHeading, double aMach);
   void forcev(double spd, std::string spdmod, double headng, double rclimb);
   void dirct(double spddes, std::string spdmd, double rolds, std::string rolmd, double geedes);
   void SetMaxThrottle(double aThrottle);
   void aproj3(double timeDelta, UtEntity& proj);
   void allim(double aldes, double alpha, double cosalf, double gx, double acm, double& along, double& thrust);
   void grthr(double aldes, double drag, double cosaoa, double gx, double& grosst, double& epd);
   void laccel(double&            along,
               double             almin_use,
               UtVec3d&           atrans,
               const UtVec3d&     v,
               double             spd,
               const UtFortMat3d& rbe,
               double             acm,
               double             amax,
               double&            atload,
               double&            erate);
   void prjacc(const UtVec3d& xin, const UtVec3d& vin, const UtVec3d& a, double dt, UtVec3d& xout, UtVec3d& vout);
   void spdtoa(double         spd0x,
               double         spdtmp,
               double         dspeed,
               const UtVec3d& vdes,
               double         dt,
               double         gmax,
               const UtVec3d& vtemp,
               double&        aldes,
               UtVec3d&       atrans);
   void thstoa(double         gx,
               double         frclon,
               const UtVec3d& dir0,
               const UtVec3d& vtemp,
               double         spdtmp,
               double         dt,
               double         gmax,
               double&        aldes,
               UtVec3d&       atrans);
   void manwnd(const UtVec3d& v, const UtVec3d& a, UtFortMat3d& rw, double& al, double& at);

   ////////////////////////////////////////////////////////////////////////////
   // main maneuvering command interpretation logic
   ////////////////////////////////////////////////////////////////////////////
   // updates derived performance values
   void   perfrm();
   double corner(double alt, double vs, double wate);

   // flies the aircraft (main brawler platform update method)
   // void flyac(double toTime, BrawlerPlatform& plat);
   void flyac(double toTime);
   // rates() - computes actual angular rates at end of time step
   // void rates(double aSimTime, double wact[3]);
   void rates(double&   xl,
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
              double&   aldact);
   // gwreq() - computes desired angular rates from current command type
   //     parameter: wnext  - return angular rates
   //     parameter: spddes - return speed, used by flyac() -> flact1() -> seteng()
   //     parameter: aoades - not used for arotyp1
   //     parameter: gavdfl - ground avoidance situation, used to limit gmax in rates() for modifying pitch rate
   void gwreq(double& dt, double thrst, double tend, UtVec3d& aw, UtVec3d& wnext, double& spddes, double& aoades, bool& gavdfl);
   // computes other values for time step after angular rates and alpha are known
   void flact1(UtVec3d& wnext,
               double&  d /*drag*/,
               double   delt,
               double&  flow,
               double   spddes,
               double&  faxial,
               double&  wate,
               double&  xl,
               double&  efpd);
   // determines current engine state
   void seteng(double delt, double drg, double spddes, double tsmn);
   // computes desired axial force (forward) from current command type
   // void desaxf(double drg, double spddes, double tsmn, double& axfreq);
   void desaxf(double drg, double spddes, double tsmn, double& axfreq, double& accx); // my version that also returns
                                                                                      // acceleration on X axis
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   // declared for use by flyac() :
   void flacti(double& faxial, double& xl, double& flow, double& efpd);
   void flyac_short(double dt);
   // void flact0(double vw[3], double gw[3], double aw[3], double tend, double xl);
   void flact0(UtVec3d& vw, UtVec3d& gw, UtVec3d& aw);
   void acarea();
   // void gsweepc(int sweep);
   // void getdt(double aw[3], double wnow[3], double wdes[3], double ftime, int jacton, double& delt);
   void getdt(UtVec3d& aw, UtVec3d& wnow, double ftim, double& dt);
   // void gac();
   // void pac();
   // void flyconst();
   // bool flyefficient(BrawlerPlatform& plat);
   // bool benign_aero(BrawlerPlatform& plat);  //replicates benign_aero computed in set_benign()
   // declared for use by rates()
   void   ratmx1(double   xl,
                 double   acmass,
                 UtVec3d& avw,
                 UtVec3d& agw,
                 double   aoaold,
                 double   thrst,
                 double&  maxp,
                 double&  qmxpos,
                 double&  qmxneg,
                 double&  aoamx);
   double pmax(double wate, double glift);
   double gliftm(double mach, double dypr, double wate);
   double fliftm(double mach, double dypr, double wate);
   double qmax(double u, double wate, double awz, const int idir);
   void   aoamx1(double acmass, double& aoamx);
   double actxdt(double xdota0, double xdotr, double omeg, double dt);
   double gldmx(double wate, const int idir);

   // double aoamxcl(double fm);
   // void rates9(double& xl, double faxial, double delt, double wnext[3], double& spddes, double vw[3], double gw[3],
   // int arotyp, double& alfnow, double& aldnow);


   // declared for use by gwreq()
   double dsxdot(double xdes, double x, double xdot, double xdotmx, double tau);
   double stept(double ades, double w, double angmax, double& wdes, double tau);
   void   qroots(double a, double b, double c, double& x1, double& x2, bool& solutn); // needed by stept()
   bool   toltst(double xf, double x0, double tol);
   void   adjal(double& aly, double& alz, double& al, double greqm, UtVec3d& toup, bool& gavdfl);
   void   altlim(UtVec3d& vwr);
   void   vmake2(double a, UtVec3d& vin, UtVec3d& vout); // needed by altlim()
   // void chkrng(double x, double xmin, double xmax, char** text);
   void chkrng(double x, double xmin, double xmax, std::string text);
   // void ckrngi(int k, int kmin, int kmax, char** text);
   void ckrngi(const int k, const int kmin, const int kmax, const std::string& text);
   void desang(UtVec3d& vw, UtVec3d& wnow, double& cphi, double& sphi, double& psidot, double& psi);
   void orient(UtFortMat3d& dircos, double& yaw, double& pitch, double& roll);
   void rollim(UtVec3d& gw, double& roldes, double& aly, double& alz, double psi);
   void limchk(double  value,
               double  limit,
               double  maxovr,
               double  minund,
               double  timnow,
               double& toact,
               double& timovr,
               double& timund,
               bool&   ld2,
               bool&   over,
               bool    limon); // needed by rollim()
   // void vxfrmc(UtFortMat3d& b, UtVec3d& v, UtVec3d& vt, int mode);


   // declared for use by flact1()
   void   udrbe(UtFortMat3d& rbe, UtVec3d& w, double wsq, double wmag, double chi);
   void   udxv(UtVec3d& accbar, UtVec3d& pos, UtVec3d& vel, double delt, UtFortMat3d& rwea);
   double fflo(double thrtlx);
   void   getqm(double speed, double& dyp, double& fmach);
   // void  getqm(double speed);

   // declared for use by seteng()
   void   deseng(double freq, double& fggdes, double& fabdes, double& ddpdes, double& trpdes);
   double throt(double thrst);
   void   stenst(double fggdes, double fabdes, double ddpdes, double trpdes, double delt);

   // declared for use by desaxf()
   // void effth2(double fmachc, double alt, double thrtlx, double& efgrth, double& efpd);
   void effth2(double thrtlx, double& efgrth, double& efpd);
   // void effthr(double fmachc, double alt, double fggc, double fabc, double trpdc, double ddpdc, double& efgrth, double& efpd);
   void effthr(double fggc, double fabc, double trpdc, double ddpdc, double& efgrth, double& efpd);
   void thrst2(double fgg, double fab, double& tgg, double& tab, double& id);
   // double fdddrg(double ddpd);
   ////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////
   // common utility methods: table access, vector/matrix operations
   ////////////////////////////////////////////////////////////////////////////
   // All functions here moved to BrawlerUtil
   // double param1(int i, double xx, double* aray);
   // double tbl1(double x, double* z, double* xval, int nxval);
   // double tbl2(double x, double y, double* z, double* xval, int nxval, double* yval, int nyval);
   // void invtb2(double* tbl, double z, int nr, int ir, double pr, bool revdir, int nc, int& ic, double& pc);
   // void srch(double* x, int nx, double xint, int& ilow, double& p);
   // bool maxcnt(int count, bool zprnt, int mcount, char* subnam);
   // double tbl2e(double x, double y, double* z, double* xval, int nxval,double* yval, int nyval);
   // double xlimit(double xreq, double xmax, double xmin);
   // void xmit(int n, int* a, int* b);
   // void xmit(int n, double* a, double* b) { xmit(n, (int*)a, (int*)b); }
   // void xmit(int n, void* a, void* b);

   // void gtypd(int mode);   //gets TYPDAT and optionally: ACPHYS, PRFORM, RDRDAT, & PODDAT from global memory
   // void gtypd_inner(int iactyp, int mode, int sweep); //needed by gtypd()   //was implemented only for mode = [0,1,2]
   // (bits 1 and 2) void memrd(int* array, int mwds, int loc, int modep); //needed by gtypd_inner() void memrd(double*
   // array, int mwds, int loc, int modep) { memrd((int*)array, mwds, loc, modep); } void memrdc(char* carray, int
   // mchar, int loc, int mode); //needed by gtypd_inner() void xmitnc(int n, int* b, int* a); //needed by memrdc() void
   // itoc(int i, char c[4]) {}  //needed by xmitnc() void memrtn(int nwords, int locx) { }  //needed by memrdc() //not
   // implemented at this time bool lbit(int word, int n);   //needed by gtypd_inner()

   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   // aircraft performance queries
   ////////////////////////////////////////////////////////////////////////////

   // find max transverse acceleration from min longitudinal acceleration
   void max_asubt(double& g_trans_max, bool& low_spd); // needed by gwreq()
   // drag
   double drag(double tlift); // TODO
   double drag2(double xaoa);
   double fdrag2(double xaoa, const int num_acd_m, double* acd_mach, double* acd);
   double fdrag_inner(double aoa, const int num_acd_m, double* acd_mach, double* acd);
   void   gcdadd(const int num_mach, double* machs, double* cds, double& cdadd);
   double drgad();
   // lift
   double xlift(double alfa);
   double fxlift(double alfa);
   // aoa given current state & hypothetical drag
   double get_aoa(double drag);
   void   srch2(double* x, const int nx, double xint, int& ilow, double& p);
   // calibrated air speed
   double cas(double speed);
   void   atmos(double alt, double& t, double& pres, double& dens, double& vs, double& pr, double& dr);
   // thrust
   void thrust(double thrtlx, double& tgg, double& tab, double& id);
   void thrdec(double thrtlx, double& fgg, double& fab, double& trpd);
   void thrst3(double fgg, double fab, double& tgg, double& tab, double& id, double trpd, bool destrv);
   void fthrst(double fgg, double fab, double& tgg, double& tab, double& id);
   void fntthr(double fgg, double fab, double& tgg, double& tab);
   // void fgrthr(double fggc, double fabc, double& tggg, double& tabg);
   // sustained Gees for max turn?
   double susgmx(double wate, double thrtlx);
   // alpha at max instantaneous lift?
   double aoamcl(double wate);
   // angle of attack given current state & hypothetical lift
   double aoa(double xl, bool postall);
   // angle of attack given current state & hypothetical lift & area
   double faoa(double xl, bool postall);
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////


   // FORTRAN common blocks
   //#include "par.fi"     //NOTE - just include par.h for now
   //#include "pcon.fi"    //NOTE - just include pcon.h   for now
   //#include "extst.fi"   //NOTE - just include extst.h  for now, only used for: area[mac], itype[mac], &
   // wing_sweep[mac] #include "typdat.fi"  //NOTE - just include typdat.h for now #include "acphys.fi"  //NOTE - just
   // include acphys.h for now #include "acstat.fi"  //NOTE - just include acstat.h for now, only used for:
   // num_ac_acd_m[mac], ac_acd_mach[2*max_acd_mach][mac], & ac_acd[2*max_acd_mach][mac]. Only used by drag2()

   ////////////////////////////////////////////////////////////////////////////
   //#include "pcon.fi"
   //#include "pcon.h"
   ////////////////////////////////////////////////////////////////////////////
   //#define lnpcon  (13)
   //#define grav    (32.17405)
   //#define eps     (1.0e-35)
   //#define bolk    (1.38e-23)
   //#define antmp   (290.0)
   //#define pi      (3.141592654)
   //#define twopi   (6.283185308)
   //#define hafpi   (1.570796327)
   //#define root2   (1.414213562)
   //#define root3   (1.732050808)
   //#define xlarge  (1.0e38)
   //#define rad     (pi/180.0)
   //#define deg     (1.0/rad)
   //#define ftnmi   (1.64578834e-4)
   //#define unused  (-2147463603.0)
   //#define re      (20890377.0)
   //#define re_rf   (4.0*re/3.0)
#define psl (2116.216)
#define tsl (518.688)
#define dsl (0.0023769)
   //#define warp1   (0.98357119e+09)
   //#define ftcmsq  (929.0304)
   //#define m22ft2  (10.763911)
   //#define ft2m    (2.54*12.0/100.0)
   //#define m2ft    (1.0/ft2m)
   //#define rem     (re*ft2m)
   //#define log10_e (0.43429448)
   ////C     --32 BIT MACHINE VERSION
   //#define large (2147483647.0)
   //#define blank ('    ')
   //#define end   ('END ')
   ////double gravec[3],unit3[3][3],unitx[3],unity[3],unitz[3],xnu,zero_3vec[3];
   ////equivalence
   ////   .(unit3(1,1),unitx(1)),
   ////   .(unit3(1,2),unity(1)),
   ////   .(unit3(1,3),unitz(1))
   // static union {
   //    double unit3[3][3];
   //    struct {
   //       double unitx[3];
   //       double unity[3];
   //       double unitz[3];
   //    };
   // };

   ////static union {
   //   UtFortMat3d unit3;
   //   //struct {
   //      UtVec3d unitx;
   //      UtVec3d unity;
   //      UtVec3d unitz;
   //   //};
   ////};


   UtVec3d gravec;
   // double xnu;
   // double zero_3vec[3];


   ////////////////////////////////////////////////////////////////////////////
   // current status / capability of aircraft:
   // LBM - use BrawlerPlatform for state
   double      tsv;    // current time (time of state vector)
   UtVec3d     pos;    // position
   UtVec3d     vel;    // velocity
   UtVec3d     acc;    // acceleration
   UtFortMat3d rbea;   // earth-to-body rotation matrix (orientation)
   UtFortMat3d rwea;   // earth-to-wind axis rotation matrix (orientation)
   UtVec3d     wnow;   // angular rates now (last time step)
   double      alfa;   //"old" angle of attack (AOA)
   double      alfdot; // angle of attack rate of change
   double      amass;  // taken from svwork.fi. Aircraft mass (empty weight + fuel) in slugs
   double      spd;    // speed
   double      turn;   // direction of turn??? magnitude of turn???
   double      wsqr;   // angular rates squared???
   double      rate;   // rate of turn???
   double      fm;     // mach number
   double      bvrmch;
   double      dyp; // dynamic pressure

   double omegp; // 1st order lag filter frequency for pitch rates
   double omegq; // 1st order lag filter frequency for roll rates

   double pmx;    // max roll RATE limit
   double qmxpos; // max positive pitch RATE limit
   double rlthrs; // some large roll threshold for limit checking (180*rad ???)
   double rolmx;  // max allowed roll limit
   double rarea;  // aircraft reference area (sq feet)
   double gas;    // taken from svwork.fi. Aircraft fuel mass in slugs
   double tset;
   double ttset;
   double fggsv;
   double fabsv;
   double trpdsv;
   double ddpdsv;
   // int wsweep;       //wing sweep number???
   // strange time limit values:
   double mxexrl; // max time over limit before limit reimposed
   double mnonrl; // min time limits must be imposed before they can be exceeded again
   double tmexrl; // time last exceeded the limit to determine if "mxexrl" has been reached --> for purposes of
                  // reapplying limit (not negligible short excursions past limit due to control system)
   double tmonrl; // time last dipped back under limit to determine if "mnonrl" has been reached
   double tmacex; // time last exceeded the limit (including negligible short excursions due to control system???)
   bool   ld2;    // flag for if the limit may now be exceeded, because it was previously applied long enough
   bool   over;   // flag for if the limit is currently being exceeded


   ////////////////////////////////////////////////////////////////////////////
   //#include "extst.fi"
   //#include "extst.h"    //NOTE - only include necessary parts, for now
   //    aircraft "external" state data (truth data)
   ////////////////////////////////////////////////////////////////////////////
   // double svtime[mac];
   // double acmass[mac];
   // double fuel[mac];
   // int itype[mac];
   // double area[mac];
   // double barems[mac];
   // int wing_sweep[mac];
   // int bd[mac];
   double svtime;
   // double acmass;  // Using amass from svwork
   // double fuel;    // Using gas from svwork
   // int itype;
   // double area;
   double barems; // Aircraft empty/design weight in slugs
   // int wing_sweep;
   // int bd;
   // double xe[mac][3];
   // double ve[mac][3];
   // double ae[mac][3];
   // double rbe[mac][9];
   // double rwe[mac][9];
   // double wb[mac][3];
   // double alpha[mac];
   // double speed[mac];
   // double radmin[mac];
   // double wsq[mac];
   // double wmag[mac];
   // double fmach[mac];
   // int istorl[mac][mxdev];
   // int imind[mac];
   // double timded[mac];
   // double dypres[mac];
   // int alive[mac];
   // int nactot;
   // int armnt[mac][numwpn];
   // int killer[mac];
   // int entity[mac];
   // int nsamst;
   // int avdptr[mac];
   // int fcptr[mac];
   // double fgg[mac];
   // double fab[mac];
   // int mdctrl[mac];
   // double ddpd[mac];
   // double trpd[mac];
   // int updmod[mac];
   // double ecmlvl;
   // double freqp[mac];
   // double freqq[mac];
   // double rratmx[mac];
   // double pratmx[mac];
   // int exprfm[mac][manten];
   // double alrate[mac];
   // double crnrv[mac];
   double crnrv;
   // int nnofly;
   // int c2func[mac];
   // int gcmndp[mac];
   // int damage[mac];
   // int cev_off[mac];
   // int ssm_char_p[mac];
   // int ssm_trj_p[mac];
   // int is_ssm[mac];
   // int slot_in_use[mac];
   // int deactivated[mac];
   // int l_cap[mac];
   // int sys_damage[num_sys_defined][mac];
   // int msl_damage[numwpn][mac];
   // int dalet_ptr[mac];


   ////////////////////////////////////////////////////////////////////////////
   //#include "par.fi"
   //#include "par.h"         //NOTE - only include necessary parts, for now
   //    utility constants
   ////////////////////////////////////////////////////////////////////////////
#define msweep (3)
#define max_acd_mach (20)
   //#define desspd (1)
   //#define thrttl (2)
#define desacc (3)
#define undnum (-999.99e30)
#define bay_closed (4)
#define trkr_neg_alt (5000.0)


   ////////////////////////////////////////////////////////////////////////////
   //#include "prform.fi"              //NOTE - only include necessary parts, for now
   //    aircraft performance data
   ////////////////////////////////////////////////////////////////////////////
   double fmmax, casmax, trefgg, trmla;
   double abdt, dtggup, dtggdn;
   bool   tdlim;
   bool   tmlim;
   bool   tablim;
   double trmm;
   double trtd;
   double alpha0;
   // int lperf, lperf2;
#define mmach1 (25)
#define mhts (21)
#define aero1 (1)
#define aero2 (2)
   //#define lpform (21+mmach1*(5+12*mhts)+mhts+2*msweep+(msweep-1))
   // double perf[lpform];
#define mfm (30)
#define malpha (45)
   //#define lwingsweep (7 + 6*mfm + 2*malpha + 2*mfm*malpha)
   // double wingsweep[lwingsweep];
   // int sweep_ptr[msweep];
#define mdpv1 (10)
#define mgtv (10)
#define malv (10)
#define mdpv2 (20)
#define malfa (19)
#define mmach2 (25)
   //#define lpfrm2 (15+mdpv1+mdpv2+4*malv+mgtv*(6+mdpv2)+malfa*(4+mdpv1+3*mgtv)+mmach2*(1+4*malv+2*mgtv))
   // double perf2[lpfrm2];
   int aoa_tab_type;
#define reflective (1)
#define full (2)
   // equivalence(wingsweep(1),nfm1)
   int    nfm1;        // <= mfm, size of fmvec1[]
   int    nfm2;        // <= mfm, size of fmvec2[]
   int    nfm3;        // <= mfm, size of fmvec3[]
   double fmvec1[mfm]; // mach for clmax
   double fmvec2[mfm]; // mach for clift
   double fmvec3[mfm]; // mach for cdrag
   // tbl1() only used on clmin[] & clmax[], both with fmvec1[]
   // double clmin[      mfm]; //not used if "AOA_TABLE" is reflective
   double clmax[mfm];
   // double alpha_clmax[mfm];
   int    nalph1;        // <= malpha, size of alfv1[]
   int    nalph2;        // <= malpha, size of alfv2[]
   double alfv1[malpha]; // AOA values for clift[]
   double alfv2[malpha]; // AOA values for cdrag[]
   // invtb2() only used on "2d" arrays clift[] & cdrag[]
   double clift[mfm * malpha]; // coefficient lift(mach,aoa)
   double cdrag[mfm * malpha]; // coefficient drag(mach,aoa)
   int    nmach1;              // <= mmach1
   int    nhts;                // <= mhts
   double mchval[mmach1];      // mach
   double htvec[mhts];         // alt
   // all these "2d" arrays accessed with tbl2()
   double tidle[mmach1 * mhts]; // thrust idle(mach,alt)
   double tmil[mmach1 * mhts];  // thrust mil (mach,alt)
   double thrmx[mmach1 * mhts]; // thrust max (mach,alt)
   // LBM - no gross thrust effects
   // double gtidle[mmach1*mhts]; //gross thrust idle(mach,alt)
   // double gtmil [mmach1*mhts]; //gross thrust mil (mach,alt)
   // double gthrmx[mmach1*mhts]; //gross thrust max (mach,alt)
   // LBM - no drag deployment devices
   // double drgdmx[mmach1*mhts]; //coefficient of additive drag at full drag device employment
   double abcorl;
   // structural G limit (load factor limit)
   //[0] = positive limit, [1] = negative limit (negative number)
   double gcap[2][mmach1]; // indexed as fortran would???
   // structural G limit (load factor limit) when weight of aircraft above miss_mass (design weight for max gee
   // performance) [0] = positive limit, [1] = negative limit (negative number)
   double gcap2[2][mmach1]; // indexed as fortran would???
   // double sweep_machs[msweep-1];
   double rfarea[msweep];
   // int sweep_cp;

   // ZU - Keep these here in case a fuel object is not defined on the platform
   // These are copied exactly in WsfBrawlerFuel
   double floidl[mmach1 * mhts];
   double flomil[mmach1 * mhts];
   double flomax[mmach1 * mhts];
   double ff_adj;


   ////////////////////////////////////////////////////////////////////////////
   // myprfm.fi
   ////////////////////////////////////////////////////////////////////////////
#define ndatbl (10)
#define ndtbl2 (10)
   double  dragvl[ndatbl];
   double  aoavl[ndatbl];
   double  drgaoa[ndtbl2];
   UtVec3d aw;
   UtVec3d gwind;
   double  almax;
   double  almin;
   double  gmxin;
   double  thrmin;
   double  thrnow;
   double  thrmax;
   double  drgnow;
   double  gmxsu;
   double  almaxg;
   double  alming;
   double  amxin;
   double  drgsu;
   double  wmax;
   double  wate;
   double  gmnin;
   double  gmxsut;
   double  psubs;
   double  cornrv;
   double  tempme;
   double  presme;
   double  densme;
   double  vsme;
   double  prme;
   double  drme;
   // double dyprsp;
   // double acmasp;
   double radmnp;
   // double throtp;
   double aoamx;
   double aoamn;
   double epdmin;
   double epdnow;
   double epdmax;
   double thrstl[5]; // double thrstl(-1:3);  //TODO - figure this out
   ////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////
   //#include "bugger.fi"     //NOTE - only include necessary parts, for now
   ////////////////////////////////////////////////////////////////////////////
   //#define lbuggr (5*mac+1)
   // double throtm[mac];
   // double altmin[mac];
   // double altmax[mac];
   // double casmin[mac];
   double throtm; // these are aircraft specific, no need for array
   double altmin;
   double altmax;
   double casmin;
   // double rabnm;
   // bool nopost[mac];
   ////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////
   //#include "adaero.fi"     //NOTE - only include necessary parts, for now
   // int lnadar; // = mac
   // bool adactv[mac];
   // bool adactv;   //this is aircraft specific, no need for array
   // should be false
   ////////////////////////////////////////////////////////////////////////////

   //   //#include "opmode.fi"
   //   int progrm;
   //#define trprog (9)

   //   //#include "evtim.fi"
   //#define levtim (4)
   //   int indev,indf;
   //   double time,futime;

   ////////////////////////////////////////////////////////////////////////////
   //#include "acphys.fi"
   //#include "acphys.h"     //NOTE - only include necessary parts, for now
   ////////////////////////////////////////////////////////////////////////////
   // int lphys;
   double miss_mass;
   // int has_bay_door;
   // double bay_drag;
   double parea[3];
   // double varea[3];
   double deswt;
   // double vis[4];
   // double delxg[2][3];
   // double rgb[2][3][3];
   // double tefac;
   // int nguns;
   // double intksn[3];
   double ac_fuel_cap;
   // double bay_cycle_time;
   // double bay_delay_time;
   // int fwez_acid;


   ////////////////////////////////////////////////////////////////////////////
   //#include "typdat.fi"
   //#include "typdat.h"     //NOTE - only include necessary parts, for now
   ////////////////////////////////////////////////////////////////////////////
   int    arotyp;
   double thrst_adj;
   //#include "par.h"  //for: mxdev
   //#define lentyp (19*1+2*mxdev)
   //#define mxphys (1)
   //#define mxaero (2)
   //#define mxir (2)
   //#define lntypc (20)
   // int lactyp;
   // int typptr;
   // int iperf;
   // int iphys;
   // int istors[mxdev];
   // int iamlev[mxdev];
   // double fueli;
   // double valac;
   // double cmbeff;
   // int pystyp;
   // int inimsl;
   // int iir;
   // int avtptr;
   // double rcsadj;
   // double vaadj;
   // int ir_spec_type;
   // int iamlev_set;
   // double ff_adj;
   // int methdx;
   // int rcsptr;


   ////////////////////////////////////////////////////////////////////////////
   //#include "acstat.fi"
   //#include "acstat.h"     //NOTE - only include necessary parts, for now
   ////////////////////////////////////////////////////////////////////////////
   // int num_ac_acd_m[mac];
   // double ac_acd_mach[mac][2*max_acd_mach];
   // double ac_acd[mac][2*max_acd_mach];
   // int sweep0[mac];
   int    num_ac_acd_m;
   double ac_acd_mach[2 * max_acd_mach];
   double ac_acd[2 * max_acd_mach];
   // int sweep0;

   //#define lacsta ((28*1+1*2+3*3+max_acd_mach*4)*mac)
   // int mdactn[mac];
   // double wb0[mac][3];
   // double head0[mac][2];
   // double climb0[mac];
   // double gmax0[mac];
   // double speed0[mac];
   // double alt0[mac];
   // double acc0[mac][3];
   // double direc0[mac][3];
   // double throtl[mac];
   double throtl;
   // double thrott[mac];
   // double ddpd0[mac];
   // double trpd0[mac];
   // double rlexmx[mac];
   // double rlonmn[mac];
   // double tmrlex[mac];
   // double tmrlon[mac];
   // double maxrol[mac];
   // double tmexac[mac];
   // int limd2[mac];
   // int overl[mac];
   // double rollth[mac];
   // double tzero[mac];
   // double aztrak[mac];
   // double eltrak[mac];
   // int spdmd0[mac];
   // int lin_fly[mac];
   // int benign_aero[mac];
   // int gun_maneuver[mac];
   // int tgt_for_gun[mac];
   // int sweep0[mac];
   bool             aeroLoaded;
   BrawlerPlatform* mBrawlerPlatformPtr;

private:
   ////////////////////////////////////////////////////////////////////////////
   //#include "prjct.fi"  //NOTE - only include necessary parts, for now
   ////////////////////////////////////////////////////////////////////////////

   //! Energy rate, calculated by aproj3 / laccel
   double erate;
};

#endif
