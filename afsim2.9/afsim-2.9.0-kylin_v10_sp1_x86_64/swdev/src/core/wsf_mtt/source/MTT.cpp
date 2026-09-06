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

#include "MTT.hpp"

#include "MTT_ActiveTrack.hpp"
#include "MTT_NonlocalTrack.hpp"
#include "MTT_Parameters.hpp"
#include "MTT_PerceivedCluster.hpp"
#include "MTT_RadarMeasurement.hpp"
#include "MTT_Tracks.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp" // because we include WsfTrackManager
#include "WsfMTT_Interface.hpp"
#include "WsfMTT_Observer.hpp"
#include "WsfPlatform.hpp"
#include "WsfTrackManager.hpp"

MTT::MTT()
   : mHF_Correlator()
   , mMTT_InterfacePtr(nullptr)
   , mParameters()
   , mTracks()
   , mDebug(false)
   , mName()
   , mManagerPtr(nullptr)
   , mCorrelateAllSources(true)
   , mTrackIdToSavedDataMap()
{
}

MTT::MTT(const MTT& aSrc)
   : mHF_Correlator()
   , mMTT_InterfacePtr(nullptr)
   , mParameters(aSrc.mParameters)
   , mTracks(aSrc.mTracks)
   , mDebug(aSrc.mDebug)
   , mName(aSrc.mName)
   , mManagerPtr(nullptr)
   , mCorrelateAllSources(aSrc.mCorrelateAllSources)
   , mTrackIdToSavedDataMap()
{
}

//! Called from MTT_Fusion/MTT_Correlation during initialization.
void MTT::SetTrackManager(WsfTrackManager* aTrackManager)
{
   mManagerPtr = aTrackManager;
   if (mManagerPtr != nullptr)
   {
      mName             = mManagerPtr->GetPlatform()->GetName();
      mDebug            = mManagerPtr->DebugEnabled();
      mMTT_InterfacePtr = WsfMTT_Interface::Find(*aTrackManager->GetSimulation());
   }
   mHF_Correlator.Initialize(aTrackManager);
}

// virtual
//! Process input from a generic source.
//! @param aInput The input source.
bool MTT::ProcessInput(UtInput& aInput)
{
   std::string command = aInput.GetCommand();
   if (mParameters.ProcessInput(aInput))
   {
   }
   else if (command == "mttc_track_fusion")
   {
      // strange input format, but that's suppressor...
      std::string fusion_type;
      aInput.ReadCommand(fusion_type);
      if (fusion_type == "all_sources")
      {
         mCorrelateAllSources = true;
      }
      else if (fusion_type == "initial_source_only")
      {
         mCorrelateAllSources = false;
      }
      else
      {
         throw(UtInput::UnknownCommand(aInput));
      }
   }
   else
   {
      throw(UtInput::UnknownCommand(aInput));
   }
   return true;
}

//! Save temporary data from the correlation step, that is needed for fusion.
void MTT::SaveData(MTT_PerceivedCluster*  aPerceivedClusterPtr,
                   MTT_ActiveTrack::Data* aTempDataPtr,
                   MTT_ActiveTrack*       aLocalTrackPtr)
{
   SavedData sd;
   sd.mNonLocalTrackPtr = aPerceivedClusterPtr;
   sd.mTempDataPtr      = aTempDataPtr;
   sd.mLocalTrackPtr    = aLocalTrackPtr;
   mTrackIdToSavedDataMap.insert(std::make_pair(aLocalTrackPtr->mTrackId, sd));
}

//! Remove data saved between correlation and fusion steps.
//! The data should be deleted after use.
bool MTT::RemoveSavedData(int                     aMTT_TrackId,
                          MTT_PerceivedCluster*&  aNonLocalTrackPtr, // out
                          MTT_ActiveTrack::Data*& aTempDataPtr,      // out
                          MTT_ActiveTrack*&       aLocalTrackPtr)          // out
{
   TrackIdToSavedDataMap::iterator iter  = mTrackIdToSavedDataMap.find(aMTT_TrackId);
   bool                            found = (iter != mTrackIdToSavedDataMap.end());
   if (found)
   {
      aNonLocalTrackPtr = iter->second.mNonLocalTrackPtr;
      aTempDataPtr      = iter->second.mTempDataPtr;
      aLocalTrackPtr    = iter->second.mLocalTrackPtr;
      mTrackIdToSavedDataMap.erase(iter);
   }
   return found;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Fortran-Converted routines from Suppressor 7+, mttexe.f
//!
//! Generic distribution statement follows:
//----------------------------------------------------------------------+
// DISTRIBUTION STATEMENT D.                                            |
//  Distribution limited to DOD and DOD contractors; Critical           |
//  Technology, November 2008. Other requests shall be referred to      |
//  ASC/ENDA, Wright-Patterson Air Force Base, OH 45433-7210.           |
// WARNING - This document contains technical data whose export is      |
//  restricted by the Arms Export Control Act (Title 22, U.S.//.,        |
//  Sec 2751, et. seq.) or the Export Administration Act of 1979, as    |
//  amended, Title 50, U.S.//., App. 2401 et. seq. Violations of these   |
//  export laws are subject to severe criminal penalties. Disseminate   |
//  in accordance with provisions of DoD Directive 5230.25.             |
// CHANGE HISTORY:  7.0-000                                             |
// VARIABLES                                                            |
//----------------------------------------------------------------------+
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//      SUBROUTINE MTTEXE(JNR,GRL,TIME,ASGID,PTR017,PTR383,PTR388,
//     + NUMTGT,PLRTGT,NUMPLY,PLRSND,IPLYR,PLRRCV,PTR5 )
//! Top-level correlation method.
MTT_ActiveTrack* MTT::MTT_Correlate(double                      aSimTime,
                                    const MTT_PerceivedCluster& aNonlocalTrack,
                                    MTT_ActiveTrack::Data&      aTempActiveData)
{
   // C     *if jnr(ptr017+39), which is a pointer to 359 block--a measurement
   // C     *header, is nonzero, and jnr(ptr017+41), which holds a track number
   // C     *for this player, is zero, then we have a radar measurement coming
   // C     *into the filter center;
   MTT_ActiveTrack* activeTrackPtr = nullptr;

   const MTT_PerceivedCluster* ptr017 = &aNonlocalTrack;
   MTT_ActiveTrack::Data*      ptr384 = &aTempActiveData;
   //       IF (JNR(PTR017+39).NE.0) THEN
   if (ptr017->mMeasurementPtr != nullptr)
   {
      //          CALL MTTDPS(JNR,GRL,TIME,PTR017,PTR383,PTR388,PTR384)
      mttdps_Correlation(aSimTime, *ptr017, activeTrackPtr);
      //       ELSE
   }
   else
   {
      // C        *if jnr(ptr017+39) is zero and jnr(ptr017+37), which is a
      // C        *pointer to a combined track, is nonzero, then we have a
      // C        *track coming into the filter center
      //           IF (JNR(PTR017+37).NE.0) THEN
      if (ptr017->mCombinedTrackPtr != nullptr)
      {
         //             CALL MTTTFS(JNR,GRL,TIME,ASGID,PTR017,PTR383,PTR388,
         //     +        PTR384)
         mtttfs_Correlation(aSimTime, *ptr017, *ptr384, activeTrackPtr);
      }
      //          ELSE
      // C           *the setup calling this routine has an error
      //             PTR384=0
      //          ENDIF
      //       ENDIF
   }

   return activeTrackPtr;
}

//      SUBROUTINE MTTEXE(JNR,GRL,TIME,ASGID,PTR017,PTR383,PTR388,
//     + NUMTGT,PLRTGT,NUMPLY,PLRSND,IPLYR,PLRRCV,PTR5 )
//! Top-level fusion method.
void MTT::MTT_Fuse(double                      aSimTime,
                   const MTT_PerceivedCluster& aNonlocalTrack,
                   MTT_ActiveTrack&            aLocalTrack,
                   MTT_ActiveTrack::Data&      aTempActiveData)
{
   // C     *if jnr(ptr017+39), which is a pointer to 359 block--a measurement
   // C     *header, is nonzero, and jnr(ptr017+41), which holds a track number
   // C     *for this player, is zero, then we have a radar measurement coming
   // C     *into the filter center;
   const MTT_PerceivedCluster* ptr017 = &aNonlocalTrack;
   MTT_ActiveTrack::Data&      tmp387 = aTempActiveData;
   MTT_ActiveTrack*            ptr384 = &aLocalTrack;

   //       IF (JNR(PTR017+39).NE.0) THEN
   if (ptr017->mMeasurementPtr != nullptr)
   {
      //          CALL MTTDPS(JNR,GRL,TIME,PTR017,PTR383,PTR388,PTR384)
      // mttdps_Fusion(aSimTime, *ptr017, ptr384);
      //       ELSE
   }
   else
   {
      // C        *if jnr(ptr017+39) is zero and jnr(ptr017+37), which is a
      // C        *pointer to a combined track, is nonzero, then we have a
      // C        *track coming into the filter center
      //           IF (JNR(PTR017+37).NE.0) THEN
      if (ptr017->mCombinedTrackPtr != nullptr)
      {
         //             CALL MTTTFS(JNR,GRL,TIME,ASGID,PTR017,PTR383,PTR388,
         //     +        PTR384)
         mtttfs_Fusion(aSimTime, *ptr017, tmp387, ptr384);
      }
      //          ELSE
      // C           *the setup calling this routine has an error
      //             PTR384=0
      //          ENDIF
      //       ENDIF
   }
}

//       SUBROUTINE MTTC2A(JNR,GRL,PTR386,PTR387,ERR)
//! Multiple target tracking - promote candidate track
void MTT::mttc2a(const MTT_CandidateTrack::Data& aCandidateTrackData, MTT_ActiveTrack::Data& aActiveTrackData, bool& aError)
{
   //
   //     *promote a candidate track to an active track;

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *june 27, 2005;

   const MTT_CandidateTrack::Data* ptr386 = &aCandidateTrackData;
   MTT_ActiveTrack::Data*          ptr387 = &aActiveTrackData;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       REAL RCT(52)
   //
   //       INTEGER PTR386,PTR387
   //
   //       DOUBLE PRECISION CT(26)
   //       DOUBLE PRECISION XIC(4),PIC(4,4),PIVC(2,2),XIVC(2)
   //       EQUIVALENCE (RCT(1),CT(1))
   //       EQUIVALENCE (PIC,CT(1)),(XIC,CT(17))
   //       EQUIVALENCE (PIVC,CT(21)),(XIVC,CT(25))
   //
   MTT_CandidateTrack::Data ct;
   UtMatrixd&               pic  = ct.mErrorInfoMatrix;
   UtMatrixd&               xic  = ct.mHorizontalState;
   UtMatrixd&               pivc = ct.mVerticalErrorInfoMatrix;

   //       REAL RAT(184)
   //       DOUBLE PRECISION AT(92)
   //       DOUBLE PRECISION XHORZ(6),PHORZ(6,6),PIVERT(2,2),XIVERT(2)
   //       DOUBLE PRECISION MTPLF,MTPTF,XHORZP(6),PHORZP(6,6)
   //
   //       EQUIVALENCE (RAT(1),AT(1))
   //       EQUIVALENCE (PHORZ,AT(1)),(XHORZ,AT(37))
   //       EQUIVALENCE (PIVERT,AT(43)),(XIVERT,AT(47))
   //       EQUIVALENCE (MTPLF,AT(49)),(MTPTF,AT(50))
   //
   MTT_ActiveTrack::Data at;
   UtMatrixd&            phorz  = at.mHorizontalInfoMatrix;
   UtMatrixd&            xhorz  = at.mFilterStates;
   UtMatrixd&            pivert = at.mVerticalInfoMatrix;
   double&               mtplf  = at.mStraightFlightModeProbability;
   double&               mtptf  = at.mTurningFlightModeProbability;
   UtMatrixd&            xhorzp = at.mPreviousFilterStates;
   UtMatrixd&            phorzp = at.mPreviousHorizontalMatrix;

   //       DOUBLE PRECISION PC(4,4),X(4),DUM1(100)
   //       INTEGER I,J
   //       LOGICAL ERR
   //
   //*load the double precision candidate track data from GRL
   //       DO I=1,52
   //          RCT(I)=GRL(PTR386+I-1)
   //       ENDDO
   ct = *ptr386;
   //*invert information matrix and store in active track error cov mtx
   //       ERR=.FALSE.
   //       CALL MTXINV(PIC,4,PC,DUM1,ERR)
   UtMatrixd pc(pic);
   try
   {
      pc.Invert();
      aError = false;
   }
   //*if there are problems inverting pic, then return
   //       IF (ERR) GOTO 100
   catch (UtMatrixd::SingularMatrixError&)
   {
      aError = true;
      return;
   }
   //*put pc into active track's error covariance matrix
   //       CALL MTXZRO(PHORZ,6,6)
   //       DO I=1,4
   //          DO J=1,4
   //             PHORZ(I,J)=PC(I,J)
   //          END DO
   //       END DO
   phorz.CopyFrom(pc, 0, 0, 4, 4);
   //*compute states from information filter and store in active track
   //*state vector
   //       CALL MTXMUL(PC,4,4,XIC,4,1,X)
   UtMatrixd x(4);
   x.Multiply(pc, xic);
   //       CALL MTXZRO(XHORZ,4,1)
   //       DO I=1,4
   //          XHORZ(I)=X(I)
   //       END DO
   xhorz.CopyFrom(x, 0, 0, 4, 1);
   //*copy candiate's vertical information filter to active track's
   //*vertical information filter
   //       DO I=1,2
   //          XIVERT(I)=XIVC(I)
   //          DO J=1,2
   //             PIVERT(I,J)=PIVC(I,J)
   //          END DO
   //       END DO
   pivert = pivc;
   //*initialize mode transition probabilities to 1/2 each
   //       MTPLF=0.5
   mtplf = 0.5;
   //       MTPTF=0.5
   mtptf = 0.5;
   //*initialize previous state values to zero
   //       DO I=1,6
   //          XHORZP(I)=0.0
   //          DO J=1,6
   //             PHORZP(I,J)=0.0
   //          ENDDO
   //       ENDDO
   xhorzp.Fill(0.0);
   phorzp.Fill(0.0);
   //
   //       DO I=1,184
   //          GRL(PTR387+I-1)=RAT(I)
   //       ENDDO
   //
   *ptr387 = at;
   //  100  RETURN
   //       END
   //
   //
}

//       INTEGER FUNCTION MTTCAT(JNR,GRL,TIME,PTR017,PTR383,PTR388,LNKEY)
//! Multiple target tracking - check active track.
MTT_ActiveTrack* MTT::mttcat(double aSimTime, const MTT_PerceivedCluster& aMeasurement)
{
   //*check to see if measurement belongs to an active track;

   //*fred kiefer;
   //*bogdon associates inc;
   //*june 14, 2005;

   // References:
   const MTT_PerceivedCluster* ptr017 = &aMeasurement;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER LMASTR,LSUPPL,LNKEY

   //       INTEGER I,PTR017,PTR383,PTR388,PTR384,PTR359,LST384,NXT384,PTR387
   //       REAL TIME,TRKTIM
   //       DOUBLE0 PRECISION ALPHAM,ZC,MINNRM,NORM,ZTST,DT,DTV
   //
   //       COMMON /TRUST/ TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,
   //     +         TRUSPD,IDTRGT,IDSNDR,IDRCVR,PLTRGT,PLSNDR,PLRCVR,GAMTIM
   //       REAL TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,TRUSPD
   //       INTEGER IDTRGT,IDSNDR,IDRCVR
   //       CHARACTER*20 PLTRGT,PLSNDR,PLRCVR
   //       REAL GAMTIM

   //       MTTCAT=0
   MTT_ActiveTrack* mttcat = nullptr;

   // C     *initialize for output incidents;
   //        LMASTR=JNR(2)-JNR(1)
   //        LSUPPL=JNR(JNR(JNR(LMASTR+18)+7)+1)

   //       //*obtain probability of type i error for measurement correlaiton
   //       ALPHAM=DBLE(GRL(PTR388+7))
   double alpham = mParameters.mT2T_FalseRejectProbability;

   //       //*fwk: 31may07 -- compute critical threshold for measurement correlation
   //       ZC=DSQRT(-2.0*DLOG(ALPHAM))
   double zc = sqrt(-2.0 * log(alpham));
   //       MINNRM=1.0D200
   double minnrm = 1.0e+200;
   //       //*pointer to measurement header (359 block)
   //       PTR359=JNR(PTR017+39)
   const MTT_Measurement* ptr359 = ptr017->mMeasurementPtr;
   //       //*loop over linked list of tracks
   //       PTR384=JNR(PTR383+5)
   //       LST384=PTR383+5
   MTT_Tracks::MTT_ActiveTrackList& lst384   = mTracks.mActiveTrackList;
   unsigned                         trackNum = 0;
   //       DO WHILE (PTR384.NE.0)
   while (trackNum < lst384.GetCount())
   {
      MTT_ActiveTrack* ptr384 = lst384.GetEntry(trackNum);
      //          //*determine if track is still active
      //          TRKTIM=GRL(PTR384+1)
      double trktim = ptr384->mUpdateTime;
      //          PTR387=JNR(PTR384+3)
      MTT_ActiveTrack::Data* ptr387 = &(static_cast<MTT_ActiveTrack*>(ptr384)->GetData());
      // (Active track data accessed directly from the track instance)

      //          DT=DBLE(TIME-TRKTIM)
      double dt = aSimTime - trktim;

      //          IF (DT.GT.DBLE(GRL(PTR388))) THEN
      if (dt > mParameters.mActiveTrackDropTime)
      {
         //             //*drop track:  first give back track data block
         //             CALL DATGIV(JNR,GRL,-387,0,0, PTR387 )

         // C*====================================================== .ctk =======;
         //              LMASTR=JNR(2)-JNR(1)
         //              IF(MOD(JNR(LMASTR+30)/2,2).EQ.1)THEN
         //                 WRITE(62,1113)TIME,4,0,JNR(PTR384+2),0,IDTRGT,PLTRGT,
         //      +          IDSNDR,PLSNDR,IDRCVR,PLRCVR
         //  1113           FORMAT(E15.7,2I3,2I8,3(I8,1X,A20),2X,15E15.7)
         //              ENDIF
         // C*====================================================== .ctk =======;
         // C           *write drops-mttc-active-track incident;
         //              JNR(LSUPPL)=JNR(PTR384+2)
         //              CALL OUTSIT(JNR,GRL,208,TIME,LNKEY,-1,-1,-1,-1,-1 )

         //             //*reset links
         //             NXT384=JNR(PTR384)
         //             JNR(LST384)=NXT384
         //             CALL DATGIV(JNR,GRL,-384,0,0, PTR384 )
         WsfObserver::ActiveTrackDropped (&mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, ptr384);
         lst384.Delete(ptr384->GetId());
         // NOTE: Do NOT increment 'trackNum'. The next (and following entries) are shifted to fill
         //       the hole made by deleting the current entry.
         //             PTR384=NXT384
         //             GOTO 100
         continue;
         //          END IF
      }
      //          //*determine if vertical channel is still active
      //          IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //             DTV=DBLE(TIME-GRL(PTR384+7))
         double dtv = aSimTime - ptr384->mVerticalChannelUpdateTime;
         //             IF (DTV.GT.DBLE(GRL(PTR388+32))) THEN
         if (dtv > mParameters.mVerticalChannelDropTime)
         {
            ////             *reset flags, time, and information matrices
            //                JNR(PTR384+6)=0
            ptr384->mVerticalChannelActive = false;
            //                GRL(PTR384+7)=0.
            ptr384->mVerticalChannelUpdateTime = 0.0;
            //                DO I=84,95
            //                   GRL(PTR387+I)=0.
            //                ENDDO
            ptr387->mVerticalInfoMatrix.Fill(0.0);
            //             ENDIF
         }
         //          ENDIF
      }
      // C==================================================== .cov output ======
      //           IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)
      //      +    WRITE(67,1112)TIME,DT,JNR(JNR(PTR017+10)+8),JNR(PTR384+2)
      //  1112     FORMAT(' MTTCAT - TIME,DT,TGTID,TRKID',/,9X,2F10.3,2I10)
      //  (TODO) .cov output
      // C==================================================== .cov output ======
      //           //*compute gating ratio and chi-squared test value
      //           PTR387=JNR(PTR384+3)
      //  active track data is contained within the active track structure.
      //           CALL MTTGTM(JNR,GRL,PTR387,PTR384,PTR388,PTR359,DT,ZTST,NORM)
      double ztst, norm;
      mttgtm(*ptr387, *ptr384, *ptr359, dt, ztst, norm);
      // C==================================================== .dbg output ======
      //           IF(MOD(JNR(LMASTR+30)/32,2).EQ.1)
      //      +    WRITE(66,1111)TIME,DT,ZTST
      //  1111     FORMAT(3F10.2)
      //  (TODO) .dbg output
      // C==================================================== .dbg output ======
      //           //*fwk: 31may07 - MTTGTM returns the hypothesis test value.  we
      //           //*consider the measurement if test value is less than critical
      //           //*value and we keep the track with smallest test value
      //           IF (ZTST.LT.ZC) THEN
      if (ztst < zc)
      {
         //            IF (NORM.LT.MINNRM) THEN
         if (norm < minnrm)
         {
            //              MINNRM=NORM
            minnrm = norm;
            //              MTTCAT=PTR384
            mttcat = ptr384;
            //            ENDIF
         }
         //          ENDIF
      }
      //
      //          LST384=PTR384
      //          PTR384=JNR(LST384)
      ++trackNum; // Advance to next track in list
      //  100  END DO
   } // while
   //
   //       RETURN
   //       END
   //
   //
   return mttcat;
}

//       INTEGER FUNCTION MTTCCT(JNR,GRL,TIME,PTR017,PTR383,PTR388)
//! Multiple target tracking - check candidate track.
//! Note: In the fortran function, the address of the active track is returned
//! if the candidate track is promoted, -1 if the CT is correlated but not promoted,
//! and 0 if no CT is correlated.  For C++ we can't return these results without
//! nasty casts, so I'm returning an int result of -1, 0, 1 (1 corresponding with
//! the first case above), and the address is an in/out pointer parameter.
//! 10/14/2010 In the latest version, the function has been replaced by a subroutine,
//! where the pointer to the new active track and the new active track id are returned.
//! So the new conversion will be closer to the original fortran.
//       SUBROUTINE MTTCCT(JNR,GRL,TIME,PTR017,PTR383,PTR388,LNKEY,
//     +  PTRTRK,ICT)
int MTT::mttcct(double aSimTime, const MTT_PerceivedCluster& aMeasurement, MTT_ActiveTrack*& aActiveTrackPtr)
{
   //*check to see if measurement can promote a candidate track to an
   //*active track;

   //*fred kiefer;
   //*bogdon associates inc;
   //*june 14, 2005;

   // References:
   const MTT_PerceivedCluster* ptr017 = &aMeasurement;
   MTT_Tracks*                 ptr383 = &mTracks;

   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER PTR017,PTR383,PTR388,PTR384,PTR359,LST384,NXT384,PTR386
   //       INTEGER ATP384,ATPLST,PTR387
   //       INTEGER TMP387,WIN384,WINLST,WIN386
   //       REAL TIME,TRKTIM
   //       DOUBLE PRECISION DT,DTV,ALPHAM,ZTST,ZC,MINNRM,W(2),PC(4,4),NORM
   //       DOUBLE PRECISION DUM1(100),DUM2(100),DUM3(100),VARSPD,VARDST
   //       LOGICAL ERR,PROMOT
   //
   //       INTEGER I,PTR360,IAVAIL,IEL,IRNG
   //       DOUBLE PRECISION SRANGE,RANGE,AZ,EL,SIGRNG,SIGAZ,SIGEL
   //       DOUBLE PRECISION RDRX,RDRY,RDRZ
   //       DOUBLE PRECISION DX,DY,DZ,R(2,2),A11,A12,B(2,2),BT(2,2)
   //       DOUBLE PRECISION TMP2X2(2,2),TMP4X4(4,4)
   //       DOUBLE PRECISION H(2,4),HT(4,2),RINVV(2,2),VARZ
   //
   //       REAL RCTRCK(52)
   //       DOUBLE PRECISION CTRCK(26),CNDTST,CNDTHR
   //       DOUBLE PRECISION XIC(4),PIC(4,4),PIVC(2,2),XIVC(2),COND
   //       EQUIVALENCE (RCTRCK(1),CTRCK(1))
   //       EQUIVALENCE (PIC,CTRCK(1)),(XIC,CTRCK(17))
   //       EQUIVALENCE (PIVC,CTRCK(21)),(XIVC,CTRCK(25))

   MTT_CandidateTrack::Data ctrck;
   UtMatrixd&               pic  = ctrck.mErrorInfoMatrix;
   UtMatrixd&               xic  = ctrck.mHorizontalState;
   UtMatrixd&               pivc = ctrck.mVerticalErrorInfoMatrix;
   UtMatrixd&               xivc = ctrck.mVerticalState;

   aActiveTrackPtr = nullptr; // default is no correlation.
   //       MTTCCT=0
   int mttcct = 0; // return value is -1, 0, 1 (1==correlated)
   //       WIN384=0
   MTT_CandidateTrack* win384 = nullptr;
   //       WINLST=0
   //
   //*obtain probability of type i error for measurement correlaiton
   //       ALPHAM=DBLE(GRL(PTR388+7))
   double alpham = mParameters.mM2T_FalseRejectProbability;
   //*fwk: 31may07 -- compute critical threshold for measurement correlation
   //       ZC=DSQRT(-2.0*DLOG(ALPHAM))
   double zc = sqrt(-2.0 * log(alpham));
   //       MINNRM=1.0D200
   double minnrm = 1.0e+200;
   //
   //*pointer to measurement header (359 block)
   //       PTR359=JNR(PTR017+39)
   const MTT_Measurement* ptr359 = ptr017->mMeasurementPtr;
   //
   //*first reserve room for a temporary 387 block
   //       CALL DATGET(JNR,GRL,1,-387, TMP387)
   MTT_ActiveTrack::Data tmp387;
   //*loop over linked list of candidate tracks
   //       PTR384=JNR(PTR383+3)
   MTT_CandidateTrack* ptr384 = nullptr;
   //       LST384=PTR383+3
   MTT_Tracks::MTT_CandidateTrackList& lst384   = ptr383->mCandidateTracks;
   unsigned                            trackNum = 0;
   double                              dt       = 0.0;
   //       DO WHILE (PTR384.NE.0)
   while (trackNum < lst384.GetCount())
   {
      ptr384 = lst384.GetEntry(trackNum);
      //*determine if track is still active
      //          TRKTIM=GRL(PTR384+1)
      double trktim = ptr384->mUpdateTime;
      //          PTR386=JNR(PTR384+3)
      //          DT=DBLE(TIME-TRKTIM)
      dt = aSimTime - trktim;
      //          IF (DT.GT.DBLE(GRL(PTR388+19))) THEN
      if (dt > mParameters.mCandidateTrackDropTime)
      {
         //*drop track:  first give back track data block
         //             CALL DATGIV(JNR,GRL,-386,0,0, PTR386 )
         //*reset links
         //             NXT384=JNR(PTR384)
         //             JNR(LST384)=NXT384
         //             CALL DATGIV(JNR,GRL,-384,0,0, PTR384 )
         WsfObserver::CandidateTrackDropped (&mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, ptr384);
         lst384.Delete(ptr384->GetId());
         // NOTE: Do NOT increment 'trackNum'. The next (and following entries) are shifted to fill
         //       the hole made by deleting the current entry.
         //             PTR384=NXT384
         //             GOTO 100
         continue;
         //          END IF
      }

      //*determine if vertical channel is still active
      //          IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //             DTV=DBLE(TIME-GRL(PTR384+7))
         double dtv = aSimTime - ptr384->mVerticalChannelUpdateTime;
         //             IF (DTV.GT.DBLE(GRL(PTR388+32))) THEN
         if (dtv > mParameters.mVerticalChannelDropTime)
         {
            //*reset flags, time, and information matrices
            //                JNR(PTR384+6)=0
            ptr384->mVerticalChannelActive = false;
            //                GRL(PTR384+7)=0.
            ptr384->mVerticalChannelUpdateTime = 0.0;
            //                DO I=40,51
            //                   GRL(PTR386+I)=0.
            //                ENDDO
            ptr384->GetData().mVerticalErrorInfoMatrix.Fill(0.0);
            ptr384->GetData().mVerticalState.Fill(0.0);
            //             ENDIF
         }
         //          ENDIF
      }
      //*determine if this candidate track can be promoted. since we're
      //*not sure,we'll copy the candidate track into a temporary
      //*temporarily promote candidate track to an active track
      //          CALL MTTC2A(JNR,GRL,PTR386,TMP387,ERR)
      bool err = false;
      mttc2a(ptr384->GetData(), tmp387, err);
      //          IF (.NOT. ERR) THEN
      if (!err)
      {
         ////==================================================== .cov output ======
         //             WRITE(67,1112)TIME,DT,JNR(JNR(PTR017+10)+8),JNR(PTR384+2)
         // 1112        FORMAT(' MTTCCT - TIME,DT,TGTID,TRKID',/,9X,2F10.3,2I10)
         ////==================================================== .cov output ======
         //*perform gating operation on temporary active track
         //             CALL MTTGTM(JNR,GRL,TMP387,PTR384,PTR388,PTR359,DT,ZTST,
         //     +       NORM)
         double ztst = 0.0; // hypothesis test value
         double norm = 0.0; // innovation value
         mttgtm(tmp387, *ptr384, *ptr359, dt, ztst, norm);
         //*we have a potential promotable track if measurement is within
         //*error elllipse whose bounds are defined by the probabiliyt of
         //*of a type i error
         //             IF (ZTST.LT.ZC) THEN
         if (ztst < zc)
         {
            //               IF (NORM.LT.MINNRM) THEN
            if (norm < minnrm)
            {
               //                 WIN384=PTR384
               win384 = ptr384;
               //                 MINNRM=NORM
               minnrm = norm;
               //               ENDIF
            }
            //             ENDIF
         }
         //          ENDIF
      }
      //          LST384=PTR384
      ++trackNum;
      //          PTR384=JNR(LST384)
      // 100    END DO
   }
   //
   //       IF (WIN384.GT.0) THEN
   if (win384 != nullptr)
   {
      //*extract necessary radar measurement data
      //          PTR360=JNR(PTR359+4)
      const MTT_RadarMeasurement::Data* ptr360 = dynamic_cast<const MTT_RadarMeasurement::Data*>(ptr359->mDataPtr);
      //          SRANGE=DBLE(GRL(PTR360))
      double srange = ptr360->mRange;
      //          AZ=DBLE(GRL(PTR360+1))
      double az = ptr360->mAzimuth;
      //          EL=DBLE(GRL(PTR360+2))
      double el = ptr360->mElevation;
      //          SIGRNG=DBLE(GRL(PTR360+4))
      double sigrng = ptr360->mRangeStDev;
      //          SIGAZ=DBLE(GRL(PTR360+5))
      double sigaz = ptr360->mAzimuthStDev;
      //          SIGEL=DBLE(GRL(PTR360+6))
      double sigel = ptr360->mElevationStDev;
      //          RDRX=DBLE(GRL(PTR360+8))
      double rdrx = ptr360->mSensorLocation[0];
      //          RDRY=DBLE(GRL(PTR360+9))
      double rdry = ptr360->mSensorLocation[1];
      //          RDRZ=DBLE(GRL(PTR360+10))
      double rdrz = ptr360->mSensorLocation[2];
      //*determine if elevation is available and adjust for ground range
      //          IAVAIL=JNR(PTR359+6)
      unsigned int iavail = ptr359->mDataAvailable;
      //          IEL=MOD(IAVAIL/4,2)
      unsigned iel = (iavail / 4) % 2;
      //          IF (IEL.EQ.1) THEN
      double range = srange;
      if (iel == 1)
      {
         //              RANGE=SRANGE*DCOS(EL)
         range *= cos(el);
      }
      //          ELSE
      //              RANGE=SRANGE
      //          END IF
      //          IRNG=MOD(JNR(PTR359+6),2)
      unsigned irng = ptr359->mDataAvailable % 2;
      //*statistics for delx and dely are in a system that has been rotated
      //*"az" degrees,where "az" measured counterclockwise from east
      //*(per suppressor);
      //          DX=RANGE*DCOS(AZ)
      double dx = range * cos(az);
      //          DY=RANGE*DSIN(AZ)
      double dy = range * sin(az);
      //          CALL MTXZRO(R,2,2)
      UtMatrixd r(2, 2);
      r.Fill(0.0);
      //          R(1,1)=SIGRNG*SIGRNG
      r(0, 0) = sigrng * sigrng;
      //          R(2,2)=RANGE*RANGE*SIGAZ*SIGAZ
      r(1, 1) = range * range * sigaz * sigaz;
      //*do a transformation of coordinates involving pure rotation to bring
      //*the system into our standard coordinate system
      //          A11=DX/RANGE
      double a11 = dx / range;
      //          A12=DY/RANGE
      double    a12 = dy / range;
      UtMatrixd b(2, 2);
      //          B(1,1)=A11
      b(0, 0) = a11;
      //          B(1,2)=-A12
      b(0, 1) = -a12;
      //          B(2,1)=A12
      b(1, 0) = a12;
      //          B(2,2)=A11
      b(1, 1) = a11;
      //          CALL TRNPSE(B,2,2,BT)
      UtMatrixd bt(4, 2);
      bt.Transpose(b);
      //*now compute the error covariance matrix for delx and dely
      //          CALL MTXMUL(B,2,2,R,2,2,TMP2X2)
      UtMatrixd tmp2x2(2, 2);
      tmp2x2.Multiply(b, r);
      //          CALL MTXMUL(TMP2X2,2,2,BT,2,2,R)
      r.Multiply(tmp2x2, bt);
      //*then we have the connection matrix that takes us from the state
      //*space of [x,y,xdot,zdot] to the measurement space of [x,y]
      UtMatrixd h(2, 4);
      //          CALL MTXZRO(H,2,4)
      h.Fill(0.0);
      //          H(1,1)=1
      h(0, 0) = 1.0;
      //          H(2,2)=1
      h(1, 1) = 1.0;
      //          CALL TRNPSE(H,2,4,HT)
      UtMatrixd ht;
      ht.Transpose(h);
      //*extract candidate information filter from GRL and copy to candiate
      //          WIN386=JNR(WIN384+3)
      MTT_CandidateTrack::Data* win386 = dynamic_cast<MTT_CandidateTrack::Data*>(win384->mDataPtr);
      //          DO I=1,52
      //             RCTRCK(I)=GRL(WIN386+I-1)
      //          ENDDO
      ctrck = *win386; // ctrck == (double)rctrck
      //*update the horizontal candidate information filter
      UtMatrixd w(2);
      //          W(1)=DBLE(GRL(PTR388+14))
      w(0) = mParameters.mCandidateTrackX_Acceleration;
      //          W(2)=DBLE(GRL(PTR388+15))
      w(1) = mParameters.mCandidateTrackY_Acceleration;
      //          CALL MTTUHI(DT,W,PIC,XIC,H,HT,R,RDRX,RDRY,DX,DY,2)
      mttuhi(dt, w, pic, xic, h, ht, r, rdrx, rdry, dx, dy, 2);
      //*vertical channel mesurement data
      //          CALL MTXZRO(RINVV,2,2)
      UtMatrixd rinvv(2, 2);
      //          DZ=0.0
      double dz = 0.0;
      //          IF ((IEL.EQ.1) .AND. (IRNG.EQ.1)) THEN
      if ((iel == 1) && (irng == 1))
      {
         //*measurement error covariance matrix transformed to enu z-axis
         //             VARZ=DSIN(EL)*DSIN(EL)*SIGRNG*SIGRNG
         double sinEl = sin(el);
         double varz  = sinEl * sinEl * sigrng * sigrng;
         //             VARZ=VARZ+SRANGE*SRANGE*SIGEL*SIGEL
         varz += srange * srange * sigel * sigel;
         //             RINVV(1,1)=1.0/VARZ
         rinvv(0, 0) = 1.0 / varz;
         //*measurement transformed to enu z-axis
         //             DZ=SRANGE*DSIN(EL)
         dz = srange * sinEl;
         //*update vertical measurement time
         //             GRL(WIN384+7)=TIME
         win384->mVerticalChannelUpdateTime = aSimTime;
         //          ENDIF
      }
      //*update the vertical candidate information filter
      //          W(1)=DBLE(GRL(PTR388+12))
      w(0) = mParameters.mVerticalVelocity;
      //          W(2)=DBLE(GRL(PTR388+13))
      w(1) = mParameters.mVerticalVelocityDecorrelationTime;
      //          CALL MTTUVI(DT,W,PIVC,XIVC,RINVV,RDRZ,DZ,IRNG,IEL,2)
      mttuvi(dt, w, pivc, xivc, rinvv, rdrz, dz, irng, iel, 2);
      //          CALL MTTPVF(JNR,GRL,PIVC,PTR388,WIN384)
      mttpvf(pivc, *win384);
      //
      //*to promote to candidate track, cond number must be sufficently small
      //          CNDTST=COND(PIC,TMP4X4,DUM1,DUM2,DUM3,4)
      double cndtst = pic.GetConditionNumber();
      //          CNDTHR=DBLE(GRL(PTR388+16))
      double cndthr = mParameters.mPromoteTrackHorizontalThreshold;
      //          MTTCCT=-1
      mttcct = -1;
      //          PROMOT=.FALSE.
      bool promot = false;
      //          IF (CNDTST.LT. CNDTHR) THEN
      if (cndtst < cndthr)
      {
         //*invert information matrix and check for adequate error bounds
         //*on speed.  promote to candidate track if within bounds.
         //             CALL MTXINV(PIC,4,PC,DUM1,ERR)
         UtMatrixd pc(pic);
         pc.Invert();
         //             VARDST=DBLE(GRL(PTR388+21))
         double vardst = mParameters.mPositionVariancePromoteCandidateTrack;
         //             VARSPD=DBLE(GRL(PTR388+22))
         double varspd = mParameters.mVelocityVariancePromoteCandidateTrack;
         //             IF((PC(1,1).LT.VARDST) .AND. (PC(2,2).LT.VARDST) .AND.
         //     +          (PC(3,3).LT.VARSPD) .AND. (PC(4,4).LT.VARSPD)) THEN
         if ((pc(0, 0) < vardst) && (pc(1, 1) < vardst) && (pc(2, 2) < varspd) && (pc(3, 3) < varspd))
         {
            //*temporarily promote winning candidate track
            //                CALL MTTC2A(JNR,GRL,WIN386,TMP387,ERR)
            bool err = false;
            mttc2a(*win386, tmp387, err);
            //                IF (.NOT. ERR) THEN
            if (!err)
            {
               //*promote the candidate track
               //                   PROMOT=.TRUE.
               promot = true;
               //*update temp active track
               //                   JNR(WIN384+3)=TMP387
               win384->mDataPtr = &tmp387;
               //                   CALL MTTUAT(JNR,GRL,TIME,PTR017,PTR388,WIN384)
               mttuat(aSimTime, *ptr017, *win384); // todo code
               //                   JNR(WIN384+3)=WIN386
               win384->mDataPtr = win386;
               //*find end of active tracks linked list
               //                   ATP384=JNR(PTR383+5)
               //                   ATPLST=PTR383+5
               //                   DO WHILE (ATP384.NE.0)
               //                      ATPLST=ATP384
               //                      ATP384=JNR(ATPLST)
               // 200                END DO
               //                   CALL DATGET(JNR,GRL,1,-384, PTR384)
               // (mjm) at this point, we're "re-using" ptr384, need to call it something else.
               auto atPtr = ut::make_unique<MTT_ActiveTrack>();
               //                   MTTCCT=PTR384
               // Return AT pointer as a parameter:
               aActiveTrackPtr = atPtr.get();
               mttcct          = 1; // Result is correlated
               // NOTE: Track cannot be 'linked' (added) to the list until the track ID is assigned
               //*add new 384 to end of linked list
               //                   JNR(ATPLST)=PTR384
               //                   CALL DATGET(JNR,GRL,1,-387, PTR387)
               //                   DO I=1,184
               //                      GRL(PTR387+I-1)=GRL(TMP387+I-1)
               //                   ENDDO
               atPtr->SetData(tmp387);
               //*increment track id
               //                   JNR(PTR383+4)=JNR(PTR383+4)+1
               ++(ptr383->mActiveTrackCounter);
               //*load 384 block
               //                   JNR(PTR384)=0
               //                   GRL(PTR384+1)=TIME
               atPtr->mUpdateTime = aSimTime;
               //                   JNR(PTR384+2)=JNR(PTR383+4)
               atPtr->mTrackId = ptr383->mActiveTrackCounter;
               //                   JNR(PTR384+3)=PTR387
               //                   JNR(PTR384+6)=JNR(WIN384+6)
               atPtr->mVerticalChannelActive = win384->mVerticalChannelActive;
               //                   GRL(PTR384+7)=GRL(WIN384+7)
               atPtr->mVerticalChannelUpdateTime = win384->mVerticalChannelUpdateTime;
               //*drop the winning candidate track
               //                   PTR384=JNR(PTR383+3)
               //                   LST384=PTR383+3
               //                   DO WHILE (PTR384.GT.0)
               //                      IF (PTR384.EQ.WIN384) THEN
               //                         CALL DATGIV(JNR,GRL,-386,0,0, WIN386 )
               //                         NXT384=JNR(WIN384)
               //                         JNR(LST384)=NXT384
               //                         CALL DATGIV(JNR,GRL,-384,0,0, WIN384 )
               //                         GO TO 300
               //                      ENDIF
               //                      LST384=PTR384
               //                      PTR384=JNR(LST384)
               //                   END DO
               if (mDebug)
               {
                  auto logger = ut::log::debug() << "Promoting candidate track to active track.";
                  logger.AddNote() << "T = " << aSimTime;
                  logger.AddNote() << "Name: " << mName;
                  logger.AddNote() << "Track ID: " << win384->mTrackId;
                  logger.AddNote() << "Active Track ID: " << atPtr->mTrackId;
               }
               // Add to active track list (see NOTE above)
               mTracks.mActiveTrackList.Add(std::move(atPtr));
               WsfObserver::ActiveTrackInitiated (
                  &mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, aActiveTrackPtr, win384);
               WsfObserver::CandidateTrackDropped (&mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, win384);
               lst384.Delete(win384->GetId());
               //  300              CONTINUE
               //                ENDIF
            }
            else if (mDebug)
            {
               auto logger = ut::log::debug() << "Cannot promote candidate track. Velocity/Position error too large.";
               logger.AddNote() << "T = " << aSimTime;
               logger.AddNote() << "Name: " << mName;
               logger.AddNote() << "Track ID: " << win384->mTrackId;
            }
            //             ENDIF
         }
         else if (mDebug)
         {
            auto logger = ut::log::debug() << "Cannot promote candidate track. Horizontal threshold too large.";
            logger.AddNote() << "T = " << aSimTime;
            logger.AddNote() << "Name: " << mName;
            logger.AddNote() << "Track ID: " << win384->mTrackId;
         }
         //          ENDIF
      }
      //          IF (.NOT.PROMOT) THEN
      if (!promot)
      {
         //*copy updated candidate information filter data block to GRL
         //             DO I=1,52
         //                GRL(WIN386+I-1)=RCTRCK(I)
         //             ENDDO
         *win386 = ctrck;
         //*update time in 384 block
         //             GRL(WIN384+1)=TIME
         win384->mUpdateTime = aSimTime;
      }
      //          ENDIF
   }
   //       ENDIF
   //*return memory for temporary 387 block
   //  999  CALL DATGIV(JNR,GRL,-387,0,0, TMP387 )
   //
   //       RETURN
   return mttcct;
   //       END
}

