// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_FORMATION_HPP
#define WSF_SIXDOF_FORMATION_HPP

#include "wsf_six_dof_export.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "UtScriptAccessible.hpp"
#include "WsfSixDOF_FormationCommand.hpp"
#include "WsfSixDOF_FormationOffset.hpp"
#include "maneuvers/WsfSixDOF_RelativeManeuver.hpp"

class UtInput;
class WsfPlatform;
class WsfScenario;
class Mover;

namespace wsf
{
namespace six_dof
{
class FormationManager;

//! The formation base class.
//! This class defines the interface that formations must support, and provides a few
//! utility functions. A key concept for formations are the relative and qualified names
//! of the formation. A formation is generally a tree structure, with a formation having
//! zero to many sub-formations. The relative name is a means of identifying
//! sub-formations relative to their common parent. The relative name must be distinct
//! among the sub-formations of any given formation. In addition to this relative
//! name, one can specify the formation directly with its fully qualified name.
//! The qualified name is a concatenation of all of the relative names of a formation
//! from the root down to the specific formation. So a formation with the relative
//! name 'alpha' having two sub-formations with names 'one' and 'two' would generate
//! the following fully qualified names: 'alpha' for the root formation, 'alpha.one'
//! for the first sub-formation, and 'alpha.two' for the second sub-formation.
//! The fully qualified names must be unique across all formations defined in a
//! simulation, otherwise referring to a formation by name will no longer map into
//! a unique object.
class WSF_SIX_DOF_EXPORT Formation : public UtScriptAccessible
{
public:
   Formation(FormationManager& aManager, const std::string& aName);
   Formation(Formation&& aOther) = delete;

   ~Formation() override;

   Formation& operator=(const Formation& aOther) = delete;
   Formation& operator=(Formation&& aOther) = delete;

   virtual std::unique_ptr<Formation> Duplicate(FormationManager&  aManager,
                                                Formation*         aParentPtr,
                                                const std::string& aQualifiedName,
                                                const std::string& aName) const;
   virtual bool                       ProcessInput(UtInput& aInput);

   const char* GetScriptClassName() const override { return "WsfSixDOF_Formation"; }

   //! Get this formation's parent formation.
   Formation* GetParentFormation() const { return mParentPtr; }

   //! Returns if this a top-level formation.
   bool IsRoot() const { return !mParentPtr; }

   //! Returns if this a leaf formation.
   bool IsLeaf() const { return GetNumChildFormations() == 0; }

   int                      GetNumChildFormations() const;
   Formation*               GetChildFormation(const std::string& aName) const;
   std::vector<std::string> GetDescendantFormationNames() const;
   virtual int              GetNumMembers() const;

   //! Get the name of the member platform.
   //!
   //! This will only produce a non-empty string when the formation subclass is capable of
   //! having a single member directly. For an example, see FormationUnit.
   virtual std::string GetMemberPlatformName() const { return std::string{}; }

   //! Return the member platform.
   //!
   //! This will only produce a non-nullptr result when the formation subclass is capable
   //! of having a single member directly. For an example, see FormationUnit.
   virtual WsfPlatform* GetMemberPlatform() const { return nullptr; }

   //! Return the mover for the member.
   //!
   //! This will only produce a non-nullptr result when the formation subclass is capable
   //! of having a single member directly. For an example, see FormationUnit.
   virtual Mover* GetMemberMover() const { return nullptr; }

   //! Return of the formation has a member platform.
   //!
   //! This will only produce true for a formation subclass that is capable of having
   //! a single member directly. For an example, see FormationUnit.
   virtual bool HasMemberPlatform() const { return false; }

   //! Return if the formation is a unit formation.
   //!
   //! A unit formation is a formation that has a single member platform and does not
   //! have any sub-formations. For an example of such a formation subclass, see
   //! FormationUnit.
   virtual bool IsUnitFormation() const { return false; }

   RelativeManeuverKinematics GetMemberKinematicState() const;
   RelativeManeuverKinematics GetMemberInitialKinematicState() const;
   void                       SetMemberInitialKinematicState(RelativeManeuverKinematics& aState) const;

