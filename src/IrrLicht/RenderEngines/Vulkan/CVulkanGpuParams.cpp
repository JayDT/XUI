#include "CVulkanGpuParams.h"
#include "CVulkanCommandBuffer.h"
#include "CVulkanDriver.h"
#include "CVulkanShader.h"
#include "CVulkanUtility.h"
#include "CVulkanDevice.h"
#include "CVulkanTexture.h"
#include "CVulkanHardwareBuffer.h"
#include "CVulkanFramebuffer.h"
#include "CVulkanEventQuery.h"
#include "CVulkanSwapChain.h"
#include "CVulkanDescriptorSet.h"

#include "os.h"

using namespace irr;
using namespace irr::video;

VulkanGpuParams::VulkanGpuParams(CVulkanDriver* driver, CVulkanGLSLProgram* shader, GpuDeviceFlags deviceMask)
    : CVulkanDeviceResource(driver)
    , mPerDeviceData()
    , mDeviceMask(deviceMask)
    , mShader(shader)
{

}

VulkanGpuParams::~VulkanGpuParams()
{
    //std::unique_lock<std::mutex> lock(mMutex);

    //u32 numSets = mParamInfo->getNumSets();
    //for (u32 i = 0; i < _MAX_DEVICES; i++)
    //{
    //    if (mPerDeviceData[i].perSetData == nullptr)
    //        continue;
    //
    //    for (u32 j = 0; j < numSets; j++)
    //    {
    //        for (auto& entry : mPerDeviceData[i].perSetData[j].sets)
    //            entry->drop();
    //
    //        mPerDeviceData[i].perSetData[j].sets.~Vector<VulkanDescriptorSet*>();
    //    }
    //}
}

void VulkanGpuParams::initialize()
{
    VulkanDevice* devices[_MAX_DEVICES];
    
    VulkanUtility::getDevices(Driver, mDeviceMask, devices);
    
    u32 numDevices = 0;
    for (u32 i = 0; i < _MAX_DEVICES; i++)
    {
        if (devices[i] != nullptr)
            numDevices++;
    }
    
    u32 numParamBlocks = 0;
    u32 numTextures = mShader->GetTextureInfos().size();
    u32 numStorageTextures = 0;
    u32 numBuffers = mShader->GetBlockBufferInfos().size();
    u32 numSamplers = numTextures;
    u32 numSets = mShader->getLayout() ? 1 : 0;
    u32 numBindings = mShader->GetTextureInfos().size() + mShader->GetBlockBufferInfos().size();
    
    if (numSets == 0)
        return;
    
    bufferBindingSlot.resize(mShader->GetBlockBufferInfos().size());
    TextureBindingSlot.resize(mShader->GetTextureInfos().size());

    //Lock lock(mMutex); // Set write operations need to be thread safe
    
    for (irr::u8 i = 0; i < _MAX_DEVICES; i++)
    {
        if (devices[i] == nullptr)
            continue;
    
        mPerDeviceData[i].perSetData.resize(numSets);
        mPerDeviceData[i].sampledImages.resize(numTextures);
        mPerDeviceData[i].storageImages.resize(numStorageTextures);
    
        for (u32 j = 0; j < numSets; j++)
        {
            u32 numBindingsPerSet = numBindings;
    
            PerSetData& perSetData = mPerDeviceData[i].perSetData[j];
            perSetData.sets.resize(numSets);
    
            perSetData.writeSetInfos = numBindingsPerSet > 0 ? new VkWriteDescriptorSet[numBindingsPerSet] : nullptr;
            perSetData.writeInfos = new WriteInfo[numBindingsPerSet];
    
            VulkanDescriptorLayout* layout = mShader->getLayout();
            perSetData.numElements  = numBindingsPerSet;
            perSetData.latestSet    = devices[i]->createSet(layout);
            perSetData.sets[j] = perSetData.latestSet;
    
            const std::vector<VkDescriptorSetLayoutBinding>& perSetBindings = mShader->GetBindings();
            for (u32 k = 0; k < numBindingsPerSet; k++)
            {
                // Note: Instead of using one structure per binding, it's possible to update multiple at once
                // by specifying larger descriptorCount, if they all share type and shader stages.
                VkWriteDescriptorSet& writeSetInfo = perSetData.writeSetInfos[k];
                writeSetInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeSetInfo.pNext = nullptr;
                writeSetInfo.dstSet = VK_NULL_HANDLE;
                writeSetInfo.dstBinding = perSetBindings[k].binding;
                writeSetInfo.dstArrayElement = 0;
                writeSetInfo.descriptorCount = perSetBindings[k].descriptorCount;
                writeSetInfo.descriptorType = perSetBindings[k].descriptorType;
    
                bool isImage = writeSetInfo.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                               writeSetInfo.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
                               writeSetInfo.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    
                if (isImage)
                {
                    TextureBindingSlot[k - numBuffers] = k;

                    bool isLoadStore = writeSetInfo.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    
                    VkDescriptorImageInfo& imageInfo = perSetData.writeInfos[k].image;
                    imageInfo.sampler = mShader->GetDriver()->GetDummySampler();
    
                    if (isLoadStore)
                    {
                        imageInfo.imageView = mShader->GetDriver()->GetDummyTexture()->GetVkImages(i)->getView(false);
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                    }
                    else
                    {
                        imageInfo.imageView = mShader->GetDriver()->GetDummyTexture()->GetVkImages(i)->getView(false);
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    }
    
                    writeSetInfo.pImageInfo = &imageInfo;
                    writeSetInfo.pBufferInfo = nullptr;
                    writeSetInfo.pTexelBufferView = nullptr;
                }
                else
                {
                    bool useView = writeSetInfo.descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
                        writeSetInfo.descriptorType != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    
                    bufferBindingSlot[k] = k;

                    if (!useView)
                    {
                        VkDescriptorBufferInfo& bufferInfo = perSetData.writeInfos[k].buffer;
                        bufferInfo.offset = 0;
                        bufferInfo.range = VK_WHOLE_SIZE;
    
                        if (writeSetInfo.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                            bufferInfo.buffer = mShader->GetDriver()->GetDummyStorageBuffer()->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE)->Buffer->getHandle();
                        else
                            bufferInfo.buffer = mShader->GetDriver()->GetDummyStorageBuffer()->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)->Buffer->getHandle();
    
                        writeSetInfo.pBufferInfo = &bufferInfo;
                        writeSetInfo.pTexelBufferView = nullptr;
                    }
                    else
                    {
                        writeSetInfo.pBufferInfo = nullptr;
    
                        bool isLoadStore = writeSetInfo.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
                        VkBufferView bufferView;
                        if (isLoadStore)
                            bufferView = VK_NULL_HANDLE;
                        else
                            bufferView = VK_NULL_HANDLE;
    
                        perSetData.writeInfos[k].bufferView = bufferView;
                        writeSetInfo.pBufferInfo = nullptr;
                        writeSetInfo.pTexelBufferView = &perSetData.writeInfos[k].bufferView;
                    }
    
                    writeSetInfo.pImageInfo = nullptr;
                }
            }
        }
    }
}