//       SUBROUTINE MTTDPS(JNR,GRL,TIME,PTR017,PTR383,PTR388,LNKEY,
//     +  PTR384,IOPR,ITRK)
//! Multiple target tracking - detection processing system (correlation).
void MTT::mttdps_Correlation(const double&               aSimTime,
                             const MTT_PerceivedCluster& aMeasurement,
                             MTT_ActiveTrack*&           aTrackPtr) // out
{
   //*update mtt system with a new (possibly the first) measurement;

   //*fred kiefer;
   //*bogdon associates inc;
   //*june 14, 2005;

   const MTT_PerceivedCluster* ptr017 = &aMeasurement;
   MTT_ActiveTrack*&           ptr384 = aTrackPtr;
   //       IMPLICIT NONE

   //       INTEGER JNR(*)
   //       REAL GRL(*),TIME

   //*pointer to radar measurement block;
   //       INTEGER MTTCAT,PTR017,PTR383,PTR384,PTR388,PTR363,LNKEY
   //       INTEGER IOPR,ITRK

   //*pointer to external track header block
   //       PTR363=JNR(PTR017+37)
   // (var. not used) const MTT_CombinedTrack* ptr363 = ptr017->mCombinedTrackPtr;

   //*when 2d/3d radar measurement:
   //       IF (JNR(PTR017+41).EQ.0) THEN
   if (ptr017->mTrackNumber == 0)
   {
      //*check to see if 2d/3d measurement belongs to an active track
      //          PTR384=MTTCAT(JNR,GRL,TIME,PTR017,PTR383,PTR388,LNKEY )
      ptr384 = mttcat(aSimTime, *ptr017);

      //(mjm) skip fusion
      //          //*update active track if one has been selected
      //          IF (PTR384.GT.0) THEN
      if (ptr384 != nullptr)
      {
         //             CALL MTTUAT(JNR,GRL,TIME,PTR017,PTR388,PTR384)
         mttuat(aSimTime, *ptr017, *ptr384);
         // C ========= *set output flags for .ctk file ===============
         //              IOPR=3
         //              ITRK=0
      }
      //          ELSE
      //            else
      if (ptr384 == nullptr)
      {
         //           *see if the measurement promotes one of the candidate tracks
         //             CALL MTTCCT(JNR,GRL,TIME,PTR017,PTR383,PTR388,LNKEY,
         //     +        PTR384,ITRK)
         //             IOPR=1
         int CT_CorrelationResult = mttcct(aSimTime, *ptr017, ptr384);
         //=======================================================================
         //            upon return from MTTCTT, PTR384 either:
         //              =  1 - correlated and promoted CT to AT (changed from >0 in Suppressor (mjm))
         //              = -1 - updated correlated CT (write this to .ctk)
         //              =  0 - no correlation to CT; continue with ET's
         //=======================================================================
         //             IF (PTR384.EQ.0) THEN
         // if (ptr384 == 0)
         if (CT_CorrelationResult == 0)
         {
            //*use measurement to create a new embryonic track and
            //*upgrade existing embryonic tracks to candidate tracks
            //                CALL MTTET(JNR,GRL,TIME,PTR017,PTR383,PTR388,LNKEY )
            mttet(aSimTime, *ptr017);
            if (mDebug)
            {
               auto logger = ut::log::debug() << "No correlation (eval. of embryonic track).";
               logger.AddNote() << "T = " << aSimTime;
               logger.AddNote() << "Name: " << mName;
            }
            //             ENDIF
         }
         else
         {
            if (CT_CorrelationResult == 1)
            {
               if (mDebug)
               {
                  auto logger = ut::log::debug() << "Promotion of candidate to active track.";
                  logger.AddNote() << "T = " << aSimTime;
                  logger.AddNote() << "Name: " << mName;
                  logger.AddNote() << "Track ID: " << ptr384->mTrackId;
               }
               // mjm not ready to output active track for fusion
               // TODO-JAJ aTrackPtr = 0;  // TODO mark as candidate track?
            }
            else
            {
               assert(CT_CorrelationResult == -1);
               if (mDebug)
               {
                  auto logger = ut::log::debug() << "Candidate Track Correlation (continue with eval).";
                  logger.AddNote() << "T = " << aSimTime;
                  logger.AddNote() << "Name: " << mName;
               }
            }
         }
         //          END IF
      }
      else if (mDebug)
      {
         auto logger = ut::log::debug() << "Correlation with active track.";
         logger.AddNote() << "T = " << aSimTime;
         logger.AddNote() << "Track ID: " << ptr384->mTrackId;
      }
   }
   //*otherwise, height finder measurement: (mjm-fusion)
   //       ELSE
   else
   {
      //        *process height finder measurement
      //          CALL MTTUHF(JNR,GRL,TIME,PTR017,PTR388,PTR383,PTR384)
      mttuhf(aSimTime, *ptr017, ptr384);
      // C ====== *set output flags for .ctk file ===============
      //           IOPR=3
      //           ITRK=0
      //          //*end of test for 2d/3d or height finder.
      //        ENDIF
   }
   //       RETURN
   //       END
}

// SUBROUTINE MTTDPS(JNR,GRL,TIME,PTR017,PTR383,PTR388,PTR384)
//! Multiple target tracking - detection processing system (fusion).
void MTT::mttdps_Fusion(const double&               aSimTime,
                        const MTT_PerceivedCluster& aMeasurement,
                        MTT_ActiveTrack*            aTrackPtr) // in-out
{
   //*update mtt system with a new (possibly the first) measurement;

   //*fred kiefer;
   //*bogdon associates inc;
   //*june 14, 2005;

   const MTT_PerceivedCluster* ptr017 = &aMeasurement;
   // MTT_ActiveTrack*               ptr384 = aTrackPtr;
   //       IMPLICIT NONE

   //       INTEGER JNR(*)
   //       REAL GRL(*),TIME

   //*pointer to radar measurement block;
   //       INTEGER MTTCAT,MTTCCT,PTR017,PTR383,PTR384,PTR388,PTR363

   //*pointer to external track header block
   //       PTR363=JNR(PTR017+37)
   // (var. not used) const MTT_CombinedTrack* ptr363 = ptr017->mCombinedTrackPtr;

   //*when 2d/3d radar measurement:
   //       IF (JNR(PTR017+41).EQ.0) THEN
   if (ptr017->mTrackNumber == 0)
   {
      //        *check to see if 2d/3d measurement belongs to an active track
      //          PTR384=MTTCAT(JNR,GRL,TIME,PTR017,PTR383,PTR388)
      //             aTrackPtr = mttcat(aSimTime, *ptr017);
      //
      //        *update active track if one has been selected
      //          IF (PTR384.GT.0) THEN
      //            if (aTrackPtr != 0)
      //            {
      //             CALL MTTUAT(JNR,GRL,TIME,PTR017,PTR388,PTR384)
      // temp               mttuat(aSimTime, *ptr017, *ptr384);
      //            }
      //          ELSE
      //            else
      //            {
      //           *see if the measurement promotes one of the candidate tracks
      //             PTR384=MTTCCT(JNR,GRL,TIME,PTR017,PTR383,PTR388)
      //               aTrackPtr = mttcct(aSimTime, *ptr017);
      //=======================================================================
      //            upon return from MTTCTT, PTR384 either:
      //              >  0 - correlated and promoted CT to AT
      //              = -1 - updated correlated CT (write this to .ctk)
      //              =  0 - no correlation to CT; continue with ET's
      //=======================================================================
      //             IF (PTR384.EQ.0) THEN
      //               if (aTrackPtr == 0)
      //               {
      //              *use measurement to create a new embryonic track and
      //              *upgrade existing embryonic tracks to candidate tracks
      //                CALL MTTET(JNR,GRL,TIME,PTR017,PTR383,PTR388)
      //                  mttet(aSimTime, *ptr017);
      //             ENDIF
      //               }
      //          END IF
      //            }
   }
   //*otherwise, height finder measurement:
   //       ELSE
   else
   {
      //        *process height finder measurement
      //          CALL MTTUHF(JNR,GRL,TIME,PTR017,PTR388,PTR383,PTR384)
      // (mjm) note that the local track may evaluate to zero
      // check if this is an internal error (todo).
      mttuhf(aSimTime, *ptr017, aTrackPtr);
      //*end of test for 2d/3d or height finder.
      //       ENDIF
   }
   //       RETURN
   //       END
}

//       SUBROUTINE MTTET(JNR,GRL,TIME,PTR017,PTR383,PTR388,LNKEY )
//! Multiple target tracking - update embryonic tracks.
void MTT::mttet(double aSimTime, const MTT_PerceivedCluster& aMeasurement)
{
   //     *measurement cannot be associated with either an active track or a
   //     *candidate track, so we make a new embryonic track and possibly
   //     *upgrade one or more embryonic tracks to candidate tracks;

   //     *if we arrive here, we have been unable to associate the detection with
   //     *either an existing local track or an existing candidate track, so we'll
   //     *create as many new candidate tracks as there are existing embryonic
   //     *tracks and we'll add one more embryonic track;

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *june 14, 2005;

   const MTT_PerceivedCluster* ptr017 = &aMeasurement;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER LMASTR,LSUPPL,LNKEY
   //       INTEGER I,PTR017,PTR383,PTR388,PTR384,PTR359,LST384,NXT384,PTR385
   //       INTEGER PTR360,IAVAIL,IEL,IRNG,IDUM(100)
   //       INTEGER ETRKID,CTRKID,PTC384,LSC384,PTR386,LSORCE,LRCVR
   //
   //       DOUBLE PRECISION DZ
   //       REAL RETRCK(52),RCTRCK(52)
   //       DOUBLE PRECISION ETRCK(26),CTRCK(26),CNDTST,CNDTHR
   //       DOUBLE PRECISION XI(4),PI(4,4),PIV(2,2),XIV(2),W(2)
   //       DOUBLE PRECISION XIC(4),PIC(4,4),PIVC(2,2),XIVC(2),COND
   //
   //       REAL TIME,TRKTIM,SNTVL
   //       DOUBLE PRECISION DT,DTV
   //       DOUBLE PRECISION SRANGE,RANGE,AZ,EL,DX,DY,R(2,2),RINVV(2,2)
   //       DOUBLE PRECISION SIGRNG,SIGAZ,SIGEL,PC(4,4),TMP4X4(4,4)
   //       DOUBLE PRECISION RDRX,RDRY,RDRZ,VARSPD,VARZ
   //       DOUBLE PRECISION A11,A12,TMP2X2(2,2),H(2,4),HT(4,2)
   //       DOUBLE PRECISION B(2,2),BT(2,2),DUM1(100),DUM2(100),DUM3(100)
   //       DOUBLE PRECISION DUM4(100),DUM5(100),HRZFLT(4),SPDHRZ,MAXSPD
   //       DOUBLE PRECISION TWOPI
   //       DATA TWOPI / 6.28318530717959/
   //
   //       EQUIVALENCE (RETRCK(1),ETRCK(1))
   //       EQUIVALENCE (PI,ETRCK(1)),(XI,ETRCK(17))
   //       EQUIVALENCE (PIV,ETRCK(21)),(XIV,ETRCK(25))
   //       EQUIVALENCE (RCTRCK(1),CTRCK(1))
   //       EQUIVALENCE (PIC,CTRCK(1)),(XIC,CTRCK(17))
   //       EQUIVALENCE (PIVC,CTRCK(21)),(XIVC,CTRCK(25))
   //       LOGICAL ERR,PRMTED,OKHITS,LHITS
   //       EXTERNAL LHITS
   //
   //       DOUBLE PRECISION XDD,YDD,VRTFLT(2)
   //
   //       COMMON /TRUST/ TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,
   //     +         TRUSPD,IDTRGT,IDSNDR,IDRCVR,PLTRGT,PLSNDR,PLRCVR,GAMTIM
   //       REAL TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,TRUSPD
   //       INTEGER IDTRGT,IDSNDR,IDRCVR
   //       CHARACTER*20 PLTRGT,PLSNDR,PLRCVR
   //       REAL GAMTIM


   MTT_EmbryonicTrack::Data etrck;
   UtMatrixd&               pi  = etrck.mErrorInfoMatrix;
   UtMatrixd&               xi  = etrck.mHorizontalState;
   UtMatrixd&               piv = etrck.mVerticalErrorInfoMatrix;
   UtMatrixd&               xiv = etrck.mVerticalState;

   MTT_CandidateTrack::Data ctrck;
   UtMatrixd&               pic  = ctrck.mErrorInfoMatrix;
   UtMatrixd&               xic  = ctrck.mHorizontalState;
   UtMatrixd&               pivc = ctrck.mVerticalErrorInfoMatrix;
   UtMatrixd&               xivc = ctrck.mVerticalState;

   // Additional definitions made necessary by use of goto statement:
   UtMatrixd w(2);

   //*for each embryonic track promoted to candidate and for the new embryonic
   //*track we'll need the following measurement update information
   //*use measurement to update the information matrix and state as follows;
   //
   //*initialize for output incidents;
   //       LMASTR=JNR(2)-JNR(1)
   //       LSUPPL=JNR(JNR(JNR(LMASTR+18)+7)+1)

   //*pointer to measurement header (359 block)
   //       PTR359=JNR(PTR017+39)
   MTT_Measurement* ptr359 = ptr017->mMeasurementPtr;
   //
   //*extract necessary radar data
   //       PTR360=JNR(PTR359+4)
   MTT_RadarMeasurement::Data* ptr360 = &(dynamic_cast<MTT_RadarMeasurement*>(ptr359)->GetData());
   //       SRANGE=DBLE(GRL(PTR360))
   double srange = ptr360->mRange;
   //       AZ=DBLE(GRL(PTR360+1))
   double az = ptr360->mAzimuth;
   //       EL=DBLE(GRL(PTR360+2))
   double el = ptr360->mElevation;
   //       SIGRNG=DBLE(GRL(PTR360+4))
   double sigrng = ptr360->mRangeStDev;
   //       SIGAZ=DBLE(GRL(PTR360+5))
   double sigaz = ptr360->mAzimuthStDev;
   //       SIGEL=DBLE(GRL(PTR360+6))
   double sigel = ptr360->mElevationStDev;
   //       RDRX=DBLE(GRL(PTR360+8))
   double rdrx = ptr360->mSensorLocation[0];
   //       RDRY=DBLE(GRL(PTR360+9))
   double rdry = ptr360->mSensorLocation[1];
   //       RDRZ=DBLE(GRL(PTR360+10))
   double rdrz = ptr360->mSensorLocation[2];
   //
   //*determine if elevation is available and adjust for ground range
   //       IAVAIL=JNR(PTR359+6)
   unsigned iavail = ptr359->mDataAvailable;
   //       IEL=MOD(IAVAIL/4,2)
   unsigned iel = (iavail / 4) % 2;
   //       IF (IEL.EQ.1) THEN
   double range = srange; // Use the initialization to minimize computation.
   if (iel == 1)
   {
      //          RANGE=SRANGE*DCOS(EL)
      range *= cos(el);
   }
   //       IRNG=MOD(JNR(PTR359+6),2)
   unsigned irng = ptr359->mDataAvailable % 2;
   //
   //*statistics for delx and dely are in a system that has been rotated "az"
   //*degrees,where "az" measured counterclockwise from east (per suppressor);
   //       DX=RANGE*DCOS(AZ)
   double dx = range * cos(az);
   //       DY=RANGE*DSIN(AZ)
   double dy = range * sin(az);
   //       CALL MTXZRO(R,2,2)
   UtMatrixd r(2, 2);
   //       R(1,1)=SIGRNG*SIGRNG
   r(0, 0) = sigrng * sigrng;
   //       R(2,2)=RANGE*RANGE*SIGAZ*SIGAZ
   r(1, 1) = range * range * sigaz * sigaz;
   //*do a transformation of coordinates involving pure rotation to bring the
   //*system into our standard coordinate system
   //       A11=DX/RANGE
   double a11 = dx / range;
   //       A12=DY/RANGE
   double a12 = dy / range;
   //       B(1,1)=A11
   UtMatrixd b(2, 2);
   b(0, 0) = a11;
   //       B(1,2)=-A12
   b(0, 1) = -a12;
   //       B(2,1)=A12
   b(1, 0) = a12;
   //       B(2,2)=A11
   b(1, 1) = a11;
   //       CALL TRNPSE(B,2,2,BT)
   UtMatrixd bt(2, 2);
   bt.Transpose(b);
   //*now compute the error covariance matrix for delx and dely
   //       CALL MTXMUL(B,2,2,R,2,2,TMP2X2)
   UtMatrixd tmp2x2;
   tmp2x2.Multiply(b, r);
   //       CALL MTXMUL(TMP2X2,2,2,BT,2,2,R)
   r.Multiply(tmp2x2, bt);
   //       CALL FIXSYM(R,2,1,1)
   r.Symmetrize();
   //*then we have the connection matrix that takes us from the state space of
   //*[x,y,xdot,zdot] to the measurement space of [x,y]
   //       CALL MTXZRO(H,2,4)
   UtMatrixd h(2, 4);
   //       H(1,1)=1
   h(0, 0) = 1.0; // (identity 2x2 matrix)
   //       H(2,2)=1
   h(1, 1) = 1.0;
   //       CALL TRNPSE(H,2,4,HT)
   UtMatrixd ht(4, 2);
   ht.Transpose(h);
   //*vertical channel mesurement data
   //       CALL MTXZRO(RINVV,2,2)
   UtMatrixd rinvv(2, 2);
   //       DZ=0.0
   double dz = 0.0;
   //       IF ((IEL.EQ.1) .AND. (IRNG.EQ.1)) THEN
   if ((iel == 1) && (irng == 1))
   {
      //*measurement error covariance matrix transformed to enu z-axis
      //             VARZ=DSIN(EL)*DSIN(EL)*SIGRNG*SIGRNG
      double varz = sin(el) * sin(el) * sigrng * sigrng;
      //             VARZ=VARZ+SRANGE*SRANGE*SIGEL*SIGEL
      varz += srange * srange * sigel * sigel;
      //             RINVV(1,1)=1.0/VARZ
      rinvv(0, 0) = 1.0 / varz;
      //*measurement transformed to enu z-axis
      //             DZ=SRANGE*DSIN(EL)
      dz = srange * sin(el);
      //       ENDIF
   }
   //
   //*now loop over linked list of embryonic tracks and promote each to a
   //*candidate track
   //       PRMTED=.FALSE.
   // (not used) bool prmted = false;
   //       PTR384=JNR(PTR383+1)
   //       LST384=PTR383+1
   MTT_Tracks::MTT_EmbryonicTrackList& lst384   = mTracks.mEmbryonicTracks;
   unsigned                            trackNum = 0;
   //       DO WHILE (PTR384.NE.0)
   while (trackNum < lst384.GetCount())
   {
      //*determine if track is still active
      //          TRKTIM=GRL(PTR384+1)
      MTT_EmbryonicTrack* ptr384 = lst384.GetEntry(trackNum);
      double              trktim = ptr384->mUpdateTime;
      //          PTR385=JNR(PTR384+3)
      MTT_EmbryonicTrack::Data* ptr385 = &ptr384->GetData();
      //          DT=DBLE(TIME-TRKTIM)
      double dt = aSimTime - trktim;
      //          IF (DT.GT.DBLE(GRL(PTR388+11))) THEN
      if (dt > mParameters.mEmbryonicTrackDropTime)
      {
         //*drop track:  first give back track data block
         //             CALL DATGIV(JNR,GRL,-385,0,0, PTR385 )
         // C*====================================================== .ctk =======;
         //             IF(MOD(JNR(LMASTR+30)/2,2).EQ.1)THEN
         //                WRITE(62,1113)TIME,2,0,JNR(PTR384+2),0,IDTRGT,PLTRGT,
         //     +          IDSNDR,PLSNDR,IDRCVR,PLRCVR
         // 1113           FORMAT(E15.7,2I3,2I8,3(I8,1X,A20),2X,15E15.7)
         //             ENDIF
         // C*====================================================== .ctk =======;
         // C           *write drops-mttc-embryonic-track incident;
         //             JNR(LSUPPL)=JNR(PTR384+2)
         //             CALL OUTSIT(JNR,GRL,212,TIME,LNKEY,-1,-1,-1,-1,-1 )

         //*reset links
         //             NXT384=JNR(PTR384)
         //             JNR(LST384)=NXT384
         //             CALL DATGIV(JNR,GRL,-384,0,0, PTR384 )
         WsfObserver::EmbryonicTrackDropped (&mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, ptr384);
         lst384.Delete(ptr384->GetId());
         // NOTE: Do NOT increment 'trackNum'. The next (and following entries) are shifted to fill
         //       the hole made by deleting the current entry.
         //             PTR384=NXT384
         //             GOTO 100
         continue;
         //          END IF
      }
      //*determine if vertical channel is still active
      //          IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //             DTV=DBLE(TIME-GRL(PTR384+7))
         double dtv = aSimTime - ptr384->mVerticalChannelUpdateTime;
         //             IF (DTV.GT.DBLE(GRL(PTR388+32))) THEN
         if (dtv > mParameters.mVerticalChannelDropTime)
         {
            //*reset flags, time, and information matrices
            //                JNR(PTR384+6)=0
            ptr384->mVerticalChannelActive = false;
            //                GRL(PTR384+7)=0.
            ptr384->mVerticalChannelUpdateTime = 0.0;
            //                DO I=40,51
            //                   GRL(PTR385+I)=0.
            //                ENDDO
            ptr385->mVerticalErrorInfoMatrix.Fill(0.0);
            ptr385->mVerticalState.Fill(0.0);
            //             ENDIF
         }
         //          ENDIF
      }
      //*extract embryonic information filter from GRL and copy to candidate
      //          DO I=1,52
      //             RCTRCK(I)=GRL(PTR385+I-1)
      //          ENDDO
      ctrck.CopyFromEmbryonicData(*ptr385);
      //*update the horizontal candidate information filter
      //          W(1)=DBLE(GRL(PTR388+14))
      w(0) = mParameters.mCandidateTrackX_Acceleration;
      //          W(2)=DBLE(GRL(PTR388+15))
      w(1) = mParameters.mCandidateTrackY_Acceleration;
      //          CALL MTTUHI(DT,W,PIC,XIC,H,HT,R,RDRX,RDRY,DX,DY,1)
      mttuhi(dt, w, pic, xic, h, ht, r, rdrx, rdry, dx, dy, 1);
      //*update the vertical candidate information filter
      //          W(1)=DBLE(GRL(PTR388+12))
      w(0) = mParameters.mVerticalVelocity;
      //          W(2)=DBLE(GRL(PTR388+13))
      w(1) = mParameters.mVerticalVelocityDecorrelationTime;
      //          CALL MTTUVI(DT,W,PIVC,XIVC,RINVV,RDRZ,DZ,IRNG,IEL,1)
      mttuvi(dt, w, pivc, xivc, rinvv, rdrz, dz, irng, iel, 1);
      //          CALL MTTPVF(JNR,GRL,PIVC,PTR388,PTR384)
      mttpvf(pivc, *ptr384);
      //          IF ((IEL.EQ.1).AND.(IRNG.EQ.1)) GRL(PTR384+7)=TIME
      //
      if ((iel == 1) && (irng == 1))
      {
         ptr384->mVerticalChannelUpdateTime = aSimTime;
      }
      //*to promote to candidate track, cond number must be sufficently small
      //          CNDTST=COND(PIC,TMP4X4,DUM1,DUM2,DUM3,4)
      double cndtst = pic.GetConditionNumber();
      //          CNDTHR=DBLE(GRL(PTR388+16))
      double cndthr = mParameters.mPromoteTrackHorizontalThreshold;
      //          IF (CNDTST.LT. CNDTHR) THEN
      if (cndtst < cndthr)
      {
         //*invert information matrix and check for adequate error bounds
         //*on speed.  promote to candidate track if within bounds.
         //             CALL MTXINV(PIC,4,PC,DUM1,ERR)
         UtMatrixd pc(pic);
         pc.Invert();
         //             VARSPD=DBLE(GRL(PTR388+17))
         double varspd = mParameters.mVelocityVariancePromoteEmbryonicTrack;
         //             IF ((PC(3,3)+PC(4,4)).LT.VARSPD) THEN
         if ((pc(2, 2) + pc(3, 3)) < varspd)
         {
            //             CALL DFMCP(PIC,4,XIC,1,DUM1,DUM2,DUM3,DUM4,DUM5,IDUM,
            //     +        HRZFLT,ERR)
            //(mjm) above is solving for 'x' where Ax=B, A==PIC, B==XIC, x==HRZFLT
            UtMatrixd hrzflt(4, 1);   // solution vector 'x' for ax=b
            hrzflt.Multiply(pc, xic); // because 'pc' is the inverse of 'pic.'
            // (mjm) done with dfmcp call.

            //*get vertical states if they exist
            //             CALL DFMCP(PIVC,2,XIVC,1,DUM1,DUM2,DUM3,DUM4,DUM5,IDUM,
            //     +             VRTFLT,ERR)
            //(mjm) above is solving for 'x' where Ax=B, A==PIVC, B==XIVC, x==VRTFLT
            UtMatrixd vrtflt(2, 1); // solution vector 'x' for ax=b
            UtMatrixd pvc(pivc);
            try
            {
               pvc.Invert2x2();
               vrtflt.Multiply(pvc, xivc);
            }
            catch (UtMatrixd::SingularMatrixError&)
            {
               //*if they do not exist, fill with zero
               //                IF (ERR) THEN
               //                   VRTFLT(1)=0.0
               //                   VRTFLT(2)=0.0
               //                END IF
            }
            // (mjm) done with dfmcp call.

            //             SPDHRZ=SQRT(HRZFLT(3)**2+HRZFLT(4)**2)
            double spdhrz = sqrt(hrzflt(2) * hrzflt(2) + hrzflt(3) * hrzflt(3));
            //             MAXSPD=DBLE(GRL(PTR388+2))
            double maxspd = mParameters.mVelocityLimitPromoteEmbryonicTrack;
            //             IF (SPDHRZ.LE.MAXSPD) THEN
            if (spdhrz < maxspd)
            {
               // if consecutive-single-source-hits required:
               //  todo-mjm only needed if the single source threshold > 0
               // OKHITS=.TRUE.
               bool okhits = true;
               //                IF(JNR(PTR388+9).GT.0)THEN
               if (mParameters.mPromoteSingleSourceTrackHitThreshold > 0)
               {
                  // if player sending the measurement is still alive;
                  // OKHITS=.FALSE.
                  // okhits = false;
                  // LSORCE=JNR(JNR(JNR(PTR017+3)+2))
                  // IF(LSORCE.GT.0)THEN
                  // find the sensor source of the measurement;
                  //  CALL LNKSEA(JNR,GRL,JNR(LSORCE+11),
                  //      +                JNR(PTR359+1),-1,4,0,-1,-1, LRCVR)
                  // if embryonic trk and measurement have same source:
                  // IF(LRCVR.GT.0)THEN
                  //    IF(JNR(PTR384+5).EQ.JNR(LRCVR+4))THEN
                  // if emb-trk time within last sense interval:
                  //  mjm-todo not supporting this check yet
                  //       SNTVL=1.0/GRL(JNR(JNR(JNR(LRCVR+2)+11))+2)
                  //       IF(GRL(PTR017+8)-GRL(PTR384+1).LE.SNTVL)THEN
                  // test consecutive-single-source-hits;
                  //          OKHITS=LHITS(JNR,GRL,LRCVR,JNR(PTR017+1),
                  //      +                        JNR(PTR388+9)
                  okhits = (ptr017->mConsecutiveSingleSourceHits >= mParameters.mPromoteSingleSourceTrackHitThreshold);
                  //*end if emb-trk time within sense interval.
                  // ENDIF
                  // ENDIF
                  // end test if e-trk and measurement have same source.
                  // ENDIF
                  // end test of player sending the measurement;
                  // ENDIF
                  // end test if consecutive-single-source-hits required.
                  // ENDIF
               }
               // C              *when consecutive-single-source-hits ok or not needed:
               //                 IF(OKHITS)THEN
               if (okhits)
               {
                  //                PRMTED=.TRUE.
                  // (not used) prmted = true;
                  //*get new candidate track number
                  //                JNR(PTR383+2)=JNR(PTR383+2)+1
                  ++mTracks.mCandidateTrackCounter;
                  //                CTRKID=JNR(PTR383+2)
                  int ctrkid = mTracks.mCandidateTrackCounter;
                  //*get last cd in linked list
                  //                LSC384=PTR383+3
                  //                PTC384=JNR(LSC384)
                  //                DO WHILE (PTC384.GT.0)
                  //                   LSC384=PTC384
                  //                   PTC384=JNR(LSC384)
                  //                END DO
                  //*allocate for new candidate track header (384 block)                  //*and store the pointer to
                  // this bloc in jnr(lst384);
                  //                CALL DATGET(JNR,GRL,1,-384, PTC384)
                  auto ptc384 = ut::make_unique<MTT_CandidateTrack>();
                  // NOTE: Track cannot be 'linked' (added) to the list until the track ID is assigned
                  //                JNR(LSC384)=PTC384
                  //*allocate a new candidate track data block (386);
                  //                CALL DATGET(JNR,GRL,1,-386, PTR386)
                  MTT_CandidateTrack::Data* ptr386 = &ptc384->GetData();
                  //*fill in candidate track header block;
                  //                JNR(PTC384)=0
                  //                GRL(PTC384+1)=TIME
                  ptc384->mUpdateTime = aSimTime;
                  //                JNR(PTC384+2)=CTRKID
                  ptc384->mTrackId = ctrkid;
                  //                JNR(PTC384+3)=PTR386  // no need to set data.
                  //                JNR(PTC384+6)=JNR(PTR384+6)
                  ptc384->mVerticalChannelActive = ptr384->mVerticalChannelActive;
                  //                GRL(PTC384+7)=GRL(PTR384+7)
                  ptc384->mVerticalChannelUpdateTime = ptr384->mVerticalChannelUpdateTime;
                  //*with only two measurements, accl cannot be computed,
                  //* so fill with zero.
                  // mjm xdd, ydd not used
                  //                   XDD=0.0
                  //                   YDD=0.0
                  //                   IF(MOD(JNR(LMASTR+30)/2,2).EQ.1)THEN
                  //                     WRITE(62,1113)TIME,3,1,CTRKID,JNR(PTR384+2),IDTRGT,
                  //     +               PLTRGT,IDSNDR,PLSNDR,IDRCVR,PLRCVR,
                  //     +               (HRZFLT(I),I=1,4),(VRTFLT(I),I=1,2),
                  //     +               XDD,YDD,GRL(PTC384+7),
                  //     +               TRUEX,TRUEY,TRUVX,TRUVY,TRUEZ,TRUVZ
                  //                   ENDIF
                  //*write initiates-mttc-candidate-track incident;
                  //                   JNR(LSUPPL)=JNR(PTC384+2)
                  //                   JNR(LSUPPL+1)=JNR(PTR384+2)
                  //                   CALL OUTSIT(JNR,GRL,209,TIME,LNKEY,-1,-1,-1,-1,-1 )
                  //*load double precision candidate track data into GRL;
                  //                DO I=1,52
                  //                   GRL(PTR386+I-1)=RCTRCK(I)
                  //                ENDDO
                  *ptr386 = ctrck;
                  if (mDebug)
                  {
                     auto logger = ut::log::debug() << "Track promoted to candidate track.";
                     logger.AddNote() << "T = " << aSimTime;
                     logger.AddNote() << "Name: " << mName;
                     logger.AddNote() << "Track ID: " << ptr384->mTrackId;
                     logger.AddNote() << "Candidate Track ID: " << ptc384->mTrackId;
                  }
                  WsfObserver::CandidateTrackInitiated (
                     &mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, ptc384.get(), ptr384);
                  mTracks.mCandidateTracks.Add(std::move(ptc384));
                  //*end test consecutive-single-source-hits or not needed.
                  //              ENDIF
               }
               //             END IF
            }
            else if (mDebug)
            {
               auto logger = ut::log::debug() << "Embryonic track not promoted. Velocity limit check failed.";
               logger.AddNote() << "T = " << aSimTime;
               logger.AddNote() << "Name: " << mName;
            }
            //             END IF
         }
         else if (mDebug)
         {
            auto logger = ut::log::debug() << "Embryonic track not promoted. Velocity error limit failed.";
            logger.AddNote() << "T = " << aSimTime;
            logger.AddNote() << "Name: " << mName;
         }
         //          END IF
      }
      //       LST384=PTR384
      //       PTR384=JNR(LST384)
      //  100  END DO
      ++trackNum;
   }
   //
   //*instantiate new embryonic track
   //       DT=0.0
   double dt = 0.0;
   //*increment available track number
   //       JNR(PTR383)=JNR(PTR383)+1
   ++mTracks.mEmbryonicTrackCounter;
   //       ETRKID=JNR(PTR383)
   int etrkid = mTracks.mEmbryonicTrackCounter;
   //*allocate storage for a new embryonic track header (384 block) and store
   //*the pointer to this bloc in jnr(lst384);
   //       CALL DATGET(JNR,GRL,1,-384, PTR384)
   auto ptr384 = ut::make_unique<MTT_EmbryonicTrack>();
   // NOTE: Track cannot be 'linked' (added) to the list until the track ID is assigned
   //       JNR(LST384)=PTR384
   //*allocate storage for a new embryonic track data block (385 block)
   //       CALL DATGET(JNR,GRL,1,-385, PTR385)
   MTT_EmbryonicTrack::Data* ptr385 = &ptr384->GetData();
   //*fill in embryonic track header block
   //       JNR(PTR384)=0
   //       GRL(PTR384+1)=TIME
   ptr384->mUpdateTime = aSimTime;
   //       JNR(PTR384+2)=ETRKID
   ptr384->mTrackId = etrkid;
   //       JNR(PTR384+3)=PTR385
   //       (mjm- no set necessary here)
   //       JNR(PTR384+5)=JNR(PTR359+1)
   //       (mjm- interaction key not set)
   // C     *write initiates-mttc-embryonic-track incident;
   //       JNR(LSUPPL)=JNR(PTR384+2)
   //       CALL OUTSIT(JNR,GRL,211,TIME,LNKEY,-1,-1,-1,-1,-1 )

   //*initialize the horizontal candidate information filter
   //       W(1)=DBLE(GRL(PTR388+14))
   w(0) = mParameters.mCandidateTrackX_Acceleration;
   //       W(2)=DBLE(GRL(PTR388+15))
   w(1) = mParameters.mCandidateTrackY_Acceleration;
   //       CALL MTXZRO(PI,4,4)
   pi.Fill(0.0);
   //       CALL MTXZRO(XI,4,1)
   xi.Fill(0.0);
   //       CALL MTTUHI(DT,W,PI,XI,H,HT,R,RDRX,RDRY,DX,DY,1)
   mttuhi(dt, w, pi, xi, h, ht, r, rdrx, rdry, dx, dy, 1);
   //*initialize the vertical candidate information filter
   //       CALL MTXZRO(PIV,2,2)
   piv.Fill(0.0);
   //       CALL MTXZRO(XIV,2,1)
   xiv.Fill(0.0);
   //       W(1)=DBLE(GRL(PTR388+12))
   w(0) = mParameters.mVerticalVelocity;
   //       W(2)=DBLE(GRL(PTR388+13))
   w(1) = mParameters.mVerticalVelocityDecorrelationTime;
   //       CALL MTTUVI(DT,W,PIV,XIV,RINVV,RDRZ,DZ,IRNG,IEL,1)
   mttuvi(dt, w, piv, xiv, rinvv, rdrz, dz, irng, iel, 1);
   //       IF ((IEL.EQ.1).AND.(IRNG.EQ.1)) GRL(PTR384+7)=TIME
   if ((iel == 1) && (irng == 1))
   {
      ptr384->mVerticalChannelUpdateTime = aSimTime;
   }
   // C*====================================================== .ctk =======
   // C      *write embryonic track to output file;
   //         XPOS=RDRX+DX
   //         YPOS=RDRY+DY
   //         ZPOS=RDRZ+DZ
   // C      *with only one measurement: vel and accl cannot be computed
   //         IF(MOD(JNR(LMASTR+30)/2,2).EQ.1)THEN
   //            WRITE(62,1113)TIME,2,1,ETRKID,0,IDTRGT,PLTRGT,IDSNDR,PLSNDR,
   //      +     IDRCVR,PLRCVR,XPOS,YPOS,0.0,0.0,ZPOS,0.0,
   //      +     0.0,0.0,GRL(PTR384+7),TRUEX,TRUEY,TRUVX,TRUVY,
   //      +     TRUEZ,TRUVZ
   //         ENDIF
   // C*====================================================== .ctk =======
   //*load the double precision embryonic track data into GRL
   //        DO I=1,52
   //          GRL(PTR385+I-1)=RETRCK(I)
   //        ENDDO
   *ptr385 = etrck;
   // Add to embryonic track list (see NOTE above)
   WsfObserver::EmbryonicTrackInitiated (&mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, ptr384.get());
   mTracks.mEmbryonicTracks.Add(std::move(ptr384));
   //
   //       RETURN
   //       END
}

