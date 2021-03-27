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
#include "InstanceProperties.h"
#include "Misc/Misc.h"

namespace CAULDRON_VK
{    
    VkResult InstanceProperties::Init()
    {   
        // Query instance layers.
        //
        VkResult res = m_instanceInitHelp.EnumerateLayers();
        assert(res == VK_SUCCESS);
        m_instanceInitHelp.EnableAllLayers(false);

        // Query instance extensions.
        //
        res = m_instanceInitHelp.EnumerateExtensions();
        assert(res == VK_SUCCESS);
        m_instanceInitHelp.EnableAllExtensions(false);

        return res;
    }

    bool InstanceProperties::AddInstanceLayerName(const char *instanceLayerName)
    {
        if(m_instanceInitHelp.EnableLayer(instanceLayerName, true))
        {
            return true;
        }

        Trace("Opps!! The instance layer '%s' has not been found\n", instanceLayerName);

        return false;
    }

    bool InstanceProperties::AddInstanceExtensionName(const char *instanceExtensionName)
    {
        if(m_instanceInitHelp.EnableExtension(instanceExtensionName, true))
        {
            return true;
        }

        Trace("Opps!! The instance extension '%s' has not been found\n", instanceExtensionName);

        return false;
    }
}
