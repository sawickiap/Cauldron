// AMD AMDUtils code
// 
// Copyright(c) 2018 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#include "stdafx.h"

#include "ExtGPUValidation.h"
#include "Misc/Misc.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

namespace CAULDRON_VK
{
    static VkValidationFeatureEnableEXT enabledValidationFeatures[] = { VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT };

    bool ExtGPUValidationCheckExtensions(InstanceProperties *pIP)
    {
        bool bFp16Enabled = true;
        if (pIP->AddInstanceExtensionName(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME) == false)
        {
            Trace(format("GPU validation disabled, missing extension: %s\n", VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME));
            return false;
        }
        
        pIP->m_instanceInitHelp.GetVkValidationFeaturesEXT().enabledValidationFeatureCount = 1;
        pIP->m_instanceInitHelp.GetVkValidationFeaturesEXT().pEnabledValidationFeatures = enabledValidationFeatures;
        pIP->m_instanceInitHelp.EnableFeatureStruct(VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT, true);

        return true;
    }
}