//       SUBROUTINE MTTFLF(JNR,GRL,XLF,PLF,XMST,PMST,XMSTP,PMSTP,
//     +                   PTR388,PTR359,DT,PZ0LF)
//!  Multiple target tracking - fuse external track into
//!              local straight flight filter
//!              with measurement.
void MTT::mttflf(UtMatrixd&       aFilterStates,   // in/out
                 UtMatrixd&       aHorizontalInfo, // in/out
                 const UtMatrixd& aFilterStatesUpdate,
                 const UtMatrixd& aHorizontalInfoUpdate,
                 const UtMatrixd& aPrevFilterStates,
                 const UtMatrixd& aPrevHorizInfoMatrix,
                 double           aDt,
                 double&          aLikelihoodValue) // out
{
   //     *use external track as a measurement & fuse into straight-flight filter

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *july 8, 2005;

   //       SUBROUTINE MTTFLF(JNR,GRL,XLF,PLF,XMST,PMST,XMSTP,PMSTP,
   //     +                   PTR388,PTR359,DT,PZ0LF)
   UtMatrixd&       xlf   = aFilterStates;
   UtMatrixd&       plf   = aHorizontalInfo;
   const UtMatrixd& xmst  = aFilterStatesUpdate;
   const UtMatrixd& pmst  = aHorizontalInfoUpdate;
   const UtMatrixd& pmstp = aPrevHorizInfoMatrix;
   double&          pzolf = aLikelihoodValue;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER I,J,PTR359,PTR388
   //       DOUBLE PRECISION XMST(4),PMST(4,4),XMSTP(4),PMSTP(4,4)
   //       DOUBLE PRECISION XLF(4),PLF(4,4),DT
   //       DOUBLE PRECISION X(4),P(4,4),PZ0LF
   // CBA       DOUBLE PRECISION DISTLF,DETLF
   //       DOUBLE PRECISION DISTLF(1,1),DETLF
   //       DOUBLE PRECISION DELZXY(2),SXY(2,2),TMP1X2(2)
   //       DOUBLE PRECISION TWOPI,DELZ(4),S(4,4)
   //       DOUBLE PRECISION DUM1(100),DUM2(100),DUM3(100)
   //       DOUBLE PRECISION MI(4,4),MIMST(4,4),PIMST(4,4),PILF(4,4)
   //       LOGICAL ERR
   //       DATA TWOPI / 6.28318530717959/
   //
   //       COMMON /TRUST/ TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,
   //     +         TRUSPD,IDTRGT,IDSNDR,IDRCVR,PLTRGT,PLSNDR,PLRCVR,GAMTIM
   //       REAL TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,TRUSPD
   //       INTEGER IDTRGT,IDSNDR,IDRCVR
   //       CHARACTER*20 PLTRGT,PLSNDR,PLRCVR
   //       REAL GAMTIM
   //
   //       DOUBLE PRECISION K(4,4),DELX(4)
   //       DOUBLE PRECISION TMP4X4(4,4),EYE4(4,4)
   //       DOUBLE PRECISION DUM4(100)
   //       LOGICAL CHECK,POSDEF
   //
   //*update local active filter to current time
   //       CALL MTXSET(PLF,4,4,P)
   UtMatrixd p(plf);
   //       CALL MTXSET(XLF,4,1,X)
   UtMatrixd x(xlf);
   //       CALL MTTLFP(JNR,GRL,PTR388,DT,P,X)
   mttlfp(aDt, p, x);
   //*compute innovation (residual)
   //       CALL MTXSUB(XMST,X,4,1,DELZ)
   UtMatrixd delz(4);
   delz.Subtract(xmst, x);
   //*compute covariance of innovation
   //       CALL MTXADD(PMST,P,4,4,S)
   UtMatrixd s(4, 4);
   s.Add(pmst, p);
   //*normalized innovation error over position states
   //       DO I=1,2
   //          DELZXY(I)=DELZ(I)
   //          DO J=1,2
   //             SXY(I,J)=S(I,J)
   //          ENDDO
   //       ENDDO
   UtMatrixd delzxy(1, 2);
   delzxy(0, 0) = delz(0);
   delzxy(0, 1) = delz(1);
   UtMatrixd sxy(2, 2);
   sxy.CopyFrom(s, 0, 0, 2, 2);
   //       CALL MTXDIV(DELZXY,SXY,1,2,DUM1,DUM2,DUM3,TMP1X2)
   UtMatrixd tmp1x2(1, 2);
   tmp1x2.Divide(delzxy, sxy);
   //       CALL MTXMUL(TMP1X2,1,2,DELZXY,2,1,DISTLF)
   UtMatrixd distlf(1);
   UtMatrixd delzxyt(2); // Must use transpose for matrix multiplication.
   delzxyt.Transpose(delzxy);
   distlf.Multiply(tmp1x2, delzxyt);
   //*to compute straight-flight likelihood function value, first compute
   //*the determinate of S, a 4x4 matrix that we'll expand about first row
   //       DETLF=SXY(1,1)*SXY(2,2)-SXY(1,2)*SXY(2,1)
   double detlf = sxy(0, 0) * sxy(1, 1) - sxy(0, 1) * sxy(1, 0);
   //*compute straight-flight likelihood function value
   //       IF ((DETLF.GE.0.0) .AND. (DISTLF(1,1).LE.50.0)) THEN
   if ((detlf >= 0.0) && (distlf(0, 0) <= 50.0))
   {
      //          PZ0LF=(1.0/(TWOPI*DSQRT(DETLF)))*DEXP(-0.5*DISTLF(1,1))
      pzolf = (1.0 / (UtMath::cTWO_PI * sqrt(detlf))) * exp(-0.5 * distlf(0, 0));
      //          IF (PZ0LF.LT. 1.0D-10) PZ0LF=1.0D-10
      if (pzolf < 1.0e-10)
      {
         pzolf = 1.0e-10;
      }
      //       ELSE
   }
   else
   {
      //          PZ0LF=1.0D-10
      pzolf = 1.0e-10;
      //       ENDIF
   }
   //
   //*combine local with external using centralized kalman filter
   //       CALL MTXINV(P,4,MI,DUM1,ERR)
   UtMatrixd mi(p);
   mi.Invert();
   //       CALL FIXSYM(MI,4,1,4)
   mi.Symmetrize();
   //       CALL MTXINV(PMSTP,4,MIMST,DUM1,ERR)
   UtMatrixd mimst(pmstp);
   mimst.Invert();
   //       CALL FIXSYM(MIMST,4,1,4)
   mimst.Symmetrize();
   //       CALL MTXINV(PMST,4,PIMST,DUM1,ERR)
   UtMatrixd pimst(pmst);
   pimst.Invert();
   //       CALL FIXSYM(PIMST,4,1,4)
   pimst.Symmetrize();
   //       DO I=1,4
   //          DO J=1,4
   //             PILF(I,J)=PIMST(I,J)-MIMST(I,J)+MI(I,J)
   //          ENDDO
   //       ENDDO
   UtMatrixd pilf(4, 4);
   pilf = pimst - mimst + mi;
   //       CALL FIXSYM(PILF,4,1,4)
   pilf.Symmetrize();
   //
   //*then compute the new covariance matrix
   //       CALL MTXINV(PILF,4,PLF,DUM1,ERR)
   plf = pilf;
   plf.Invert();
   //       CALL FIXSYM(PLF,4,1,4)
   plf.Symmetrize();
   //       CHECK = POSDEF(PLF,DUM1,DUM2,DUM3,DUM4,4)
   // (mjm) this would be an assert? (todo)
   //*compute kalman gain
   //       CALL MTXDIV(P,S,4,4,DUM1,DUM2,DUM3,K)
   UtMatrixd k(4, 4);
   k.Divide(p, s);
   //*a posteriori state estimate and covariance matrix (updated with measurement)
   //*delx = K*delz, xhat=xhat+delx, P = (eye(4) - K)*P
   //       CALL MTXMUL(K,4,4,DELZ,4,1,DELX)
   UtMatrixd delx(4, 1);
   delx.Multiply(k, delz);
   //       CALL MTXADD(X,DELX,4,1,XLF)
   xlf.Add(x, delx);
   //       CALL IDENTY(4,EYE4)
   UtMatrixd eye4(4, 4);
   eye4.LoadIdentity();
   //       CALL MTXSUB(EYE4,K,4,4,TMP4X4)
   UtMatrixd tmp4x4(4, 4);
   tmp4x4.Subtract(eye4, k);
   //       CALL MTXMUL(TMP4X4,4,4,P,4,4,PLF)
   plf.Multiply(tmp4x4, p);
   //
   //       RETURN
   //       END
   //
   //
}

//       SUBROUTINE MTTFTF(JNR,GRL,XTF,PTF,XMST,PMST,XMSTP,PMSTP,
//     +                   PTR388,PTR359,DT,PZ0TF)
//! Multiple target tracking - fuse external track into
//!              local turning flight filter
//!              with measurement
void MTT::mttftf(UtMatrixd&       aFilterStates,   // in/out
                 UtMatrixd&       aHorizontalInfo, // in/out
                 const UtMatrixd& aFilterStatesUpdate,
                 const UtMatrixd& aHorizontalInfoUpdate,
                 const UtMatrixd& aPrevFilterStates,
                 const UtMatrixd& aPrevHorizInfoMatrix,
                 double           aDt,
                 double&          aLikelihoodValue) // out
{
   //     *use external track as a measurement & fuse into turning-flight filter

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *july 8, 2005;
   UtMatrixd&       xtf   = aFilterStates;
   UtMatrixd&       ptf   = aHorizontalInfo;
   const UtMatrixd& xmst  = aFilterStatesUpdate;
   const UtMatrixd& pmst  = aHorizontalInfoUpdate;
   const UtMatrixd& pmstp = aPrevHorizInfoMatrix;
   double&          pzotf = aLikelihoodValue;

   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER I,J,PTR359,PTR388
   //       DOUBLE PRECISION XMST(6),PMST(6,6),XMSTP(6),PMSTP(6,6)
   //       DOUBLE PRECISION XTF(6),PTF(6,6),DT
   //       DOUBLE PRECISION X(6),P(6,6),PZ0TF
   // CBA       DOUBLE PRECISION DISTTF,DETTF
   //       DOUBLE PRECISION DISTTF(1,1),DETTF
   //       DOUBLE PRECISION DELZXY(2),SXY(2,2),TMP1X2(2)
   //       DOUBLE PRECISION TWOPI,DELZ(6),S(6,6)
   //       DOUBLE PRECISION DUM1(100),DUM2(100),DUM3(100)
   //       DOUBLE PRECISION MI(6,6),MIMST(6,6),PIMST(6,6),PITF(6,6)
   //       LOGICAL ERR
   //       DATA TWOPI / 6.28318530717959/
   //
   //       COMMON /TRUST/ TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,
   //     +         TRUSPD,IDTRGT,IDSNDR,IDRCVR,PLTRGT,PLSNDR,PLRCVR,GAMTIM
   //       REAL TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,TRUSPD
   //       INTEGER IDTRGT,IDSNDR,IDRCVR
   //       CHARACTER*20 PLTRGT,PLSNDR,PLRCVR
   //       REAL GAMTIM
   //
   //       DOUBLE PRECISION K(6,6),DELX(6)
   //       DOUBLE PRECISION TMP6X6(6,6),EYE6(6,6)
   //       DOUBLE PRECISION DUM4(100)
   //       LOGICAL CHECK,POSDEF
   //
   //
   //*update local active filter to current time
   //       CALL MTXSET(PTF,6,6,P)
   UtMatrixd p(ptf);
   //       CALL MTXSET(XTF,6,1,X)
   UtMatrixd x(xtf);
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,P,X)
   mtttfp(aDt, p, x);
   //*compute innovation (residual)
   //       CALL MTXSUB(XMST,X,6,1,DELZ)
   UtMatrixd delz(6);
   delz.Subtract(xmst, x);
   //*compute covariance of innovation
   //       CALL MTXADD(PMST,P,6,6,S)
   UtMatrixd s(6, 6);
   s.Add(pmst, p);
   //*normalized innovation error over position states
   //       DO I=1,2
   //          DELZXY(I)=DELZ(I)
   //          DO J=1,2
   //             SXY(I,J)=S(I,J)
   //          ENDDO
   //       ENDDO
   UtMatrixd delzxy(1, 2);
   delzxy(0, 0) = delz(0);
   delzxy(0, 1) = delz(1);
   UtMatrixd sxy(2, 2);
   sxy.CopyFrom(s, 0, 0, 2, 2);
   //       CALL MTXDIV(DELZXY,SXY,1,2,DUM1,DUM2,DUM3,TMP1X2)
   UtMatrixd tmp1x2(1, 2);
   tmp1x2.Divide(delzxy, sxy);
   //       CALL MTXMUL(TMP1X2,1,2,DELZXY,2,1,DISTTF)
   UtMatrixd disttf(1); // a scalar
   UtMatrixd delzxyt(2);
   delzxyt.Transpose(delzxy);
   disttf.Multiply(tmp1x2, delzxyt);
   //*to compute turning-flight likelihood function value, first compute
   //*the determinate of S, a 4x4 matrix that we'll expand about first row
   //       DETTF=SXY(1,1)*SXY(2,2)-SXY(1,2)*SXY(2,1)
   double dettf = sxy(0, 0) * sxy(1, 1) - sxy(0, 1) * sxy(1, 0);
   //*compute turning-flight likelihood function value
   //       IF ((DETTF.GE.0.0) .AND. (DISTTF(1,1).LE.50.0)) THEN
   if ((dettf >= 0.0) && (disttf(0, 0) <= 50.0))
   {
      //          PZ0TF=(1.0/(TWOPI*DSQRT(DETTF)))*DEXP(-0.5*DISTTF(1,1))
      pzotf = (1.0 / (UtMath::cTWO_PI * sqrt(dettf))) * exp(-0.5 * disttf(0, 0));
      //          IF (PZ0TF.LT. 1.0D-10) PZ0TF=1.0D-10
      if (pzotf < 1.0e-10)
      {
         pzotf = 1.0e-10;
      }
   }
   //       ELSE
   else
   {
      //          PZ0TF=1.0D-10
      pzotf = 1.0e-10;
      //       ENDIF
   }
   //
   //*combine local with external using centralized kalman filter
   //       CALL MTXINV(P,6,MI,DUM1,ERR)
   UtMatrixd mi(p);
   mi.Invert();
   //       CALL FIXSYM(MI,6,1,4)
   mi.Symmetrize();
   //       CALL MTXINV(PMSTP,6,MIMST,DUM1,ERR)
   UtMatrixd mimst(pmstp);
   mimst.Invert();
   //       CALL FIXSYM(MIMST,6,1,4)
   mimst.Symmetrize();
   //       CALL MTXINV(PMST,6,PIMST,DUM1,ERR)
   UtMatrixd pimst(pmst);
   pimst.Invert();
   //       CALL FIXSYM(PIMST,6,1,4)
   pimst.Symmetrize();
   //       DO I=1,6
   //          DO J=1,6
   //             PITF(I,J)=PIMST(I,J)-MIMST(I,J)+MI(I,J)
   //          ENDDO
   //       ENDDO
   UtMatrixd pitf(6, 6);
   pitf = pimst - mimst + mi;
   //       CALL FIXSYM(PITF,6,1,4)
   pitf.Symmetrize();
   //
   //*then compute the new covariance matrix
   //       CALL MTXINV(PITF,6,PTF,DUM1,ERR)
   ptf = pitf;
   ptf.Invert();
   //       CALL FIXSYM(PTF,6,1,4)
   ptf.Symmetrize();
   //       CHECK = POSDEF(PTF,DUM1,DUM2,DUM3,DUM4,6)
   //         bool check = ptf.IsPositiveDefinite();
   //         if (! check)
   //         {
   //            cout << ptf << " not pos. def.!!" << endl;
   //         }
   //*compute kalman gain
   //       CALL MTXDIV(P,S,6,6,DUM1,DUM2,DUM3,K)
   UtMatrixd k(4, 4);
   k.Divide(p, s);
   //*a posteriori state estimate and covariance matrix (updated with measurement)
   //*delx = K*delz, xhat=xhat+delx, P = (eye(4) - K)*P
   //       CALL MTXMUL(K,6,6,DELZ,6,1,DELX)
   UtMatrixd delx(6, 1);
   delx.Multiply(k, delz);
   //       CALL MTXADD(X,DELX,6,1,XTF)
   xtf.Add(x, delx);
   //       CALL IDENTY(6,EYE6)
   UtMatrixd eye6(6, 6);
   eye6.LoadIdentity();
   //       CALL MTXSUB(EYE6,K,6,6,TMP6X6)
   UtMatrixd tmp6x6(6, 6);
   tmp6x6.Subtract(eye6, k);
   //       CALL MTXMUL(TMP6X6,6,6,P,6,6,PTF)
   ptf.Multiply(tmp6x6, p);
   //
   //       RETURN
   //       END
   //
}

//      SUBROUTINE MTTGTM(JNR,GRL,PTR387,PTR384,PTR388,PTR359,DT,ZTST,
//    + NORM)
//! Multiple target tracking - measurement to trk gate test.
void MTT::mttgtm(const MTT_ActiveTrack::Data& aActiveTrackData,
                 const MTT_Track&             aTrackHeader,
                 const MTT_Measurement&       aMeasurement,
                 const double&                aDt,
                 double&                      aHypothesisTestValue, // out
                 double&                      aInnovationValue)                          // out
{
   //     *determine pertinent statistical values when the current measurment
   //     *is applied to this active track

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *june 14, 2005;

   // References
   const MTT_ActiveTrack::Data* ptr387 = &aActiveTrackData;
   const MTT_Track*             ptr384 = &aTrackHeader;
   const MTT_Measurement*       ptr359 = &aMeasurement;
   double&                      ztst   = aHypothesisTestValue;
   double&                      norm   = aInnovationValue;
   //       IMPLICIT NONE

   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //*pointer to radar measurement block;
   //       INTEGER I,J,PTR387,PTR384,PTR388,PTR359
   //       DOUBLE PRECISION ZTST,DT,NORM
   //       DOUBLE PRECISION PLF(4,4),XLF(4),ZTSTLF,NORMLF
   //       DOUBLE PRECISION PTF(6,6),XTF(6),ZTSTTF,NORMTF
   //
   //       REAL RAT(100)
   //       DOUBLE PRECISION AT(50)
   //       DOUBLE PRECISION XHORZ(6),PHORZ(6,6),PIVERT(2,2),XIVERT(2)
   //       DOUBLE PRECISION MTPLF,MTPTF
   //
   //       EQUIVALENCE (RAT(1),AT(1))
   //       EQUIVALENCE (PHORZ,AT(1)),(XHORZ,AT(37))
   //       EQUIVALENCE (PIVERT,AT(43)),(XIVERT,AT(47))
   //       EQUIVALENCE (MTPLF,AT(49)),(MTPTF,AT(50))
   //
   MTT_ActiveTrack::Data at;
   UtMatrixd&            phorz  = at.mHorizontalInfoMatrix;
   UtMatrixd&            xhorz  = at.mFilterStates;
   UtMatrixd&            pivert = at.mVerticalInfoMatrix;
   UtMatrixd&            xivert = at.mVerticalState;

   //*load the double precision active track data from GRL
   //       DO I=1,100
   //          RAT(I)=GRL(PTR387+I-1)
   //       ENDDO
   at = *ptr387;
   //*first we apply the gate test to the straight-flight (non turning)
   //*process model of the kalman filter
   //       DO I=1,4
   //          XLF(I)=XHORZ(I)
   //          DO J=1,4
   //             PLF(I,J)=PHORZ(I,J)
   //          END DO
   //       END DO
   UtMatrixd xlf(4);
   xlf.CopyFrom(xhorz, 0, 0, 4, 1);
   UtMatrixd plf(4, 4); // copy from original 6x6
   plf.CopyFrom(phorz, 0, 0, 4, 4);
   //       CALL MTTLFQ(JNR,GRL,XLF,PLF,PIVERT,XIVERT,PTR384,PTR388,PTR359,
   //     + DT,ZTSTLF,NORMLF)
   double ztstlf = 0.0;
   double normlf = 0.0;
   mttlfq(xlf, plf, pivert, xivert, *ptr384, *ptr359, aDt, ztstlf, normlf);
   //*next we apply the gate test to the turning flight process
   //*model of the kalman filter
   //       DO I=1,6
   //          XTF(I)=XHORZ(I)
   //          DO J=1,6
   //             PTF(I,J)=PHORZ(I,J)
   //          END DO
   //       END DO
   UtMatrixd xtf(xhorz);
   UtMatrixd ptf(phorz);
   //       CALL MTTTFQ(JNR,GRL,XTF,PTF,PIVERT,XIVERT,PTR384,PTR388,PTR359,
   //     + DT,ZTSTTF,NORMTF)
   double ztsttf;
   double normtf;
   mtttfq(xtf, ptf, pivert, xivert, *ptr384, *ptr359, aDt, ztsttf, normtf);
   //*fwk: 31may-7 -- possible correlation if measurment falls within error
   //*ellipse - minimum test value is most likely
   //       IF (ZTSTLF.LT.ZTSTTF) THEN
   if (ztstlf < ztsttf)
   {
      //         ZTST=ZTSTLF
      ztst = ztstlf;
      //         NORM=NORMLF
      norm = normlf;
   }
   //       ELSE
   else
   {
      //         ZTST=ZTSTTF
      ztst = ztsttf;
      //         NORM=NORMTF
      norm = normtf;
      //       END IF
   }
   //
   //       RETURN
   //       END
}

