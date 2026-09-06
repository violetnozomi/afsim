// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ASTROLABEDOCKWIDGETBASE_HPP
#define ASTROLABEDOCKWIDGETBASE_HPP

#include "wsf_spaceg_export.h"

#include <memory>

#include <QDockWidget>

#include "OrbitPreviewManager.hpp"
#include "OrbitalMissionModel.hpp"
#include "ui_AstrolabeDockWidgetBase.h"

class WsfOrbitalMissionSequence;

namespace wsfg
{

namespace spaceg
{

class AstrolabeConfig;
class MissionVerificationResults;

class WSF_SPACEG_EXPORT AstrolabeDockWidgetBase : public QDockWidget
{
   Q_OBJECT

public:
   explicit AstrolabeDockWidgetBase(const AstrolabeConfig& aConfig,
                                    QWidget*               aParentPtr = nullptr,
                                    Qt::WindowFlags        aFlags     = Qt::WindowFlags{});
   ~AstrolabeDockWidgetBase() override = default;

   void ClearStatusMessage();
   void SetStatusMessage(const QString& aMessage);
   void SetWarningMessage(const QString& aMessage);
   void SetErrorMessage(const QString& aMessage);

   QString GetSelectedPlatform() const;
   QString GetRequestedPlatform() const;

   void SetMissionVerificationResults(const MissionVerificationResults& aResults);

   void FinishVerify(std::unique_ptr<WsfOrbitalMissionSequence> aSequencePtr);
   void FinishApply(std::unique_ptr<WsfOrbitalMissionSequence> aSequencePtr);

signals:
   void PlatformSelected(const QString& aPlatformName);

protected:
   bool CheckEventParameters(std::unique_ptr<WsfOrbitalMissionSequence> aSequencePtr);
   void HandleTopLevelChanged(bool aTopLevel);
   void HandleVisibilityChanged(bool aIsVisible);
   void HandleCurrentChanged(const QModelIndex& aCurrentIndex, bool aSaveModifications);
   void CreateConnections();
   void UpdateActionButtons();
   void OnVerify();
   void OnApply();

   // Note: The two phase approach to verification and application of the designed mission
   //       sequence with Astrolabe is because, in the case of Warlock, the GUI thread should
   //       not deal with simulation data. By breaking the operations into two phases, the
   //       Warlock Astrolabe plugin can avoid thread-unsafe operations.

   //! Begin the process of verification.
   //!
   //! Subclasses should implement this method to provide the specific details about how the
   //! use case produces a mission sequence from the model created in the GUI. The end result
   //! of this method should be to call directly, or cause to be called, the template
   //! method FinishVerify.
   virtual void InitialVerifyAction() = 0;

   //! Complete the process of verification.
   //!
   //! Subclasses should implement this method to provide the specific details about what
   //! actions to take to complete the verification. This typically means making use of a
   //! MissionVerifierBase subclass relevant for the use case in question.
   virtual void VerifyAction() = 0;

   //! Begin the process of application of the sequence.
   //!
   //! Subclasses should implement this method to provide the specific details about how the
   //! use case produces a mission sequence from the model created in the GUI. The end result
   //! of this method should be to call directly, or cause to be called, the template method
   //! FinishApply.
   virtual void InitialApplyAction() = 0;

   //! Complete the process of application of a mission sequence.
   //!
   //! Subclasses should implement this method to provide the specific details about how the
   //! mission sequence is to be applied.
   virtual void ApplyAction() = 0;

   Ui::AstrolabeDockWidgetBase          mUI;
   std::unique_ptr<OrbitalMissionModel> mCurrentModelPtr;
   OrbitPreviewManager                  mPreviewManager;
};

} // namespace spaceg

} // namespace wsfg

#endif // ASTROLABEDOCKWIDGETBASE_HPP
