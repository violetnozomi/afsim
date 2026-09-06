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
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms ofthe license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
// ****************************************************************************

#ifndef WSFEM_ALARM_PROPAGATION_HPP
#define WSFEM_ALARM_PROPAGATION_HPP

#include <complex>
#include <string>
#include <vector>

class WsfEM_Antenna;
#include "WsfEM_ALARM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Propagation.hpp"
#include "WsfEM_Types.hpp"
class WsfEM_XmtrRcvr;
class WsfSimulation;

//! An interface to the ALARM propagation methods.
//!
//! @note This code is NON-EXPORTABLE!

class WsfEM_ALARM_Propagation : public WsfEM_Propagation
{
public:
   WsfEM_ALARM_Propagation();
   WsfEM_ALARM_Propagation& operator=(const WsfEM_ALARM_Propagation&) = delete;
   ~WsfEM_ALARM_Propagation() override                                = default;

   static void ResetState();

   static WsfEM_Propagation* ObjectFactory(const std::string& aTypeName);

   WsfEM_Propagation* Clone() const override;

   double ComputePropagationFactor(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment) override;

   bool Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr) override;

   bool ProcessInput(UtInput& aInput) override;

protected:
   WsfEM_ALARM_Propagation(const WsfEM_ALARM_Propagation& aSrc);

