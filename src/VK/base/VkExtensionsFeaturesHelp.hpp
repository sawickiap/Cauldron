#pragma once

#include <vector>
#include <algorithm>
#include <cstdint>
#include <cassert>
#include <cstring>

namespace VKEFH
{

template<typename MainT, typename NewT>
inline void PnextChainPushFront(MainT* mainStruct, NewT* newStruct)
{
    newStruct->pNext = (decltype(newStruct->pNext))mainStruct->pNext;
    mainStruct->pNext = (decltype(mainStruct->pNext))newStruct;
}

struct EnabledItem
{
    const char* m_Name;
    bool m_Supported, m_Enabled;
};

class EnabledItemVector
{
public:
    std::vector<EnabledItem> m_Items;
    std::vector<const char*> m_EnabledItemNames;

    bool IsSupported(const char* name) const
    {
        size_t index = Find(name);
        if(index != SIZE_MAX)
            return m_Items[index].m_Supported;
        assert(0 && "You can query only for items specified in VkExtensionsFeatures.inl.");
        return false;
    }
    bool IsEnabled(const char* name) const
    {
        size_t index = Find(name);
        if(index != SIZE_MAX)
            return m_Items[index].m_Enabled;
        assert(0 && "You can query only for items specified in VkExtensionsFeatures.inl.");
        return false;
    }
    bool Enable(const char* name, bool enabled)
    {
        size_t index = Find(name);
        if(index != SIZE_MAX)
        {
            if(enabled && m_Items[index].m_Supported)
            {
                m_Items[index].m_Enabled = true;
                return true;
            }
            m_Items[index].m_Enabled = false;
            return false;
        }
        assert(0 && "You can enable only for items specified in VkExtensionsFeatures.inl.");
        return false;
    }
    void EnableAll(bool enabled)
    {
        for(size_t i = 0, count = m_Items.size(); i < count; ++i)
        {
            m_Items[i].m_Enabled = enabled && m_Items[i].m_Supported;
        }
    }
    void PrepareEnabled()
    {
        m_EnabledItemNames.clear();
        for(size_t i = 0, count = m_Items.size(); i < count; ++i)
        {
            if(m_Items[i].m_Enabled)
            {
                assert(m_Items[i].m_Supported);
                m_EnabledItemNames.push_back(m_Items[i].m_Name);
            }
        }
    }

private:
    size_t Find(const char* name) const
    {
        for(size_t i = 0, count = m_Items.size(); i < count; ++i)
        {
            if(strcmp(name, m_Items[i].m_Name) == 0)
            {
                return i;
            }
        }
        return SIZE_MAX;
    }
};

class InitHelpBase
{
public:
    bool IsExtensionSupported(const char* extensionName) const
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        return m_Extensions.IsSupported(extensionName);
    }
    bool IsExtensionEnabled(const char* extensionName) const
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        return m_Extensions.IsEnabled(extensionName);
    }
    bool EnableExtension(const char* extensionName, bool enabled)
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        return m_Extensions.Enable(extensionName, enabled);
    }
    void EnableAllExtensions(bool enabled)
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        m_Extensions.EnableAll(enabled);
    }

    uint32_t GetEnabledExtensionCount() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return (uint32_t)m_Extensions.m_EnabledItemNames.size();
    }
    const char* const* GetEnabledExtensionNames() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return !m_Extensions.m_EnabledItemNames.empty() ? m_Extensions.m_EnabledItemNames.data() : nullptr;
    }

protected:
    bool m_ExtensionsEnumerated = false;
    bool m_CreationPrepared = false;
    EnabledItemVector m_Extensions;

    void LoadExtensions(const VkExtensionProperties* extProps, size_t extPropCount)
    {
        assert(!m_ExtensionsEnumerated && "You should call EnumerateExtensions only once.");
        for(size_t extPropIndex = 0; extPropIndex < extPropCount; ++extPropIndex)
        {
            for(size_t extIndex = 0, extCount = m_Extensions.m_Items.size(); extIndex < extCount; ++extIndex)
            {
                if(strcmp(extProps[extPropIndex].extensionName, m_Extensions.m_Items[extIndex].m_Name) == 0)
                {
                    m_Extensions.m_Items[extIndex].m_Supported = true;
                    m_Extensions.m_Items[extIndex].m_Enabled = true;
                }
            }
        }
        m_ExtensionsEnumerated = true;
    }

    void PrepareEnabledExtensionNames()
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        m_Extensions.PrepareEnabled();
    }
};