//       SUBROUTINE MTTGTT(JNR,GRL,PTR387,TMP387,PTR388,PTR359,DT,ZTST,
//     +            NORM)
//! Multiple target tracking - track to track gate test.
//!@param aActiveTrackData (active track data)
//!@param aTempActiveTrackData (measurement data)
//!@param aDt Time difference between measurment and last track update.
//!@param aTestValue TODO_MIKE please document.
//!@param aInnovationValue (test value to determine if within error ellipse)
void MTT::mttgtt(const MTT_ActiveTrack::Data& aActiveTrackData,
                 const MTT_ActiveTrack::Data& aTempActiveTrackData,
                 double                       aDt,
                 double&                      aTestValue,
                 double&                      aInnovationValue)
{
   //     *determine pertinent statistical values for combining two tracks

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *july 1, 2005;

   const MTT_ActiveTrack::Data* ptr387 = &aActiveTrackData;
   const MTT_ActiveTrack::Data* tmp387 = &aTempActiveTrackData;
   double&                      ztst   = aTestValue;
   double&                      norm   = aInnovationValue;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //*pointer to radar measurement block;
   //       INTEGER I,J,PTR387,TMP387,PTR388,LMASTR
   //       DOUBLE PRECISION PLF(4,4),XLF(4),P(4,4),X(4)
   //       DOUBLE PRECISION PTF(6,6),XTF(6)
   //       DOUBLE PRECISION DELZ(4),S(4,4)
   //       DOUBLE PRECISION MINVAR,MAXVAR,DISCRM
   //       DOUBLE PRECISION //(2,2),CINV(2,2),X1,X2,ZTSTLF,ZTSTTF,ZTST,DT
   //       DOUBLE PRECISION NORM,NORMLF,NORMTF
   //
   //       REAL RAT(100)
   //       DOUBLE PRECISION AT(50)
   //       DOUBLE PRECISION XHORZ(6),PHORZ(6,6),PIVERT(2,2),XIVERT(2)
   //       DOUBLE PRECISION MTPLF,MTPTF
   //       EQUIVALENCE (RAT(1),AT(1))
   //       EQUIVALENCE (PHORZ,AT(1)),(XHORZ,AT(37))
   //       EQUIVALENCE (PIVERT,AT(43)),(XIVERT,AT(47))
   //       EQUIVALENCE (MTPLF,AT(49)),(MTPTF,AT(50))
   //
   MTT_ActiveTrack::Data at;
   UtMatrixd&            phorz = at.mHorizontalInfoMatrix;
   UtMatrixd&            xhorz = at.mFilterStates;

   //       REAL RATCMP(100)
   //       DOUBLE PRECISION ATCMP(50)
   //       DOUBLE PRECISION XCMP(6),PCMP(6,6),PIVCMP(2,2),XIVCMP(2)
   //       DOUBLE PRECISION MTPLFC,MTPTFC
   //       EQUIVALENCE (RATCMP(1),ATCMP(1))
   //       EQUIVALENCE (PCMP,ATCMP(1)),(XCMP,ATCMP(37))
   //       EQUIVALENCE (PIVCMP,ATCMP(43)),(XIVCMP,ATCMP(47))
   //       EQUIVALENCE (MTPLFC,ATCMP(49)),(MTPTFC,ATCMP(50))
   //
   MTT_ActiveTrack::Data atcmp;
   UtMatrixd&            pcmp = atcmp.mHorizontalInfoMatrix;
   UtMatrixd&            xcmp = atcmp.mFilterStates;

   //*load the double precision active track data from GRL
   //       LMASTR=JNR(2)-JNR(1)
   //       DO I=1,100
   //          RAT(I)=GRL(PTR387+I-1)
   //          RATCMP(I)=GRL(TMP387+I-1)
   //       END DO
   at    = *ptr387;
   atcmp = *tmp387;
   //*first we apply the gate test to the straight-flight (non turning)
   //*process model of the kalman filter
   //       DO I=1,4
   //          XLF(I)=XHORZ(I)
   //          DO J=1,4
   //             PLF(I,J)=PHORZ(I,J)
   //          END DO
   //       END DO
   UtMatrixd xlf(4);
   xlf.CopyFrom(xhorz, 0, 0, 4, 1);
   UtMatrixd plf(4, 4);
   plf.CopyFrom(phorz, 0, 0, 4, 4);
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   //       WRITE(67,1111)
   // 1111  FORMAT(' MTTGTT - XLF_/S/DELZ,ZTSTLF')
   // C1111  FORMAT(' MTTGTT - PLF/XLF/PLF_/XLF_/S/DELZ,ZTSTLF')
   // C      WRITE(67,1112)((PLF(I,J),J=1,4),I=1,4)
   // C      WRITE(67,1112)XLF
   // 1112  FORMAT(9X,4E23.15)
   //       ENDIF
   ////==================================================== .cov output ======
   //*propagate straight-flight filter in time
   //       CALL MTTLFP(JNR,GRL,PTR388,DT,PLF,XLF)
   mttlfp(aDt, plf, xlf);
   //*determine maximum diagonal entry
   //       MAXVAR=0.0
   double maxvar = 0.0;
   //       DO I=1,4
   //          IF (PLF(I,I).GT.MAXVAR) MAXVAR=PLF(I,I)
   //       END DO
   for (int i = 0; i < 4; ++i)
   {
      if (plf(i, i) > maxvar)
      {
         maxvar = plf(i, i);
      }
   }
   //*rescale P if max variance is too large
   //       IF (MAXVAR.GT.DBLE(GRL(PTR388+24))) THEN
   if (maxvar > mParameters.mStraightFlightMaxVariance)
   {
      //          DO I=1,4
      //             DO J=1,4
      //                PLF(I,J)=DBLE(GRL(PTR388+24))*(PLF(I,J)/MAXVAR)
      //             END DO
      //          END DO
      plf *= mParameters.mStraightFlightMaxVariance / maxvar;
      //       END IF
   }
   //*determine minimum diagonal entry
   //       MINVAR=DBLE(GRL(PTR388+26))
   double minvar = mParameters.mStraightFlightMinVariance;
   //       DO I=1,2
   //          IF (PLF(I,I).LT.MINVAR) MINVAR=PLF(I,I)
   //       END DO
   for (int i = 0; i < 2; ++i)
   {
      if (plf(i, i) < minvar)
      {
         minvar = plf(i, i);
      }
   }
   //*rescale P if min variance is too small
   //       IF (MINVAR.LT.DBLE(GRL(PTR388+26))) THEN
   if (minvar < mParameters.mStraightFlightMinVariance)
   {
      //          DO I=1,4
      //             DO J=1,4
      //                PLF(I,J)=DBLE(GRL(PTR388+26))*(PLF(I,J)/MINVAR)
      //             END DO
      //          END DO
      plf *= mParameters.mStraightFlightMinVariance / minvar;
      //       END IF
   }
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   // C      WRITE(67,1112)((PLF(I,J),J=1,4),I=1,4)
   //       WRITE(67,1112)XLF
   //       ENDIF
   ////==================================================== .cov output ======
   //*store covariance and state into local arrays
   //       CALL MTXSET(PLF,4,4,P)
   UtMatrixd p(plf);
   //       CALL MTXSET(XLF,4,1,X)
   UtMatrixd x(4);
   x.CopyFrom(xlf, 0, 0, 4, 1);
   //*compute innovation
   //       CALL MTXSUB(XCMP,X,4,1,DELZ)
   UtMatrixd xcmp4(4);
   xcmp4.CopyFrom(xcmp, 0, 0, 4, 1);
   UtMatrixd delz(4);
   delz.Subtract(xcmp4, x);
   //*compute covariance of innovation
   //       DO I=1,4
   //          DO J=1,4
   //             S(I,J)=P(I,J)+PCMP(I,J)
   //          ENDDO
   //       ENDDO
   //
   UtMatrixd pcmp4(4, 4);
   pcmp4.CopyFrom(pcmp, 0, 0, 4, 4);
   UtMatrixd s(4, 4);
   s = p + pcmp4;

   //*fwk: 31may07 -- compute the linear transformation that maps the innovation
   //*covariance to a covariance of the product of two independent standard normal
   //*distributions; because S is positive definite, we know there exists a
   //*lower triangular matrix C, such that C*C'=S
   //       CALL MTXZRO(C,2,2)
   UtMatrixd c(2, 2);
   double    ztstlf = 9999999999.99999;
   double    normlf = 9999999999.99999;
   //       IF(S(1,1).GE.0.0D0)THEN
   if (s(0, 0) >= 0.0)
   {
      //          C(1,1)=DSQRT(S(1,1))
      c(0, 0) = sqrt(s(0, 0));
      //          C(2,1)=S(2,1)C/(1,1)
      c(1, 0) = s(1, 0) / c(0, 0);
      //          DISCRM=S(2,2)-(C(2,1)*C(2,1))
      double discrm = s(1, 1) - (c(1, 0) * c(1, 0));
      //          IF(DISCRM.GE.0.0D0)THEN
      if (discrm >= 0.0)
      {
         //             C(2,2)=DSQRT(DISCRM)
         c(1, 1) = sqrt(discrm);
         //*the inverse of C maps correlated random variables to uncorrelated
         //             CALL MTXZRO(CINV,2,2)
         UtMatrixd cinv(2, 2);
         //             CINV(1,1)=1.0/C(1,1)
         cinv(0, 0) = 1.0 / c(0, 0);
         //             CINV(2,1)=-C(2,1)/(C(1,1)*C(2,2))
         cinv(1, 0) = -c(1, 0) / (c(0, 0) * c(1, 1));
         //             CINV(2,2)=1.0/C(2,2)
         cinv(1, 1) = 1.0 / c(1, 1);
         //*map the innovation from correlated system to uncorrelated system
         //             X1=CINV(1,1)*DELZ(1)
         double x1 = cinv(0, 0) * delz(0);
         //             X2=CINV(2,1)*DELZ(1)+CINV(2,2)*DELZ(2)
         double x2 = cinv(1, 0) * delz(0) + cinv(1, 1) * delz(1);
         //*the distance to this point is the hypothesis test value
         //             ZTSTLF=DSQRT(X1*X1+X2*X2)
         ztstlf = sqrt(x1 * x1 + x2 * x2);
         //             NORMLF=DSQRT(DELZ(1)*DELZ(1)+DELZ(2)*DELZ(2))
         normlf = sqrt(delz(0) * delz(0) + delz(1) * delz(1));
         //          ELSE
         //*guard against invalid operation
         //             ZTSTLF=9999999999.99999
         //             NORMLF=9999999999.99999
         //          ENDIF
      }
      //       ELSE
      //*guard against invalid operation
      //          ZTSTLF=9999999999.99999
      //          NORMLF=9999999999.99999
      //       ENDIF
   }
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   //       WRITE(67,1112)((S(I,J),J=1,4),I=1,4)
   //       WRITE(67,1112)DELZ,ZTSTLF
   //       ENDIF
   ////==================================================== .cov output ======
   //
   //*next we apply the gate test to the turning flight process
   //*model of the kalman filter
   //       DO I=1,6
   //          XTF(I)=XHORZ(I)
   //          DO J=1,6
   //             PTF(I,J)=PHORZ(I,J)
   //          END DO
   //       END DO
   UtMatrixd xtf(xhorz);
   UtMatrixd ptf(phorz);
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   //       WRITE(67,1113)
   // 1113  FORMAT(' MTTGTT - XTF_/S/DELZ,ZTSTTF')
   // C1113  FORMAT(' MTTGTT - PTF/XTF/PTF_/XTF_/S/DELZ,ZTSTTF')
   // C      WRITE(67,1114)((PTF(I,J),J=1,6),I=1,6)
   // C      WRITE(67,1114)XTF
   // 1114  FORMAT(9X,6E23.15)
   //       ENDIF
   ////==================================================== .cov output ======
   //*propagate turning flight filter in time
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,PTF,XTF)
   mtttfp(aDt, ptf, xtf);
   //*determine maximum diagonal entry of the non-acc states
   //       MAXVAR=0.0
   maxvar = 0.0;
   //       DO I=1,4
   //          IF (PTF(I,I).GT.MAXVAR) MAXVAR=PTF(I,I)
   //       END DO
   for (int i = 0; i < 4; ++i)
   {
      if (ptf(i, i) > maxvar)
      {
         maxvar = ptf(i, i);
      }
   }
   //*rescale P if max variance is too large
   //       IF (MAXVAR.GT.DBLE(GRL(PTR388+25))) THEN
   if (maxvar > mParameters.mTurningFlightMaxVariance)
   {
      //          DO I=1,6
      //             DO J=1,6
      //                PTF(I,J)=DBLE(GRL(PTR388+25))*(PTF(I,J)/MAXVAR)
      //             END DO
      //          END DO
      ptf *= mParameters.mTurningFlightMaxVariance / maxvar;
      //       END IF
   }
   //*determine minimum diagonal entry of the non-acc states
   //       MINVAR=DBLE(GRL(PTR388+27))
   minvar = mParameters.mTurningFlightMinVariance;
   //       DO I=1,2
   //          IF (PTF(I,I).LT.MINVAR) MINVAR=PTF(I,I)
   //       END DO
   for (int i = 0; i < 2; ++i)
   {
      if (ptf(i, i) < minvar)
      {
         minvar = ptf(i, i);
      }
   }
   //*rescale P if min variance is too small
   //       IF (MINVAR.LT.DBLE(GRL(PTR388+27))) THEN
   if (minvar < mParameters.mTurningFlightMinVariance)
   {
      //          DO I=1,6
      //             DO J=1,6
      //                PTF(I,J)=DBLE(GRL(PTR388+27))*(PTF(I,J)/MINVAR)
      //             END DO
      //          END DO
      ptf *= mParameters.mTurningFlightMinVariance / minvar;
      //       END IF
   }
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   // C      WRITE(67,1114)((PTF(I,J),J=1,6),I=1,6)
   //       WRITE(67,1114)XTF
   //       ENDIF
   ////==================================================== .cov output ======
   //*store cov and state in local arrays, use only the non accelleration states
   //       DO I=1,4
   //          X(I)=XTF(I)
   //          DO J=1,4
   //             P(I,J)=PTF(I,J)
   //          END DO
   //       END DO
   x = xtf;
   p = ptf;
   //*compute innovation
   //       CALL MTXSUB(XCMP,X,4,1,DELZ)
   delz.Subtract(xcmp, x);
   //*compute covariance of innovation
   //       DO I=1,4
   //          DO J=1,4
   //             S(I,J)=P(I,J)+PCMP(I,J)
   //          ENDDO
   //       ENDDO
   //
   s = p + pcmp;

   //*fwk: 31may07 -- compute the linear transformation that maps the innovation
   //*covariance to a covariance of the product of two independent standard normal
   //*distributions; because S is positive definite, we know there exists a
   //*lower triangular matrix //, such that //*//'=S
   //       CALL MTXZRO(C,2,2)
   c.Fill(0.0);
   double ztsttf = 9999999999.99999;
   double normtf = 9999999999.99999;
   //       IF(S(1,1).GE.0.0D0)THEN
   if (s(0, 0) >= 0.0)
   {
      //          C(1,1)=DSQRT(S(1,1))
      c(0, 0) = sqrt(s(0, 0));
      //          C(2,1)=S(2,1)/C(1,1)
      c(1, 0) = s(1, 0) / c(0, 0);
      //          DISCRM=S(2,2)-(C(2,1)*C(2,1))
      double discrm = s(1, 1) - (c(1, 0) * c(1, 0));
      //          IF(DISCRM.GE.0)THEN
      if (discrm >= 0.0)
      {
         //             C(2,2)=DSQRT(DISCRM)
         c(1, 1) = sqrt(discrm);
         //*the inverse of C maps correlated random variables to uncorrelated
         //             CALL MTXZRO(CINV,2,2)
         UtMatrixd cinv(2, 2);
         //             CINV(1,1)=1.0/C(1,1)
         cinv(0, 0) = 1.0 / c(0, 0);
         //             CINV(2,1)=-C(2,1)/(C(1,1)*C(2,2))
         cinv(1, 0) = -c(1, 0) / (c(0, 0) * c(1, 1));
         //             CINV(2,2)=1.0/C(2,2)
         cinv(1, 1) = 1.0 / c(1, 1);
         //*map the innovation from correlated system to uncorrelated system
         //             X1=CINV(1,1)*DELZ(1)
         double x1 = cinv(0, 0) * delz(0);
         //             X2=CINV(2,1)*DELZ(1)+CINV(2,2)*DELZ(2)
         double x2 = cinv(1, 0) * delz(0) + cinv(1, 1) * delz(1);
         //*the distance to this point is the hypothesis test value
         //             ZTSTTF=DSQRT(X1*X1+X2*X2)
         ztsttf = sqrt(x1 * x1 + x2 * x2);
         //             NORMTF=DSQRT(DELZ(1)*DELZ(1)+DELZ(2)*DELZ(2))
         normtf = sqrt(delz(0) * delz(0) + delz(1) * delz(1));
         //          ELSE
         //*guard against invalid operation
         //             ZTSTTF=9999999999.99999
         //             NORMTF=9999999999.99999
         //          ENDIF
      }
      //       ELSE
      //*guard against invalid operation
      //          ZTSTTF=9999999999.99999
      //          NORMTF=9999999999.99999
      //       ENDIF
   }
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   //       WRITE(67,1112)((S(I,J),J=1,4),I=1,4)
   //       WRITE(67,1112)DELZ,ZTSTTF
   //       ENDIF
   ////==================================================== .cov output ======
   //
   //*return minimmum test statistic
   //       ZTST=DMIN1(ZTSTLF,ZTSTTF)
   ztst = std::min(ztstlf, ztsttf);
   //       NORM=DMIN1(NORMLF,NORMTF)
   norm = std::min(normlf, normtf);
   //       RETURN
   //       END
   //
}

// SUBROUTINE MTTLFM(JNR,GRL,XLF,PLF,PIVERT,XIVERT,PTR384,PTR388,
// + PTR359,DT,PZ0LF)
//! Multiple target tracking - update straight-flight model
//! with measurement
//!@param aStraightFlightStateVector (straight-flight state vector)
//!@param aStraightFlightErrorCovarianceMatrix    (straight-flight error covariance matrix)
//!@param aVerticalErrorCovarianceMatrix (vertical error covariance matrix)
//!@param aVerticalFlightInformationState (vertical-flight information state)
//!@param aTrack (pointer to track header)
//!@param aMeasurement (pointer to measurement header block)
//!@param aDt     The time difference between measurment and last track update.
//!@param aStraightFlightLikelihood  (likelihood of straight-flight)
void MTT::mttlfm(UtMatrixd&             aStraightFlightStateVector,
                 UtMatrixd&             aStraightFlightErrorCovarianceMatrix,
                 const UtMatrixd&       aVerticalErrorCovarianceMatrix,
                 const UtMatrixd&       aVerticalFlightInformationState,
                 const MTT_Track&       aTrack,
                 const MTT_Measurement& aMeasurement,
                 double                 aDt,
                 double&                aStraightFlightLikelihood)
{
   //     *fold measurement into straight-flight model

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *july 2, 2005;

   UtMatrixd&             xlf    = aStraightFlightStateVector;
   UtMatrixd&             plf    = aStraightFlightErrorCovarianceMatrix;
   const UtMatrixd&       pivert = aVerticalErrorCovarianceMatrix;
   const UtMatrixd&       xivert = aVerticalFlightInformationState;
   const MTT_Track*       ptr384 = &aTrack;
   const MTT_Measurement* ptr359 = &aMeasurement;
   double&                pzolf  = aStraightFlightLikelihood;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER PTR384,PTR359,PTR388,PTR360,IAVAIL,IEL
   //       DOUBLE PRECISION XLF(4),PLF(4,4),PV(2,2),XV(2),DT
   //       DOUBLE PRECISION PIVERT(2,2),XIVERT(2)
   //       DOUBLE PRECISION X(4),P(4,4),PZ0LF
   //       DOUBLE PRECISION SRANGE,RANGE,AZ,EL,DX,DY,R(2,2),SIGRNG,SIGAZ
   // CBA       DOUBLE PRECISION RDRX,RDRY,RDRZ,DETLF,DISTLF
   //       DOUBLE PRECISION RDRX,RDRY,RDRZ,DETLF,DISTLF(1,1)
   //       DOUBLE PRECISION A11,A12,TMP2X2(2,2),H(2,4),HT(4,2),S(2,2)
   //       DOUBLE PRECISION DELZ(2),B(2,2),BT(2,2),TMP2X4(2,4),K(4,2)
   //       DOUBLE PRECISION TMP4X2(4,2),DUM1(100),DUM2(100),DUM3(100)
   //       DOUBLE PRECISION DELX(4),TMP1X2(2),KH(4,4)
   //       DOUBLE PRECISION TMP4X4(4,4),EYE4(4,4)
   //       DOUBLE PRECISION TWOPI
   //       LOGICAL ERR
   //*variables used for doppler augmentation
   //       INTEGER IRR
   //       DOUBLE PRECISION SIGRR,RDRXD,RDRYD,RDRZD,RRATE,RES
   //       DOUBLE PRECISION VARXD,VARYD,VARRR,//,CXD,CYD
   //*
   //       DATA TWOPI / 6.28318530717959/
   //
   //*update filter to current time
   //       CALL MTXSET(PLF,4,4,P)
   UtMatrixd p(plf);
   //       CALL MTXSET(XLF,4,1,X)
   UtMatrixd x(xlf);
   //       CALL MTTLFP(JNR,GRL,PTR388,DT,P,X)
   mttlfp(aDt, p, x);
   //*extract necessary radar measurement data
   //       PTR360=JNR(PTR359+4)
   const MTT_RadarMeasurement::Data* ptr360 = &(dynamic_cast<const MTT_RadarMeasurement*>(ptr359)->GetData());
   //       SRANGE=DBLE(GRL(PTR360))
   double srange = ptr360->mRange;
   //       AZ=DBLE(GRL(PTR360+1))
   double az = ptr360->mAzimuth;
   //       EL=DBLE(GRL(PTR360+2))
   double el = ptr360->mElevation;
   //       SIGRNG=DBLE(GRL(PTR360+4))
   double sigrng = ptr360->mRangeStDev;
   //       SIGAZ=DBLE(GRL(PTR360+5))
   double sigaz = ptr360->mAzimuthStDev;
   //       RDRX=DBLE(GRL(PTR360+8))
   double rdrx = ptr360->mSensorLocation[0];
   //       RDRY=DBLE(GRL(PTR360+9))
   double rdry = ptr360->mSensorLocation[1];
   //       RDRZ=DBLE(GRL(PTR360+10))
   double rdrz = ptr360->mSensorLocation[2];
   //
   //*range rate information (7/24/08)
   //       SIGRR=DBLE(GRL(PTR360+7))
   double sigrr = ptr360->mRangeRateStDev;
   //       RRATE=DBLE(GRL(PTR360+3))
   double rrate = ptr360->mRangeRate;
   //       RDRXD=DBLE(GRL(PTR360+16))
   double rdrxd = ptr360->mSensorVelocity[0];
   //       RDRYD=DBLE(GRL(PTR360+17))
   double rdryd = ptr360->mSensorVelocity[1];
   //       RDRZD=DBLE(GRL(PTR360+18))
   // (var. not used) double rdrzd = ptr360->mSensorVelocity[2];
   //*determine if elevation is available and adjust for ground range
   //       IAVAIL=JNR(PTR359+6)
   unsigned iavail = ptr359->mDataAvailable;
   //       IEL=MOD(IAVAIL/4,2)
   unsigned iel = (iavail / 4) % 2;
   //       IF (IEL.EQ.1) THEN
   double range = srange;
   if (iel == 1)
   {
      //          RANGE=SRANGE*DCOS(EL)
      range *= cos(el);
      //       ELSE
   }
   else
   {
      //*set vertical flag if matrix is invertible
      //          RANGE=SRANGE
      //          IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //*invert vertical information matrix
         //             CALL MTXINV(PIVERT,2,PV,DUM1,ERR)
         UtMatrixd pv(pivert);
         try
         {
            pv.Invert();
         }
         //*ignore vertical if information matrix is not invertible
         //             IF (.NOT. ERR) THEN
         //                CALL MTXMUL(PV,2,2,XIVERT,2,1,XV)
         //                RANGE=DSQRT(SRANGE*SRANGE-(XV(1)-RDRZ)*(XV(1)-RDRZ))
         //             ENDIF
         catch (UtMatrixd::SingularMatrixError&)
         {
            UtMatrixd xv(2, 1);
            xv.Multiply(pv, xivert);
            range = sqrt(srange * srange - (xv(0) - rdrz) * (xv(0) - rdrz));
         }
         //          ENDIF
      }
      //       END IF
   }
   //*statistics for delx and dely are in a system that has been rotated "az"
   //*degrees,where "az" measured counterclockwise from east (per suppressor);
   //       DX=RANGE*DCOS(AZ)
   double dx = range * cos(az);
   //       DY=RANGE*DSIN(AZ)
   double dy = range * sin(az);
   //       CALL MTXZRO(R,2,2)
   UtMatrixd r(2, 2);
   //       R(1,1)=SIGRNG*SIGRNG
   r(0, 0) = sigrng * sigrng;
   //       R(2,2)=RANGE*RANGE*SIGAZ*SIGAZ
   r(1, 1) = range * range * sigaz * sigaz;
   //*do a transformation of coordinates involving pure rotation to bring the
   //*system into our standard coordinate system
   //       A11=DX/RANGE
   double a11 = dx / range;
   //       A12=DY/RANGE
   double a12 = dy / range;
   //       B(1,1)=A11
   UtMatrixd b(2, 2);
   b(0, 0) = a11;
   //       B(1,2)=-A12
   b(0, 1) = -a12;
   //       B(2,1)=A12
   b(1, 0) = a12;
   //       B(2,2)=A11
   b(1, 1) = a11;
   //       CALL TRNPSE(B,2,2,BT)
   UtMatrixd bt(2, 2);
   bt.Transpose(b);
   //*now compute the error covariance matrix for delx and dely
   //       CALL MTXMUL(B,2,2,R,2,2,TMP2X2)
   UtMatrixd tmp2x2;
   tmp2x2.Multiply(b, r);
   //       CALL MTXMUL(TMP2X2,2,2,BT,2,2,R)
   r.Multiply(tmp2x2, bt);
   //       CALL FIXSYM(R,2,1,3)
   r.Symmetrize();
   //*calculate the covariance of the innovation
   //       CALL MTXZRO(H,2,4)
   UtMatrixd h(2, 4);
   //       H(1,1)=1
   h(0, 0) = 1.0;
   //       H(2,2)=1
   h(1, 1) = 1.0;
   //       CALL TRNPSE(H,2,4,HT)
   UtMatrixd ht(4, 2);
   ht.Transpose(h);
   //       CALL MTXMUL(H,2,4,P,4,4,TMP2X4)
   UtMatrixd tmp2x4(2, 4);
   tmp2x4.Multiply(h, p);
   //       CALL MTXMUL(TMP2X4,2,4,HT,4,2,TMP2X2)
   tmp2x2.Multiply(tmp2x4, ht);
   //       CALL MTXADD(TMP2X2,R,2,2,S)
   UtMatrixd s(2, 2);
   s.Add(tmp2x2, r);
   //       CALL FIXSYM(S,2,1,3)
   s.Symmetrize();
   //*compute innovation
   //       DELZ(1)=DX+RDRX-X(1)
   //       DELZ(2)=DY+RDRY-X(2)
   UtMatrixd delz(2);
   delz(0) = dx + rdrx - x(0);
   delz(1) = dy + rdry - x(1);
   //*compute the kalman gain matrix
   //       CALL MTXMUL(P,4,4,HT,4,2,TMP4X2)
   UtMatrixd tmp4x2(4, 2);
   tmp4x2.Multiply(p, ht);
   //       CALL MTXDIV(TMP4X2,S,4,2,DUM1,DUM2,DUM3,K)
   UtMatrixd k(4, 2);
   k.Divide(tmp4x2, s);
   //*a posteriori state estimate and covariance matrix
   //       CALL MTXMUL(K,4,2,DELZ,2,1,DELX)
   UtMatrixd delx(4);
   delx.Multiply(k, delz);
   //       CALL MTXADD(DELX,X,4,1,XLF)
   xlf.Add(delx, x);
   //*update covariance matrix: phorz=(i-kh)*phorz;
   //       CALL MTXMUL(K,4,2,H,2,4,KH)
   UtMatrixd kh(4, 4);
   kh.Multiply(k, h);
   //       CALL IDENTY(4,EYE4)
   UtMatrixd eye4(4, 4);
   eye4.LoadIdentity();
   //       CALL MTXSUB(EYE4,KH,4,4,TMP4X4)
   UtMatrixd tmp4x4(4, 4);
   tmp4x4.Subtract(eye4, kh);
   //       CALL MTXMUL(TMP4X4,4,4,P,4,4,PLF)
   plf.Multiply(tmp4x4, p);
   //*roundoff error can corrupt symmetry of phorz, so symmetrize;
   //       CALL FIXSYM(PLF,4,1,3)
   plf.Symmetrize();
   //*augment horizontal velocity estimate if doppler is available (7/24/08)
   //*reference:  "multiple target tracking with radar applications" by
   //*samuel s. blackman, artech house, 1986
   //*note:  elevation should be available
   //       IRR=MOD(IAVAIL/8,2)
   unsigned irr = (iavail / 8) % 2;
   //       IF ((IRR.EQ.1).AND.(IEL.EQ.1)) THEN
   if ((irr == 1) && (iel == 1))
   {
      //*step 1 computes the estimate of residual error in range rate
      //          RES=(RRATE*DCOS(EL)+(RDRXD*DCOS(AZ)+RDRYD*DSIN(AZ))) -
      //     +      (XLF(3)*DCOS(AZ)+XLF(4)*DSIN(AZ))
      double res = (rrate * cos(el) + (rdrxd * cos(az) + rdryd * sin(az))) - (xlf(2) * cos(az) + xlf(3) * sin(az));
      //*step 2 computes xd and yd variances from kalman cov matrix
      //          VARXD=PLF(3,3)
      double varxd = plf(2, 2);
      //          VARYD=PLF(4,4)
      double varyd = plf(3, 3);
      //          VARRR=SIGRR*SIGRR
      double varrr = sigrr * sigrr;
      //*step 3 computes the total variance of the residual error
      //          C=(VARXD*DCOS(AZ)**2+VARYD*DSIN(AZ)**2)+VARRR
      double c = (varxd * cos(az)) * (varxd * cos(az)) + (varyd * sin(az)) * (varyd * sin(az)) + varrr;
      //*step 4 computes the individual variances
      //          CXD=VARXD*DCOS(AZ)/C
      double cxd = varxd * cos(az) / c;
      //          CYD=VARYD*DSIN(AZ)/C
      double cyd = varyd * sin(az) / c;
      //*step 5 augments the kalman velocity estimate
      //          XLF(3)=XLF(3)+CXD*RES
      xlf(2) += cxd * res;
      //          XLF(4)=XLF(4)+CYD*RES
      xlf(3) += cyd * res;
      //       ENDIF
   }
   //*compute the likelihood function for the generalized pseudo
   //*bayesian estimator of the first order
   //       CALL MTXDIV(DELZ,S,1,2,DUM1,DUM2,DUM3,TMP1X2)
   UtMatrixd tmp1x2(1, 2);
   UtMatrixd delzt(1, 2);
   delzt.Transpose(delz);
   tmp1x2.Divide(delzt, s);
   //       CALL MTXMUL(TMP1X2,1,2,DELZ,2,1,DISTLF)
   UtMatrixd distlf(1);
   distlf.Multiply(tmp1x2, delz);
   //       DETLF=S(1,1)*S(2,2)-S(1,2)*S(2,1)
   double detlf = s(0, 0) * s(1, 1) - s(0, 1) * s(1, 0);
   //       IF ((DETLF.GE.0.0) .AND. (DISTLF(1,1).LE.50.0)) THEN
   if ((detlf >= 0.0) && (distlf(0, 0) <= 50.0))
   {
      //          PZ0LF=(1.0/(TWOPI*DSQRT(DETLF)))*DEXP(-0.5*DISTLF(1,1))
      pzolf = (1.0 / (UtMath::cTWO_PI * sqrt(detlf))) * exp(-0.5 * distlf(0, 0));
      //          IF (PZ0LF.LT. 1.0D-10) PZ0LF=1.0D-10
      if (pzolf < 1.0e-10)
      {
         pzolf = 1.0e-10;
      }
   }
   //       ELSE
   else
   {
      //          PZ0LF=1.0D-10
      pzolf = 1.0e-10;
      //       ENDIF
   }
   //
   //       RETURN
   //       END
   //
   //
}

//       SUBROUTINE MTTLFP(JNR,GRL,PTR388,aDt,PLF,XLF)
//! Multiple target tracking - propagate lf states.
//!@param aDt delta time between measurment and last track update.
//!@param aCovarianceMatrix Covariance matrix to propagate.
//!@param aStateVector State vector to propagate.
void MTT::mttlfp(double aDt, UtMatrixd& aCovarianceMatrix, UtMatrixd& aStateVector)
{
   //     *update straight flight filter to current time
   //       IMPLICIT NONE

   UtMatrixd& plf = aCovarianceMatrix;
   UtMatrixd& xlf = aStateVector;
   //       REAL GRL(*)
   //       INTEGER JNR(*)
   //
   //       INTEGER PTR388
   //       DOUBLE PRECISION aDt,PLF(4,4),XLF(4),X(4)
   //       DOUBLE PRECISION PHI(4,4),PHIT(4,4),W(2),Q(4,4)
   //       DOUBLE PRECISION TMP4X4(4,4)
   //
   //       IF (aDt.GT.0.0) THEN
   if (aDt > 0.0)
   {
      //          CALL IDENTY(4,PHI)
      UtMatrixd phi(4, 4);
      phi.LoadIdentity();
      //          PHI(1,3)=aDt
      phi(0, 2) = aDt;
      //          PHI(2,4)=aDt
      phi(1, 3) = aDt;
      //          CALL TRNPSE(PHI,4,4,PHIT)
      UtMatrixd phit;
      phit.Transpose(phi);
      //*for calculation of Q,see my notes of march 28, 2004
      UtMatrixd w(2);
      //          W(1)=DBLE(GRL(PTR388+3))
      w(0) = mParameters.mStraightFlightX_Acceleration;
      //          W(2)=DBLE(GRL(PTR388+4))
      w(1) = mParameters.mStraightFlightY_Acceleration;
      //          CALL MTXZRO(Q,4,4)
      UtMatrixd q(4, 4);
      q.Fill(0.0);
      //          Q(1,1)=aDt*aDt*aDt*W(1)/3
      q(0, 0) = aDt * aDt * aDt * w(0) / 3;
      //          Q(1,3)=aDt*aDt*W(1)/2
      q(0, 2) = aDt * aDt * w(0) / 2.0;
      //          Q(2,2)=aDt*aDt*aDt*W(2)/3
      q(1, 1) = aDt * aDt * aDt * w(1) / 3.0;
      //          Q(2,4)=aDt*aDt*W(2)/2
      q(1, 3) = aDt * aDt * w(1) / 2.0;
      //          Q(3,1)=aDt*aDt*W(1)/2
      q(2, 0) = aDt * aDt * w(0) / 2.0;
      //          Q(3,3)=aDt*W(1)
      q(2, 2) = aDt * w(0);
      //          Q(4,2)=aDt*aDt*W(2)/2
      q(3, 1) = aDt * aDt * w(1) / 2.0;
      //          Q(4,4)=aDt*W(2)
      q(3, 3) = aDt * w(1);
      //*project ahead
      //          CALL MTXMUL(PHI,4,4,XLF,4,1,X)
      UtMatrixd x(4);
      x.Multiply(phi, xlf);
      //          CALL MTXSET(X,4,1,XLF)
      xlf = x;
      //          CALL MTXMUL(PHI,4,4,PLF,4,4,TMP4X4)
      UtMatrixd tmp4x4;
      tmp4x4.Multiply(phi, plf);
      //          CALL MTXMUL(TMP4X4,4,4,PHIT,4,4,PLF)
      aCovarianceMatrix.Multiply(tmp4x4, phit);
      //          CALL MTXSET(PLF,4,4,TMP4X4)
      tmp4x4 = plf;
      //          CALL MTXADD(TMP4X4,Q,4,4,PLF)
      plf.Add(tmp4x4, q);
      //          CALL FIXSYM(PLF,4,0,4)
      plf.Symmetrize();
      //       END IF
   }
   //
   //       RETURN
   //       END
   //
   //
}