void irr::video::VulkanGpuParams::reset()
{
    for (u32 i = 0; i < _MAX_DEVICES; i++)
    {
        for (PerSetData& perSetData : mPerDeviceData[i].perSetData)
        {
            perSetData.mSetsDirty = true;
        }
    }
}

void VulkanGpuParams::setParamBlockBuffer(u32 set, u32 slot, const CVulkanHardwareBuffer* paramBlockBuffer)
{
    if (bufferBindingSlot.size() <= slot)
        return;

    u32 bindingIdx = bufferBindingSlot[slot];

    const auto* bufferDesc = paramBlockBuffer->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE);
    for (u32 i = 0; i < _MAX_DEVICES; i++)
    {
        if (mPerDeviceData[i].perSetData.size() <= i)
            break;

        VulkanBuffer* bufferRes;
        if (bufferDesc)
            bufferRes = bufferDesc->Buffer;
        else
            bufferRes = nullptr;

        PerSetData& perSetData = mPerDeviceData[i].perSetData[set];
        if (perSetData.writeInfos[bindingIdx].buffer.buffer == bufferRes->getHandle())
            continue;

        if (bufferRes != nullptr)
        {
            VkBuffer buffer = bufferRes->getHandle();

            perSetData.writeInfos[bindingIdx].buffer.buffer = buffer;
        }
        else
        {
            if (perSetData.writeInfos[bindingIdx].buffer.buffer == mShader->GetDriver()->GetDummyStorageBuffer()->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE)->Buffer->getHandle())
                continue;

            perSetData.writeInfos[bindingIdx].buffer.buffer = mShader->GetDriver()->GetDummyStorageBuffer()->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE)->Buffer->getHandle();
        }

        perSetData.mSetsDirty = true;
    }
}

