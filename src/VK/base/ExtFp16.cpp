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

#include "ExtFp16.h"
#include "Misc/Misc.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

namespace CAULDRON_VK
{
    bool ExtFp16CheckExtensions(DeviceProperties *pDP)
    {
        std::vector<const char *> required_extension_names = { VK_KHR_16BIT_STORAGE_EXTENSION_NAME, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME };

        bool bFp16Enabled = true;
        for (auto& ext : required_extension_names)
        {
            if (pDP->AddDeviceExtensionName(ext) == false)
            {
                Trace(format("FP16 disabled, missing extension: %s\n", ext));
                bFp16Enabled = false;
            }

        }

        if (bFp16Enabled)
        {
            // Query 16 bit storage
            bFp16Enabled = bFp16Enabled && pDP->m_deviceInitHelp.GetVkPhysicalDevice16BitStorageFeatures().storageBuffer16BitAccess;
            // Query 16 bit ops
            bFp16Enabled = bFp16Enabled && pDP->m_deviceInitHelp.GetVkPhysicalDeviceFloat16Int8FeaturesKHR().shaderFloat16;
        }

        if (bFp16Enabled)
        {
            // Query 16 bit storage
            pDP->m_deviceInitHelp.EnableFeatureStruct(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, true);
            // Query 16 bit ops
            pDP->m_deviceInitHelp.EnableFeatureStruct(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR, true);
        }

        return bFp16Enabled;
    }
}
