// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef PROXYINTERFACE_HPP
#define PROXYINTERFACE_HPP

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

#include "UtCallbackHolder.hpp"
#include "WsfPProxyNode.hpp"

namespace PostProcessor
{
class ProxyInterface : public QObject
{
      Q_OBJECT
   public:
      struct Platform
      {
         QString mName;
         QString mType;
         QString mSide;
         QString mMover;

         Platform(QString aName, QString aType, QString aSide, QString aMover)
            : mName(aName)
            , mType(aType)
            , mSide(aSide)
            , mMover(aMover) { }

         Platform() = default;

         bool operator<(const Platform& p) const
         {
            if (mName == p.mName)
            {
               if (mType == p.mType)
               {
                  return mSide < p.mSide;
               }
               else
               {
                  return mType < p.mType;
               }
            }
            else
            {
               return mName < p.mName;
            }
         }
      };

      struct PlatformPart
      {
         QString  mName;
         QString  mType;
         Platform mPlatform;

         PlatformPart(QString aName, QString aType, Platform aPlatform)
            : mName(aName)
            , mType(aType)
            , mPlatform(aPlatform) { }

         PlatformPart() = default;

         bool operator<(const PlatformPart& aPart) const
         {
            if (mName == aPart.mName)
            {
               if (mType == aPart.mType)
               {
                  return mPlatform < aPart.mPlatform;
               }
               else
               {
                  return mType < aPart.mType;
               }
            }
            else
            {
               return mName < aPart.mName;
            }
         }
      };

      ProxyInterface();
      ~ProxyInterface() override = default;

      void SetProxyNode(const WsfPProxyNode& aProxyNode) { mProxyNode = aProxyNode; }
      void ResetProxyNode() { mProxyNode = WsfPProxyNode(); }
      bool ProxyNodeIsEqualTo(const WsfPProxyNode& aProxyNode) { return mProxyNode == aProxyNode; }

      std::string GetProxyNodeName() const { return mProxyNode.GetName(); }
      UtPath GetProxyNodeFile() const { return mProxyNode.GetSourceDocumentRange().mSource->GetFilePath(); }

      void ProxyAvailable(WsfPProxy* aProxyPtr);

      void Update();

   signals:
      void PlatformAdded(const QMap<Platform, bool>& aAddedPlatforms);
      void PlatformRemoved(const QVector<Platform>& aRemovedPlatforms);
      void SensorAdded(const QVector<PlatformPart>& aAddedSensors);
      void SensorRemoved(const QVector<PlatformPart>& aRemovedSensors);
      void WeaponAdded(const QVector<PlatformPart>& aAddedWeapons);
      void WeaponRemoved(const QVector<PlatformPart>& aRemovedWeapons);
      void TransmitterAdded(const QVector<PlatformPart>& aAddedTransmitters);
      void TransmitterRemoved(const QVector<PlatformPart>& aRemovedTransmitters);
      void ReceiverAdded(const QVector<PlatformPart>& aAddedReceivers);
      void ReceiverRemoved(const QVector<PlatformPart>& aRemovedReceivers);
      void MoverAdded(const QVector<PlatformPart>& aAddedMovers);
      void MoverRemoved(const QVector<PlatformPart>& aRemovedMovers);
      void EndTimeChanged(const WsfProxy::Time& aEndTime);


   private:
      void GetDiff(QVector<PlatformPart>& aNew,
                   QVector<PlatformPart>& aOld,
                   QVector<PlatformPart>& aAdded,
                   QVector<PlatformPart>& aRemoved,
                   bool                   aAppend = false);

      void GetDiff(QVector<Platform>&    aNew,
                   QVector<Platform>&    aOld,
                   QMap<Platform, bool>& aAdded,
                   QVector<Platform>&    aRemoved,
                   bool                  aAppend = false,
                   bool                  aIsWeapon = false);

      WsfPProxyNode         mProxyNode;
      UtCallbackHolder      mCallbacks;

      QVector<Platform>       mPlatforms;
      QVector<Platform>       mWeaponPlatforms;
      QVector<PlatformPart>   mSensors;
      QVector<PlatformPart>   mWeapons;
      QVector<PlatformPart>   mTransmitters;
      QVector<PlatformPart>   mReceivers;
      QVector<PlatformPart>   mMovers;

      WsfProxy::Time        mEndTime;
};
}
#endif