void VulkanGpuParams::setTexture(u32 set, u32 slot, const ITexture* texture, const TextureSurface& surface)
{
    if (TextureBindingSlot.size() <= slot)
        return;

    u32 bindingIdx = TextureBindingSlot[slot];
    
    const CVulkanTexture* vulkanTexture = static_cast<const CVulkanTexture*>(texture);
    for (u32 i = 0; i < _MAX_DEVICES; i++)
    {
        if (mPerDeviceData[i].perSetData.size() <= i)
            break;

        VulkanImage* imageRes;
        if (vulkanTexture != nullptr)
            imageRes = vulkanTexture->GetVkImages(i);
        else
            imageRes = nullptr;

        PerSetData& perSetData = mPerDeviceData[i].perSetData[set];
        if (imageRes != nullptr)
        {
            TextureSurface actualSurface = surface;
    
            if (mPerDeviceData[i].sampledImages[slot] == imageRes->getHandle())
                continue;

            perSetData.writeInfos[bindingIdx].image.imageView = imageRes->getView(actualSurface, false);
            mPerDeviceData[i].sampledImages[slot] = imageRes->getHandle();
        }
        else
        {
            if (mPerDeviceData[i].sampledImages[slot] == mShader->GetDriver()->GetDummyTexture()->GetVkImages(i)->getHandle())
                continue;

            perSetData.writeInfos[bindingIdx].image.imageView = mShader->GetDriver()->GetDummyTexture()->GetVkImages(i)->getView(false);
            mPerDeviceData[i].sampledImages[slot] = mShader->GetDriver()->GetDummyTexture()->GetVkImages(i)->getHandle();
        }

        perSetData.mSetsDirty = true;
    }
}

void VulkanGpuParams::setLoadStoreTexture(u32 set, u32 slot, const ITexture* texture, const TextureSurface& surface)
{
    if (TextureBindingSlot.size() <= slot)
        return;

    u32 bindingIdx = TextureBindingSlot[slot];

    const CVulkanTexture* vulkanTexture = static_cast<const CVulkanTexture*>(texture);
    for (u32 i = 0; i < _MAX_DEVICES; i++)
    {
        if (mPerDeviceData[i].perSetData.size() <= i)
            break;

        VulkanImage* imageRes;
        if (vulkanTexture != nullptr)
            imageRes = vulkanTexture->GetVkImages(i);
        else
            imageRes = nullptr;

        PerSetData& perSetData = mPerDeviceData[i].perSetData[set];
        if (imageRes != nullptr)
        {
            if (mPerDeviceData[i].storageImages[slot] == imageRes->getHandle())
                continue;

            perSetData.writeInfos[bindingIdx].image.imageView = imageRes->getView(surface, false);
            mPerDeviceData[i].storageImages[slot] = imageRes->getHandle();
        }
        else
        {
            // Need optimize but required reset views after frame end
            if (mPerDeviceData[i].storageImages[slot] == VK_NULL_HANDLE)
                continue;

            perSetData.writeInfos[bindingIdx].image.imageView = mShader->GetDriver()->GetDummyTexture()->GetVkImages(i)->getView(false);
            mPerDeviceData[i].storageImages[slot] = VK_NULL_HANDLE;
        }

        perSetData.mSetsDirty = true;
    }
}

