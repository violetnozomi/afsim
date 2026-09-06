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

#ifndef WSFCYBERTOGGLEEFFECT_HPP
#define WSFCYBERTOGGLEEFFECT_HPP

#include <list>
#include <unordered_map>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfComm.hpp"
#include "WsfCyberEffect.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfProcessor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{

namespace cyber
{

template<class T>
class ToggleEffect : public Effect
{
public:
   ToggleEffect();
   ToggleEffect(ToggleEffect&&) = default;
   ToggleEffect& operator=(const ToggleEffect&) = delete;
   ToggleEffect& operator=(ToggleEffect&&) = default;
   ~ToggleEffect() override                = default;

   //! @name Common infrastructure methods.
   //@{
   Effect* Clone() const override;
   bool    Initialize(Engagement& aEngagement) override;
   bool    ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Engagement methods.
   //! These methods are called during the course of the engagement.
   //! @note For WsfCyberToggleEffect, the attack method will simply
   //! "turn off" the associated components, while the restore method
   //! will "turn on" the same components.
   //@{
   void Attack(double aSimTime, Engagement& aEngagement) override;

   void Restore(double aSimTime, Engagement& aEngagement) override;
   //@}

protected:
   enum InputListType
   {
      cPLATFORM,
      cPLATFORMTYPE,
      cDEFAULT
   };

   ToggleEffect(const ToggleEffect& aSrc);

   //! @name ProcessListInput method
   //! Helper method for use with process_input in adding components
   //! to their mapped platforms, platform_types, or default
   bool ProcessListInput(UtInput& aInput, const std::string& aCommand, InputListType aType, const std::string& aPlatformName);

   //! @name AttackRestore method
   //! A condensed method for the combined attack and restore methods to
   //! reuse code logic. A passed true value indicates usage of the
   //! restore logic, while a false indicates usage of an attack.
   void AttackRestore(double aSimTime, Engagement& aEngagement, bool aRestore);

private:
   class ComponentListing;
   using ComponentMap = std::unordered_map<std::string, ComponentListing>;
   using NameList     = std::list<std::string>;

   //! @name ToggleComponent method
   //! Given a platform and a list of component names, toggle those
   //! components on or off depending on the boolean argument
   bool ToggleComponent(double          aSimTime,
                        bool            aTurnOn,
                        WsfPlatform*    aPlatformPtr,
                        const NameList& aList,
                        WsfSimulation&  aSimulation);

   //! @name ToggleAll method
   //! Given a platform, toggle all of the components of a type on
   //! or off depending on the boolean argument
   bool ToggleAll(double aSimTime, bool aTurnOn, WsfPlatform* aPlatformPtr, WsfSimulation& aSimulation);

   // Private helper class container, used for storing component names
   // tied to the platform, platform_type, or default components to be
   // toggled on or off
   class ComponentListing
   {
   public:
      ComponentListing();
      ~ComponentListing() = default;

      ComponentListing(const ComponentListing& aSrc);

      NameList mComponentList;
      bool     mSelectAll;
   };

   //! @name Map manipulation methods
   //! Provides ease of access for adding components, setting select_all status,
   //! or querying mane entry status
   //@{
   bool AddComponent(InputListType aType, const std::string& aComponentName, const std::string& aPlatformName = "");
   bool AddDisableAll(InputListType aType, const std::string& aPlatformName = "");

   bool IsDisableAll(InputListType aType, const std::string& aPlatformName);
   bool HasComponents(InputListType aType, const std::string& aPlatformName);
   //@}

   //! We have to have discrete containers for platforms and platform types.
   //! A platform instance can have the same name as a type, so we can't ensure
   //! a unique key value by combining them into the same container
   ComponentMap mPlatformComponentMap;
   ComponentMap mPlatformTypeComponentMap;

