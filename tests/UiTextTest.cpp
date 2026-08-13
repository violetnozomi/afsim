#include "NrmUiText.hpp"

#include <cassert>
#include <string>

int main()
{
   using WkNrm::UiText::TranslateCode;
   using WkNrm::UiText::TranslateCodeWithRaw;
   using WkNrm::UiText::TranslateListWithRaw;

   assert(TranslateCode("REJECTED") == "已拒绝");
   assert(TranslateCode("PARAMETERIZED_MODEL") == "参数化模型");
   assert(TranslateCode("LOW") == "低");
   assert(TranslateCode("L0_TOPOLOGY") == "L0 拓扑降级");
   assert(TranslateCode("bandwidthBps") == "链路带宽");
   assert(TranslateCode("RF_QUALITY_UNAVAILABLE") == "射频质量不可用");

   assert(TranslateCodeWithRaw("REJECTED") == "已拒绝（REJECTED）");
   assert(TranslateCodeWithRaw("LINK16") == "Link-16");
   assert(TranslateCodeWithRaw("custom-value") == "custom-value");

   assert(TranslateListWithRaw("NETWORK_PROFILE_CAPACITY,PDR_ESTIMATE") ==
          "网络配置容量（NETWORK_PROFILE_CAPACITY）、PDR估算（PDR_ESTIMATE）");
   assert(TranslateListWithRaw("") == "");
   return 0;
}
