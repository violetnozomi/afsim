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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisJammingTechniqueEnums.hpp"

#include <cassert>
#include <sstream>
#include <string>

#include "DisJammingTechniqueRecord.hpp"
#include "DisTypes.hpp"

namespace DisEnum
{
namespace jamming_technique
{
const std::string& KindToString(DisEnum8 kind)
{
   static const std::string KindStr[] = {"-", "Noise", "Deception", "Deception and Noise", "Special"};
   size_t                   index     = KindIsValid(kind) ? static_cast<size_t>(kind) : 0;
   return KindStr[index];
}

namespace noise
{
const std::string& CategoryToString(DisEnum8 cat)
{
   static const std::string CategoryStr[] = {"-",
                                             "Amplitude Modulation (AM) Noise",
                                             "Barrage Noise",
                                             "Bistatic Clutter",
                                             "Comb",
                                             "Cooperative Blinked Noise (CBN)",
                                             "Doppler Noise",
                                             "Frequency Modulation (FM) by Noise",
                                             "Impulse Noise",
                                             "Partial Band",
                                             "Pseudorandom AM",
                                             "Pulse Noise",
                                             "Quasi-Noise (aka Pseudorandom)",
                                             "Range Bin Masking (RBM) (aka Cover Pulse)",
                                             "Repeater Noise",
                                             "Spot Noise",
                                             "Swept Noise (aka Swept Spot Noise, Sweep)",
                                             "Velocity Bin Masking (VBM)"};
   size_t                   index         = CategoryIsValid(cat) ? (static_cast<size_t>(cat) / 5) : 0;
   return CategoryStr[index];
}
namespace barrage
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "Click", "Source Noise"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace barrage
namespace rbm
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "Range Bin Masking with Velocity Bin Masking"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace rbm
namespace repeater
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "Narrowband Repeater Noise", "Wide Band Repeater Noise"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace repeater
namespace spot
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-",
                                                "Automatic Spot Noise (ASJ)",
                                                "Blinking Spot Noise",
                                                "Burst Spot Noise",
                                                "Doppler Spot Noise",
                                                "Skirt Frequency"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace spot
namespace swept
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "Frequency Swept", "Swept AM"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace swept
} // namespace noise

namespace deception
{
const std::string& CategoryToString(DisEnum8 cat)
{
   static const std::string CategoryStr[] = {"-",
                                             "Analyzer",
                                             "Angle",
                                             "Angle and Gate Stealer",
                                             "Angle and False Target",
                                             "Angle and Random Range Programs (RANRAP)",
                                             "Angle and Velocity",
                                             "Automatic Gain Control (AGC)",
                                             "AGC and Gate Stealer",
                                             "Colinear",
                                             "Constant False Alarm Rate",
                                             "Double Cross",
                                             "Down Link",
                                             "False Target",
                                             "Figure Eight",
                                             "Gate Stealer",
                                             "Gate Stealer and Repeater",
                                             "Glint Enhance",
                                             "Image Frequency",
                                             "Jittered Pulse Repetition Frequency",
                                             "Jittered Pulse Width",
                                             "Pseudorandom Noise (PRN)",
                                             "Pulse",
                                             "Pulse Compression Deception",
                                             "Random Range Programs",
                                             "Refraction",
                                             "Repeater",
                                             "Scintillation",
                                             "Serrodyne",
                                             "Velocity"};
   size_t                   index         = CategoryIsValid(cat) ? (static_cast<size_t>(cat) / 5) : 0;
   return CategoryStr[index];
}
namespace angle
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-",
                                                "Angle Gete Walk-Off",
                                                "Cooperative Angle (CAJ)",
                                                "Cross-Eye",
                                                "Cross-Polarization",
                                                "Delta",
                                                "Inverse Gain (aka Inverse Amplitude)",
                                                "Sea-Bounced",
                                                "Swept Square Wave (SSW)",
                                                "Terrain Bounce"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace angle
namespace angle_and_gate
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-",
                                                "Cross-Polarization and Range Gate Pull-Off (RGPO)",
                                                "Cross-Polarization and Velocity Gate Pull-Off (VGPO)",
                                                "Cross-Polarization, RGPO and VGPO",
                                                "Inverse Gain and RGPO",
                                                "Inverse Gain and RGPO and VGPO",
                                                "Inverse Gain and VGPO",
                                                "RGPO and SSW",
                                                "SSW and VGPO"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace angle_and_gate
namespace angle_and_false_target
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-",
                                                "Angle and Velocity False Targets (VFT)",
                                                "Range False Targets (RFT) and Inverse Gain",
                                                "RFT and SSW"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
namespace vft
{
const std::string& SpecificToString(DisEnum8 spec)
{
   static const std::string SpecificStr[] = {"-", "Inverse Gain and VFT", "SSW and VFT"};
   size_t                   index         = SpecificIsValid(spec) ? (static_cast<size_t>(spec) / 5) : 0;
   return SpecificStr[index];
}
} // namespace vft
} // namespace angle_and_false_target
namespace angle_and_ranrap
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "RANRAP and SSW"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace angle_and_ranrap
namespace angle_and_velocity
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "Inverse Gain and VBM", "SSW and VBM"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace angle_and_velocity
namespace agc_and_gate
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "AGC and RGPO", "AGC and VGPO", "AGC and RGPO and VGPO"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace agc_and_gate
namespace false_target
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-",
                                                "Coherent False Targets",
                                                "False Doppler Target (FDT)",
                                                "Multiple False Targets",
                                                "Range False Targets (RFT)",
                                                "Transponder",
                                                "Velocity False Targets (VFT)"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace false_target
namespace gate
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-",
                                                "Chirp Gate Stealer (CGS)",
                                                "Range Gate Pull-Off (RGPO)",
                                                "RGPO and VGPO",
                                                "VGPO (aka Velocity Gate Stealer, VGS)"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace gate
namespace gate_and_repeater
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "Repeater Swept Amplitude Modulation (RSAM) and VGPO"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace gate_and_repeater
namespace repeater
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-",
                                                "Continuous Wave Repeater",
                                                "Repeater Noise",
                                                "Multiple Frequency Repeater (MFR)",
                                                "Narrow Band Repeater Noise (NBRN)",
                                                "Random Doppler (RD)",
                                                "Repeater Digital Radio Frequency Memory (DRFM)"
                                                "Repeater Swept Amplitude Modulation (RSAM)"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace repeater
} // namespace deception
namespace deception_and_noise
{
const std::string& CategoryToString(DisEnum8 cat)
{
   static const std::string CategoryStr[] = {"-",
                                             "Angle and Noise",
                                             "Angle and FM",
                                             "Angle and Pseudorandom AM",
                                             "Angle and Spot",
                                             "Gate Stealer and Noise",
                                             "False Target Deception and Swept Noise"};
   size_t                   index         = CategoryIsValid(cat) ? (static_cast<size_t>(cat) / 5) : 0;
   return CategoryStr[index];
}
namespace angle_and_noise
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "Angle and Barrage"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
namespace barrage
{
const std::string& SpecificToString(DisEnum8 spec)
{
   static const std::string SpecificStr[] = {"-", "Barrage and Inverse Gain", "Barrage and SSW"};
   size_t                   index         = SpecificIsValid(spec) ? (static_cast<size_t>(spec) / 5) : 0;
   return SpecificStr[index];
}
} // namespace barrage
} // namespace angle_and_noise
namespace angle_and_fm
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "FM by Noise and Inverse Gain", "FM by Noise and SSW"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace angle_and_fm
namespace angle_and_am
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-", "Inverse Gain and Pseudorandom AM", "Pseudorandom AM and SSW"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace angle_and_am
namespace angle_and_spot
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {"-",
                                                "Inverse Gain and Low Level Noise",
                                                "Inverse Gain and Spot Noise",
                                                "Spot and SSW"};
   size_t                   index            = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
} // namespace angle_and_spot
namespace gate_and_noise
{
const std::string& SubcategoryToString(DisEnum8 scat)
{
   static const std::string SubcategoryStr[] = {
      "-",
      "Noise and RGPO",
      "Noise and VGPO",
   };
   size_t index = SubcategoryIsValid(scat) ? (static_cast<size_t>(scat) / 5) : 0;
   return SubcategoryStr[index];
}
namespace rgpo
{
const std::string& SpecificToString(DisEnum8 spec)
{
   static const std::string SpecificStr[] = {"-", "Low Level Noise and RGPO"};
   size_t                   index         = SpecificIsValid(spec) ? (static_cast<size_t>(spec) / 5) : 0;
   return SpecificStr[index];
}
} // namespace rgpo
namespace vgpo
{
const std::string& SpecificToString(DisEnum8 spec)
{
   static const std::string SpecificStr[] = {"-", "Low Level Noise and VGPO"};
   size_t                   index         = SpecificIsValid(spec) ? (static_cast<size_t>(spec) / 5) : 0;
   return SpecificStr[index];
}
} // namespace vgpo
} // namespace gate_and_noise
} // namespace deception_and_noise
namespace special
{
const std::string& CategoryToString(DisEnum8 cat)
{
   static const std::string CategoryStr[] = {
      "-",
      "Super Jam",
   };
   size_t index = CategoryIsValid(cat) ? static_cast<size_t>(cat) : 0;
   return CategoryStr[index];
}
} // namespace special