class InstanceInitHelp : public InitHelpBase
{
public:
    InstanceInitHelp()
    {
#define VKEFH_INSTANCE_EXTENSION(extensionName)   m_Extensions.m_Items.push_back({(extensionName), false, false});
#define VKEFH_INSTANCE_LAYER(layerName)   m_Layers.m_Items.push_back({(layerName), false, false});
#define VKEFH_DEVICE_EXTENSION(extensionName)
#define VKEFH_DEVICE_FEATURE_STRUCT(structName, sType)

#include "VkExtensionsFeatures.inl"

#undef VKEFH_DEVICE_EXTENSION
#undef VKEFH_INSTANCE_LAYER
#undef VKEFH_DEVICE_FEATURE_STRUCT
#undef VKEFH_INSTANCE_EXTENSION
    }

    VkResult EnumerateExtensions()
    {
        uint32_t extPropCount = 0;
        VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &extPropCount, nullptr);
        if(res != VK_SUCCESS)
            return res;
        if(extPropCount)
        {
            std::vector<VkExtensionProperties> extProps(extPropCount);
            res = vkEnumerateInstanceExtensionProperties(nullptr, &extPropCount, extProps.data());
            if(res != VK_SUCCESS)
                return res;
            LoadExtensions(extProps.data(), extPropCount);
        }
        return VK_SUCCESS;
    }

    VkResult EnumerateLayers()
    {
        assert(!m_LayersEnumerated && "You should call EnumerateLayers only once.");
        uint32_t layerPropCount = 0;
        VkResult res = vkEnumerateInstanceLayerProperties(&layerPropCount, nullptr);
        if(res != VK_SUCCESS)
            return res;
        if(layerPropCount)
        {
            std::vector<VkLayerProperties> layerProps(layerPropCount);
            res = vkEnumerateInstanceLayerProperties(&layerPropCount, layerProps.data());
            if(res != VK_SUCCESS)
                return res;
            LoadLayers(layerProps.data(), layerPropCount);
        }
        m_LayersEnumerated = true;
        return VK_SUCCESS;
    }

    bool IsLayerSupported(const char* layerName) const
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        return m_Layers.IsSupported(layerName);
    }
    bool IsLayerEnabled(const char* layerName) const
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        return m_Layers.IsEnabled(layerName);
    }
    bool EnableLayer(const char* layerName, bool enabled)
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        return m_Layers.Enable(layerName, enabled);
    }
    void EnableAllLayers(bool enabled)
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        m_Layers.EnableAll(enabled);
    }

    void PrepareCreation()
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        PrepareEnabledExtensionNames();
        m_Layers.PrepareEnabled();
        m_CreationPrepared = true;
    }

    uint32_t GetEnabledLayerCount() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return (uint32_t)m_Layers.m_EnabledItemNames.size();
    }
    const char* const* GetEnabledLayerNames() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return !m_Layers.m_EnabledItemNames.empty() ? m_Layers.m_EnabledItemNames.data() : nullptr;
    }

private:
    bool m_LayersEnumerated = false;
    EnabledItemVector m_Layers;

    void LoadLayers(const VkLayerProperties* layerProps, size_t layerPropCount)
    {
        for(size_t layerPropIndex = 0; layerPropIndex < layerPropCount; ++layerPropIndex)
        {
            for(size_t layerIndex = 0, layerCount = m_Layers.m_Items.size(); layerIndex < layerCount; ++layerIndex)
            {
                if(strcmp(layerProps[layerPropIndex].layerName, m_Layers.m_Items[layerIndex].m_Name) == 0)
                {
                    m_Layers.m_Items[layerIndex].m_Supported = true;
                    m_Layers.m_Items[layerIndex].m_Enabled = true;
                }
            }
        }
    }
};

class DeviceInitHelp : public InitHelpBase
{
#define VKEFH_INSTANCE_EXTENSION(extensionName)
#define VKEFH_INSTANCE_LAYER(layerName)
#define VKEFH_DEVICE_EXTENSION(extensionName)
#define VKEFH_DEVICE_FEATURE_STRUCT(structName, sType) \
    private: structName m_##structName = { (sType) }; \
    public: structName& Get##structName() \
    { \
        assert(m_PhysicalDeviceFeaturesQueried && "You need to call GetPhysicalDeviceFeatures first."); \
        return m_##structName; \
    }

#include "VkExtensionsFeatures.inl"

#undef VKEFH_INSTANCE_EXTENSION
#undef VKEFH_INSTANCE_LAYER
#undef VKEFH_DEVICE_EXTENSION
#undef VKEFH_DEVICE_FEATURE_STRUCT

public:
    DeviceInitHelp()
    {
#define VKEFH_INSTANCE_EXTENSION(extensionName)
#define VKEFH_INSTANCE_LAYER(layerName)
#define VKEFH_DEVICE_EXTENSION(extensionName)   m_Extensions.m_Items.push_back({(extensionName), false, false});
#define VKEFH_DEVICE_FEATURE_STRUCT(structName, sType)   m_FeatureStructs.push_back({(#structName), (sType), (VkBaseInStructure*)(&m_##structName), true});

#include "VkExtensionsFeatures.inl"

#undef VKEFH_INSTANCE_EXTENSION
#undef VKEFH_INSTANCE_LAYER
#undef VKEFH_DEVICE_EXTENSION
#undef VKEFH_DEVICE_FEATURE_STRUCT
    }