//       SUBROUTINE MTTLFQ(JNR,GRL,XLF,PLF,PIVERT,XIVERT,PTR384,PTR388,
//     + PTR359,DT,ZTSTLF,NORMLF)
//! Multiple target tracking - gate querry for straight-flight.
//!@param aStraightFlightState  (straight-flight state vector)
//!@param aStraightFlightErrorCovariance (straight-flight error covariance matrix)
//!@param aVerticalErrorCovariance (vertical error covariance matrix)
//!@param aVerticalFlightState (vertical-flight information state)
//!@param aTrack (track header)
//!@param aMeasurement (measurement header)
//!@param aDt Time difference between measurment and last track update.
//!@param aHypothesisTestValue TODO_MIKE Please document.
//!@param aInnovationValue (hypothesis test value)
void MTT::mttlfq(const UtMatrixd&       aStraightFlightState,
                 const UtMatrixd&       aStraightFlightErrorCovariance,
                 const UtMatrixd&       aVerticalErrorCovariance,
                 const UtMatrixd&       aVerticalFlightState,
                 const MTT_Track&       aTrack,
                 const MTT_Measurement& aMeasurement,
                 double                 aDt,
                 double&                aHypothesisTestValue, // out
                 double&                aInnovationValue)                    // out
{
   //*determine pertinent statistical values for straight flight

   //*fred kiefer;
   //*bogdon associates inc;
   //*june 14, 2005;
   // References:
   const UtMatrixd&       xlf    = aStraightFlightState;
   const UtMatrixd&       plf    = aStraightFlightErrorCovariance;
   const UtMatrixd&       pivert = aVerticalErrorCovariance;
   const UtMatrixd&       xivert = aVerticalFlightState;
   const MTT_Track*       ptr384 = &aTrack;
   const MTT_Measurement* ptr359 = &aMeasurement;
   double&                ztstlf = aHypothesisTestValue;
   double&                normlf = aInnovationValue;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER I,J,PTR359,PTR384,PTR388,PTR360,IAVAIL,IEL,LMASTR
   //       DOUBLE PRECISION XLF(4),PLF(4,4),ZTSTLF,MAXVAR,DT,NORMLF,DISCRM
   //       DOUBLE PRECISION PIVERT(2,2),XIVERT(2),PV(2,2),XV(2)
   //       DOUBLE PRECISION X(4),P(4,4)
   //       DOUBLE PRECISION SRANGE,RANGE,AZ,EL,DX,DY,R(2,2)
   //       DOUBLE PRECISION RDRX,RDRY,RDRZ,SIGRNG,SIGAZ,SIGEL
   //       DOUBLE PRECISION A11,A12,TMP2X2(2,2),H(2,4),HT(4,2),S(2,2)
   //       DOUBLE PRECISION DELZ(2),B(2,2),BT(2,2),TMP2X4(2,4),K(4,2)
   //       DOUBLE PRECISION TMP4X2(4,2),DUM1(100),DUM2(100),DUM3(100)
   //       DOUBLE PRECISION TWOPI
   //       DOUBLE PRECISION //(2,2),CINV(2,2),X1,X2
   //       LOGICAL ERR
   //       DATA TWOPI / 6.28318530717959/
   //
   // Note: more initialization that usual must be performed here, because c does not allow
   // declaration between the goto statement and its label.
   UtMatrixd s(2, 2);
   UtMatrixd delz(2);
   UtMatrixd c(2, 2);
   UtMatrixd k(4, 2);
   UtMatrixd tmp4x2(4, 2);
   UtMatrixd tmp2x4(2, 4);
   UtMatrixd ht(4, 2);
   UtMatrixd h(2, 4);
   UtMatrixd tmp2x2(2, 2);
   UtMatrixd bt(2, 2);
   UtMatrixd b(2, 2);
   UtMatrixd r(2, 2);
   //       //*update filter to current time
   //       LMASTR=JNR(2)-JNR(1)
   //       CALL MTXSET(PLF,4,4,P)
   UtMatrixd p(plf);
   //       CALL MTXSET(XLF,4,1,X)
   UtMatrixd x(xlf);
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   //       WRITE(67,1111)
   // 1111  FORMAT(' MTTLFQ - X_/RDRX,RDRY,RDRZ/S/DELZ,ZTSTLF')
   // C1111  FORMAT(' MTTLFQ - P/X/P_/X_/AZ,EL,SRANGE,RANGE/SIGAZ,SIGEL,'
   // C    +        'SIGRNG/RDRX,RDRY,RDRZ/DX,DY/R/RXY/S/DELZ,ZTSTLF')
   // C      WRITE(67,1112)((P(I,J),J=1,4),I=1,4)
   // C      WRITE(67,1112)X
   // 1112  FORMAT(9X,4E23.15)
   //       ENDIF
   ////==================================================== .cov output ======
   //       CALL MTTLFP(JNR,GRL,PTR388,DT,P,X)
   mttlfp(aDt, p, x);
   //*determine maximum diagonal entry
   //       MAXVAR=0.0
   double maxvar = 0.0;
   //       DO I=1,4
   //          IF (P(I,I).GT.MAXVAR) MAXVAR=P(I,I)
   //       END DO
   for (int i = 0; i < 4; ++i)
   {
      if (p(i, i) > maxvar)
      {
         maxvar = p(i, i);
      }
   }
   //*rescale P if max variance is too large
   //       IF (MAXVAR.GT.DBLE(GRL(PTR388+24))) THEN
   if (maxvar > mParameters.mStraightFlightMaxVariance)
   {
      //          DO I=1,4
      //             DO J=1,4
      //                P(I,J)=DBLE(GRL(PTR388+24))*(P(I,J)/MAXVAR)
      //             END DO
      //          END DO
      p *= (mParameters.mStraightFlightMaxVariance / maxvar);
      //       END IF
   }
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   // C      WRITE(67,1112)((P(I,J),J=1,4),I=1,4)
   //       WRITE(67,1112)X
   //       ENDIF
   ////==================================================== .cov output ======
   //
   //       //*extract necessary radar data
   //       PTR360=JNR(PTR359+4)
   const MTT_RadarMeasurement::Data* ptr360 = &(dynamic_cast<const MTT_RadarMeasurement*>(ptr359)->GetData());
   //       SRANGE=DBLE(GRL(PTR360))
   double srange = ptr360->mRange;
   //       AZ=DBLE(GRL(PTR360+1))
   double az = ptr360->mAzimuth;
   //       EL=DBLE(GRL(PTR360+2))
   double el = ptr360->mElevation;
   //       SIGRNG=DBLE(GRL(PTR360+4))
   double sigrng = ptr360->mRangeStDev;
   //       SIGAZ=DBLE(GRL(PTR360+5))
   double sigaz = ptr360->mAzimuthStDev;
   //       SIGEL=DBLE(GRL(PTR360+6))
   // (var. not used) double sigel = ptr360->mElevationStDev;
   //       RDRX=DBLE(GRL(PTR360+8))
   double rdrx = ptr360->mSensorLocation[0];
   //       RDRY=DBLE(GRL(PTR360+9))
   double rdry = ptr360->mSensorLocation[1];
   //       RDRZ=DBLE(GRL(PTR360+10))
   double rdrz = ptr360->mSensorLocation[2];
   //
   //*determine if elevation is available and adjust for ground range
   //       IAVAIL=JNR(PTR359+6)
   unsigned iavail = ptr359->mDataAvailable;
   //       IEL=MOD(IAVAIL/4,2)
   unsigned iel = (iavail / 4) % 2;
   //       IF (IEL.EQ.1) THEN
   double range = srange; // Use the initialization to minimize computation.
   if (iel == 1)
   {
      //          RANGE=SRANGE*DCOS(EL)
      range *= cos(el);
   }
   //       ELSE
   else
   {
      //*when filtered height is available, estimate ground range
      //          RANGE=SRANGE
      //          IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //*invert vertical information matrix
         //             CALL MTXINV(PIVERT,2,PV,DUM1,ERR)
         UtMatrixd pv(pivert);
         bool      err = false;
         try
         {
            pv.Invert2x2();
         }
         catch (UtMatrixd::SingularMatrixError&)
         {
            err = true;
         }
         //*ignore vertical if information matrix is not invertible
         //             IF (.NOT. ERR) THEN
         if (!err)
         {
            //                CALL MTXMUL(PV,2,2,XIVERT,2,1,XV)
            UtMatrixd xv;
            xv.Multiply(pv, xivert);
            //                DISCRM=SRANGE*SRANGE-(XV(1)-RDRZ)*(XV(1)-RDRZ)
            double discrm = srange * srange - (xv(0) - rdrz) * (xv(0) - rdrz);
            //                IF(DISCRM.GE.0.0D0)THEN
            if (discrm > 0.0)
            {
               //                   RANGE=DSQRT(DISCRM)
               range = sqrt(discrm);
            }
            //                ELSE
            else
            {
               //*guard against invalid operation; set output variables
               //                   S(1,1)=9999999999.99999
               s(0, 0) = 9999999999.99999;
               //                   S(2,1)=9999999999.99999
               s(1, 0) = 9999999999.99999;
               //                   S(1,2)=9999999999.99999
               s(0, 1) = 9999999999.99999;
               //                   S(2,2)=9999999999.99999
               s(1, 1) = 9999999999.99999;
               //                   DELZ(1)=9999999999.99999
               delz(0) = 9999999999.99999;
               //                   DELZ(2)=9999999999.99999
               delz(1) = 9999999999.99999;
               //                   ZTSTLF=9999999999.99999
               ztstlf = 9999999999.99999;
               //                   NORMLF=9999999999.99999
               normlf = 9999999999.99999;
               ////==================================================== .cov output ======
               //                   IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
               //                   WRITE(67,1112)RDRX,RDRY,RDRZ
               ////==================================================== .cov output ======
               //                   GOTO 1000
               //                     goto label_1000;  // Can't do this in gcc
               return; // Jumps to end of function
               //                ENDIF
            }
            //             ENDIF
         }
         //          ENDIF
      }
      //       END IF
   }
   //
   //       //*statistics for delx and dely are in a system that has been rotated "az"
   //       //*degrees,where "az" measured counterclockwise from east (per suppressor);
   //       DX=RANGE*DCOS(AZ)
   double dx = range * cos(az);
   //       DY=RANGE*DSIN(AZ)
   double dy = range * sin(az);
   //       CALL MTXZRO(R,2,2)
   //       R(1,1)=(SIGRNG+RANGE/DCOS(SIGAZ*10.)-RANGE)**2
   r(0, 0) = sigrng + range / cos(sigaz * 10.0) - range;
   r(0, 0) *= r(0, 0);
   //       R(2,2)=RANGE*RANGE*SIGAZ*SIGAZ
   r(1, 1) = range * range * sigaz * sigaz;
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   // C      WRITE(67,1112)AZ,EL,SRANGE,RANGE,SIGAZ,SIGEL,SIGRNG
   //       WRITE(67,1112)RDRX,RDRY,RDRZ
   // C      WRITE(67,1112)DX,DY
   // C      WRITE(67,1115)((R(I,J),J=1,2),I=1,2)
   // 1115  FORMAT(9X,2E23.15)
   //       ENDIF
   ////==================================================== .cov output ======
   //       //*do a transformation of coordinates involving pure rotation to bring the
   //       //*system into our standard coordinate system
   //       A11=DX/RANGE
   double a11 = dx / range;
   //       A12=DY/RANGE
   double a12 = dy / range;
   //       B(1,1)=A11
   b(0, 0) = a11;
   //       B(1,2)=-A12
   b(0, 1) = a12;
   //       B(2,1)=A12
   b(1, 0) = a12;
   //       B(2,2)=A11
   b(1, 1) = a11;
   //       CALL TRNPSE(B,2,2,BT)
   bt.Transpose(b);
   //       //*now compute the error covariance matrix for delx and dely
   //       CALL MTXMUL(B,2,2,R,2,2,TMP2X2)
   //       CALL MTXMUL(TMP2X2,2,2,BT,2,2,R)
   r.Multiply(tmp2x2, bt);
   //*calculate the covariance of the innovation
   //       CALL MTXZRO(H,2,4)
   //       H(1,1)=1
   h(0, 0) = 1.0;
   //       H(2,2)=1
   h(1, 1) = 1.0;
   //       CALL TRNPSE(H,2,4,HT)
   ht.Transpose(h);
   //       CALL MTXMUL(H,2,4,P,4,4,TMP2X4)
   tmp2x4.Multiply(h, p);
   //       CALL MTXMUL(TMP2X4,2,4,HT,4,2,TMP2X2)
   tmp2x2.Multiply(tmp2x4, ht);
   //       CALL MTXADD(TMP2X2,R,2,2,S)
   s.Add(tmp2x2, r);
   //       //*compute the kalman gain matrix
   //       CALL MTXMUL(P,4,4,HT,4,2,TMP4X2)
   tmp4x2.Multiply(p, ht);
   //       CALL MTXDIV(TMP4X2,S,4,2,DUM1,DUM2,DUM3,K)
   k.Divide(tmp4x2, s);
   //*compute innovation
   //       DELZ(1)=DX+RDRX-X(1)
   delz(0) = dx + rdrx - x(0);
   //       DELZ(2)=DY+RDRY-X(2)
   delz(1) = dy + rdry - x(1);
   //
   //*fwk: 31may07 -- compute the linear transformation that maps the innovation
   //*covariance to a covariance of the product of two independent standard normal
   //*distributions; because S is positive definite, we know there exists a
   //*lower triangular matrix //, such that //*//'=S
   //       CALL MTXZRO(C,2,2)
   //       IF(S(1,1).GE.0.0D0)THEN
   if (s(0, 0) >= 0.0)
   {
      //          C(1,1)=DSQRT(S(1,1))
      c(0, 0) = sqrt(s(0, 0));
      //          C(2,1)=S(2,1)/C(1,1)
      c(1, 0) = s(1, 0) / c(0, 0);
      //          DISCRM=S(2,2)-(C(2,1)*C(2,1))
      double discrm = s(1, 1) - (c(1, 0) * c(1, 0));
      //          IF(DISCRM.GE.0.0D0)THEN
      if (discrm > 0.0)
      {
         //             C(2,2)=DSQRT(DISCRM)
         c(1, 1) = sqrt(discrm);
         //*the inverse of C maps correlated random variables to uncorrelated
         //             CALL MTXZRO(CINV,2,2)
         UtMatrixd cinv(2, 2);
         //             CINV(1,1)=1.0/C(1,1)
         cinv(0, 0) = 1.0 / c(0, 0);
         //             CINV(2,1)=-C(2,1)/(C(1,1)*C(2,2))
         cinv(1, 0) = -c(1, 0) / (c(0, 0) * c(1, 1));
         //             CINV(2,2)=1.0/C(2,2)
         cinv(1, 1) = 1.0 / c(1, 1);
         //*compute radius of innovation in CEP system
         //             X1=CINV(1,1)*DELZ(1)
         double x1 = cinv(0, 0) * delz(0);
         //             X2=CINV(2,1)*DELZ(1)+CINV(2,2)*DELZ(2)
         double x2 = cinv(1, 0) * delz(0) + cinv(1, 1) * delz(1);
         //*the distance to this point is the hypothesis test value
         //             ZTSTLF=DSQRT(X1*X1+X2*X2)
         ztstlf = sqrt(x1 * x1 + x2 * x2);
         //             NORMLF=DSQRT(DELZ(1)*DELZ(1)+DELZ(2)*DELZ(2))
         normlf = sqrt(delz(0) * delz(0) + delz(1) * delz(1));
      }
      //          ELSE
      else
      {
         //*guard against invalid operation
         //             ZTSTLF=9999999999.99999
         ztstlf = 9999999999.99999;
         //             NORMLF=9999999999.99999
         normlf = 9999999999.99999;
         //          ENDIF
      }
   }
   //       ELSE
   else
   {
      //          //*guard against invalid operation
      //          ZTSTLF=9999999999.99999
      ztstlf = 9999999999.99999;
      //          NORMLF=9999999999.99999
      normlf = 9999999999.99999;
      //       ENDIF
   }
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   // C      WRITE(67,1115)((R(I,J),J=1,2),I=1,2)
   // label_1000: ;  // todo- diagnostic statements
   // 1000  WRITE(67,1115)((S(I,J),J=1,2),I=1,2)
   // C      WRITE(67,1111)
   // C1111  FORMAT(' MTTLFQ - DELZ,ZTSTLF')
   //       WRITE(67,1112)DELZ,ZTSTLF
   //       ENDIF
   ////==================================================== .cov output ======
   //
   //       RETURN
   //       END
}

//       SUBROUTINE MTTPVF(JNR,GRL,PIV,PTR388,PTR384)
void MTT::mttpvf(const UtMatrixd& aVerticalCovariance, // 2x2 (covar, not info?)
                 MTT_Track&       aCandidateTrack)           // ptr384 (as candidate track)
{
   // Local parameters:
   const UtMatrixd& piv    = aVerticalCovariance;
   MTT_Track*       ptr384 = &aCandidateTrack;
   //       IMPLICIT NONE
   //       REAL GRL(*)
   //       INTEGER JNR(*),PTR388,PTR384
   //
   //       DOUBLE PRECISION COND,TMP2X2(2,2),DUM1(100),DUM2(100),DUM3(100)
   //       DOUBLE PRECISION CNDTST,CNDTHR,PV(2,2),VARSPD,VARDST
   //       DOUBLE PRECISION PIV(2,2)
   //
   //       LOGICAL ERR
   bool err = false;
   //
   //*check for promotion of vertical candidate filter
   //          IF (JNR(PTR384+6).EQ.0) THEN
   if (!ptr384->mVerticalChannelActive)
   {
      //*set vertical flag if matrix is invertible
      //             ERR=.TRUE.
      err = true;
      //             CNDTST=COND(PIV,TMP2X2,DUM1,DUM2,DUM3,2)
      double cndtst = piv.GetConditionNumber();
      //             CNDTHR=DBLE(GRL(PTR388+18))
      double cndthr = mParameters.mPromoteTrackVerticalThreshold;
      //             IF (CNDTST.LT.CNDTHR) THEN
      if (cndtst < cndthr)
      {
         //*invert vertical information matrix
         //                CALL MTXINV(PIV,2,PV,DUM1,ERR)
         UtMatrixd pv(aVerticalCovariance);
         err = false;
         try
         {
            pv.Invert();
         }
         catch (UtMatrixd::SingularMatrixError&)
         {
            err = true;
         }
         //*error should never occur, but if it does, ignore vert info
         //                IF (.NOT. ERR) THEN
         if (!err)
         {
            //                   VARDST=DBLE(GRL(PTR388+23))
            double vardst = mParameters.mPositionVariancePromoteVerticalChannel;
            //                   VARSPD=DBLE(GRL(PTR388+20))
            double varspd = mParameters.mVelocityVariancePromoteVerticalChannel;
            //                   IF ((PV(1,1).LT.VARDST).AND.(PV(2,2).LT.VARSPD)) THEN
            if ((pv(0, 0) < vardst) && (pv(1, 1) < varspd))
            {
               //                      JNR(PTR384+6)=1
               ptr384->mVerticalChannelActive = true;
               //                   ENDIF
            }
            //                ENDIF
         }
         //             ENDIF
      }
      //          ENDIF
   }
   //
   //       RETURN
   //       END
   //
   //
}


//    SUBROUTINE MTTTFE(JNR,GRL,PTR017,PTR388,MST387,PTR384,DT,TIME)
//! Given that an external track correlated statistically with a local
//! active track, combine the external with the local using a
//! centralized kalman filter.
void MTT::mtttfe(const MTT_PerceivedCluster&  aMeasurement,
                 const MTT_ActiveTrack::Data& aUpdateData, // mst387
                 MTT_Track&                   aTrack,      // ptr384
                 double                       aUpdateInterval,
                 double                       aSimTime)
{
   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *july 8, 2005;

   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*),TIME
   //       INTEGER I,J,PTR017,PTR359,PTR388,PTR384,PTR387,PTR363
   const MTT_PerceivedCluster*  ptr017 = &aMeasurement;
   const MTT_ActiveTrack::Data& mst387 = aUpdateData;
   MTT_Track*                   ptr384 = &aTrack;
   double                       dt     = aUpdateInterval;

   //       INTEGER MST387
   //       DOUBLE PRECISION DT,PZ0LF,PZ0TF,XLF(4),PLF(4,4),XTF(6),PTF(6,6)
   //       DOUBLE PRECISION PLFMST(4,4),XLFMST(4),PTFMST(6,6),XTFMST(6)
   //       DOUBLE PRECISION PLFMSP(4,4),XLFMSP(4),PTFMSP(6,6),XTFMSP(6)
   //       DOUBLE PRECISION W(2),//,MTP(2,2),MULF0,MUTF0
   //       DOUBLE PRECISION PLF6(6,6),XLF6(6),PCMBLF(6,6),PCMBTF(6,6)
   //       DOUBLE PRECISION D2LF(6,6),D2TF(6,6),DELXLF(6),DELXTF(6)
   //       DOUBLE PRECISION TMP2X2(2,2),TMP2X1(2)
   //
   //
   //       REAL RMST(184)
   //       DOUBLE PRECISION AMST(92)
   //       DOUBLE PRECISION XHMST(6),PHMST(6,6),PIVMST(2,2),XIVMST(2)
   //       DOUBLE PRECISION XHMSTP(6),PHMSTP(6,6)
   //
   //       EQUIVALENCE (RMST(1),AMST(1))
   //       EQUIVALENCE (PHMST,AMST(1)),(XHMST,AMST(37))
   //       EQUIVALENCE (PIVMST,AMST(43)),(XIVMST,AMST(47))
   //       EQUIVALENCE (XHMSTP,AMST(51)),(PHMSTP,AMST(57))
   MTT_ActiveTrack::Data amst;
   UtMatrixd&            phmst  = amst.mHorizontalInfoMatrix;
   UtMatrixd&            xhmst  = amst.mFilterStates;
   UtMatrixd&            pivmst = amst.mVerticalInfoMatrix;
   UtMatrixd&            xivmst = amst.mVerticalState;
   UtMatrixd&            xhmstp = amst.mPreviousFilterStates;
   UtMatrixd&            phmstp = amst.mPreviousHorizontalMatrix;
   //
   //
   //       REAL RAT(184)
   //       DOUBLE PRECISION AT(92)
   //       DOUBLE PRECISION XHORZ(6),PHORZ(6,6),PIVERT(2,2),XIVERT(2)
   //       DOUBLE PRECISION MTPLF,MTPTF,XHORZP(6),PHORZP(6,6)
   //
   //       EQUIVALENCE (RAT(1),AT(1))
   //       EQUIVALENCE (PHORZ,AT(1)),(XHORZ,AT(37))
   //       EQUIVALENCE (PIVERT,AT(43)),(XIVERT,AT(47))
   //       EQUIVALENCE (MTPLF,AT(49)),(MTPTF,AT(50))
   //       EQUIVALENCE (XHORZP,AT(51)),(PHORZP,AT(57))
   //
   MTT_ActiveTrack::Data at;
   UtMatrixd&            phorz  = at.mHorizontalInfoMatrix;
   UtMatrixd&            xhorz  = at.mFilterStates;
   UtMatrixd&            pivert = at.mVerticalInfoMatrix;
   UtMatrixd&            xivert = at.mVerticalState;
   double&               mtplf  = at.mStraightFlightModeProbability;
   double&               mtptf  = at.mTurningFlightModeProbability;
   UtMatrixd&            xhorzp = at.mPreviousFilterStates;
   UtMatrixd&            phorzp = at.mPreviousHorizontalMatrix;

   //*load the double precision local active and external track data from GRL
   //*note:  the external track serves as the measurement (MST)
   //       PTR387=JNR(PTR384+3)
   MTT_ActiveTrack::Data* ptr387 = dynamic_cast<MTT_ActiveTrack::Data*>(ptr384->mDataPtr);
   //       DO I=1,184
   //           RAT(I)=GRL(PTR387+I-1)
   //           RMST(I)=GRL(MST387+I-1)
   //       ENDDO
   at   = *ptr387;
   amst = mst387;
   //
   //*pointer to external track header block
   //       PTR363=JNR(PTR017+37)
   const MTT_NonlocalTrack* ptr363 = ptr017->mCombinedTrackPtr;
   //*if local filter has a running vertical filter
   //       IF (JNR(PTR384+6).EQ.1) THEN
   UtMatrixd w(2);
   if (ptr384->mVerticalChannelActive)
   {
      //*extract weights needed to propagate vertical filter
      //          W(1)=DBLE(GRL(PTR388+12))
      w(0) = mParameters.mVerticalVelocity;
      //          W(2)=DBLE(GRL(PTR388+13))
      w(1) = mParameters.mVerticalVelocityDecorrelationTime;
      //*propagate vertical filter to current time
      //          CALL MTTUVP(DT,W,PIVERT,XIVERT,4)
      mttuvp(dt, w, pivert, xivert, 4);
      //       ENDIF
   }
   //
   //*when track resulting from 2d/3d radar data:
   //       IF (JNR(PTR363+8).GT.1) THEN
   if (ptr363->mUpdateFlag > 1)
   {
      //
      //*capture the projected previous state estimate and covariance
      //          DO I=1,6
      //             XHORZP(I)=XHORZ(I)
      //             DO J=1,6
      //                PHORZP(I,J)=PHORZ(I,J)
      //             ENDDO
      //          ENDDO
      xhorzp = xhorz;
      phorzp = phorz;
      //*update previous state and covariance to current time
      //          CALL MTTTFP(JNR,GRL,PTR388,DT,PHORZP,XHORZP)
      mtttfp(dt, phorzp, xhorzp);
      //
      //*first we fuse the straight flight (non turning) filter
      //          DO I=1,4
      //             XLF(I)=XHORZ(I)
      //             XLFMST(I)=XHMST(I)
      //             XLFMSP(I)=XHMSTP(I)
      //             DO J=1,4
      //                PLF(I,J)=PHORZ(I,J)
      //                PLFMST(I,J)=PHMST(I,J)
      //                PLFMSP(I,J)=PHMSTP(I,J)
      //             END DO
      //          END DO
      UtMatrixd xlf(4);
      xlf.CopyFrom(xhorz, 0, 0, 4, 1);
      UtMatrixd xlfmst(4);
      xlfmst.CopyFrom(xhmst, 0, 0, 4, 1);
      UtMatrixd xlfmsp(4);
      xlfmsp.CopyFrom(xhmstp, 0, 0, 4, 1);
      UtMatrixd plf(4, 4);
      plf.CopyFrom(phorz, 0, 0, 4, 4);
      UtMatrixd plfmst(4, 4);
      plfmst.CopyFrom(phmst, 0, 0, 4, 4);
      UtMatrixd plfmsp(4, 4);
      plfmsp.CopyFrom(phmstp, 0, 0, 4, 4);
      //          (mjm) ptr359 in call below not used
      //          CALL MTTFLF(JNR,GRL,XLF,PLF,XLFMST,PLFMST,XLFMSP,PLFMSP,
      //     +                PTR388,PTR359,DT,PZ0LF)
      double pzolf;
      mttflf(xlf, plf, xlfmst, plfmst, xlfmsp, plfmsp, dt, pzolf);
      //
      //*next we fuse the turning flight filter
      //          DO I=1,6
      //             XTF(I)=XHORZ(I)
      //             XTFMST(I)=XHMST(I)
      //             XTFMSP(I)=XHMSTP(I)
      //             DO J=1,6
      //                PTF(I,J)=PHORZ(I,J)
      //                PTFMST(I,J)=PHMST(I,J)
      //                PTFMSP(I,J)=PHMSTP(I,J)
      //             END DO
      //          END DO
      UtMatrixd xtf(xhorz);
      UtMatrixd xtfmst(xhmst);
      UtMatrixd xtfmsp(xhmstp);
      UtMatrixd ptf(phorz);
      UtMatrixd ptfmst(phmst);
      UtMatrixd ptfmsp(phmstp);
      //          CALL MTTFTF(JNR,GRL,XTF,PTF,XTFMST,PTFMST,XTFMSP,PTFMSP,
      //     +                PTR388,PTR359,DT,PZ0TF)
      //
      double pzotf = 0.0;
      mttftf(xtf, ptf, xtfmst, ptfmst, xtfmsp, ptfmsp, dt, pzotf);
      //*extract mode transition probabilities
      //          MTP(1,1)=DBLE(GRL(PTR388+28))
      UtMatrixd mtp(2, 2);
      mtp(0, 0) = mParameters.mStraightToStraightFlight;
      //          MTP(1,2)=DBLE(GRL(PTR388+29))
      mtp(0, 1) = mParameters.mStraightToTurningFlight;
      //          MTP(2,1)=DBLE(GRL(PTR388+30))
      mtp(1, 0) = mParameters.mTurningToTurningFlight; ///!!! (mjm) check this (todo)
      //          MTP(2,2)=DBLE(GRL(PTR388+31))
      mtp(1, 1) = mParameters.mTurningToStraightFlight;
      //          MULF0=MTPLF
      double mulfo = mtplf;
      //          MUTF0=MTPTF
      double mutfo = mtptf;
      //*bayesian estimator of the first order alogrithm
      //          C = PZ0LF*(MTP(1,1)*MULF0+MTP(2,1)*MUTF0) +
      //     +        PZ0TF*(MTP(1,2)*MULF0+MTP(2,2)*MUTF0)
      double c = pzolf * (mtp(0, 0) * mulfo + mtp(1, 0) * mutfo) + pzotf * (mtp(0, 1) * mulfo + mtp(1, 1) * mutfo);
      //*update model mode weights
      //          MTPLF=(PZ0LF/C)*(MTP(1,1)*MULF0+MTP(2,1)*MUTF0)
      mtplf = (pzolf / c) * (mtp(0, 0) * mulfo + mtp(1, 0) * mutfo);
      //          MTPTF=(PZ0TF/C)*(MTP(1,2)*MULF0+MTP(2,2)*MUTF0)
      mtptf = (pzotf / c) * (mtp(0, 1) * mulfo + mtp(1, 1) * mutfo);
      //*update state estimates
      //          CALL MTXZRO(XLF6,6,1)
      UtMatrixd xlf6(6, 1);
      //          CALL MTXZRO(PLF6,6,6)
      UtMatrixd plf6(6, 6);
      //          DO I=1,4
      //             XLF6(I)=XLF(I)
      //             DO J=1,4
      //                PLF6(I,J)=PLF(I,J)
      //             ENDDO
      //          ENDDO
      //          DO I=1,6
      //             XHORZ(I)=MTPLF*XLF6(I)+MTPTF*XTF(I)
      //          ENDDO
      //
      xlf6.CopyFrom(xlf, 0, 0, 4, 1);
      plf6.CopyFrom(plf, 0, 0, 4, 4);
      xhorz = mtplf * xlf6 + mtptf * xtf;
      //          CALL MTXSUB(XHORZ,XLF6,6,1,DELXLF)
      UtMatrixd delxlf(6, 1);
      delxlf.Subtract(xhorz, xlf6);
      //          CALL MTXMUL(DELXLF,1,6,DELXLF,6,1,D2LF)
      // (mjm) This is very strange... d2lf is a 6x6, but it's only using the first element.
      UtMatrixd delxlft(1, 6);
      delxlft.Transpose(delxlf);
      UtMatrixd d2lf(6, 6);
      UtMatrixd d2lfs(1);
      d2lfs.Multiply(delxlft, delxlf);
      d2lf(0, 0) = d2lfs(0);
      //          CALL MTXSUB(XHORZ,XTF,6,1,DELXTF)
      UtMatrixd delxtf(6, 1);
      delxtf.Subtract(xhorz, xtf);
      //          CALL MTXMUL(DELXTF,1,6,DELXTF,6,1,D2TF)
      UtMatrixd delxtft(1, 6);
      delxtft.Transpose(delxtf);
      UtMatrixd d2tf(6, 6); // see comment above (todo-error ?)
      UtMatrixd d2tfs(1);
      d2tfs.Multiply(delxtft, delxtf);
      d2tf(0, 0) = d2tfs(0);
      //          CALL MTXADD(PLF6,D2LF,6,6,PCMBLF)
      UtMatrixd pcmblf(6, 6); // ??? What? need to do test case; I come up with 1x1 for d2lf, not 6x6.
      pcmblf.Add(plf6, d2lf);
      //          CALL MTXADD(PTF,D2TF,6,6,PCMBTF)
      UtMatrixd pcmbtf(6, 6);
      pcmbtf.Add(ptf, d2tf);
      //          DO I=1,6
      //             DO J=1,6
      //                PHORZ(I,J)=MTPLF*PCMBLF(I,J)+MTPTF*PCMBTF(I,J)
      //             ENDDO
      //          ENDDO
      //
      phorz = mtplf * pcmblf + mtptf * pcmbtf;
   }
   //*otherwise, track resulting from height finder data and no horz data:
   //       ELSE
   else
   {
      //
      //*no horizontal data - propogate horz filter
      //*update previous state and covariance to current time
      //          CALL MTTTFP(JNR,GRL,PTR388,DT,PHORZP,XHORZP)
      mtttfp(dt, phorzp, xhorzp);
      //*update current state and covariance to current time
      //          CALL MTTTFP(JNR,GRL,PTR388,DT,PHORZ,XHORZ)
      mtttfp(dt, phorz, xhorz);
      //*end of test for height only data.
      //       ENDIF
   }
   //
   //*if a 3d track or a height finder, update local filter with input track
   //       IF ((JNR(PTR363+8).EQ.1) .OR. (JNR(PTR363+8).EQ.3)) THEN
   if ((ptr363->mUpdateFlag == 1) || (ptr363->mUpdateFlag == 3))
   {
      //*when local filter has a running vertical filter
      //          IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //*update by applying fusion equations (see "Estimation with
         //*Applications to Tracking and Navigation" by Bar-Shalom, page 529)
         //             CALL MTXSET(PIVERT,2,2,TMP2X2)
         UtMatrixd tmp2x2(pivert);
         //             CALL MTXADD(TMP2X2,PIVMST,2,2,PIVERT)
         pivert.Add(tmp2x2, pivmst);
         //             CALL MTXSET(XIVERT,2,1,TMP2X1)
         UtMatrixd tmp2x1(xivert);
         //             CALL MTXADD(TMP2X1,XIVMST,2,1,XIVERT)
         xivert.Add(tmp2x1, xivmst);
      }
      //*otherwise, no local vertical filter
      //          ELSE
      else
      {
         //*start local vertical filter with external vertical filter
         //             CALL MTXSET(PIVMST,2,2,PIVERT)
         pivert = pivmst;
         //             CALL MTXSET(XIVMST,2,1,XIVERT)
         xivert = xivmst;
         //             JNR(PTR384+6)=1
         ptr384->mVerticalChannelActive = true;
         //          ENDIF
      }
      //*set vertical measurement update time
      //          GRL(PTR384+7)=TIME
      ptr384->mVerticalChannelUpdateTime = aSimTime;
      //       ENDIF
   }
   //
   //*load the local double precision array into the 387 block
   //       DO I=1,184
   //          GRL(PTR387+I-1)=RAT(I)
   //       ENDDO
   //
   *ptr387 = at;
   //       RETURN
   //       END
   //
   //
}