bool IsValid(const DisJammingTechniqueRecord& jtr)
{
   if (!KindIsValid(jtr.GetKind()))
   {
      return false;
   }
   switch (jtr.GetKind())
   {
   case Noise:
   {
      using namespace noise;
      if (!CategoryIsValid(jtr.GetCategory()))
      {
         return false;
      }
      switch (jtr.GetCategory())
      {
      case BarrageNoise:
      {
         using namespace barrage;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case RangeBinMasking:
      {
         using namespace rbm;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case RepeaterNoise:
      {
         using namespace repeater;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case SpotNoise:
      {
         using namespace spot;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case SweptNoise:
      {
         using namespace swept;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      default:
      {
         // No further specifics
         return (jtr.GetSubcategory() == 0 && jtr.GetSpecific() == 0);
      }
      } // switch(jtr.GetCategory());
      break;
   } // case Noise:
   case Deception:
   {
      using namespace deception;
      if (!CategoryIsValid(jtr.GetCategory()))
      {
         return false;
      }
      switch (jtr.GetCategory())
      {
      case Angle:
      {
         using namespace angle;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case AngleAndGateStealer:
      {
         using namespace angle_and_gate;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case AngleAndFalseTarget:
      {
         using namespace angle_and_false_target;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case AngleAndRandomRangePrograms:
      {
         using namespace angle_and_ranrap;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case AngleAndVelocity:
      {
         using namespace angle_and_velocity;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case AGC_AndGateStealer:
      {
         using namespace agc_and_gate;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case FalseTarget:
      {
         using namespace false_target;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case GateStealer:
      {
         using namespace gate;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case GateStealerAndRepeater:
      {
         using namespace gate_and_repeater;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case Repeater:
      {
         using namespace repeater;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      default:
      {
         // No further specifics
         return (jtr.GetSubcategory() == 0 && jtr.GetSpecific() == 0);
      }
      } // switch(jtr.GetCategory());
      break;
   } // case Deception:
   case DeceptionAndNoise:
   {
      using namespace deception_and_noise;
      if (!CategoryIsValid(jtr.GetCategory()))
      {
         return false;
      }
      switch (jtr.GetCategory())
      {
      case AngleAndNoise:
      {
         using namespace angle_and_noise;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         switch (jtr.GetSubcategory())
         {
         case AngleAndBarrage:
         {
            using namespace barrage;
            return SpecificIsValid(jtr.GetSpecific());
         }
         default:
         {
            // No further specifics
            return (jtr.GetSpecific() == 0);
         }
         } // switch(jtr.GetSubcategory())
         break;
      }
      case AngleAndFM_ByNoise:
      {
         using namespace angle_and_fm;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case AngleAndPseudorandomAM:
      {
         using namespace angle_and_am;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case AngleAndSpot:
      {
         using namespace angle_and_spot;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         // No further specifics
         return (jtr.GetSpecific() == 0);
      }
      case GateStealerAndNoise:
      {
         using namespace gate_and_noise;
         if (!SubcategoryIsValid(jtr.GetSubcategory()))
         {
            return false;
         }
         switch (jtr.GetSubcategory())
         {
         case NoiseAndRGPO:
         {
            using namespace rgpo;
            return SpecificIsValid(jtr.GetSpecific());
         }
         case NoiseAndVGPO:
         {
            using namespace vgpo;
            return SpecificIsValid(jtr.GetSpecific());
         }
         default:
         {
            // No further specifics
            return (jtr.GetSpecific() == 0);
         }
         } // switch(jtr.GetSubcategory())
         break;
      }
      default:
      {
         // No further specifics
         return (jtr.GetSubcategory() == 0 && jtr.GetSpecific() == 0);
      }
      } // switch(jtr.GetCategory())
   }    // case DeceptionAndNoise:
   case Special:
   {
      using namespace special;
      if (!CategoryIsValid(jtr.GetCategory()))
      {
         return false;
      }
      // No further specifics
      return (jtr.GetSubcategory() == 0 && jtr.GetSpecific() == 0);
   } // case Special:
   default:
   {
      // All have to be 0 for this to be good
      return (jtr.GetCategory() == 0 && jtr.GetSubcategory() == 0 && jtr.GetSpecific() == 0);
   }
   } // switch(jtr.GetKind())
} // bool RecordIsValid()

std::string ToString(const DisJammingTechniqueRecord& jtr)
{
   std::stringstream ss;
   ss << static_cast<DisUint16>(jtr.GetKind()) << ":";
   ss << static_cast<DisUint16>(jtr.GetCategory()) << ":";
   ss << static_cast<DisUint16>(jtr.GetSubcategory()) << ":";
   ss << static_cast<DisUint16>(jtr.GetSpecific()) << " [";
   ss << KindToString(jtr.GetKind()) << "|";

   if (!KindIsValid(jtr.GetKind()))
   {
      ss << "**INVALID**]";
      return ss.str();
   }
   switch (jtr.GetKind())
   {
   case Noise:
   {
      using namespace noise;
      ss << CategoryToString(jtr.GetCategory()) << "|";
      switch (jtr.GetCategory())
      {
      case BarrageNoise:
      {
         using namespace barrage;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case RangeBinMasking:
      {
         using namespace rbm;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case RepeaterNoise:
      {
         using namespace repeater;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case SpotNoise:
      {
         using namespace spot;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case SweptNoise:
      {
         using namespace swept;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      default:
      {
         ss << "-|-]";
         break;
      }
      } // switch(jtr.GetCategory());
      break;
   } // case Noise:
   case Deception:
   {
      using namespace deception;
      ss << CategoryToString(jtr.GetCategory()) << "|";
      switch (jtr.GetCategory())
      {
      case Angle:
      {
         using namespace angle;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case AngleAndGateStealer:
      {
         using namespace angle_and_gate;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case AngleAndFalseTarget:
      {
         using namespace angle_and_false_target;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case AngleAndRandomRangePrograms:
      {
         using namespace angle_and_ranrap;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case AngleAndVelocity:
      {
         using namespace angle_and_velocity;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case AGC_AndGateStealer:
      {
         using namespace agc_and_gate;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case FalseTarget:
      {
         using namespace false_target;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case GateStealer:
      {
         using namespace gate;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case GateStealerAndRepeater:
      {
         using namespace gate_and_repeater;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case Repeater:
      {
         using namespace repeater;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      default:
      {
         ss << "-|-]";
         break;
      }
      } // switch(jtr.GetCategory());
      break;
   } // case Deception:
   case DeceptionAndNoise:
   {
      using namespace deception_and_noise;
      ss << CategoryToString(jtr.GetCategory()) << "|";
      switch (jtr.GetCategory())
      {
      case AngleAndNoise:
      {
         using namespace angle_and_noise;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|";
         switch (jtr.GetSubcategory())
         {
         case AngleAndBarrage:
         {
            using namespace barrage;
            ss << SpecificToString(jtr.GetSpecific()) << "]";
            break;
         }
         default:
         {
            ss << "-]";
            break;
         }
         } // switch(jtr.GetSubcategory())
         break;
      }
      case AngleAndFM_ByNoise:
      {
         using namespace angle_and_fm;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case AngleAndPseudorandomAM:
      {
         using namespace angle_and_am;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case AngleAndSpot:
      {
         using namespace angle_and_spot;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|-]";
         break;
      }
      case GateStealerAndNoise:
      {
         using namespace gate_and_noise;
         ss << SubcategoryToString(jtr.GetSubcategory()) << "|";
         switch (jtr.GetSubcategory())
         {
         case NoiseAndRGPO:
         {
            using namespace rgpo;
            ss << SpecificToString(jtr.GetSpecific()) << "]";
            break;
         }
         case NoiseAndVGPO:
         {
            using namespace vgpo;
            ss << SpecificToString(jtr.GetSpecific()) << "]";
            break;
         }
         default:
         {
            // No further specifics
            ss << "-]";
            break;
         }
         } // switch(jtr.GetSubcategory())
         break;
      }
      default:
      {
         // No further specifics
         ss << "-|-]";
         break;
      }
      } // switch(jtr.GetCategory())
   }    // case DeceptionAndNoise:
   case Special:
   {
      using namespace special;
      ss << CategoryToString(jtr.GetCategory()) << "|-|-]";
      break;
   } // case Special:
   default:
   {
      ss << "-|-|-]";
      break;
   }
   } // switch(jtr.GetKind())
   return ss.str();
} // bool RecordIsValid()
} // namespace jamming_technique
} // namespace DisEnum
