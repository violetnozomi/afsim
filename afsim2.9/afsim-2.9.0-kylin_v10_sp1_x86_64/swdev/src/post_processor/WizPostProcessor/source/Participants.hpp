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

#ifndef PARTICIPANTS_HPP
#define PARTICIPANTS_HPP

// Qt
#include <QWidget>

#include "ProxyInterface.hpp"
#include "ReportWidget.hpp"

namespace Ui
{
class Participants;
}

namespace PostProcessor
{
// Participants Widget for Engagement Report
class Participants : public ReportWidget
{
      Q_OBJECT
   public:
      enum ParticipantType
      {
         cACTING,
         cAGAINST
      };

      explicit Participants(ParticipantType aParticipantType);
      ~Participants() override;


      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;
      void AddPlatforms(const QMap<ProxyInterface::Platform, bool>& aPlatforms);
      void RemovePlatforms(const QVector<ProxyInterface::Platform>& aPlatforms);

   private:
      Ui::Participants* mUIPtr;
      ParticipantType mParticipantType;
};
}
#endif // !PARTICIPANTS_HPP