//    SUBROUTINE MTTTFM(JNR,GRL,XTF,PTF,PIVERT,XIVERT,PTR384,PTR388,
//  + PTR359,DT,PZ0TF)
//! Multiple target tracking - update turning flight model
//! with measurement.
//!@param aTurningFlightStateVector (turning-flight state vector)
//!@param aTurningFlightErrorCovarianceMatrix (turning-flight error covariance matrix)
//!@param aVerticalErrorCovarianceMatrix (vertical error covariance matrix)
//!@param aVerticalFlightInformationState (vertical-flight information state)
//!@param aTrack (track header)
//!@param aMeasurement (measurement header block)
//!@param aDt Time difference between measurment and last track update.
//!@param aTurningFlightLikelihood (likelihood of turning-flight)
void MTT::mtttfm(UtMatrixd&             aTurningFlightStateVector,
                 UtMatrixd&             aTurningFlightErrorCovarianceMatrix,
                 const UtMatrixd&       aVerticalErrorCovarianceMatrix,
                 const UtMatrixd&       aVerticalFlightInformationState,
                 const MTT_Track&       aTrack,
                 const MTT_Measurement& aMeasurement,
                 double                 aDt,
                 double&                aTurningFlightLikelihood)
{
   //     *fold measurement into turning-flight model

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *july 2, 2005;

   UtMatrixd&             xtf    = aTurningFlightStateVector;
   UtMatrixd&             ptf    = aTurningFlightErrorCovarianceMatrix;
   const UtMatrixd&       pivert = aVerticalErrorCovarianceMatrix;
   const UtMatrixd&       xivert = aVerticalFlightInformationState;
   const MTT_Track*       ptr384 = &aTrack;
   const MTT_Measurement* ptr359 = &aMeasurement;
   double&                pzotf  = aTurningFlightLikelihood;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER PTR384,PTR359,PTR388,PTR360,IAVAIL,IEL
   //       DOUBLE PRECISION XTF(6),PTF(6,6),DT
   //       DOUBLE PRECISION PIVERT(2,2),XIVERT(2),PV(2,2),XV(2)
   // CBA       DOUBLE PRECISION X(6),P(6,6),DISTTF,DETTF,PZ0TF
   //       DOUBLE PRECISION X(6),P(6,6),DISTTF(1,1),DETTF,PZ0TF
   //       DOUBLE PRECISION SRANGE,RANGE,AZ,EL,DX,DY,R(2,2),SIGRNG,SIGAZ
   //       DOUBLE PRECISION RDRX,RDRY,RDRZ
   //       DOUBLE PRECISION A11,A12,TMP2X2(2,2),H(2,6),HT(6,2),S(2,2)
   //       DOUBLE PRECISION DELZ(2),B(2,2),BT(2,2),TMP2X6(2,6),K(6,2)
   //       DOUBLE PRECISION TMP6X2(6,2),DUM1(100),DUM2(100),DUM3(100)
   //       DOUBLE PRECISION DELX(6),EYE6(6,6),KH(6,6),TMP1X2(2)
   //       DOUBLE PRECISION TMP6X6(6,6)
   //       DOUBLE PRECISION TWOPI
   //       LOGICAL ERR
   //*variables used for doppler augmentation
   //       INTEGER IRR
   //       DOUBLE PRECISION SIGRR,RDRXD,RDRYD,RDRZD,RRATE,RES
   //       DOUBLE PRECISION VARXD,VARYD,VARRR,//,CXD,CYD
   //*
   //       DATA TWOPI / 6.28318530717959/
   //
   //*update filter to current time
   //       CALL MTXSET(PTF,6,6,P)
   UtMatrixd p(ptf);
   //       CALL MTXSET(XTF,6,1,X)
   UtMatrixd x(xtf);
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,P,X)
   mtttfp(aDt, p, x);
   //
   //*extract necessary radar data
   //       PTR360=JNR(PTR359+4)
   const MTT_RadarMeasurement::Data* ptr360 = &(dynamic_cast<const MTT_RadarMeasurement*>(ptr359)->GetData());
   //       SRANGE=DBLE(GRL(PTR360))
   double srange = ptr360->mRange;
   //       AZ=DBLE(GRL(PTR360+1))
   double az = ptr360->mAzimuth;
   //       EL=DBLE(GRL(PTR360+2))
   double el = ptr360->mElevation;
   //       SIGRNG=DBLE(GRL(PTR360+4))
   double sigrng = ptr360->mRangeStDev;
   //       SIGAZ=DBLE(GRL(PTR360+5))
   double sigaz = ptr360->mAzimuthStDev;
   //       RDRX=DBLE(GRL(PTR360+8))
   double rdrx = ptr360->mSensorLocation[0];
   //       RDRY=DBLE(GRL(PTR360+9))
   double rdry = ptr360->mSensorLocation[1];
   //       RDRZ=DBLE(GRL(PTR360+10))
   double rdrz = ptr360->mSensorLocation[2];
   //
   //*range rate information (7/24/08)
   //       SIGRR=DBLE(GRL(PTR360+7))
   double sigrr = ptr360->mRangeRateStDev;
   //       RRATE=DBLE(GRL(PTR360+3))
   double rrate = ptr360->mRangeRate;
   //       RDRXD=DBLE(GRL(PTR360+16))
   double rdrxd = ptr360->mSensorVelocity[0];
   //       RDRYD=DBLE(GRL(PTR360+17))
   double rdryd = ptr360->mSensorVelocity[1];
   //       RDRZD=DBLE(GRL(PTR360+18))
   // (var. not used) double rdrzd = ptr360->mSensorVelocity[2];
   //*determine if elevation is available and adjust for ground range
   //       IAVAIL=JNR(PTR359+6)
   unsigned iavail = ptr359->mDataAvailable;
   //       IEL=MOD(IAVAIL/4,2)
   unsigned iel = (iavail / 4) % 2;
   //       IF (IEL.EQ.1) THEN
   double range = srange;
   if (iel == 1)
   {
      //          RANGE=SRANGE*DCOS(EL)
      range *= cos(el);
      //       ELSE
   }
   else
   {
      //*set vertical flag if matrix is invertible
      //          RANGE=SRANGE
      //          IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //*invert vertical information matrix
         //             CALL MTXINV(PIVERT,2,PV,DUM1,ERR)
         UtMatrixd pv(pivert);
         try
         {
            pv.Invert();
         }
         //*ignore vertical if information matrix is not invertible
         //             IF (.NOT. ERR) THEN
         //                CALL MTXMUL(PV,2,2,XIVERT,2,1,XV)
         //                RANGE=DSQRT(SRANGE*SRANGE-(XV(1)-RDRZ)*(XV(1)-RDRZ))
         //             ENDIF
         catch (UtMatrixd::SingularMatrixError&)
         {
            UtMatrixd xv(2, 1);
            xv.Multiply(pv, xivert);
            range = sqrt(srange * srange - (xv(0) - rdrz) * (xv(0) - rdrz));
         }
         //          ENDIF
      }
      //       END IF
   }
   //*statistics for delx and dely are in a system that has been rotated "az"
   //*degrees,where "az" measured counterclockwise from east (per suppressor);
   //       DX=RANGE*DCOS(AZ)
   double dx = range * cos(az);
   //       DY=RANGE*DSIN(AZ)
   double dy = range * sin(az);
   //       CALL MTXZRO(R,2,2)
   UtMatrixd r(2, 2);
   //       R(1,1)=SIGRNG*SIGRNG
   r(0, 0) = sigrng * sigrng;
   //       R(2,2)=RANGE*RANGE*SIGAZ*SIGAZ
   r(1, 1) = range * range * sigaz * sigaz;
   //*do a transformation of coordinates involving pure rotation to bring the
   //*system into our standard coordinate system
   //       A11=DX/RANGE
   double a11 = dx / range;
   //       A12=DY/RANGE
   double a12 = dy / range;
   //       B(1,1)=A11
   UtMatrixd b(2, 2);
   b(0, 0) = a11;
   //       B(1,2)=-A12
   b(0, 1) = -a12;
   //       B(2,1)=A12
   b(1, 0) = a12;
   //       B(2,2)=A11
   b(1, 1) = a11;
   //       CALL TRNPSE(B,2,2,BT)
   UtMatrixd bt(2, 2);
   bt.Transpose(b);
   //*now compute the error covariance matrix for delx and dely
   //       CALL MTXMUL(B,2,2,R,2,2,TMP2X2)
   UtMatrixd tmp2x2;
   tmp2x2.Multiply(b, r);
   //       CALL MTXMUL(TMP2X2,2,2,BT,2,2,R)
   r.Multiply(tmp2x2, bt);
   //       CALL FIXSYM(R,2,1,3)
   r.Symmetrize();
   //*calculate the covariance of the innovation
   //       CALL MTXZRO(H,2,6)
   UtMatrixd h(2, 6);
   //       H(1,1)=1
   h(0, 0) = 1.0;
   //       H(2,2)=1
   h(1, 1) = 1.0;
   //       CALL TRNPSE(H,2,6,HT)
   UtMatrixd ht(6, 2);
   ht.Transpose(h);
   //       CALL MTXMUL(H,2,6,P,6,6,TMP2X6)
   UtMatrixd tmp2x6(2, 6);
   tmp2x6.Multiply(h, p);
   //       CALL MTXMUL(TMP2X6,2,6,HT,6,2,TMP2X2)
   tmp2x2.Multiply(tmp2x6, ht);
   //       CALL MTXADD(TMP2X2,R,2,2,S)
   UtMatrixd s(2, 2);
   s.Add(tmp2x2, r);
   //       CALL FIXSYM(S,2,1,3)
   s.Symmetrize();
   //*compute the kalman gain matrix
   //       CALL MTXMUL(P,6,6,HT,6,2,TMP6X2)
   UtMatrixd tmp6x2(6, 2);
   tmp6x2.Multiply(p, ht);
   //       CALL MTXDIV(TMP6X2,S,6,2,DUM1,DUM2,DUM3,K)
   UtMatrixd k(6, 2);
   k.Divide(tmp6x2, s);
   //*compute innovation
   //       DELZ(1)=DX+RDRX-X(1)
   UtMatrixd delz(2);
   delz(0) = dx + rdrx - x(0);
   //       DELZ(2)=DY+RDRY-X(2)
   delz(1) = dy + rdry - x(1);
   //*a posteriori state estimate and covariance matrix
   //       CALL MTXMUL(K,6,2,DELZ,2,1,DELX)
   UtMatrixd delx(6);
   delx.Multiply(k, delz);
   //       CALL MTXADD(DELX,X,6,1,XTF)
   xtf.Add(delx, x);
   //*update covariance matrix: phorz=(i-kh)*phorz;
   //       CALL MTXMUL(K,6,2,H,2,6,KH)
   UtMatrixd kh(6, 6);
   kh.Multiply(k, h);
   //       CALL IDENTY(6,EYE6)
   UtMatrixd eye6(6, 6);
   eye6.LoadIdentity();
   //       CALL MTXSUB(EYE6,KH,6,6,TMP6X6)
   UtMatrixd tmp6x6(6, 6);
   tmp6x6.Subtract(eye6, kh);
   //       CALL MTXMUL(TMP6X6,6,6,P,6,6,PTF)
   ptf.Multiply(tmp6x6, p);
   //*roundoff error can corrupt symmetry of phorz, so symmetrize;
   //       CALL FIXSYM(PTF,6,1,3)
   ptf.Symmetrize();
   //*augment horizontal velocity estimate if doppler is available (7/24/08)
   //*reference:  "multiple target tracking with radar applications" by
   //*samuel s. blackman, artech house, 1986
   //*note:  elevation should be available
   //       IRR=MOD(IAVAIL/8,2)
   unsigned irr = (iavail / 8) % 2;
   //       IF ((IRR.EQ.1).AND.(IEL.EQ.1)) THEN
   if ((irr == 1) && (iel == 1))
   {
      //*step 1 computes the estimate of residual error in range rate
      //          RES=(RRATE*DCOS(EL)+(RDRXD*DCOS(AZ)+RDRYD*DSIN(AZ))) -
      //     +      (XTF(3)*DCOS(AZ)+XTF(4)*DSIN(AZ))
      double res = (rrate * cos(el) + (rdrxd * cos(az) + rdryd * sin(az))) - (xtf(2) * cos(az) + xtf(3) * sin(az));
      //*step 2 computes xd and yd variances from kalman cov matrix
      //          VARXD=PTF(3,3)
      double varxd = ptf(2, 2);
      //          VARYD=PTF(4,4)
      double varyd = ptf(3, 3);
      //          VARRR=SIGRR*SIGRR
      double varrr = sigrr * sigrr;
      //*step 3 computes the total variance of the residual error
      //          C=(VARXD*DCOS(AZ)**2+VARYD*DSIN(AZ)**2)+VARRR
      double c = (varxd * cos(az)) * (varxd * cos(az)) + (varyd * sin(az)) * (varyd * sin(az)) + varrr;
      //*step 4 computes the individual variances
      //          CXD=VARXD*DCOS(AZ)/C
      double cxd = varxd * cos(az) / c;
      //          CYD=VARYD*DSIN(AZ)/C
      double cyd = varyd * sin(az) / c;
      //*step 5 augments the kalman velocity estimate
      //          XTF(3)=XTF(3)+CXD*RES
      xtf(2) += cxd * res;
      //          XTF(4)=XTF(4)+CYD*RES
      xtf(3) += cyd * res;
      //       ENDIF
   }
   //*compute the likelihood function for the generalized pseudo
   //*bayesian estimator of the first order
   //       CALL MTXDIV(DELZ,S,1,2,DUM1,DUM2,DUM3,TMP1X2)
   UtMatrixd tmp1x2(1, 2);
   UtMatrixd delzt(1, 2);
   delzt.Transpose(delz);
   tmp1x2.Divide(delzt, s);
   //       CALL MTXMUL(TMP1X2,1,2,DELZ,2,1,DISTTF)
   UtMatrixd disttf(1);
   disttf.Multiply(tmp1x2, delz);
   //       DETTF=S(1,1)*S(2,2)-S(1,2)*S(2,1)
   double dettf = s(0, 0) * s(1, 1) - s(0, 1) * s(1, 0);
   //       IF ((DETTF.GE.0.0) .AND. (DISTTF(1,1).LE.50.0)) THEN
   if ((dettf >= 0.0) && (disttf(0, 0) <= 50.0))
   {
      //          PZ0TF=(1.0/(TWOPI*DSQRT(DETTF)))*DEXP(-0.5*DISTTF(1,1))
      pzotf = (1.0 / (UtMath::cTWO_PI * sqrt(dettf))) * exp(-0.5 * disttf(0, 0));
      //          IF (PZ0TF.LT. 1.0D-10) PZ0TF=1.0D-10
      if (pzotf < 1.0e-10)
      {
         pzotf = 1.0e-10;
      }
   }
   //       ELSE
   else
   {
      //          PZ0TF=1.0D-10
      pzotf = 1.0e-10;
      //       ENDIF
   }
   //
   //       RETURN
   //       END
   //
}

// SUBROUTINE MTTTFP(JNR,GRL,PTR388,DT,PTF,XTF)
//! Multiple target tracking - propagate tf states.
//!@param aDt  Delta time between measurment and last track update.
//!@param aCovarianceMatrix (covariance matrix)
//!@param aStateVector (state vector)
void MTT::mtttfp(double aDt, UtMatrixd& aCovarianceMatrix, UtMatrixd& aStateVector)
{
   //     *update turning flight filter to current time
   //       IMPLICIT NONE
   //
   // Parameter refs:
   UtMatrixd& ptf = aCovarianceMatrix;
   UtMatrixd& xtf = aStateVector;

   //       REAL GRL(*)
   //       INTEGER JNR(*)
   //
   //       INTEGER PTR388
   //       DOUBLE PRECISION DT,PTF(6,6),XTF(6),X(6)
   //       DOUBLE PRECISION PHI(6,6),PHIT(6,6),W(2),Q(6,6)
   //       DOUBLE PRECISION TMP6X6(6,6)
   //
   //*update filter to current time
   //       IF (DT.GT.0.0) THEN
   if (aDt > 0.0)
   {
      //          CALL IDENTY(6,PHI)
      UtMatrixd phi(6, 6);
      phi.LoadIdentity();
      //          PHI(1,3)=DT
      phi(0, 2) = aDt;
      //          PHI(1,5)=DT*DT/2.0
      phi(0, 4) = aDt * aDt / 2.0;
      //          PHI(2,4)=DT
      phi(1, 3) = aDt;
      //          PHI(2,6)=DT*DT/2.0
      phi(1, 5) = aDt * aDt / 2.0;
      //          PHI(3,5)=DT
      phi(2, 4) = aDt;
      //          PHI(4,6)=DT
      phi(3, 5) = aDt;
      //          CALL TRNPSE(PHI,6,6,PHIT)
      UtMatrixd phit(6, 6);
      phit.Transpose(phi);
      //*for calculation of Q,see my notes of march 28, 2004
      UtMatrixd w(2);
      //          W(1)=DBLE(GRL(PTR388+5))
      w(0) = mParameters.mTurningFlightX_Acceleration;
      //          W(2)=DBLE(GRL(PTR388+6))
      w(1) = mParameters.mTurningFlightY_Acceleration;
      //          CALL MTXZRO(Q,6,6)
      UtMatrixd q(6, 6);
      //          Q(1,1)=DT**5*W(1)/20.0
      q(0, 0) = pow(aDt, 5.0) * w(0) / 20.0;
      //          Q(1,3)=DT**4*W(1)/8.0
      q(0, 2) = pow(aDt, 4.0) * w(0) / 8.0;
      //          Q(1,5)=DT**3*W(1)/6.0
      q(0, 4) = pow(aDt, 3.0) * w(0) / 6.0;
      //          Q(2,2)=DT**5*W(2)/20.0
      q(1, 1) = pow(aDt, 5.0) * w(1) / 20.0;
      //          Q(2,4)=DT**4*W(2)/8.0
      q(1, 3) = pow(aDt, 4.0) * w(1) / 8.0;
      //          Q(2,6)=DT**3*W(2)/6.0
      q(1, 5) = pow(aDt, 3.0) * w(1) / 6.0;
      //          Q(3,1)=DT**4*W(1)/8.0
      q(2, 0) = pow(aDt, 4.0) * w(0) / 8.0;
      //          Q(3,3)=DT**3*W(1)/3.0
      q(2, 2) = pow(aDt, 3.0) * w(0) / 3.0;
      //          Q(3,5)=DT**2*W(1)/2.0
      q(2, 4) = pow(aDt, 2.0) * w(0) / 2.0;
      //          Q(4,2)=DT**4*W(2)/8.0
      q(3, 1) = pow(aDt, 4.0) * w(1) / 8.0;
      //          Q(4,4)=DT**3*W(2)/3.0
      q(3, 3) = pow(aDt, 3.0) * w(1) / 3.0;
      //          Q(4,6)=DT**2*W(2)/2.0
      q(3, 5) = pow(aDt, 2.0) * w(1) / 2.0;
      //          Q(5,1)=DT**3*W(1)/6.0
      q(4, 0) = pow(aDt, 3.0) * w(0) / 6.0;
      //          Q(5,3)=DT**2*W(1)/2.0
      q(4, 2) = pow(aDt, 2.0) * w(0) / 2.0;
      //          Q(5,5)=DT**1*W(1)/1.0
      q(4, 4) = pow(aDt, 1.0) * w(0) / 1.0;
      //          Q(6,2)=DT**3*W(2)/6.0
      q(5, 1) = pow(aDt, 3.0) * w(1) / 6.0;
      //          Q(6,4)=DT**2*W(2)/2.0
      q(5, 3) = pow(aDt, 2.0) * w(1) / 2.0;
      //          Q(6,6)=DT**1*W(2)/1.0
      q(5, 5) = pow(aDt, 1.0) * w(1) / 1.0;
      //*project ahead
      //          CALL MTXMUL(PHI,6,6,XTF,6,1,X)
      UtMatrixd x(6);
      x.Multiply(phi, xtf);
      //          CALL MTXSET(X,6,1,XTF)
      xtf = x;
      //          CALL MTXMUL(PHI,6,6,PTF,6,6,TMP6X6)
      UtMatrixd tmp6x6(6, 6);
      tmp6x6.Multiply(phi, ptf);
      //          CALL MTXMUL(TMP6X6,6,6,PHIT,6,6,PTF)
      ptf.Multiply(tmp6x6, phit);
      //          CALL MTXSET(PTF,6,6,TMP6X6)
      tmp6x6 = ptf;
      //          CALL MTXADD(TMP6X6,Q,6,6,PTF)
      ptf.Add(tmp6x6, q);
      //          CALL FIXSYM(PTF,6,0,4)
      ptf.Symmetrize();
      //       END IF
   }
   //
   //       RETURN
   //       END
   //
}

//       SUBROUTINE MTTTFQ(JNR,GRL,XTF,PTF,PIVERT,XIVERT,PTR384,PTR388,
//     + PTR359,DT,ZTSTTF,NORMTF)
//! Multiple target tracking - gate querry turning flight.
//!@param aTurningFlightState (turning flight state vector)
//!@param aTurningFlightErrorCovariance (turning flight error covariance matrix)
//!@param aVerticalErrorCovariance (vertical error covariance matrix)
//!@param aVerticalFlightState (vertical-flight information state)
//!@param aTrack (track header)
//!@param aMeasurement (measurement header)
//!@param aDt Time difference between measurment and last track update.
//!@param aHypothesisTestValue TODO_MIKE Please document.
//!@param aInnovationValue (hypothesis test value)
void MTT::mtttfq(const UtMatrixd&       aTurningFlightState,
                 const UtMatrixd&       aTurningFlightErrorCovariance,
                 const UtMatrixd&       aVerticalErrorCovariance,
                 const UtMatrixd&       aVerticalFlightState,
                 const MTT_Track&       aTrack,
                 const MTT_Measurement& aMeasurement,
                 const double&          aDt,
                 double&                aHypothesisTestValue, // out
                 double&                aInnovationValue)                    // out
{
   //     *determine pertinent statistical values for turning flight

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *june 17, 2005;

   // References
   const UtMatrixd&       xtf    = aTurningFlightState;
   const UtMatrixd&       ptf    = aTurningFlightErrorCovariance;
   const UtMatrixd&       pivert = aVerticalErrorCovariance;
   const UtMatrixd&       xivert = aVerticalFlightState;
   const MTT_Track*       ptr384 = &aTrack;
   const MTT_Measurement* ptr359 = &aMeasurement;
   double&                ztsttf = aHypothesisTestValue;
   double&                normtf = aInnovationValue;

   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       INTEGER I,J,PTR359,PTR384,PTR388,PTR360,IAVAIL,IEL,LMASTR
   //       DOUBLE PRECISION XTF(6),PTF(6,6),ZTSTTF,MAXVAR,DT,NORMTF,DISCRM
   //       DOUBLE PRECISION PIVERT(2,2),XIVERT(2),PV(2,2),XV(2)
   //       DOUBLE PRECISION X(4),P(4,4),PUPD(6,6),XUPD(6)
   //       DOUBLE PRECISION SRANGE,RANGE,AZ,EL,DX,DY,R(2,2)
   //       DOUBLE PRECISION RDRX,RDRY,RDRZ,SIGRNG,SIGAZ,SIGEL
   //       DOUBLE PRECISION A11,A12,TMP2X2(2,2),H(2,4),HT(4,2),S(2,2)
   //       DOUBLE PRECISION DELZ(2),B(2,2),BT(2,2),TMP2X4(2,6),K(4,2)
   //       DOUBLE PRECISION TMP4X2(4,2),DUM1(100),DUM2(100),DUM3(100)
   //       DOUBLE PRECISION TWOPI
   //       DOUBLE PRECISION //(2,2),CINV(2,2),X1,X2
   //       LOGICAL ERR
   //       DATA TWOPI / 6.28318530717959/
   //
   // Note: more initialization that usual must be performed here, because c does not allow
   // declaration between the goto statement and its label.
   UtMatrixd s(2, 2);
   UtMatrixd delz(2);
   UtMatrixd c(2, 2);
   UtMatrixd k(4, 2);
   UtMatrixd tmp4x2(4, 2);
   UtMatrixd tmp2x4(2, 4);
   UtMatrixd ht(4, 2);
   UtMatrixd h(2, 4);
   UtMatrixd tmp2x2(2, 2);
   UtMatrixd bt(2, 2);
   UtMatrixd b(2, 2);
   UtMatrixd r(2, 2);
   //*update filter to current time
   //       LMASTR=JNR(2)-JNR(1)
   //       CALL MTXSET(PTF,6,6,PUPD)
   UtMatrixd pupd(ptf);
   //       CALL MTXSET(XTF,6,1,XUPD)
   UtMatrixd xupd(xtf);
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   //       WRITE(67,1111)
   // 1111  FORMAT(' MTTTFQ - X_/RDRX,RDRY,RDRZ/S/DELZ,ZTSTTF')
   // C1111  FORMAT(' MTTTFQ - PUPD/XUPD/P_/X_/AZ,EL,SRANGE,RANGE/SIGAZ,SIGEL'
   // C    +        ',SIGRNG/RDRX,RDRY,RDRZ/DX,DY/R/RXY/S/DELZ,ZTSTTF')
   // C      WRITE(67,1112)((PUPD(I,J),J=1,4),I=1,4)
   // C      WRITE(67,1112)(XUPD(I),I=1,4)
   // 1112  FORMAT(9X,4E23.15)
   //       ENDIF
   ////==================================================== .cov output ======
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,PUPD,XUPD)
   mtttfp(aDt, pupd, xupd);
   //*determine maximum diagonal entry of the non-acc states
   //       MAXVAR=0.0
   double maxvar = 0.0;
   //       DO I=1,4
   //          IF (PUPD(I,I).GT.MAXVAR) MAXVAR=PUPD(I,I)
   //       END DO
   for (int i = 0; i < 4; ++i)
   {
      if (pupd(i, i) > maxvar)
      {
         maxvar = pupd(i, i);
      }
   }
   //*rescale P if max variance is too large
   //       IF (MAXVAR.GT.DBLE(GRL(PTR388+25))) THEN
   if (maxvar > mParameters.mTurningFlightMaxVariance)
   {
      //          DO I=1,6
      //             DO J=1,6
      //                PUPD(I,J)=DBLE(GRL(PTR388+25))*(PUPD(I,J)/MAXVAR)
      //             END DO
      //          END DO
      pupd *= mParameters.mTurningFlightMaxVariance / maxvar;
      //       END IF
   }
   //*use only the non acceleration states
   //       DO I=1,4
   //          X(I)=XUPD(I)
   //          DO J=1,4
   //             P(I,J)=PUPD(I,J)
   //          END DO
   //       END DO
   UtMatrixd x(4);
   x.CopyFrom(xupd, 0, 0, 4, 1);
   UtMatrixd p(4, 4);
   p.CopyFrom(pupd, 0, 0, 4, 4);
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   // C      WRITE(67,1112)((P(I,J),J=1,4),I=1,4)
   //       WRITE(67,1112)X
   //       ENDIF
   ////==================================================== .cov output ======
   //
   //*extract necessary radar data
   //       PTR360=JNR(PTR359+4)
   const MTT_RadarMeasurement::Data* ptr360 = &(dynamic_cast<const MTT_RadarMeasurement*>(ptr359)->GetData());
   //       SRANGE=DBLE(GRL(PTR360))
   double srange = ptr360->mRange;
   //       AZ=DBLE(GRL(PTR360+1))
   double az = ptr360->mAzimuth;
   //       EL=DBLE(GRL(PTR360+2))
   double el = ptr360->mElevation;
   //       SIGRNG=DBLE(GRL(PTR360+4))
   double sigrng = ptr360->mRangeStDev;
   //       SIGAZ=DBLE(GRL(PTR360+5))
   double sigaz = ptr360->mAzimuthStDev;
   //       SIGEL=DBLE(GRL(PTR360+6))
   // (var. not used) double sigel = ptr360->mElevationStDev;
   //       RDRX=DBLE(GRL(PTR360+8))
   double rdrx = ptr360->mSensorLocation[0];
   //       RDRY=DBLE(GRL(PTR360+9))
   double rdry = ptr360->mSensorLocation[1];
   //       RDRZ=DBLE(GRL(PTR360+10))
   double rdrz = ptr360->mSensorLocation[2];

   //*determine if elevation is available and adjust for ground range
   //       IAVAIL=JNR(PTR359+6)
   unsigned iavail = ptr359->mDataAvailable;
   //       IEL=MOD(IAVAIL/4,2)
   unsigned iel = (iavail / 4) % 2;
   //       IF (IEL.EQ.1) THEN
   double range = srange; // Use the initialization to minimize computation.
   if (iel == 1)
   {
      //          RANGE=SRANGE*DCOS(EL)
      range *= cos(el);
   }
   //       ELSE
   else
   {
      //*set vertical flag if matrix is invertible
      //          RANGE=SRANGE
      //          IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //*invert vertical information matrix
         //             CALL MTXINV(PIVERT,2,PV,DUM1,ERR)
         UtMatrixd pv(pivert);
         bool      err = false;
         try
         {
            pv.Invert2x2();
         }
         catch (UtMatrixd::SingularMatrixError&)
         {
            err = true;
         }
         //*ignore vertical if information matrix is not invertible
         //             IF (.NOT. ERR) THEN
         if (!err)
         {
            //                CALL MTXMUL(PV,2,2,XIVERT,2,1,XV)
            UtMatrixd xv;
            xv.Multiply(pv, xivert);
            //                DISCRM=SRANGE*SRANGE-(XV(1)-RDRZ)*(XV(1)-RDRZ)
            double discrm = srange * srange - (xv(0) - rdrz) * (xv(0) - rdrz);
            //                IF(DISCRM.GE.0.0D0)THEN
            if (discrm > 0.0)
            {
               //                   RANGE=DSQRT(DISCRM)
               range = sqrt(discrm);
            }
            //                ELSE
            else
            {
               //*guard against invalid operation; set output variables
               //                   S(1,1)=9999999999.99999
               s(0, 0) = 9999999999.99999;
               //                   S(2,1)=9999999999.99999
               s(1, 0) = 9999999999.99999;
               //                   S(1,2)=9999999999.99999
               s(0, 1) = 9999999999.99999;
               //                   S(2,2)=9999999999.99999
               s(1, 1) = 9999999999.99999;
               //                   DELZ(1)=9999999999.99999
               delz(0) = 9999999999.99999;
               //                   DELZ(2)=9999999999.99999
               delz(1) = 9999999999.99999;
               //                   ZTSTTF=9999999999.99999
               ztsttf = 9999999999.99999;
               //                   NORMTF=9999999999.99999
               normtf = 9999999999.99999;
               ////==================================================== .cov output ======
               //                   IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)
               //                   WRITE(67,1112)RDRX,RDRY,RDRZ
               //                   ENDIF
               ////==================================================== .cov output ======
               //                   GOTO 1000
               //                     goto label_1000;  // Can't do this with gcc
               return; // Jumps to end of function.
               //                ENDIF
            }
            //             ENDIF
         }
         //          ENDIF
      }
      //       END IF
   }
   //
   //       //*statistics for delx and dely are in a system that has been rotated "az"
   //       //*degrees,where "az" measured counterclockwise from east (per suppressor);
   //       DX=RANGE*DCOS(AZ)
   double dx = range * cos(az);
   //       DY=RANGE*DSIN(AZ)
   double dy = range * sin(az);
   //       CALL MTXZRO(R,2,2)
   //       R(1,1)=(SIGRNG+RANGE/DCOS(SIGAZ*10.)-RANGE)**2
   r(0, 0) = sigrng + range / cos(sigaz * 10.0) - range;
   r(0, 0) *= r(0, 0);
   //       R(2,2)=RANGE*RANGE*SIGAZ*SIGAZ
   r(1, 1) = range * range * sigaz * sigaz;
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   // C      WRITE(67,1112)AZ,EL,SRANGE,RANGE,SIGAZ,SIGEL,SIGRNG
   //       WRITE(67,1112)RDRX,RDRY,RDRZ
   // C      WRITE(67,1112)DX,DY
   // C      WRITE(67,1115)((R(I,J),J=1,2),I=1,2)
   // 1115  FORMAT(9X,2E23.15)
   //       ENDIF
   ////==================================================== .cov output ======
   //       //*do a transformation of coordinates involving pure rotation to bring the
   //       //*system into our standard coordinate system
   //       A11=DX/RANGE
   double a11 = dx / range;
   //       A12=DY/RANGE
   double a12 = dy / range;
   //       B(1,1)=A11
   b(0, 0) = a11;
   //       B(1,2)=-A12
   b(0, 1) = a12;
   //       B(2,1)=A12
   b(1, 0) = a12;
   //       B(2,2)=A11
   b(1, 1) = a11;
   //       CALL TRNPSE(B,2,2,BT)
   bt.Transpose(b);
   //       //*now compute the error covariance matrix for delx and dely
   //       CALL MTXMUL(B,2,2,R,2,2,TMP2X2)
   //       CALL MTXMUL(TMP2X2,2,2,BT,2,2,R)
   r.Multiply(tmp2x2, bt);
   //*calculate the covariance of the innovation
   //       CALL MTXZRO(H,2,4)
   //       H(1,1)=1
   h(0, 0) = 1.0;
   //       H(2,2)=1
   h(1, 1) = 1.0;
   //       CALL TRNPSE(H,2,4,HT)
   ht.Transpose(h);
   //       CALL MTXMUL(H,2,4,P,4,4,TMP2X4)
   tmp2x4.Multiply(h, p);
   //       CALL MTXMUL(TMP2X4,2,4,HT,4,2,TMP2X2)
   tmp2x2.Multiply(tmp2x4, ht);
   //       CALL MTXADD(TMP2X2,R,2,2,S)
   s.Add(tmp2x2, r);
   //       //*compute the kalman gain matrix
   //       CALL MTXMUL(P,4,4,HT,4,2,TMP4X2)
   tmp4x2.Multiply(p, ht);
   //       CALL MTXDIV(TMP4X2,S,4,2,DUM1,DUM2,DUM3,K)
   k.Divide(tmp4x2, s);
   //*compute innovation
   //       DELZ(1)=DX+RDRX-X(1)
   delz(0) = dx + rdrx - x(0);
   //       DELZ(2)=DY+RDRY-X(2)
   delz(1) = dy + rdry - x(1);
   //
   //*fwk: 31may07 -- compute the linear transformation that maps the innovation
   //*covariance to a covariance of the product of two independent standard normal
   //*distributions; because S is positive definite, we know there exists a
   //*lower triangular matrix //, such that //*//'=S
   //       CALL MTXZRO(C,2,2)
   //       IF(S(1,1).GE.0.0D0)THEN
   if (s(0, 0) >= 0.0)
   {
      //          C(1,1)=DSQRT(S(1,1))
      c(0, 0) = sqrt(s(0, 0));
      //          C(2,1)=S(2,1)/C(1,1)
      c(1, 0) = s(1, 0) / c(0, 0);
      //          DISCRM=S(2,2)-(C(2,1)*C(2,1))
      double discrm = s(1, 1) - (c(1, 0) * c(1, 0));
      //          IF(DISCRM.GE.0.0D0)THEN
      if (discrm > 0.0)
      {
         //             C(2,2)=DSQRT(DISCRM)
         c(1, 1) = sqrt(discrm);
         //*the inverse of C maps correlated random variables to uncorrelated
         //             CALL MTXZRO(CINV,2,2)
         UtMatrixd cinv(2, 2);
         //             CINV(1,1)=1.0/C(1,1)
         cinv(0, 0) = 1.0 / c(0, 0);
         //             CINV(2,1)=-C(2,1)/(C(1,1)*C(2,2))
         cinv(1, 0) = -c(1, 0) / (c(0, 0) * c(1, 1));
         //             CINV(2,2)=1.0/C(2,2)
         cinv(1, 1) = 1.0 / c(1, 1);
         //*compute radius of innovation in CEP system
         //             X1=CINV(1,1)*DELZ(1)
         double x1 = cinv(0, 0) * delz(0);
         //             X2=CINV(2,1)*DELZ(1)+CINV(2,2)*DELZ(2)
         double x2 = cinv(1, 0) * delz(0) + cinv(1, 1) * delz(1);
         //*the distance to this point is the hypothesis test value
         //             ZTSTTF=DSQRT(X1*X1+X2*X2)
         ztsttf = sqrt(x1 * x1 + x2 * x2);
         //             NORMTF=DSQRT(DELZ(1)*DELZ(1)+DELZ(2)*DELZ(2))
         normtf = sqrt(delz(0) * delz(0) + delz(1) * delz(1));
      }
      //          ELSE
      else
      {
         //*guard against invalid operation
         //             ZTSTTF=9999999999.99999
         ztsttf = 9999999999.99999;
         //             NORMTF=9999999999.99999
         normtf = 9999999999.99999;
         //          ENDIF
      }
   }
   //       ELSE
   else
   {
      //          //*guard against invalid operation
      //          ZTSTTF=9999999999.99999
      ztsttf = 9999999999.99999;
      //          NORMTF=9999999999.99999
      normtf = 9999999999.99999;
      //       ENDIF
   }
   ////==================================================== .cov output ======
   //       IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)THEN
   // C      WRITE(67,1115)((R(I,J),J=1,2),I=1,2)
   //       WRITE(67,1115)((S(I,J),J=1,2),I=1,2)
   // label_1000: ;  // todo- diagnostic statements
   // 1000  WRITE(67,1115)((S(I,J),J=1,2),I=1,2)
   // C      WRITE(67,1111)
   // C1111  FORMAT(' MTTLFQ - DELZ,ZTSTTF')
   //       WRITE(67,1112)DELZ,ZTSTTF
   //       ENDIF
   ////==================================================== .cov output ======
   //
   //       RETURN
   //       END
}