private:
   using COMPLEX = std::complex<float>;
   using antenna = WsfEM_ALARM_Antenna::antenna;

   //! @name from ALARM propagation.f90
   //{@
   void        laprop(antenna& ant_data,
                      double   alphat,
                      double   const3,
                      double   const4,
                      double   deltag,
                      double   epslnt,
                      double   epsln1,
                      COMPLEX& fsquared,
                      double   grangt,
                      double   htmmsl,
                      int      ipolar,
                      bool     water_cover,
                      bool     lpprop,
                      bool&    masked,
                      double   pulwid,
                      double   ranget,
                      double   rkfact,
                      double   rlamda,
                      double   rrough,
                      double   sigmho,
                      double   sitlam,
                      double   sitphi,
                      double   tanept,
                      double   tarlam,
                      double   tarphi,
                      double   taurlx,
                      double   wndkno,
                      double   xtprof,
                      double   ztprof,
                      bool     diff_sw,
                      bool     use_surface_height,
                      double   surface_height,
                      double   soil_moisture,
                      double   water_temp,
                      bool     sea_water);
   static void first(std::vector<double>&       dratio,
                     double&                    frcmin,
                     double                     hammsl,
                     int&                       indxfc,
                     int                        nprofl,
                     double                     rlamda,
                     double                     tanept,
                     const std::vector<double>& xprofl,
                     double                     xtprof,
                     const std::vector<double>& zprofl);

   static void second(double                     deltag,
                      const std::vector<double>& elvmsl,
                      double                     grangt,
                      double&                    hmdzro,
                      int                        indxfc,
                      int                        nprofl,
                      double                     rlamda);

   static void linfit(double& aline0, double& aline1, double deltag, const std::vector<double>& elvmsl, int nprofl);
   //@}

   //! @name from ALARM diffration.f90
   //@{
   void sediff(antenna&                   ant_data,
               double                     alphat,
               double                     const3,
               double                     const4,
               bool&                      convrg,
               double                     deltag,
               const std::vector<double>& dratio,
               const std::vector<double>& elvmsl,
               double                     epslnt,
               double&                    fsubs,
               double                     grangt,
               double                     hammsl,
               double                     htmmsl,
               int                        indxfc,
               int                        nprofl,
               double                     rkfact,
               double                     rlamda,
               const std::vector<double>& xprofl,
               const std::vector<double>& zprofl);

   void kediff(antenna&                   ant_data,
               double                     alphat,
               std::vector<double>&       dratio,
               double                     epslnt,
               double&                    fsubk,
               double                     hammsl,
               int                        indxfc,
               int                        nprofl,
               double                     rlamda,
               const std::vector<double>& xprofl,
               double                     xtprof,
               const std::vector<double>& zprofl,
               double                     ztprof);

   static COMPLEX airy(const COMPLEX& zargmt);

   static COMPLEX conect(const COMPLEX& zargmt);

   static COMPLEX gaussq(const COMPLEX& zargmt);

   static COMPLEX powers(const COMPLEX& zargmt);

   static void
   parfit(double& apara0, double& apara1, double& apara2, double deltag, const std::vector<double>& elvmsl, int nprofl);

   static void deygou(antenna&                   ant_data,
                      double                     alphat,
                      const std::vector<double>& dratio,
                      double                     epslnt,
                      double&                    fsubk,
                      double                     hammsl,
                      int                        ileft,
                      int                        imain,
                      int                        indxfc,
                      int                        iright,
                      double                     rlamda,
                      const std::vector<double>& xprofl,
                      double                     xtprof,
                      const std::vector<double>& zprofl,
                      double                     ztprof);

   static void fresnl(double& cosint, double& sinint, double xargmt);
   //@}

   //! @name from ALARAM multipath.f90
   //@{
   void mltpth(antenna&                   ant_data,
               double                     alphat,
               double                     epslnt,
               double                     epsln1,
               COMPLEX&                   fsubm,
               double                     hammsl,
               std::vector<int>&          iend,
               int                        ipolar,
               std::vector<int>&          istart,
               bool                       water_cover,
               int&                       nareas,
               int                        nprofl,
               double                     pulwid,
               double                     ranget,
               double                     rlamda,
               double                     rrough_i,
               double                     sigmho,
               const std::vector<double>& tanepp,
               double                     taurlx,
               std::vector<bool>&         visibl,
               double                     wndkno,
               const std::vector<double>& xprofl,
               double                     xtprof,
               const std::vector<double>& zprofl,
               double                     ztprof,
               const std::vector<double>& elvmsl,
               bool                       use_surface_height,
               double                     surface_height,
               double                     soil_moisture,
               double                     water_temp,
               bool                       sea_water);

   static void rflect(double gamma, int ipolar, COMPLEX& rcoeff, COMPLEX ysquar);

   static COMPLEX water_dielectric(double freq, bool sea_water, double water_temp);

   static COMPLEX soil_dielectric(double freq, double soil_moisture);

   static double stddev(int n, const std::vector<double>& a);
   //@}

   //! @name from SalramClutter.cpp
   //@{
   static void GetLandCoverData(int aLandCover, double& aDielectricConstant, double& aConductivity);
   //@}

   //! @name from SalramPropagation.cpp
   //@{
   static double RoughSurfaceReflection(int aLandForm, bool aOnWater, int aSeaState, double aWavelength, double aPsi);
   //@}

   std::string water_type;
   double      soil_moisture;  //!< % [0..100]
   double      surface_height; //!< meters
   double      water_temp;     //!< degrees C
   double      epsilon_one;
   double      sigma_zero;
   double      roughness;
   double      sea_relaxation;
   double      wind_speed; //!< knots

   bool diff_sw;
   bool prop_sw;
   bool sea_water;
   bool use_surface_height;

   //! @name Internal saved variables in LAPROP.
   //! The following variables were local variables in LAPROP that had the 'SAVE'd attribute.
   //! They could have remained in LAPROP as locally scoped static variables, but I decided
   //! to move them out here in preparation for multi-threading.
   //!
   //! @note As written the code is NOT thread-safe!!!
   //@{
   static bool                uninit;
   static int                 aprofile;
   static std::vector<double> dratio;
   static std::vector<double> elvmsl;
   static std::vector<int>    iend;
   static std::vector<int>    istart;
   static std::vector<double> tanepp;
   static std::vector<bool>   visibl;
   static std::vector<double> xprofl;
   static std::vector<double> zprofl;

   static int              indxmx_size;
   static std::vector<int> indxmx;
   //@}

   bool           mUseMIT_LL_DataTables; // true if to use MIT-LL data tables (from SALRAM)
   bool           mAllowCalculationShortcuts;
   int            mWSF_LandCover; // land cover from WSF environment
   int            mWSF_LandForm;  // land form from WSF environment
   int            mWSF_SeaState;  // sea state from WSF environment
   WsfSimulation* mSimulationPtr;
};
#endif