void VulkanGpuParams::setBuffer(u32 set, u32 slot, const CVulkanHardwareBuffer* buffer)
{
    if (bufferBindingSlot.size() <= slot)
        return;

    u32 bindingIdx = bufferBindingSlot[slot];

    const auto* bufferDesc = buffer->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS);
    for (u32 i = 0; i < _MAX_DEVICES; i++)
    {
        if (mPerDeviceData[i].perSetData.size() <= i)
            break;

        VulkanBuffer* bufferRes;
    
        if (bufferDesc != nullptr)
            bufferRes = bufferDesc->Buffer;
        else
            bufferRes = nullptr;
    
        PerSetData& perSetData = mPerDeviceData[i].perSetData[set];
        VkWriteDescriptorSet& writeSetInfo = perSetData.writeSetInfos[bindingIdx];
    
        bool useView = writeSetInfo.descriptorType != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        if (useView)
        {
            VkBufferView bufferView;
            if (bufferRes != nullptr)
            {
                bufferView = bufferRes->getView();
            }
            else
            {
                bufferView = mShader->GetDriver()->GetDummyStorageBuffer()->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)->Buffer->getView();
            }

            if (perSetData.writeInfos[bindingIdx].bufferView == bufferView)
                continue;
    
            perSetData.writeInfos[bindingIdx].bufferView = bufferView;
            writeSetInfo.pTexelBufferView = &perSetData.writeInfos[bindingIdx].bufferView;
        }
        else // Structured storage buffer
        {
            if (bufferRes != nullptr)
            {
                VkBuffer vkBuffer = bufferRes->getHandle();
                if (perSetData.writeInfos[bindingIdx].buffer.buffer == vkBuffer)
                    continue;

                perSetData.writeInfos[bindingIdx].buffer.buffer = vkBuffer;
            }
            else
            {
                if (perSetData.writeInfos[bindingIdx].buffer.buffer == mShader->GetDriver()->GetDummyStorageBuffer()->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)->Buffer->getHandle())
                    continue;

                perSetData.writeInfos[bindingIdx].buffer.buffer = mShader->GetDriver()->GetDummyStorageBuffer()->GetBufferDesc(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)->Buffer->getHandle();
            }
    
            writeSetInfo.pTexelBufferView = nullptr;
        }

        perSetData.mSetsDirty = true;
    }
}

void VulkanGpuParams::setSamplerState(u32 set, u32 slot, const VkSampler& sampler)
{
    if (TextureBindingSlot.size() <= slot)
        return;

    u32 bindingIdx = TextureBindingSlot[slot];
    
    for (u32 i = 0; i < _MAX_DEVICES; i++)
    {
        if (mPerDeviceData[i].perSetData.size() <= i)
            break;

        PerSetData& perSetData = mPerDeviceData[i].perSetData[set];

        if (perSetData.writeInfos[bindingIdx].image.sampler == sampler)
            continue;
    
        if (sampler != nullptr)
        {
            VkSampler vkSampler = sampler;

            perSetData.writeInfos[bindingIdx].image.sampler = vkSampler;
        }
        else
        {
            perSetData.writeInfos[bindingIdx].image.sampler = mShader->GetDriver()->GetDummySampler();
        }

        perSetData.mSetsDirty = true;
    }
}

u32 VulkanGpuParams::getNumSets() const
{
    return mPerDeviceData[0].perSetData.size();
}

void VulkanGpuParams::UpdateDescriptors(VulkanCmdBuffer& buffer, VkDescriptorSet* sets)
{
    u32 deviceIdx = buffer.getDeviceIdx();
    
    PerDeviceData& perDeviceData = mPerDeviceData[deviceIdx];
    if (perDeviceData.perSetData.empty())
        return;
    
    bool changed = false;
    u32 numSets = mShader->getLayout() ? 1 : 0;
    
    for (u32 i = 0; i < numSets; i++)
    {
        PerSetData& perSetData = perDeviceData.perSetData[i];
    
        if (!perSetData.mSetsDirty) // Set not dirty, just use the last one we wrote (this is fine even across multiple command buffers)
            continue;
    
        // Set is dirty, we need to update
        //// Use latest unless already used, otherwise try to find an unused one
        if (perSetData.latestSet->isBound()) // Checking this is okay, because it's only modified below when we call registerResource, which is under the same lock as this
        {
            perSetData.latestSet = nullptr;
    
            for (auto& entry : perSetData.sets)
            {
                if (!entry->isBound())
                {
                    perSetData.latestSet = entry;
                    break;
                }
            }
    
            // Cannot find an empty set, allocate a new one
            if (perSetData.latestSet == nullptr)
            {
                VulkanDescriptorLayout* layout = mShader->getLayout();
                perSetData.latestSet = Driver->_getPrimaryDevice()->createSet(layout);
                perSetData.sets.push_back(perSetData.latestSet);
            }
        }
    
        // Note: Currently I write to the entire set at once, but it might be beneficial to remember only the exact
        // entries that were updated, and only write to them individually.
        perSetData.latestSet->write(perSetData.writeSetInfos, perSetData.numElements);
    
        perSetData.mSetsDirty = false;
        changed = true;
    }
    
    for (u32 i = 0; i < numSets; i++)
    {
        VulkanDescriptorSet* set = perDeviceData.perSetData[i].latestSet;
    
        if (changed)
            buffer.registerResource(set, VulkanUseFlag::Read);
        sets[i] = set->getHandle();
    }
}

void irr::video::VulkanGpuParams::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::VulkanGpuParams::OnDeviceRestored(CVulkanDriver * device)
{
}