   //! Component listing for "default" usage
   ComponentListing mDefaultComponentList;
};


// Definitions

template<class T>
ToggleEffect<T>::ToggleEffect()
   : Effect()
   , mPlatformComponentMap()
   , mPlatformTypeComponentMap()
   , mDefaultComponentList()
{
}

template<class T>
ToggleEffect<T>::ToggleEffect(const ToggleEffect& aSrc)
   : Effect(aSrc)
   , mPlatformComponentMap(aSrc.mPlatformComponentMap)
   , mPlatformTypeComponentMap(aSrc.mPlatformTypeComponentMap)
   , mDefaultComponentList(aSrc.mDefaultComponentList)
{
}

template<class T>
Effect* ToggleEffect<T>::Clone() const
{
   return new ToggleEffect<T>(*this);
}

template<class T>
bool ToggleEffect<T>::Initialize(Engagement& /*aEngagement*/)
{
   return true;
}

// =================================================================================================
// virtual
template<class T>
bool ToggleEffect<T>::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "platform_type")
   {
      UtInputBlock inputBlock(aInput);
      std::string  platformType;
      aInput.ReadValue(platformType);
      while (inputBlock.ReadCommand(command))
      {
         if (!ProcessListInput(aInput, command, cPLATFORMTYPE, platformType))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "platform")
   {
      UtInputBlock inputBlock(aInput);
      std::string  platformName;
      aInput.ReadValue(platformName);
      while (inputBlock.ReadCommand(command))
      {
         if (!ProcessListInput(aInput, command, cPLATFORM, platformName))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "default")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (!ProcessListInput(aInput, command, cDEFAULT, ""))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = Effect::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
// virtual
template<class T>
bool ToggleEffect<T>::ProcessListInput(UtInput&           aInput,
                                       const std::string& aCommand,
                                       InputListType      aType,
                                       const std::string& aPlatformName)
{
   bool        myCommand = true;
   std::string command   = "";
   if (aCommand == "target_list")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         bool ok = AddComponent(aType, command, aPlatformName);

         if (!ok)
         {
            return false;
         }
      }
   }
   else if (aCommand == "select_all")
   {
      bool ok = AddDisableAll(aType, aPlatformName);

      if (!ok)
      {
         return false;
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
// virtual
template<class T>
void ToggleEffect<T>::Attack(double aSimTime, Engagement& aEngagement)
{
   AttackRestore(aSimTime, aEngagement, false);
}

// =================================================================================================
// virtual
template<class T>
void ToggleEffect<T>::Restore(double aSimTime, Engagement& aEngagement)
{
   AttackRestore(aSimTime, aEngagement, true);
}

// =================================================================================================
// virtual
template<class T>
void ToggleEffect<T>::AttackRestore(double aSimTime, Engagement& aEngagement, bool aRestore)
{
   //! Try to find a matching mapped value by the most specific criteria
   //! 1. Mapped platform name
   //! 2. Mapped platform type
   //! 3. A parent platform type that matches a platform_type
   //! 4. Default (if it exists)

   //! Find the appropriate victim platform in the simulation
   auto&        sim    = aEngagement.GetSimulation();
   WsfPlatform* victim = sim.GetPlatformByName(aEngagement.GetVictim());
   if (!victim)
   {
      return;
   }

   //! Try to match the platform name to a platform listed with the effect
   auto platIt = mPlatformComponentMap.find(victim->GetName());
   if (platIt != mPlatformComponentMap.end())
   {
      // Found a match
      if (platIt->second.mSelectAll)
      {
         ToggleAll(aSimTime, aRestore, victim, sim);
         return;
      }
      else
      {
         ToggleComponent(aSimTime, aRestore, victim, platIt->second.mComponentList, sim);
         return;
      }
   }
   //! Try to match the platform_type (or any of the types it derives from) to a platform_type listed
   //! with the effect
   else
   {
      //! Iterate through the typelist (most specific type to least specific) to try to find
      //! a matching type the effect has listed. The first, most specific entry found with be
      //! used to define the effect.
      auto typeList = victim->GetTypeList();
      for (auto& listIt : typeList)
      {
         auto platTypeIt = mPlatformTypeComponentMap.find(listIt);
         if (platTypeIt != mPlatformTypeComponentMap.end())
         {
            // Found a match
            if (platTypeIt->second.mSelectAll)
            {
               ToggleAll(aSimTime, aRestore, victim, sim);
               return;
            }
            else
            {
               ToggleComponent(aSimTime, aRestore, victim, platTypeIt->second.mComponentList, sim);
               return;
            }
         }
      }
   }

   //! All attempts to match specified platform/platform_types failed. Use the default, if available
   if (mDefaultComponentList.mSelectAll)
   {
      ToggleAll(aSimTime, aRestore, victim, sim);
      return;
   }
   else
   {
      ToggleComponent(aSimTime, aRestore, victim, mDefaultComponentList.mComponentList, sim);
      return;
   }
}

// =================================================================================================
template<class T>
bool ToggleEffect<T>::ToggleComponent(double          aSimTime,
                                      bool            aTurnOn,
                                      WsfPlatform*    aPlatformPtr,
                                      const NameList& aList,
                                      WsfSimulation&  aSimulation)
{
   bool ok = true;
   for (const auto& name : aList)
   {
      T* component = aPlatformPtr->GetComponent<T>(WsfStringId(name));
      if (component)
      {
         if (aTurnOn)
         {
            ok &= aSimulation.TurnPartOn(aSimTime, component);
         }
         else
         {
            ok &= aSimulation.TurnPartOff(aSimTime, component);
         }
      }
   }
   return ok;
}

template<class T>
bool ToggleEffect<T>::ToggleAll(double aSimTime, bool aTurnOn, WsfPlatform* aPlatformPtr, WsfSimulation& aSimulation)
{
   bool         ok    = true;
   unsigned int count = aPlatformPtr->GetComponentCount<T>();
   for (unsigned int i = 0; i < count; ++i)
   {
      T* component = aPlatformPtr->GetComponentEntry<T>(i);
      if (aTurnOn)
      {
         ok &= aSimulation.TurnPartOn(aSimTime, component);
      }
      else
      {
         ok &= aSimulation.TurnPartOff(aSimTime, component);
      }
   }
   return ok;
}

// =================================================================================================
template<class T>
ToggleEffect<T>::ComponentListing::ComponentListing()
   : mComponentList()
   , mSelectAll(false)
{
}

// =================================================================================================
template<class T>
ToggleEffect<T>::ComponentListing::ComponentListing(const ComponentListing& aSrc)
   : mComponentList(aSrc.mComponentList)
   , mSelectAll(aSrc.mSelectAll)
{
}

// =================================================================================================
template<class T>
bool ToggleEffect<T>::AddComponent(InputListType      aType,
                                   const std::string& aComponentName,
                                   const std::string& aPlatformName /*= ""*/)
{
   // Ensure select_all was not previously set
   if (IsDisableAll(aType, aPlatformName))
   {
      return false;
   }

   if (aType == cPLATFORM)
   {
      if (aPlatformName.empty())
      {
         return false;
      }

      mPlatformComponentMap[aPlatformName].mComponentList.push_back(aComponentName);
      return true;
   }
   else if (aType == cPLATFORMTYPE)
   {
      if (aPlatformName.empty())
      {
         return false;
      }

      mPlatformTypeComponentMap[aPlatformName].mComponentList.push_back(aComponentName);
      return true;
   }
   else // cDEFAULT
   {
      mDefaultComponentList.mComponentList.push_back(aComponentName);
      return true;
   }
}

// =================================================================================================
template<class T>
bool ToggleEffect<T>::AddDisableAll(InputListType aType, const std::string& aPlatformName)
{
   // Ensure components are not listed for a disable all
   if (HasComponents(aType, aPlatformName))
   {
      return false;
   }

   if (aType == cPLATFORM)
   {
      mPlatformComponentMap[aPlatformName].mSelectAll = true;
      return true;
   }
   else if (aType == cPLATFORMTYPE)
   {
      mPlatformTypeComponentMap[aPlatformName].mSelectAll = true;
      return true;
   }
   else // cDEFAULT
   {
      mDefaultComponentList.mSelectAll = true;
      return true;
   }
}

// =================================================================================================
template<class T>
bool ToggleEffect<T>::IsDisableAll(InputListType aType, const std::string& aPlatformName)
{
   if (aType == cPLATFORM)
   {
      auto it = mPlatformComponentMap.find(aPlatformName);
      if (it != mPlatformComponentMap.end() && it->second.mSelectAll)
      {
         return true;
      }
      return false;
   }
   else if (aType == cPLATFORMTYPE)
   {
      auto it = mPlatformTypeComponentMap.find(aPlatformName);
      if (it != mPlatformTypeComponentMap.end() && it->second.mSelectAll)
      {
         return true;
      }
      return false;
   }
   else // cDEFAULT
   {
      return mDefaultComponentList.mSelectAll;
   }
}

// =================================================================================================
template<class T>
bool ToggleEffect<T>::HasComponents(InputListType aType, const std::string& aPlatformName)
{
   if (aType == cPLATFORM)
   {
      auto it = mPlatformComponentMap.find(aPlatformName);
      if (it != mPlatformComponentMap.end() && it->second.mComponentList.size() > 0)
      {
         return true;
      }
      return false;
   }
   else if (aType == cPLATFORMTYPE)
   {
      auto it = mPlatformTypeComponentMap.find(aPlatformName);
      if (it != mPlatformTypeComponentMap.end() && it->second.mComponentList.size() > 0)
      {
         return true;
      }
      return false;
   }
   else // cDEFAULT
   {
      return (mDefaultComponentList.mComponentList.size() > 0);
   }
}

} // namespace cyber

} // namespace wsf

#endif