//       SUBROUTINE MTTTFS(JNR,GRL,TIME,ASGID,PTR017,PTR383,PTR388,LNKEY,
//     +  PTR384,IOPR)
//! Multiple target tracking - track fusion system (correlation).
//!@param aSimTime The current simulation time in seconds.
//!@param aNonlocalTrack A new target perception data.
//!@param aTempActiveData  Temp mtt header (383) data.
//!@param aLocalTrackPtr Active track (384).
void MTT::mtttfs_Correlation(double                      aSimTime,
                             const MTT_PerceivedCluster& aNonlocalTrack,
                             MTT_ActiveTrack::Data&      aTempActiveData,
                             MTT_ActiveTrack*&           aLocalTrackPtr)
{
   //     *fuse mtt system with a new (possibly the first) track input

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *july 7, 2005;

   const MTT_PerceivedCluster* ptr017 = &aNonlocalTrack;
   MTT_ActiveTrack::Data&      tmp387 = aTempActiveData;
   MTT_ActiveTrack*&           ptr384 = aLocalTrackPtr;
   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*),TIME,TRKTIM
   //       INTEGER LMASTR,LSUPPL,LNKEY,MTTFUS
   //*pointer to radar measurement block;
   //       INTEGER PTR017,PTR383,PTR384,PTR388,MTTCTF,TMP387
   //       INTEGER I,J,K,LST384,NXT384,PTR387,PTR363,PTR367,PTR359
   //       INTEGER ASGID,ASG384,SDR039,IOPR
   //       DOUBLE PRECISION DT,DTV,XV(2),W(2)
   //       DOUBLE PRECISION ALPHAT,ZTST,ZC,MINNRM,NORM,ALT
   //
   //
   //       REAL RATCMP(184)
   //       DOUBLE PRECISION ATCMP(92)
   //       DOUBLE PRECISION XCMP(6),PCMP(6,6),PIVCMP(2,2),XIVCMP(2)
   //       DOUBLE PRECISION MTPLFC,MTPTFC,XCMPP(6),PCMPP(6,6)
   //       EQUIVALENCE (RATCMP(1),ATCMP(1))
   //       EQUIVALENCE (PCMP,ATCMP(1)),(XCMP,ATCMP(37))
   //       EQUIVALENCE (PIVCMP,ATCMP(43)),(XIVCMP,ATCMP(47))
   //       EQUIVALENCE (MTPLFC,ATCMP(49)),(MTPTFC,ATCMP(50))
   //       EQUIVALENCE (XCMPP,ATCMP(51)),(PCMPP,ATCMP(57))
   //
   MTT_ActiveTrack::Data atcmp;
   UtMatrixd&            pcmp   = atcmp.mHorizontalInfoMatrix;
   UtMatrixd&            xcmp   = atcmp.mFilterStates;
   UtMatrixd&            pivcmp = atcmp.mVerticalInfoMatrix;
   UtMatrixd&            xivcmp = atcmp.mVerticalState;
   double&               mtplfc = atcmp.mStraightFlightModeProbability;
   double&               mtptfc = atcmp.mTurningFlightModeProbability;
   UtMatrixd&            xcmpp  = atcmp.mPreviousFilterStates;
   UtMatrixd&            pcmpp  = atcmp.mPreviousHorizontalMatrix;

   //       DOUBLE PRECISION TTRACK,FILTXP,FILTYP,FILTXV,FILTYV
   //       DOUBLE PRECISION FILTZP,FILTZV,HVMATR(6,6)
   //       DOUBLE PRECISION TMVERT,FILTXA,FILTYA,VMAT(2,2)
   //
   //       DOUBLE PRECISION HGTCYL,RADCYL,DIACYL
   //       LOGICAL LCORL8
   //
   //       REAL RTRK(184)
   //       DOUBLE PRECISION TRK(92)
   //       EQUIVALENCE (RTRK(1),TRK(1))
   //       EQUIVALENCE (TTRACK,TRK(1)),(FILTXP,TRK(2)),(FILTYP,TRK(3))
   //       EQUIVALENCE (FILTXV,TRK(4)),(FILTYV,TRK(5)),(FILTZP,TRK(6))
   //       EQUIVALENCE (FILTZV,TRK(7)),(HVMATR,TRK(8)),(TMVERT,TRK(44))
   //       EQUIVALENCE (FILTXA,TRK(45)),(FILTYA,TRK(46)),(VMAT,TRK(47))
   //
   //       COMMON /TRUST/ TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,
   //     +         TRUSPD,IDTRGT,IDSNDR,IDRCVR,PLTRGT,PLSNDR,PLRCVR,GAMTIM
   //       REAL TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,TRUSPD
   //       INTEGER IDTRGT,IDSNDR,IDRCVR
   //       CHARACTER*20 PLTRGT,PLSNDR,PLRCVR
   //       REAL GAMTIM

   //*initialize track pointer
   MTT_NonlocalTrack::Filter trk;
   double&                   ttrack = trk.mUpdateTime;
   double&                   filtxp = trk.mFilteredState(0);
   double&                   filtyp = trk.mFilteredState(1);
   double&                   filtxv = trk.mFilteredState(2);
   double&                   filtyv = trk.mFilteredState(3);
   double&                   filtzp = trk.mFilteredState(4);
   double&                   filtzv = trk.mFilteredState(5);
   UtMatrixd&                hvmatr = trk.mInformationMatrix;
   double&                   filtxa = trk.mX_Acceleration;
   double&                   filtya = trk.mY_Acceleration;
   UtMatrixd&                vmat   = trk.mVerticalInformationMatrix;

   //       MTTCTF=0
   MTT_ActiveTrack* mttctf = nullptr;
   //       PTR384=0
   ptr384 = nullptr;
   //
   // C     *initialize for output incidents;
   //       LMASTR=JNR(2)-JNR(1)
   //       LSUPPL=JNR(JNR(JNR(LMASTR+18)+7)+1)

   //*obtain probability of type i error for track correlaiton
   //       ALPHAT=DBLE(GRL(PTR388+8))
   double alphaT = mParameters.mT2T_FalseRejectProbability;
   //*compute critical threshold for track correlation
   //       ZC=DSQRT(-2.0*DLOG(ALPHAT))
   double zc = sqrt(-2.0 * log(alphaT));
   //       MINNRM=1.0D200
   double minnrm = 1.0e+200;
   //
   //*pointer to external track header block
   //       PTR363=JNR(PTR017+37)
   const MTT_NonlocalTrack* ptr363 = ptr017->mCombinedTrackPtr;
   //*get source of track data
   //       SDR039=JNR(JNR(PTR017+3)+2)
   // This is an indirect sender list, pointing to an interaction key
   // We're not using the assignment portion of the tracker,
   // as it does not affect correlation/fusion (mjm)
   //*pointer to track data block
   //       PTR367=JNR(PTR363+1)
   const MTT_NonlocalTrack::Filter* ptr367 = &ptr363->mFilter;
   // track data contained in the nonlocal track (mjm)
   //*load external track data into local array from general array
   //*note:  last 42 dp words hold previous state and covariance
   //       DO I=1,184
   //          RTRK(I)=GRL(PTR367+I-1)
   //       ENDDO
   trk = *ptr367;
   //*load  a temporary 387 block with 367 block data
   //       XCMP(1)=FILTXP
   xcmp(0) = filtxp;
   //       XCMP(2)=FILTYP
   xcmp(1) = filtyp;
   //       XCMP(3)=FILTXV
   xcmp(2) = filtxv;
   //       XCMP(4)=FILTYV
   xcmp(3) = filtyv;
   //       XCMP(5)=FILTXA
   xcmp(4) = filtxa;
   //       XCMP(6)=FILTYA
   xcmp(5) = filtya;
   //       DO I=1,6
   //          DO J=1,6
   //             PCMP(I,J)=HVMATR(I,J)
   //          ENDDO
   //       ENDDO
   pcmp = hvmatr;
   //*load previous state estimate and covariance matrix
   //       Same as before but with the previous state / info matrix (mjm).
   //       DO I=1,6
   //          XCMPP(I)=TRK(50+I)
   //       ENDDO
   xcmpp = trk.mPreviousFilteredState;
   //       K=0
   //       DO I=1,6
   //          DO J=1,6
   //             K=K+1
   //             PCMPP(J,I)=TRK(56+K)
   //          ENDDO
   //       ENDDO
   pcmpp = trk.mPreviousInformationMatrix;
   //*fill in vertical data, if any
   //       IF (JNR(PTR363+4).EQ.1) THEN
   if (ptr363->mHasVerticalData)
   {
      //          DO I=1,2
      //             DO J=1,2
      //               PIVCMP(I,J)=VMAT(I,J)
      //             ENDDO
      //          ENDDO

      pivcmp = vmat;
      UtMatrixd xv(2);
      //          XV(1)=FILTZP
      xv(0) = filtzp; // z position
      //          XV(2)=FILTZV
      xv(1) = filtzv; // z velocity
      //          CALL MTXMUL(PIVCMP,2,2,XV,2,1,XIVCMP)
      xivcmp.Multiply(pivcmp, xv);
      UtMatrixd w(2);
      //*propagate vertical filter to current time
      //          W(1)=DBLE(GRL(PTR388+12))
      w(0) = mParameters.mVerticalVelocity;
      //          W(2)=DBLE(GRL(PTR388+13))
      w(1) = mParameters.mVerticalVelocityDecorrelationTime;
      //          CALL MTTUVP(DT,W,PIVCMP,XIVCMP,4)
      //!!!!(mjm) Added the following line (error- todo report)
      double dt = aSimTime - ttrack;
      mttuvp(dt, w, pivcmp, xivcmp, 4);
   }
   //       ELSE
   else
   {
      //*vertical filter is initialized to no information
      //          CALL MTXZRO(XIVCMP,2,1)
      //          CALL MTXZRO(PIVCMP,2,2)
      xivcmp.Fill(0.0);
      pivcmp.Fill(0.0);
      //       ENDIF
   }
   //       MTPLFC=0.5
   mtplfc = 0.5;
   //       MTPTFC=0.5
   mtptfc = 0.5;
   //*update to present time
   //       DT=DBLE(TIME)-TTRACK
   double dt = aSimTime - ttrack;
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,PCMP,XCMP)
   mtttfp(dt, pcmp, xcmp);
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,PCMPP,XCMPP)
   mtttfp(dt, pcmpp, xcmpp);
   //
   //*reserve room in general array for the temporary 387 block
   //       CALL DATGET(JNR,GRL,1,-387, TMP387)
   //*store temporary 387 into general array
   //       DO I=1,184
   //          GRL(TMP387+I-1)=RATCMP(I)
   //       ENDDO
   tmp387 = atcmp;
   //
   //       (mjm) Following is the correlator, need the up-to-date 387 block (active track) as input
   //*now loop over linked list of tracks and correlate
   //       ASG384=0
   //       PTR384=JNR(PTR383+5)
   unsigned trackNum = 0;
   //       LST384=PTR383+5
   MTT_Tracks::MTT_ActiveTrackList& lst384 = mTracks.mActiveTrackList;
   //       DO WHILE (PTR384.GT.0)
   while (trackNum < lst384.GetCount())
   {
      ptr384 = lst384.GetEntry(trackNum);
      //*determine if track is still active
      //           TRKTIM=GRL(PTR384+1)
      double trktim = ptr384->mUpdateTime;
      //           PTR387=JNR(PTR384+3)
      MTT_ActiveTrack::Data* ptr387 = &ptr384->GetData();
      //           DT=DBLE(TIME-TRKTIM)
      dt = aSimTime - trktim;
      //           IF (DT.GT.DBLE(GRL(PTR388))) THEN
      if (dt > mParameters.mActiveTrackDropTime)
      {
         //*drop track:  first give back track data block
         //               CALL DATGIV(JNR,GRL,-387,0,0, PTR387 )
         // C*====================================================== .ctk =======;
         //               IF(MOD(JNR(LMASTR+30)/2,2).EQ.1)THEN
         //                  WRITE(62,1113)TIME,4,0,JNR(PTR384+2),0,IDTRGT,PLTRGT,
         //     +            IDSNDR,PLSNDR,IDRCVR,PLRCVR
         // 1113             FORMAT(E15.7,2I3,2I8,3(I8,1X,A20),2X,15E15.7)
         //               ENDIF
         // C*====================================================== .ctk =======;
         //*reset links
         //               NXT384=JNR(PTR384)
         //               JNR(LST384)=NXT384
         // C             *write drops-mttc-active-track incident;
         //               JNR(LSUPPL)=JNR(PTR384+2)
         //               CALL OUTSIT(JNR,GRL,208,TIME,LNKEY,-1,-1,-1,-1,-1 )
         //               CALL DATGIV(JNR,GRL,-384,0,0, PTR384 )
         WsfObserver::ActiveTrackDropped (&mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, ptr384);
         lst384.Delete(ptr384->GetId());
         // NOTE: Do NOT increment 'trackNum'. The next (and following entries) are shifted to fill
         //       the hole made by deleting the current entry.
         //               PTR384=NXT384
         //               GOTO 100
         continue;
         //           END IF
      }
      //*determine if vertical channel is still active
      //           IF (JNR(PTR384+6).EQ.1) THEN
      if (ptr384->mVerticalChannelActive)
      {
         //              DTV=DBLE(TIME-GRL(PTR384+7))
         double dtv = aSimTime - ptr384->mVerticalChannelUpdateTime;
         //              IF (DTV.GT.DBLE(GRL(PTR388+32))) THEN
         if (dtv > mParameters.mVerticalChannelDropTime)
         {
            //*reset flags, time, and information matrices
            //                 JNR(PTR384+6)=0
            ptr384->mVerticalChannelActive = false;
            //                 GRL(PTR384+7)=0.
            ptr384->mVerticalChannelUpdateTime = 0.0;
            //                 DO I=84,95
            //                    GRL(PTR387+I)=0.
            //                 ENDDO
            ptr387->mVerticalInfoMatrix.Fill(0.0);
            //              ENDIF
         }
         //           ENDIF
      }
      //*skip track if unmatched assignment update; save matched asg
      //           (mjm) Assignment logic not necessary.
      //           IF(ASGID.GT.0)THEN
      //              IF(ASGID.EQ.JNR(PTR384+4).AND.SDR039.EQ.JNR(PTR384+5))THEN
      //                 ASG384=PTR384
      //              ELSEIF(JNR(PTR384+4).GT.0.AND.((JNR(PTR384+4).NE.ASGID)
      //     +        .OR.JNR(PTR384+5).NE.SDR039))THEN
      //                 GOTO 90
      //              ENDIF
      //           ENDIF
      ////==================================================== .cov output ======
      //           IF(MOD(JNR(LMASTR+30)/64,2).EQ.1)
      //     +     WRITE(67,1112)TIME,DT,JNR(JNR(PTR017+10)+8),JNR(PTR384+2)
      // 1112      FORMAT(' MTTTFS - TIME,DT,TGTID,TRKID',/,9X,2F10.3,2I10)
      ////==================================================== .cov output ======
      //*compute test value to determine if correlation is within error
      //*ellipse
      //           PTR387=JNR(PTR384+3)
      //           CALL MTTGTT(JNR,GRL,PTR387,TMP387,PTR388,PTR359,DT,ZTST,NORM)
      double ztst, norm;
      mttgtt(*ptr387, tmp387, dt, ztst, norm);
      ////==================================================== .dbg output ======
      //           IF(MOD(JNR(LMASTR+30)/32,2).EQ.1)
      //     +     WRITE(66,1111)TIME,DT,ZTST
      // 1111      FORMAT(3F10.2)
      ////==================================================== .dbg output ======
      // C         *test track correlation using either
      // C         * mttc-track-correlation-cylinder or
      // C         * mttc-probability-of-falsely-rejecting-correlation-(type-i;
      bool lcorl8 = false;
      // C         *when track correlation cylinder specified:
      //            DIACYL=DBLE(GRL(PTR388+34))
      double diacyl = mParameters.mTrackCorrelationCylinderDiameter;
      //           IF(DIACYL.GT.0.0D0)THEN
      if (diacyl > 0.0)
      {
         // C            *when track correlation cylinder radius ok;
         //               RADCYL=DIACYL/2.0D0
         double radcyl = diacyl / 2.0;
         //              IF(NORM.LT.9999999999.99999.AND.NORM.LE.RADCYL)THEN
         if ((norm < 9999999999.99999) && (norm < radcyl))
         {
            // C               *set correlation to true;
            //                  LCORL8=.TRUE.
            lcorl8 = true;
            // C               *when vertical data is available:
            //                  IF(JNR(PTR363+4).EQ.1.AND.JNR(PTR384+6).EQ.1) THEN
            if (ptr363->mHasVerticalData && ptr384->mVerticalChannelActive)
            {
               //                    HGTCYL=DBLE(GRL(PTR388+33))/2.0D0
               double hgtcyl = mParameters.mTrackCorrelationCylinderHeight / 2.0; // why divide by 2?
               // C                  *get altitude from ptr387;
               //                     CALL MTTALT(JNR,GRL,PTR384, ALT)
               double alt = 0.0;
               mttalt(ptr384, alt);
               // C                  *when track correlation cylinder height fails:
               //                     IF(DABS(FILTZP-ALT).GT.HGTCYL)THEN
               if (fabs(filtzp - alt) > hgtcyl)
               {
                  // C                     *set correlation to false;
                  //                        LCORL8=.FALSE.
                  lcorl8 = false;
               }
               // C                  *end test if track correlation cylinder height ok.
               //                     ENDIF
            }
            // C               *end of test if vertical data is available.
            //                  ENDIF
         }
         // C            *end test if track correlation cylinder radius ok.
         //               ENDIF
      }
      // C         *otherwise, test mttc-probability-of-falsely-rejecting-:
      //            ELSEIF(ZTST.LT.ZC)THEN
      else if (ztst < zc)
      {
         // C            *for this track to be considered, it must fall within error
         // C            *  ellipse, whose size is determined by the probability of
         // C            *  a type i error;
         //               LCORL8=.TRUE.
         lcorl8 = true;
      }
      // C         *end test for track correlation.
      //            ENDIF
      // C         *remember the best of all correlated tracks;
      //            IF(LCORL8)THEN
      if (lcorl8)
      {
         //*the smallest test value wins;
         //               IF (NORM.LT.MINNRM) THEN
         if (norm < minnrm)
         {
            //                   MINNRM=NORM
            minnrm = norm;
            //                   MTTCTF=PTR384
            mttctf = ptr384;
            //               ENDIF
         }
         //           ENDIF
      }
      //   90      LST384=PTR384
      //           PTR384=JNR(LST384)
      ++trackNum;
      // C     *end of loop over linked list of tracks.
      //   100  END DO
   }
   // NOTE: We are not considering assignment updates as special (mjm/jaj)
   MTT_ActiveTrack* asg384 = nullptr;
   //*when not correlated with existing track and not an assignment update:
   //       IF (MTTCTF.EQ.0.AND.ASG384.EQ.0) THEN
   if ((mttctf == nullptr) && (asg384 == nullptr))
   {
      // (mjm) This is an uncorrelated track initiation;
      // A WsfLocalTrack will be produced in the trackmanager.
      //*start a new track using the input data of the external track
      //          CALL DATGET(JNR,GRL,1,-384, PTR384)
      auto ptr384up = ut::make_unique<MTT_ActiveTrack>();
      ptr384        = ptr384up.get();
      //*add to linked list
      // NOTE: Track cannot be 'linked' (added) to the list until the track ID is assigned
      //          JNR(LST384)=PTR384
      //*increment track id
      //          JNR(PTR383+4)=JNR(PTR383+4)+1
      ++mTracks.mActiveTrackCounter;
      //*load 384 block
      //          JNR(PTR384)=0
      //          GRL(PTR384+1)=TIME
      ptr384->mUpdateTime = aSimTime;
      //          JNR(PTR384+2)=JNR(PTR383+4)
      ptr384->mTrackId = mTracks.mActiveTrackCounter;
      // C        *write initiates-mttc-active-track incident;
      // todo-mjm (interface call?)
      //           JNR(LSUPPL)=JNR(PTR384+2)
      //           JNR(LSUPPL+1)=JNR(PTR363+3)
      //           JNR(LSUPPL+2)=1
      //           CALL OUTSIT(JNR,GRL,207,TIME,LNKEY,-1,-1,-1,SDR039,-1 )
      //*make the temporary 387 block permanent
      //           JNR(PTR384+3)=TMP387
      ptr384->SetData(tmp387);
      //*store assignment id, if any, and commander (sender) pointer
      //          JNR(PTR384+4)=ASGID
      ptr384->mAssignmentId = 0; // mjm- no assignments.
      //          JNR(PTR384+5)=SDR039
      ptr384->mOriginatorId = ptr017->mLastOriginatorId;
      ptr384->mSensorId     = ptr017->mLastSensorId;
      //*if the input track has a height filter running, then set height flag
      //          IF (JNR(PTR363+4).EQ.1) THEN
      if (ptr363->mHasVerticalData)
      {
         //             JNR(PTR384+6)=1
         ptr384->mVerticalChannelActive = true;
         //             GRL(PTR384+7)=TIME
         ptr384->mVerticalChannelUpdateTime = aSimTime;
      }
      //          ELSE
      else
      {
         //             JNR(PTR384+6)=0
         ptr384->mVerticalChannelActive = false;
         //          ENDIF
      }
      // Add to list of active tracks (see NOTE above)
      mTracks.mActiveTrackList.Add(std::move(ptr384up));
      WsfObserver::ActiveTrackInitiated (&mMTT_InterfacePtr->GetSimulation())(aSimTime, mManagerPtr, ptr384, nullptr);
   }
   // C ====== *set output flags for .ctk file ===============
   //           IOPR=2
   // C     *but, when non-correlated assignment update:
   //        ELSEIF (MTTCTF.EQ.0.AND.ASG384.GT.0) THEN
   else if ((mttctf == nullptr) && (asg384 != nullptr))
   {
      // This should not occur in WSF/MTT (jaj)
      // C        *update existing assigned track
      //          GRL(ASG384+1)=TIME
      // C        *recycle old active track data;
      //          PTR387=JNR(ASG384+3)
      //          CALL DATGIV(JNR,GRL,-387,0,0, PTR387 )
      // C        *replace old assigned track data;
      //          JNR(ASG384+3)=TMP387
      //          PTR384=ASG384
      //          IF (JNR(PTR363+4).EQ.1) THEN
      //             GRL(PTR384+7)=TIME
      //          ENDIF
   }
   // C ====== *set output flags for .ctk file ===============
   //           IOPR=4
   // C     *otherwise, correlated target:
   //        ELSE
   else
   {
      // C        *when mttc-track-fusion initial-source-only and
      // C        * sources do not match:
      //           MTTFUS=JNR(JNR(JNR(JNR(LNKEY)+3)+36)+5)
      //           IF(MTTFUS.EQ.1.AND.SDR039.NE.JNR(MTTCTF+5))THEN
      if ((!mCorrelateAllSources) &&
          ((ptr017->mLastOriginatorId != mttctf->mOriginatorId) || (ptr017->mLastSensorId == mttctf->mSensorId)))
      {
         // return no updated track, clear trk id, prevent call update;
         //              PTR384=0
         ptr384 = nullptr;
         //             JNR(PTR017+41)=0
         const_cast<MTT_PerceivedCluster*>(ptr017)->mTrackNumber = 0;
      }
      // otherwise, mttc-track-fusion all-sources or sources match:
      //           ELSE
      else
      {
         // C           *fuse external track with correlated local active track
         //              PTR384=MTTCTF
         ptr384 = mttctf; // return correlated track
         // Fusion is done separately in MTT_Fuse (mjm)
         //             DT=DBLE(TIME-GRL(PTR384+1))
         //             CALL MTTTFE(JNR,GRL,PTR017,PTR388,TMP387,PTR384,DT,TIME)
         // C           *update track measurement time
         //             GRL(PTR384+1)=TIME
         // C           *return memory of temporary 387 block
         //             CALL DATGIV(JNR,GRL,-387,0,0, TMP387 )
         // C           *adjust assignment if correlation changes
         //             IF(ASG384.GT.0.AND.PTR384.NE.ASG384)THEN
         //                JNR(PTR384+4)=JNR(ASG384+4)
         //                JNR(PTR384+5)=JNR(ASG384+5)
         //                JNR(ASG384+4)=0
         //             ENDIF
         // C            *set output flags for .ctk file;
         //             IOPR=4
         // C        *end test of mttc-track-fusion and matching sources.
         //          ENDIF
         // C        *return memory of temporary 387 block
         //          CALL DATGIV(JNR,GRL,-387,0,0, TMP387 )
         // C     *end of correlation result.
         //       ENDIF
      }
      aLocalTrackPtr = ptr384; // Return the correlated track, if any.
   }
}
//       SUBROUTINE MTTTFS(JNR,GRL,TIME,ASGID,PTR017,PTR383,PTR388,LNKEY,
//     +  PTR384,IOPR)
//! Multiple target tracking - track fusion system (fusion).
//!@param aSimTime The current simulation time in seconds.
//!@param aNonlocalTrack A new target perception data.
//!@param aTempActiveData  Temp mtt header (383) data.
//!@param aLocalTrackPtr Active track (384).
void MTT::mtttfs_Fusion(double                      aSimTime,
                        const MTT_PerceivedCluster& aNonlocalTrack,
                        MTT_ActiveTrack::Data&      aTempActiveData,
                        MTT_ActiveTrack*&           aLocalTrackPtr)
{
   // References:
   const MTT_PerceivedCluster* ptr017 = &aNonlocalTrack;
   MTT_ActiveTrack::Data&      tmp387 = aTempActiveData;
   MTT_ActiveTrack*&           mttctf = aLocalTrackPtr; // mttctf determined from MTT_Correlate.
   // (mjm) not handling assignment updates....
   //*but, when non-correlated assignment update:
   //       ELSEIF (MTTCTF.EQ.0.AND.ASG384.GT.0) THEN
   //*replace existing assigned track
   //          GRL(ASG384+1)=TIME
   //          JNR(ASG384+3)=TMP387
   //          PTR384=ASG384
   //          IF (JNR(PTR363+4).EQ.1) THEN
   //             GRL(PTR384+7)=TIME
   //          ENDIF
   //*otherwise, correlated target:
   // (mjm) proceed with fusion; this will be called from the track manager.
   //       ELSE
   //*fuse external track with correlated local active track
   //          PTR384=MTTCTF
   MTT_Track* ptr384 = mttctf;
   //          DT=DBLE(TIME-GRL(PTR384+1))
   double dt = aSimTime - ptr384->mUpdateTime;
   //          CALL MTTTFE(JNR,GRL,PTR017,PTR388,TMP387,PTR384,DT,TIME)
   mtttfe(*ptr017, tmp387, *ptr384, dt, aSimTime);
   //*update track measurement time
   //          GRL(PTR384+1)=TIME
   ptr384->mUpdateTime = aSimTime;
   //*return memory of temporary 387 block
   //          CALL DATGIV(JNR,GRL,-387,0,0, TMP387 )
   //*adjust assignment if correlation changes
   //          IF(ASG384.GT.0.AND.PTR384.NE.ASG384)THEN
   //             JNR(PTR384+4)=JNR(ASG384+4)
   //             JNR(PTR384+5)=JNR(ASG384+5)
   //             JNR(ASG384+4)=0
   //          ENDIF
   //*end of correlation result.
   //       ENDIF
   //
   //       RETURN
   //       END
   //
}

//       SUBROUTINE MTTUAT(JNR,GRL,TIME,PTR017,PTR388,PTR384)
//! Multiple target tracking - update an active track.
void MTT::mttuat(double                      aSimTime,
                 const MTT_PerceivedCluster& aNonLocalTrack, // ptr017
                 MTT_Track&                  aActiveTrack)                    // ptr384 (as active track)
{
   //*measurement is associated with an active track

   //*fred kiefer;
   //*bogdon associates inc;
   //*july 2, 2005;

   // parameter references
   const MTT_PerceivedCluster* ptr017 = &aNonLocalTrack; // ptr017
   MTT_Track*                  ptr384 = &aActiveTrack;   // ptr384 (as active track)

   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*),TIME,TRKTIM
   //       INTEGER I,J,PTR017,PTR359,PTR388,PTR384,PTR387,PTR360,IAVAIL
   //       INTEGER IEL,IRNG,LMASTR
   //       DOUBLE PRECISION SRANGE,EL,SIGRNG,SIGEL,RDRZ,DZ,VARZ
   //       DOUBLE PRECISION DT,PZ0LF,PZ0TF,XLF(4),PLF(4,4),XTF(6),PTF(6,6)
   //       DOUBLE PRECISION W(2),RINVV(2,2),C,MTP(2,2),MULF0,MUTF0
   //       DOUBLE PRECISION PLF6(6,6),XLF6(6),PCMBLF(6,6),PCMBTF(6,6)
   //       DOUBLE PRECISION D2LF(6,6),D2TF(6,6),DELXLF(6),DELXTF(6)
   //
   //       REAL RAT(184)
   //       DOUBLE PRECISION AT(92)
   //       DOUBLE PRECISION XHORZ(6),PHORZ(6,6),PIVERT(2,2),XIVERT(2)
   //       DOUBLE PRECISION MTPLF,MTPTF,XHORZP(6),PHORZP(6,6)
   //
   //       EQUIVALENCE (RAT(1),AT(1))
   //       EQUIVALENCE (PHORZ,AT(1)),(XHORZ,AT(37))
   //       EQUIVALENCE (PIVERT,AT(43)),(XIVERT,AT(47))
   //       EQUIVALENCE (MTPLF,AT(49)),(MTPTF,AT(50))
   //       EQUIVALENCE (XHORZP,AT(51)),(PHORZP,AT(57))
   //
   MTT_ActiveTrack::Data at;
   UtMatrixd&            phorz  = at.mHorizontalInfoMatrix;
   UtMatrixd&            xhorz  = at.mFilterStates;
   UtMatrixd&            pivert = at.mVerticalInfoMatrix;
   UtMatrixd&            xivert = at.mVerticalState;
   double&               mtplf  = at.mStraightFlightModeProbability;
   double&               mtptf  = at.mTurningFlightModeProbability;
   UtMatrixd&            xhorzp = at.mPreviousFilterStates;
   UtMatrixd&            phorzp = at.mPreviousHorizontalMatrix;

   //       COMMON /TRUST/ TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,
   //     +         TRUSPD,IDTRGT,IDSNDR,IDRCVR,PLTRGT,PLSNDR,PLRCVR,GAMTIM
   //       REAL TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,TRUSPD
   //       INTEGER IDTRGT,IDSNDR,IDRCVR
   //       CHARACTER*20 PLTRGT,PLSNDR,PLRCVR
   //       REAL GAMTIM
   //
   //       //*load the double precision active track data from GRL (do not load prev)
   //       LMASTR=JNR(2)-JNR(1)
   //       PTR387=JNR(PTR384+3)
   MTT_ActiveTrack::Data* ptr387 = dynamic_cast<MTT_ActiveTrack::Data*>(ptr384->mDataPtr);
   //       DO I=1,100
   //          RAT(I)=GRL(PTR387+I-1)
   //       ENDDO
   at = *ptr387;
   //
   //       //*load previous state and covariance matrix for dkf
   //       DO I=1,6
   //          XHORZP(I)=XHORZ(I)
   //          DO J=1,6
   //             PHORZP(I,J)=PHORZ(I,J)
   //          ENDDO
   //       ENDDO
   //
   xhorzp = xhorz;
   phorzp = phorz;
   //       //*time of measurement
   //       PTR359=JNR(PTR017+39)
   MTT_Measurement* ptr359 = ptr017->mMeasurementPtr;
   //       TRKTIM=GRL(PTR384+1)
   double trktim = ptr384->mUpdateTime;
   //       DT=DBLE(TIME-TRKTIM)
   double dt = aSimTime - trktim;
   //       //*update previous state and covariance to current time
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,PHORZP,XHORZP)
   mtttfp(dt, phorzp, xhorzp);
   //       //*first we update the straight-flight (non turning) model
   //       DO I=1,4
   //          XLF(I)=XHORZ(I)
   //          DO J=1,4
   //             PLF(I,J)=PHORZ(I,J)
   //          END DO
   //       END DO
   UtMatrixd xlf(4);
   xlf.CopyFrom(xhorz, 0, 0, 4, 1);
   UtMatrixd plf(4, 4);
   plf.CopyFrom(phorz, 0, 0, 4, 4);
   //       CALL MTTLFM(JNR,GRL,XLF,PLF,PIVERT,XIVERT,PTR384,PTR388,PTR359,
   //     + DT,PZ0LF)
   double pzolf = 0.0;
   mttlfm(xlf, plf, pivert, xivert, *ptr384, *ptr359, dt, pzolf);
   //       //*next we apply the gate test to the turning flight process
   //       //*model of the kalman filter
   //       DO I=1,6
   //          XTF(I)=XHORZ(I)
   //          DO J=1,6
   //             PTF(I,J)=PHORZ(I,J)
   //          END DO
   //       END DO
   UtMatrixd xtf(xhorz);
   UtMatrixd ptf(phorz);
   //       CALL MTTTFM(JNR,GRL,XTF,PTF,PIVERT,XIVERT,PTR384,PTR388,PTR359,
   //     + DT,PZ0TF)
   double pzotf = 0.0;
   mtttfm(xtf, ptf, pivert, xivert, *ptr384, *ptr359, dt, pzotf);
   //
   //       //*extract mode transition probabilities
   //       MTP(1,1)=DBLE(GRL(PTR388+28))
   UtMatrixd mtp(2, 2);
   mtp(0, 0) = mParameters.mStraightToStraightFlight;
   //       MTP(1,2)=DBLE(GRL(PTR388+29))
   mtp(0, 1) = mParameters.mStraightToTurningFlight;
   //       MTP(2,1)=DBLE(GRL(PTR388+30))
   mtp(1, 0) = mParameters.mTurningToTurningFlight; ///!!! (mjm) check this (todo)
   //       MTP(2,2)=DBLE(GRL(PTR388+31))
   mtp(1, 1) = mParameters.mTurningToStraightFlight;
   //       MULF0=MTPLF
   double mulfo = mtplf;
   //       MUTF0=MTPTF
   double mutfo = mtptf;
   //       //*bayesian estimator of the first order alogrithm
   //       C = PZ0LF*(MTP(1,1)*MULF0+MTP(2,1)*MUTF0) +
   //     +     PZ0TF*(MTP(1,2)*MULF0+MTP(2,2)*MUTF0)
   double c = pzolf * (mtp(0, 0) * mulfo + mtp(1, 0) * mutfo) + pzotf * (mtp(0, 1) * mulfo + mtp(1, 1) * mutfo);
   //       //*update model mode weights
   //       MTPLF=(PZ0LF/C)*(MTP(1,1)*MULF0+MTP(2,1)*MUTF0)
   mtplf = (pzolf / c) * (mtp(0, 0) * mulfo + mtp(1, 0) * mutfo);
   //       MTPTF=(PZ0TF/C)*(MTP(1,2)*MULF0+MTP(2,2)*MUTF0)
   mtptf = (pzotf / c) * (mtp(0, 1) * mulfo + mtp(1, 1) * mutfo);
   //       //*update state estimates
   //       CALL MTXZRO(XLF6,6,1)
   UtMatrixd xlf6(6);
   //       CALL MTXZRO(PLF6,6,6)
   UtMatrixd plf6(6, 6);
   //       DO I=1,4
   //          XLF6(I)=XLF(I)
   //          DO J=1,4
   //             PLF6(I,J)=PLF(I,J)
   //          ENDDO
   //       ENDDO
   xlf6.CopyFrom(xlf, 0, 0, 4, 1);
   plf6.CopyFrom(plf, 0, 0, 4, 4);
   //       DO I=1,6
   //          XHORZ(I)=MTPLF*XLF6(I)+MTPTF*XTF(I)
   //       ENDDO
   xhorz = mtplf * xlf6 + mtptf * xtf;
   //(mjm) the following were comments in the original fortran code.
   //       //*mulf0=ltrack(trackno).mtplf(end);
   //       //*mutf0=ltrack(trackno).mtptf(end);
   //       //*//=pz0_lf*(p(1,1)*mulf0+p(2,1)*mutf0) +
   //       //*pz0_tf*(p(1,2)*mulf0+p(2,2)*mutf0);
   //       //*update the mode transition probabilites
   //       //*mulf=(pz0_lf///)*(p(1,1)*mulf0+p(2,1)*mutf0);
   //       //*mutf=(pz0_tf///)*(p(1,2)*mulf0+p(2,2)*mutf0);
   //       //*update state estimate
   //       //*xhatlf=[xhatlf;0;0];
   //       //*Plf=[Plf zeros(4,2);zeros(2,6)];
   //       //*xhat=mulf*xhatlf+mutf*xhattf;
   //       //*P=mulf*(Plf+(xhat-xhatlf)*(xhat-xhatlf)')+
   //       //*mutf*(Ptf+(xhat-xhattf)*(xhat-xhattf)');
   //(mjm) end original comments.
   //
   //       CALL MTXSUB(XHORZ,XLF6,6,1,DELXLF)
   UtMatrixd delxlf(6);
   delxlf.Subtract(xhorz, xlf6);
   //       CALL MTXMUL(DELXLF,1,6,DELXLF,6,1,D2LF)
   // (mjm) This appears to be a multiply by transpose,
   //       and this call works because it's a 1-d matrix.
   // todo Check updates; this is possibly an error.
   UtMatrixd d2lf(1);
   UtMatrixd delxlft(1, 6);
   delxlft.Transpose(delxlf);
   d2lf.Multiply(delxlft, delxlf);
   //       CALL MTXSUB(XHORZ,XTF,6,1,DELXTF)
   UtMatrixd delxtf(6);
   delxtf.Subtract(xhorz, xtf);
   //       CALL MTXMUL(DELXTF,1,6,DELXTF,6,1,D2TF)
   UtMatrixd d2tf(1);
   UtMatrixd delxtft(1, 6);
   delxtft.Transpose(delxtf);
   d2tf.Multiply(delxtft, delxtf);
   //       CALL MTXADD(PLF6,D2LF,6,6,PCMBLF)
   UtMatrixd pcmblf(plf6);
   pcmblf(0, 0) += d2lf(0);
   //       CALL MTXADD(PTF,D2TF,6,6,PCMBTF)
   UtMatrixd pcmbtf(ptf);
   pcmbtf(0, 0) += d2tf(0);
   //       DO I=1,6
   //          DO J=1,6
   //             PHORZ(I,J)=MTPLF*PCMBLF(I,J)+MTPTF*PCMBTF(I,J)
   //          ENDDO
   //       ENDDO
   phorz = mtplf * pcmblf + mtptf * pcmbtf;
   //
   ////==================================================== .dkf output ======
   //       IF(MOD(JNR(LMASTR+30)/128,2).EQ.1)
   //     + WRITE(68,1112)TIME,DT,IDTRGT,IDSNDR,IDRCVR,
   //     + TRUEX,TRUEY,TRUEZ,TRUVH,TRUHDG,TRUVZ,XHORZP,PHORZP,XHORZ,PHORZ
   // 1112  FORMAT(2E15.7,3I10,6E15.7,6E15.7,36E15.7,6E15.7,36E15.7)
   ////==================================================== .dkf output ======
   //
   //       //*update the vertical candidate information filter
   //       W(1)=DBLE(GRL(PTR388+12))
   UtMatrixd w(2);
   w(0) = mParameters.mVerticalVelocity;
   //       W(2)=DBLE(GRL(PTR388+13))
   w(1) = mParameters.mVerticalVelocityDecorrelationTime;
   //       //*extract necessary radar measurement data
   //       PTR360=JNR(PTR359+4)
   MTT_RadarMeasurement::Data* ptr360 = &(dynamic_cast<MTT_RadarMeasurement*>(ptr359)->GetData());
   //       SRANGE=DBLE(GRL(PTR360))
   double srange = ptr360->mRange;
   //       EL=DBLE(GRL(PTR360+2))
   double el = ptr360->mElevation;
   //       SIGRNG=DBLE(GRL(PTR360+4))
   double sigrng = ptr360->mRangeStDev;
   //       SIGEL=DBLE(GRL(PTR360+6))
   double sigel = ptr360->mElevationStDev;
   //       RDRZ=DBLE(GRL(PTR360+10))
   double rdrz = ptr360->mSensorLocation[2];
   //       //*determine if elevation is available and adjust for ground range
   //       IAVAIL=JNR(PTR359+6)
   unsigned iavail = ptr359->mDataAvailable;
   //       IEL=MOD(IAVAIL/4,2)
   unsigned iel = (iavail / 4) % 2;
   //       IRNG=MOD(JNR(PTR359+6),2)
   unsigned irng = (ptr359->mDataAvailable) % 2;
   //       //*vertical channel mesurement data
   //       CALL MTXZRO(RINVV,2,2)
   UtMatrixd rinvv(2, 2);
   //       DZ=0
   double dz = 0.0;
   //       IF ((IEL.EQ.1) .AND. (IRNG.EQ.1)) THEN
   if ((iel == 1) && (irng == 1))
   {
      //             //*measurement error covariance matrix transformed to enu z-axis
      //             VARZ=DSIN(EL)*DSIN(EL)*SIGRNG*SIGRNG
      double varz = sin(el) * sin(el) * sigrng * sigrng;
      //             VARZ=VARZ+SRANGE*SRANGE*SIGEL*SIGEL
      varz += srange * srange * sigel * sigel;
      //             RINVV(1,1)=1.0/VARZ
      rinvv(0, 0) = 1.0 / varz;
      //             //*measurement transformed to enu z-axis
      //             DZ=SRANGE*DSIN(EL)
      dz = srange * sin(el);
      //             //*update vertical measurement time
      //             GRL(PTR384+7)=TIME
      ptr384->mVerticalChannelUpdateTime = aSimTime;
      //       ENDIF
   }
   //       CALL MTTUVI(DT,W,PIVERT,XIVERT,RINVV,RDRZ,DZ,IRNG,IEL,3)
   mttuvi(dt, w, pivert, xivert, rinvv, rdrz, dz, irng, iel, 3);
   //       CALL MTTPVF(JNR,GRL,PIVERT,PTR388,PTR384)
   mttpvf(pivert, *ptr384);
   //
   //       //*update track measurement time
   //       GRL(PTR384+1)=TIME
   ptr384->mUpdateTime = aSimTime;
   //
   //       //*load the double precision array into the 387 block
   //       DO I=1,184
   //          GRL(PTR387+I-1)=RAT(I)
   //       ENDDO
   //
   *ptr387 = at;
   //       RETURN
   //       END
   //
   //
}