   //! Get the name of the lead formation.
   std::string GetLeader() const { return mLeadChild; }

   //! Return if the formation has a lead sub-formation defined.
   bool HasLeader() const { return !GetLeader().empty(); }

   //! Get the lead sub-formation of this formation.
   Formation* GetLeadSubFormation() const { return GetChildFormation(mLeadChild); }

   Formation* GetFormationLeader();
   Formation* GetAttachedRoot();
   Formation* GetKeepStationRoot();

   bool IsLeader() const;

   //! Get this formation's relative name.
   const std::string& GetRelativeName() const { return mName; }

   //! Get this formation's full qualified name.
   const std::string& GetQualifiedName() const { return mQualifiedName; }

   std::string GetChildQualifiedName(const std::string& aChildName) const;
   bool        HasAncestor(const Formation* aFormationPtr) const;

   //! Get the offset for this formation relative to its parent.
   FormationOffset GetOffset() const { return mOffset; }

   FormationOffset            GetOffsetFrom(Formation& aFormation) const;
   RelativeManeuverKinematics GetOffsetKinematicsFrom(Formation* aFormationPtr) const;
   FormationManager&          GetManager() const;
   WsfScenario&               GetScenario() const;

   //! Return if debug output is enabled.
   bool GetDebug() const { return mDebug; }

   //! Return if the formation is attached to its parent.
   bool IsAttached() const { return mAttached; }

   bool               SetParentFormation(Formation* aParentPtr, bool aRename = true);
   bool               SetRelativeName(const std::string& aRelativeName);
   virtual bool       AddChildFormation(Formation* aChildPtr);
   virtual Formation* RemoveChildFormation(const std::string& aName, bool aRename = true);
   virtual bool       SetLead(const std::string& aName);
   void               SetOffset(const FormationOffset& aOffset);
   bool               SetMemberName(const std::string& aPlatformName);

   //! Set the debugging output flag.
   void SetDebug(bool aDebug) { mDebug = aDebug; }

   //! Set if the formation should be attached to its parent.
   void SetAttached(bool aAttached) { mAttached = aAttached; }

   FormationCommand* GetCurrentCommand() const;
   bool              ExecuteCommand(std::unique_ptr<FormationCommand> aCommandPtr, double aSimTime);

   //! Invoke a callable entity on each sub-formation of this formation.
   //!
   //! The Callable type is anything that has an invocation operation with
   //! the signature void (*)(Formation*). Any state needed by the operation
   //! should be provided in some form to that callable entity, be that a
   //! capture in a lambda, or member variables in a functor.
   //!
   //! \param aFunction - a callable entity that will be invoked on each sub-formation.
   template<typename Callable>
   void InvokeOnSubFormations(Callable aFunction)
   {
      for (auto& i : mChildren)
      {
         aFunction(i.second);
      }
   }

   static RelativeManeuverKinematics ComputeOffsetKinematics(const RelativeManeuverKinematics& aKinematics,
                                                             const FormationOffset&            aOffset,
                                                             const UtVec3d&                    aOmega);

protected:
   Formation(const Formation&   aOther,
             FormationManager&  aManager,
             Formation*         aParentPtr,
             const std::string& aQualifiedName,
             const std::string& aName);

private:
   void         UpdateQualifiedName();
   bool         UpdateChildRelativeName(const std::string& aOldName, const std::string& aNewName);
   void         AddDescendantNames(std::vector<std::string>& aNames) const;
   virtual bool SetMemberNameP(const std::string& aPlatformName);

   std::string                                 mQualifiedName;
   std::string                                 mName;
   Formation*                                  mParentPtr;
   FormationOffset                             mOffset;
   std::unordered_map<std::string, Formation*> mChildren;
   std::string                                 mLeadChild;
   std::unique_ptr<FormationCommand>           mCurrCommandPtr;
   FormationManager&                           mManager;
   bool                                        mDebug;
   bool                                        mAttached;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATION_HPP
