// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFQUANTUMGENERATOR_HPP
#define WSFQUANTUMGENERATOR_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
class UtScript;
class UtScriptClass;
class UtScriptContext;
class UtScriptExecutor;
class WsfAssetPerception;
#include "wsf_mil_export.h"
class WsfLocalTrack;
class WsfQuantumTask;
class WsfScriptContext;


class WSF_MIL_EXPORT WsfQuantumGenerator
{
public:
   WsfQuantumGenerator()                                = default;
   virtual ~WsfQuantumGenerator()                       = default;
   virtual std::string                  GeneratorType() = 0;
   virtual void                         Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) = 0;
   virtual WsfQuantumGenerator*         Clone() const                                                              = 0;
   virtual std::vector<WsfQuantumTask*> GenerateTasks(double                            aSimTime,
                                                      std::vector<WsfLocalTrack*>&      aThreats,
                                                      std::vector<WsfAssetPerception*>& aAssets)                   = 0;
};


class WSF_MIL_EXPORT WsfQuantumGeneratorFactory
{
public:
   ~WsfQuantumGeneratorFactory()                                 = default;
   WsfQuantumGeneratorFactory(WsfQuantumGeneratorFactory&& aSrc) = delete;
   WsfQuantumGeneratorFactory& operator=(const WsfQuantumGeneratorFactory&) = delete;
   WsfQuantumGeneratorFactory& operator=(WsfQuantumGeneratorFactory&&) = delete;

   static WsfQuantumGeneratorFactory* Instance();
   bool                               Register(std::unique_ptr<WsfQuantumGenerator> aGeneratorPtr);
   WsfQuantumGenerator*               CreateGenerator(const std::string& evaluatorName);
   const std::map<std::string, ut::CloneablePtr<WsfQuantumGenerator>>& Generators() const { return mFactoryMap; }

private:
   WsfQuantumGeneratorFactory();
   WsfQuantumGeneratorFactory(const WsfQuantumGeneratorFactory& aSrc) = default;

   std::map<std::string, ut::CloneablePtr<WsfQuantumGenerator>> mFactoryMap;
};


class WSF_MIL_EXPORT WsfQuantumGeneratorScript : public WsfQuantumGenerator
{
public:
   WsfQuantumGeneratorScript(WsfScriptContext* aContextPtr, std::string& aScriptMethod);
   WsfQuantumGeneratorScript& operator=(const WsfQuantumGeneratorScript&) = delete;
   ~WsfQuantumGeneratorScript() override                                  = default;
   void                         Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override;
   WsfQuantumGenerator*         Clone() const override;
   std::string                  GeneratorType() override { return mType; }
   virtual bool                 Valid() { return (mScriptPtr != nullptr); }
   std::vector<WsfQuantumTask*> GenerateTasks(double                            aSimTime,
                                              std::vector<WsfLocalTrack*>&      aThreats,
                                              std::vector<WsfAssetPerception*>& aAssets) override;

private:
   WsfQuantumGeneratorScript(const WsfQuantumGeneratorScript& aSrc);

   std::string       mType;
   UtScript*         mScriptPtr;
   UtScriptContext*  mContextPtr;
   UtScriptExecutor* mExecutorPtr;

   UtScriptClass* mLocalTrackClassPtr{nullptr};
   UtScriptClass* mTracksClassPtr{nullptr};
   UtScriptClass* mAssetClassPtr{nullptr};
   UtScriptClass* mAssetsClassPtr{nullptr};
};


class WSF_MIL_EXPORT WsfQuantumGeneratorSimpleWeapon : public WsfQuantumGenerator
{
public:
   WsfQuantumGeneratorSimpleWeapon()           = default;
   ~WsfQuantumGeneratorSimpleWeapon() override = default;
   std::string GeneratorType() override { return "simple_weapon"; }

   void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override {}
   WsfQuantumGenerator* Clone() const override { return new WsfQuantumGeneratorSimpleWeapon(); }

   std::vector<WsfQuantumTask*> GenerateTasks(double                            aSimTime,
                                              std::vector<WsfLocalTrack*>&      aThreats,
                                              std::vector<WsfAssetPerception*>& aAssets) override;
};


class WSF_MIL_EXPORT WsfQuantumGeneratorSimpleJammer : public WsfQuantumGenerator
{
public:
   WsfQuantumGeneratorSimpleJammer()           = default;
   ~WsfQuantumGeneratorSimpleJammer() override = default;
   std::string GeneratorType() override { return "simple_jammer"; }

   void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override {}
   WsfQuantumGenerator* Clone() const override { return new WsfQuantumGeneratorSimpleJammer(); }

   std::vector<WsfQuantumTask*> GenerateTasks(double                            aSimTime,
                                              std::vector<WsfLocalTrack*>&      aThreats,
                                              std::vector<WsfAssetPerception*>& aAssets) override;
};


class WSF_MIL_EXPORT WsfQuantumGeneratorSimpleSensor : public WsfQuantumGenerator
{
public:
   WsfQuantumGeneratorSimpleSensor()           = default;
   ~WsfQuantumGeneratorSimpleSensor() override = default;
   std::string GeneratorType() override { return "simple_sensor"; }

   void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override {}
   WsfQuantumGenerator* Clone() const override { return new WsfQuantumGeneratorSimpleSensor(); }

   std::vector<WsfQuantumTask*> GenerateTasks(double                            aSimTime,
                                              std::vector<WsfLocalTrack*>&      aThreats,
                                              std::vector<WsfAssetPerception*>& aAssets) override;
};

#endif