//       SUBROUTINE MTTUHF(JNR,GRL,TIME,PTR017,PTR388,PTR383,PTR384)
//! Multiple target tracking - update height filter.
void MTT::mttuhf(double aSimTime, const MTT_PerceivedCluster& aMeasurement, MTT_ActiveTrack*& aTrackPtr)
{
   //     *measurement is associated with height filter of an active track

   //     *fred kiefer;
   //     *bogdon associates inc;
   //     *july 7, 2005;

   const MTT_PerceivedCluster* ptr017 = &aMeasurement;
   MTT_ActiveTrack*&           ptr384 = aTrackPtr;

   //       IMPLICIT NONE
   //
   //       INTEGER JNR(*)
   //       REAL GRL(*),TIME,TRKTIM
   //       INTEGER I,PTR017,PTR359,PTR388,PTR383
   //       INTEGER PTR384,LST384,TRCKID,PTR387,PTR360,IAVAIL
   //       INTEGER IEL,IRNG
   //       DOUBLE PRECISION SRANGE,EL,SIGRNG,SIGEL,RDRZ,DZ
   //       DOUBLE PRECISION DT
   //       DOUBLE PRECISION W(2),RINVV(2,2),VARZ
   //
   //       REAL RAT(184)
   //       DOUBLE PRECISION AT(92)
   //       DOUBLE PRECISION XHORZ(6),PHORZ(6,6),PIVERT(2,2),XIVERT(2)
   //       DOUBLE PRECISION MTPLF,MTPTF,XHORZP(6),PHORZP(6,6)
   //
   //       EQUIVALENCE (RAT(1),AT(1))
   //       EQUIVALENCE (PHORZ,AT(1)),(XHORZ,AT(37))
   //       EQUIVALENCE (PIVERT,AT(43)),(XIVERT,AT(47))
   //       EQUIVALENCE (MTPLF,AT(49)),(MTPTF,AT(50))
   //       EQUIVALENCE (XHORZP,AT(51)),(PHORZP,AT(57))
   //
   MTT_ActiveTrack::Data at;
   UtMatrixd&            phorz  = at.mHorizontalInfoMatrix;
   UtMatrixd&            xhorz  = at.mFilterStates;
   UtMatrixd&            pivert = at.mVerticalInfoMatrix;
   UtMatrixd&            xivert = at.mVerticalState;
   UtMatrixd&            xhorzp = at.mPreviousFilterStates;
   UtMatrixd&            phorzp = at.mPreviousHorizontalMatrix;

   //*measurement from height finder is associated with a particular track
   //       TRCKID=JNR(PTR017+41)
   int trckid = ptr017->mTrackNumber;
   //*loop over linked list to find the appropriate filter for this track
   //       PTR384=JNR(PTR383+5)
   //       LST384=PTR383+5
   MTT_Tracks::MTT_ActiveTrackList& lst384 = mTracks.mActiveTrackList;
   ptr384                                  = lst384.Find(trckid);
   //       DO WHILE (PTR384.NE.0)
   //*determine if this is the track
   //          IF (TRCKID .EQ. JNR(PTR384+2)) GO TO 100
   //          LST384=PTR384
   //          PTR384=JNR(LST384)
   //       END DO
   //
   // (mjm) Don't use goto.
   if (ptr384 == nullptr)
   {
      //*the desired track could not be found, so exit with no updates
      //          PTR384=0
      //          JNR(PTR017+41)=0
      //          GOTO 200
      //* clear track id to prevent new perception in update;
      const_cast<MTT_PerceivedCluster*>(ptr017)->mTrackNumber = 0;
      return;
   }
   //*load the double precision active track data from GRL
   // 100    PTR387=JNR(PTR384+3)
   MTT_ActiveTrack::Data* ptr387 = &(dynamic_cast<MTT_ActiveTrack*>(ptr384)->GetData());
   //       DO I=1,184
   //          RAT(I)=GRL(PTR387+I-1)
   //       ENDDO
   //
   at = *ptr387;
   //*time of measurement
   //       PTR359=JNR(PTR017+39)
   MTT_Measurement* ptr359 = ptr017->mMeasurementPtr;
   //       TRKTIM=GRL(PTR384+1)
   double trktim = ptr384->mUpdateTime;
   //       DT=DBLE(TIME-TRKTIM)
   double dt = aSimTime - trktim;
   //*update previous state and covariance to current time
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,PHORZP,XHORZP)
   mtttfp(dt, phorzp, xhorzp);
   //*reset the filter if too much time has elapsed since last
   //*time a height measurement was received
   //       IF (DT.GT.DBLE(GRL(PTR388+32))) THEN
   if (dt > mParameters.mVerticalChannelDropTime)
   {
      //          CALL MTXZRO(PIVERT,2,2)
      pivert.Fill(0.0);
      //          CALL MTXZRO(XIVERT,2,1)
      xivert.Fill(0.0);
      //       ENDIF
   }
   //
   //*propagate horizontal states filter in time
   //       CALL MTTTFP(JNR,GRL,PTR388,DT,PHORZ,XHORZ)
   mtttfp(dt, phorz, xhorz);
   //
   //*update the vertical information filter
   //       W(1)=DBLE(GRL(PTR388+12))
   UtMatrixd w(2);
   w(0) = mParameters.mVerticalVelocity;
   //       W(2)=DBLE(GRL(PTR388+13))
   w(1) = mParameters.mVerticalVelocityDecorrelationTime;
   //*extract necessary radar measurement data
   //       PTR360=JNR(PTR359+4)
   MTT_RadarMeasurement::Data* ptr360 = &(dynamic_cast<MTT_RadarMeasurement*>(ptr359)->GetData());
   //       SRANGE=DBLE(GRL(PTR360))
   double srange = ptr360->mRange;
   //       EL=DBLE(GRL(PTR360+2))
   double el = ptr360->mElevation;
   //       SIGRNG=DBLE(GRL(PTR360+4))
   double sigrng = ptr360->mRangeStDev;
   //       SIGEL=DBLE(GRL(PTR360+6))
   double sigel = ptr360->mElevationStDev;
   //       RDRZ=DBLE(GRL(PTR360+10))
   double rdrz = ptr360->mSensorLocation[2];
   //*determine if elevation is available and adjust for ground range
   //       IAVAIL=JNR(PTR359+6)
   unsigned iavail = ptr359->mDataAvailable;
   //       IEL=MOD(IAVAIL/4,2)
   unsigned iel = (iavail / 4) % 2;
   //       IRNG=MOD(JNR(PTR359+6),2)
   unsigned irng = (ptr359->mDataAvailable) % 2;
   //*vertical channel mesurement data
   //       CALL MTXZRO(RINVV,2,2)
   UtMatrixd rinvv(2, 2);
   //       DZ=0.0
   double dz = 0.0;
   //       IF ((IEL.EQ.1) .AND. (IRNG.EQ.1)) THEN
   if ((iel == 1) && (irng == 1))
   {
      //*measurement error covariance matrix transformed to enu z-axis
      //             VARZ=DSIN(EL)*DSIN(EL)*SIGRNG*SIGRNG
      double varz = sin(el) * sin(el) * sigrng * sigrng;
      //             VARZ=VARZ+SRANGE*SRANGE*SIGEL*SIGEL
      varz += srange * srange * sigel * sigel;
      //             RINVV(1,1)=1.0/VARZ
      rinvv(0, 0) = 1.0 / varz;
      //*measurement transformed to enu z-axis
      //             DZ=SRANGE*DSIN(EL)
      dz = srange * sin(el);
      //*update vertical measurement time
      //             GRL(PTR384+7)=TIME
      ptr384->mVerticalChannelUpdateTime = aSimTime;
      //       ENDIF
   }
   //       CALL MTTUVI(DT,W,PIVERT,XIVERT,RINVV,RDRZ,DZ,IRNG,IEL,3)
   mttuvi(dt, w, pivert, xivert, rinvv, rdrz, dz, irng, iel, 3);
   //       CALL MTTPVF(JNR,GRL,PIVERT,PTR388,PTR384)
   mttpvf(pivert, *ptr384);
   //
   //*update track measurement time
   //       GRL(PTR384+1)=TIME
   ptr384->mUpdateTime = aSimTime;
   //
   //*load the double precision array into the 387 block
   //       DO I=1,184
   //          GRL(PTR387+I-1)=RAT(I)
   //       ENDDO
   //
   *ptr387 = at;
   //  200  RETURN
   //       END
}

//       SUBROUTINE MTTUHI(DT,W,PI,XI,H,HT,R,RDRX,RDRY,DX,DY,TRKTYP)
//! Multiple target tracking - update information filter.
void MTT::mttuhi(double           aDt,
                 const UtMatrixd& aHorizontalAcceleration,    // 2x1
                 UtMatrixd&       aHorizontalInfoMatrix,      // 4x4
                 UtMatrixd&       aHorizontalState,           // 4x1
                 const UtMatrixd& aConnectionMatrix,          // 2x4
                 const UtMatrixd& aConnectionMatrixTranspose, // 4x2
                 const UtMatrixd& aDxDyCovariance,            // 2x2
                 double           aSensorLocationX,
                 double           aSensorLocationY,
                 double           aDx,
                 double           aDy,
                 int              aTrackType)
{
   //     *update horizontal channel when in information form of filter
   //       IMPLICIT NONE
   //       SUBROUTINE MTTUHI(DT,W,PI,XI,H,HT,R,RDRX,RDRY,DX,DY,TRKTYP)
   const UtMatrixd& w    = aHorizontalAcceleration;    // 2x1
   UtMatrixd&       pi   = aHorizontalInfoMatrix;      // 4x4
   UtMatrixd&       xi   = aHorizontalState;           // 4x1
   const UtMatrixd& h    = aConnectionMatrix;          // 2x4
   const UtMatrixd& ht   = aConnectionMatrixTranspose; // 4x2
   const UtMatrixd& r    = aDxDyCovariance;            // 2x2
   double&          rdrx = aSensorLocationX;
   double&          rdry = aSensorLocationY;
   // (var. not used) int&              trktyp = aTrackType;
   //       INTEGER TRKTYP
   //       DOUBLE PRECISION H(2,4),HT(4,2),R(2,2),RDRX,RDRY,DX,DY
   //       DOUBLE PRECISION DT,W(2),PI(4,4),XI(4)
   //       DOUBLE PRECISION XIUPD(4),PIUPD(4,4),PDET(2)
   //       DOUBLE PRECISION PHINV(4,4),PHINVT(4,4),Q(4,4),QINV(4,4)
   //       DOUBLE PRECISION M(4,4),AB(4,4),N(4,4),MQI(4,4)
   //       DOUBLE PRECISION DUM1(100),DUM2(100),DUM3(100),EYE4(4,4)
   //       DOUBLE PRECISION TMP4X4(4,4),TMQ4X4(4,4),TMP4X1(4),TMP4X2(4,2)
   //       LOGICAL ERR
   //
   UtMatrixd tmp4x4(4, 4);
   UtMatrixd tmp4x1(xi);

   //*propagate filter forward in time
   //       IF (DT .GT. 0) THEN
   if (aDt > 0.0)
   {
      //          CALL IDENTY(4,PHINV)
      UtMatrixd phinv(4, 4);
      phinv.LoadIdentity();
      //          PHINV(1,3)=-DT
      phinv(0, 2) = -aDt;
      //          PHINV(2,4)=-DT
      phinv(1, 3) = -aDt;
      //          CALL IDENTY(4,PHINVT)
      UtMatrixd phinvt(4, 4);
      phinvt.LoadIdentity();
      //          PHINVT(3,1)=-DT
      phinvt(2, 0) = -aDt;
      //          PHINVT(4,2)=-DT
      phinvt(3, 1) = -aDt;
      //*for calculation of Q,see my notes of march 28, 2004
      //          CALL MTXZRO(Q,4,4)
      UtMatrixd q(4, 4);
      //          Q(1,1)=DT*DT*DT*W(1)/3
      q(0, 0) = aDt * aDt * aDt * w(0) / 3.0;
      //          Q(1,3)=DT*DT*W(1)/2
      q(0, 2) = aDt * aDt * w(0) / 2.0;
      //          Q(2,2)=DT*DT*DT*W(2)/3
      q(1, 1) = aDt * aDt * aDt * w(1) / 3.0;
      //          Q(2,4)=DT*DT*W(2)/2
      q(1, 3) = aDt * aDt * w(1) / 2.0;
      //          Q(3,1)=DT*DT*W(1)/2
      q(2, 0) = aDt * aDt * w(0) / 2.0;
      //          Q(3,3)=DT*W(1)
      q(2, 2) = aDt * w(0);
      //          Q(4,2)=DT*DT*W(2)/2
      q(3, 1) = aDt * aDt * w(1) / 2.0;
      //          Q(4,4)=DT*W(2)
      q(3, 3) = aDt * w(1);
      //          CALL MTXINV(Q,4,QINV,TMP4X4,ERR)
      UtMatrixd qinv(q);
      try
      {
         qinv.Invert();
      }
      catch (UtMatrixd::SingularMatrixError&)
      {
         //          IF (ERR) GO TO 100
         return;
      }
      //          CALL FIXSYM(QINV,4,1,TRKTYP)
      qinv.Symmetrize();
      //*m = ai'*ctk.pi*ai;;
      //          CALL MTXMUL(PHINVT,4,4,PI,4,4,AB)
      UtMatrixd ab(4, 4);
      ab.Multiply(phinvt, pi);
      //          CALL MTXMUL(AB,4,4,PHINV,4,4,M)
      UtMatrixd m(4, 4);
      m.Multiply(ab, phinv);
      //          CALL FIXSYM(M,4,1,TRKTYP)
      m.Symmetrize();
      //*n = m/(m+qi);;
      //          CALL MTXADD(M,QINV,4,4,MQI)
      UtMatrixd mqi(4, 4);
      mqi.Add(m, qinv);
      //          CALL MTXDIV(M,MQI,4,4,DUM1,DUM2,DUM3,N)
      UtMatrixd n(4, 4);
      n.Divide(m, mqi);
      //*pi = (eye(4)-n)*m;;
      //          CALL IDENTY(4,EYE4)
      UtMatrixd eye4(4, 4);
      eye4.LoadIdentity();
      //          CALL MTXSUB(EYE4,N,4,4,TMP4X4)
      tmp4x4.Subtract(eye4, n);
      //          CALL MTXMUL(TMP4X4,4,4,M,4,4,PI)
      pi.Multiply(tmp4x4, m);
      //*roundoff error can corrupt symmetry of pihorz, so symmetrize;
      //          CALL FIXSYM(PI,4,1,TRKTYP)
      pi.Symmetrize();
      //*pix = (eye(4)-n)*ai'*pix;;
      //          CALL MTXMUL(TMP4X4,4,4,PHINVT,4,4,TMQ4X4)
      UtMatrixd tmq4x4(4, 4);
      tmq4x4.Multiply(tmp4x4, phinvt);
      //          CALL MTXSET(XI,4,1,TMP4X1)
      tmp4x1 = xi;
      //          CALL MTXMUL(TMQ4X4,4,4,TMP4X1,4,1,XI)
      xi.Multiply(tmq4x4, tmp4x1);
      //       END IF
   }
   //
   //*add in measurment covariance to update information matrix
   //*pi   = (h'/r)*h = (r\h)'*h;
   //*(4x4) = ((4x2)/(2x2))*(2x4) = ((2x2)\(2x4))'*(2x4);
   //       CALL MTXDIV(HT,R,4,2,DUM1,DUM2,DUM3,TMP4X2)
   UtMatrixd tmp4x2(4, 2);
   tmp4x2.Divide(ht, r);
   //       CALL MTXMUL(TMP4X2,4,2,H,2,4,PIUPD)
   UtMatrixd piupd(4, 4);
   piupd.Multiply(tmp4x2, h);
   //*roundoff error can corrupt symmetry of pi, so symmetrize;
   //       CALL FIXSYM(PIUPD,4,1,TRKTYP)
   piupd.Symmetrize();
   //       CALL MTXSET(PI,4,4,TMP4X4)
   tmp4x4 = pi;
   //*pi = pi + piUPD;
   //       CALL MTXADD(TMP4X4,PIUPD,4,4,PI)
   pi.Add(tmp4x4, piupd);
   //       CALL FIXSYM(PI,4,1,TRKTYP)
   pi.Symmetrize();
   //*add in measurement to update information state vector
   //       PDET(1)=RDRX+DX
   UtMatrixd pdet(2);
   pdet(0) = rdrx + aDx;
   //       PDET(2)=RDRY+DY
   pdet(1) = rdry + aDy;
   //*xi = xi + xiUPD (mp4x2 = (h'/r);
   //       CALL MTXMUL(TMP4X2,4,2,PDET,2,1,XIUPD)
   UtMatrixd xiupd(4);
   xiupd.Multiply(tmp4x2, pdet);
   //       CALL MTXSET(XI,4,1,TMP4X1)
   tmp4x1 = xi;
   //       CALL MTXADD(TMP4X1,XIUPD,4,1,XI)
   xi.Add(tmp4x1, xiupd);
   //
   //  100  CONTINUE
   //
   //       RETURN
   //       END
   //
}

#include <limits>

//       SUBROUTINE MTTUVI(DT,W,PI,XI,RINV,RDRZ,DZ,IRNG,IEL,TRKTYP)
//! Multiple target tracking - update vertical info filter.
void MTT::mttuvi(double           aDt,
                 const UtMatrixd& aVerticalInfoFilter,          // 2x1, W
                 UtMatrixd&       aVerticalInfoMatrix,          // 2x2, PI
                 UtMatrixd&       aVerticalState,               // 2x1, XI
                 const UtMatrixd& aVerticalCandidateInfoFilter, // 2x2, RINV
                 double           aSensorLocationZ,             // RDRZ
                 double           aTargetLocationZ,             // DZ
                 unsigned         aRangeValid,                  // IRNG
                 unsigned         aElevationValid,              // IEL
                 int              aTrackType)                                // TRKTYP
{
   //     *update vertical channel to current time
   //       IMPLICIT NONE
   //
   //       INTEGER IRNG,IEL,TRKTYP
   //       DOUBLE PRECISION DT,W(2),PI(2,2),XI(2),RINV(2,2),RDRZ,DZ
   //       DOUBLE PRECISION PIUPD(2,2),PDET(2)
   //       DOUBLE PRECISION XIUPD(2)
   //       DOUBLE PRECISION TMP2X2(2,2)
   //       DOUBLE PRECISION TMP2X1(2)
   //
   ////     *if active, propagate vertical filter to current time
   //       IF ((XI(1).NE.0.0) .OR. (XI(2).NE.0.0))
   if ((aVerticalState(0) != 0.0) || (aVerticalState(1) != 0.0))
   {
      //     +  CALL MTTUVP(DT,W,PI,XI,TRKTYP)
      mttuvp(aDt, aVerticalInfoFilter, aVerticalInfoMatrix, aVerticalState, aTrackType);
   }
   //
   ////     *range and elevation to update information filter of vertical channel;
   //       IF ((IEL.EQ.1) .AND. (IRNG.EQ.1)) THEN
   if ((aRangeValid == 1) && (aElevationValid == 1))
   {
      ////        *measurement error covariance matrix;
      //          CALL MTXSET(RINV,2,2,PIUPD)
      UtMatrixd piupd(aVerticalCandidateInfoFilter);
      //          CALL MTXSET(PI,2,2,TMP2X2)
      UtMatrixd tmp2x2(aVerticalInfoMatrix);
      //          CALL MTXADD(TMP2X2,PIUPD,2,2,PI)
      UtMatrixd pi(2, 2);
      aVerticalInfoMatrix.Add(tmp2x2, piupd);
      //          CALL FIXSYM(PI,2,1,TRKTYP)
      aVerticalInfoMatrix.Symmetrize();
      assert(aVerticalInfoMatrix(0, 0) != std::numeric_limits<double>::infinity());
      UtMatrixd pdet(2);
      //          PDET(1)=RDRZ+DZ

      pdet(0) = aSensorLocationZ + aTargetLocationZ;
      //          PDET(2)=0.0
      pdet(1) = 0.0;
      //          CALL MTXMUL(RINV,2,2,PDET,2,1,XIUPD)
      UtMatrixd xiupd(2);
      xiupd.Multiply(aVerticalCandidateInfoFilter, pdet);
      //          CALL MTXSET(XI,2,1,TMP2X1)
      UtMatrixd tmp2x1(aVerticalState);
      //          CALL MTXADD(TMP2X1,XIUPD,2,1,XI)
      aVerticalState.Add(tmp2x1, xiupd);
      //       ENDIF
   }
   //
   //       RETURN
   //       END
   //
}

//       SUBROUTINE MTTUVP(DT,W,PI,XI,TRKTYP)
//! Multiple target tracking - update vertical info filter.
void MTT::mttuvp(double           aDt,
                 const UtMatrixd& aVerticalInfoFilter, // 2x1, W
                 UtMatrixd&       aVerticalInfoMatrix, // 2x2, PI
                 UtMatrixd&       aVerticalState,      // 2x1, XI
                 int              aTrackType)                       // TRKTYP
{
   //     *update vertical channel to current time
   //       IMPLICIT NONE
   //
   //       INTEGER IDUM(100),TRKTYP
   //       DOUBLE PRECISION DT,W(2),PI(2,2),XI(2)
   //       DOUBLE PRECISION VVTAU,VAVAR,VVTAU2
   //       DOUBLE PRECISION E1,E2,PHIH(2,2),QIH(2,2),MQI(2,2)
   //       DOUBLE PRECISION PHIHTP(2,2),QIHINV(2,2)
   //       DOUBLE PRECISION TMP2X2(2,2),M(2,2),N(2,2)
   //       DOUBLE PRECISION DUM1(20),DUM2(20),DUM3(20),DUM4(20),DUM5(20)
   //       DOUBLE PRECISION EYE2(2,2),TMP2X1(2),TMQ2X2(2,2)
   //       LOGICAL ERR
   //
   //       IF (DT .GT. 0.0) THEN
   if (aDt > 0.0)
   {
      //          VAVAR=W(1)
      double vavar = aVerticalInfoFilter(0);
      //          VVTAU=W(2)
      double vvtau = aVerticalInfoFilter(1);
      //          E1=DEXP(-DT/VVTAU)
      double e1 = exp(-aDt / vvtau);
      //          E2=DEXP(-2.0*DT/VVTAU)
      double e2 = exp(-2.0 * aDt / vvtau);
      //          VVTAU2=VVTAU*VVTAU
      double vvtau2 = vvtau * vvtau;
      //          PHIH(1,1)=1
      UtMatrixd phih(2, 2);
      phih(0, 0) = 1.0;
      //          PHIH(1,2)=VVTAU*(1.0-E1)
      phih(0, 1) = vvtau * (1.0 - e1);
      //          PHIH(2,1)=0.0
      phih(1, 0) = 0.0;
      //          PHIH(2,2)=E1
      phih(1, 1) = e1;
      //          QIH(1,1)=VAVAR*VVTAU2*(DT+2*VVTAU*(E1-1)-VVTAU*(E2-1)/2)
      UtMatrixd qih(2, 2);
      qih(0, 0) = vavar * vvtau2 * (aDt + 2.0 * vvtau * (e1 - 1.0) - vvtau * (e2 - 1.0) / 2.0);
      //          QIH(1,2)=VAVAR*VVTAU2*((E2-1)/2-(E1-1))
      qih(0, 1) = vavar * vvtau2 * ((e2 - 1.0) / 2.0 - (e1 - 1.0));
      //          QIH(2,1)=QIH(1,2)
      qih(1, 0) = qih(0, 1);
      //          QIH(2,2)=VAVAR*VVTAU*(1-E2)/2
      qih(1, 1) = vavar * vvtau * (1 - e2) / 2.0;
      //          CALL MTXINV(QIH,2,QIHINV,TMP2X2,ERR)
      UtMatrixd qihinv(qih);
      try
      {
         qihinv.Invert();
      }
      //          IF (ERR) GO TO 100
      catch (UtMatrixd::SingularMatrixError&)
      {
         return;
      }
      ////        *m = phi'\pih/phi;;
      //          CALL TRNPSE(PHIH,2,2,PHIHTP)
      UtMatrixd phihtp(2, 2);
      phihtp.Transpose(phih);
      //          CALL DFMCP(PHIHTP,2,PI,2,DUM1,DUM2,DUM3,DUM4,
      //     +               DUM5,IDUM,TMP2X2,ERR)
      //(mjm) above is solving for 'x' where Ax=B, A==PHIHTP, B==PI, x==TMP2X2
      UtMatrixd tmp2x2(2, 2); // solution vector 'x' for ax=b
      UtMatrixd phihtpinv(phihtp);
      try
      {
         phihtpinv.Invert();
         tmp2x2.Multiply(phihtpinv, aVerticalInfoMatrix);
      }
      catch (UtMatrixd::SingularMatrixError&)
      {
         //             IF (ERR) GO TO 100
         return;
      }
      //          CALL MTXDIV(TMP2X2,PHIH,2,2,DUM1,DUM2,DUM3,M)
      // Same as tmp2x2*Inverse(phih)
      UtMatrixd phihinv(phih);
      phihinv.Invert();
      UtMatrixd m(2, 2);
      m.Multiply(tmp2x2, phihinv);
      ////        *n=m/(inv(qih)+m);
      //          CALL MTXADD(M,QIHINV,2,2,MQI)
      UtMatrixd mqi(2, 2);
      mqi.Add(m, qihinv);
      //          CALL MTXDIV(M,MQI,2,2,DUM1,DUM2,DUM3,N)
      UtMatrixd mqiinv(mqi);
      mqiinv.Invert();
      UtMatrixd n(2, 2);
      n.Multiply(m, mqiinv);
      ////        *update inormation matrix to present time: pivert = (eye(2)-n)*m;
      //          CALL IDENTY(2,EYE2)
      UtMatrixd eye2(2, 2);
      eye2.LoadIdentity();
      //          CALL MTXSUB(EYE2,N,2,2,TMP2X2)
      tmp2x2.Subtract(eye2, n);
      //          CALL MTXMUL(TMP2X2,2,2,M,2,2,PI)
      aVerticalInfoMatrix.Multiply(tmp2x2, m);
      ////        *roundoff error can corrupt symmetry of pivert, so symmetrize;
      //          CALL FIXSYM(PI,2,1,TRKTYP)
      aVerticalInfoMatrix.Symmetrize();
      ////        *update information state to present time:
      ////        *xivert = ((eye(2)-n)/phihtp)*xivert;
      //          CALL MTXDIV(TMP2X2,PHIHTP,2,2,DUM1,DUM2,DUM3,TMQ2X2)
      UtMatrixd tmq2x2(2, 2);
      tmq2x2.Multiply(tmp2x2, phihtpinv);
      //          CALL MTXSET(XI,2,1,TMP2X1)
      UtMatrixd tmp2x1(aVerticalState);
      //          CALL MTXMUL(TMQ2X2,2,2,TMP2X1,2,1,XI)
      aVerticalState.Multiply(tmq2x2, tmp2x1);
      //       END IF
   }
   //
   // 100    RETURN
   //       END
   //
}


//       SUBROUTINE MTTALT(JNR,GRL,PTR384, ALT)
//! get altitude for an active track
//! @param aLocalTrackPtr The active track from which the altitude is extracted.
//! @param aAlt (output) The perceived altitude.
void MTT::mttalt(const MTT_ActiveTrack* aLocalTrackPtr, double& aAlt)
{
   // C     *begin logic to get altitute for an active track:

   //       IMPLICIT NONE
   //       INTEGER JNR(*)
   //       REAL GRL(*)
   //       DOUBLE PRECISION ALT
   //       INTEGER PTR384,PTR387,I
   //       DOUBLE PRECISION DUM1(100)
   //       DOUBLE PRECISION PV(2,2),XV(2)
   //       LOGICAL ERR

   //       REAL RAT(184)
   //       DOUBLE PRECISION AT(92)
   //       DOUBLE PRECISION XHORZ(6),PHORZ(6,6),PIVERT(2,2),XIVERT(2)
   //       DOUBLE PRECISION MTPLF,MTPTF,XHORZP(6),PHORZP(6,6)
   //
   //       EQUIVALENCE (RAT(1),AT(1))
   //       EQUIVALENCE (PHORZ,AT(1)),(XHORZ,AT(37))
   //       EQUIVALENCE (PIVERT,AT(43)),(XIVERT,AT(47))
   //       EQUIVALENCE (MTPLF,AT(49)),(MTPTF,AT(50))
   //       EQUIVALENCE (XHORZP,AT(51)),(PHORZP,AT(57))

   MTT_ActiveTrack::Data at;
   UtMatrixd&            pivert = at.mVerticalInfoMatrix;
   UtMatrixd&            xivert = at.mVerticalState;

   const MTT_ActiveTrack* ptr384 = aLocalTrackPtr;
   // C     *load the double precision active track data from GRL
   //        PTR387=JNR(PTR384+3)
   MTT_ActiveTrack::Data* ptr387 = &ptr384->GetData();

   //       DO I=1,184
   //          RAT(I)=GRL(PTR387+I-1)
   //       ENDDO
   at = *ptr387;

   //       ALT=0.0D0
   //       IF (JNR(PTR384+6).EQ.1) THEN
   if (ptr384->mVerticalChannelActive)
   {
      //          CALL MTXINV(PIVERT,2, PV,DUM1,ERR)
      UtMatrixd pv(pivert);
      try
      {
         pivert.Invert();
      }
      catch (UtMatrixd::SingularMatrixError&)
      {
         return;
      }
      //          IF (.NOT.ERR) THEN
      //             CALL MTXMUL(PV,2,2,XIVERT,2,1, XV)
      UtMatrixd xv;
      xv.Multiply(pv, xivert);
      //             ALT=XV(1)
      aAlt = xv(0);
      //          ENDIF
      //       ENDIF
   }
   // C     *end of logic for MTTALT.
   //        RETURN
   //        END
}

/* todo; not ctk currently not supported by WSF-MTT
       SUBROUTINE MTTCTK(JNR,GRL,TIME,PTR017,NUMTGT,PLRTGT,NUMPLY,
     + PLRSND,IPLYR,PLRRCV,PTR5 )
C----------------------------------------------------------------------+
C ABSTRACT     multiple target tracking - output intell report in CTK  |
C DISTRIBUTION STATEMENT D.                                            |
C  Distribution limited to DOD and DOD contractors; Critical           |
C  Technology, November 2008. Other requests shall be referred to      |
C  ASC/ENDA, Wright-Patterson Air Force Base, OH 45433-7210.           |
C WARNING - This document contains technical data whose export is      |
C  restricted by the Arms Export Control Act (Title 22, U.S.C.,        |
C  Sec 2751, et. seq.) or the Export Administration Act of 1979, as    |
C  amended, Title 50, U.S.C., App. 2401 et. seq. Violations of these   |
C  export laws are subject to severe criminal penalties. Disseminate   |
C  in accordance with provisions of DoD Directive 5230.25.             |
C CHANGE HISTORY:  7.0-038                                             |
C ARGUMENTS                                                            |
C  Gbl JNR     integer array containing data structures                |
C  Gbl GRL     real array equivalenced to JNR                          |
C  Inp TIME    current simulation time in seconds                      |
C  Inp PTR017  ptr to target perception (17) data                      |
C  Inp NUMTGT  user defined player number of target                    |
C  Inp PLRTGT  target player name                                      |
C  Inp NUMPLY  user defined number of source player                    |
C  Inp PLRSND  source player name                                      |
C  Inp IPLYR   user number of player owning the target perception      |
C  Inp PLRRCV  name of player owning the target perception             |
C  Inp PTR5    ptr to target location (5)                              |
C VARIABLES                                                            |
C----------------------------------------------------------------------+

       IMPLICIT NONE
       REAL GRL(*)
       INTEGER JNR(*),PTR017,PTR5

       INTEGER I,PTR363,PTR367,TRKID,LMASTR
       REAL TIME

       DOUBLE PRECISION TTRACK,FILTXP,FILTYP,FILTXV,FILTYV
       DOUBLE PRECISION FILTZP,FILTZV,HVMATR(6,6)
       DOUBLE PRECISION TMVERT,FILTXA,FILTYA,VMAT(2,2)

       REAL RTRK(184)
       DOUBLE PRECISION TRK(92)
       EQUIVALENCE (RTRK(1),TRK(1))
       EQUIVALENCE (TTRACK,TRK(1)),(FILTXP,TRK(2)),(FILTYP,TRK(3))
       EQUIVALENCE (FILTXV,TRK(4)),(FILTYV,TRK(5)),(FILTZP,TRK(6))
       EQUIVALENCE (FILTZV,TRK(7)),(HVMATR,TRK(8)),(TMVERT,TRK(44))
       EQUIVALENCE (FILTXA,TRK(45)),(FILTYA,TRK(46)),(VMAT,TRK(47))

       INTEGER NUMTGT,NUMPLY,IPLYR
       CHARACTER*20 PLRTGT,PLRSND,PLRRCV

       REAL TRUEX,TRUEY,TRUEZ,TRUVX,TRUVY,TRUVZ,TRUVH,TRUHDG,TRUSPD
       INTEGER IDTRGT,IDSNDR,IDRCVR
       CHARACTER*20 PLTRGT,PLSNDR,PLRCVR

       IDTRGT=NUMTGT
       IDSNDR=NUMPLY
       IDRCVR=IPLYR
       PLTRGT=PLRTGT
       PLSNDR=PLRSND
       PLRCVR=PLRRCV
       TRUEX=GRL(PTR5+1)
       TRUEY=GRL(PTR5+2)
       TRUEZ=GRL(PTR5+3)

       TRUVX=GRL(PTR5+4)*GRL(PTR5+5)
       TRUVY=GRL(PTR5+4)*GRL(PTR5+6)
       TRUVZ=GRL(PTR5+4)*GRL(PTR5+7)
       TRUHDG=0.0
       IF(GRL(PTR5+6).NE.0.0.OR.GRL(PTR5+5).NE.0.0)
     + TRUHDG=ATAN2(GRL(PTR5+6),GRL(PTR5+5))
       TRUVH=SQRT(TRUVX**2+TRUVY**2)
       TRUSPD=SQRT(TRUVH**2+TRUVZ**2)

C      *set track id
        TRKID=JNR(PTR017+41)
C      *load the double precision active track data from GRL
        PTR363=JNR(PTR017+37)
        PTR367=JNR(PTR363+1)
        DO I=1,184
           RTRK(I)=GRL(PTR367+I-1)
        ENDDO

C*====================================================== .ctk =======;
C      *write combined track to output file;
        LMASTR=JNR(2)-JNR(1)
        IF(MOD(JNR(LMASTR+30)/2,2).EQ.1)THEN
           WRITE(62,1113)TRK(1),4,5,TRKID,0,IDTRGT,PLTRGT,IDSNDR,
     +     PLSNDR,IPLYR,PLRRCV,(TRK(I),I=2,7),(TRK(I),I=44,46),
     +     TRUEX,TRUEY,TRUVX,TRUVY,TRUEZ,TRUVZ
 1113      FORMAT(E15.7,2I3,2I8,3(I8,1X,A20),2X,15E15.7)
        ENDIF
C*====================================================== .ctk =======;

       RETURN
       END
*/