    VkResult EnumerateExtensions(VkPhysicalDevice physicalDevice)
    {
        assert(physicalDevice);
        uint32_t extPropCount = 0;
        VkResult res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extPropCount, nullptr);
        if(res != VK_SUCCESS)
        {
            return res;
        }
        std::vector<VkExtensionProperties> extProps{extPropCount};
        if(extPropCount)
        {
            res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extPropCount, extProps.data());
            if(res != VK_SUCCESS)
            {
                return res;
            }
            LoadExtensions(extProps.data(), extPropCount);
        }
        return VK_SUCCESS;
    }

    void GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice)
    {
        assert(physicalDevice);
        assert(!m_PhysicalDeviceFeaturesQueried && "You should call GetPhysicalDeviceFeatures only once.");

        assert(m_Features2.sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
        m_Features2.pNext = nullptr;

        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(m_FeatureStructs[structIndex].m_Enabled)
            {
                assert(m_FeatureStructs[structIndex].m_StructPtr->sType == m_FeatureStructs[structIndex].m_sType);
                m_FeatureStructs[structIndex].m_StructPtr->pNext = nullptr;
                PnextChainPushFront(&m_Features2, m_FeatureStructs[structIndex].m_StructPtr);
            }
        }

        vkGetPhysicalDeviceFeatures2(physicalDevice, &m_Features2);

        m_PhysicalDeviceFeaturesQueried = true;
    }

    VkPhysicalDeviceFeatures& GetFeatures()
    {
        assert(m_PhysicalDeviceFeaturesQueried && "You need to call GetPhysicalDeviceFeatures first.");
        return m_Features2.features;
    }

    bool IsFeatureStructEnabled(const char* structName) const
    {
        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(strcmp(m_FeatureStructs[structIndex].m_Name, structName) == 0)
            {
                return m_FeatureStructs[structIndex].m_Enabled;
            }
        }
        assert(0 && "You can query only for feature structs specified in VkExtensionsFeatures.inl.");
        return false;
    }
    void EnableFeatureStruct(const char* structName, bool enabled)
    {
        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(strcmp(m_FeatureStructs[structIndex].m_Name, structName) == 0)
            {
                m_FeatureStructs[structIndex].m_Enabled = enabled;
                return;
            }
        }
        assert(0 && "You can enable only feature structs specified in VkExtensionsFeatures.inl.");
    }

    bool IsFeatureStructEnabled(VkStructureType sType) const
    {
        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(m_FeatureStructs[structIndex].m_sType == sType)
            {
                return m_FeatureStructs[structIndex].m_Enabled;
            }
        }
        assert(0 && "You can query only for feature structs specified in VkExtensionsFeatures.inl.");
        return false;
    }
    void EnableFeatureStruct(VkStructureType sType, bool enabled)
    {
        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(m_FeatureStructs[structIndex].m_sType == sType)
            {
                m_FeatureStructs[structIndex].m_Enabled = enabled;
                return;
            }
        }
        assert(0 && "You can enable only feature structs specified in VkExtensionsFeatures.inl.");
    }

    void PrepareCreation()
    {
        assert(m_ExtensionsEnumerated && "You need to call EnumerateExtensions first.");
        assert(m_PhysicalDeviceFeaturesQueried && "You need to call GetPhysicalDeviceFeatures first.");

        PrepareEnabledExtensionNames();

        assert(m_Features2.sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
        m_Features2.pNext = NULL;
        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(m_FeatureStructs[structIndex].m_Enabled)
            {
                assert(m_FeatureStructs[structIndex].m_StructPtr->sType == m_FeatureStructs[structIndex].m_sType);
                m_FeatureStructs[structIndex].m_StructPtr->pNext = nullptr;
                PnextChainPushFront(&m_Features2, m_FeatureStructs[structIndex].m_StructPtr);
            }
        }
        
        m_CreationPrepared = true;
    }

    const void* GetFeaturesChain() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return &m_Features2;
    }

private:
    bool m_PhysicalDeviceFeaturesQueried = false;

    struct FeatureStruct
    {
        const char* m_Name;
        VkStructureType m_sType;
        VkBaseInStructure* m_StructPtr;
        bool m_Enabled;
    };
    std::vector<FeatureStruct> m_FeatureStructs;

    VkPhysicalDeviceFeatures2 m_Features2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
};


} // namespace VKEFH
