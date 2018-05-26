/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __XAMLCPP_STATECOMMUTER_H__
#define __XAMLCPP_STATECOMMUTER_H__

#include "Interfaces/IParser.h"
#include "Interfaces/IValueContext.h"
#include "Interfaces/IInstanceLifeCycleListener.h"
#include "Interfaces/IRuntimeTypeSource.h"
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/INamedScope.h"
#include "Core/Assembler/Level/CurrentLevelWrapper.h"
#include "Core/Assembler/Level/PreviousLevelWrapper.h"
#include <string>
#include <vector>
#include <stack>

namespace XamlCPP::Core
{
    class DeferredXamlLoader;

    namespace Assembler
    {
        class CurrentLevelWrapper;
        class PreviousLevelWrapper;
        class Level;
        class InstanceProperties;
    };

    namespace ReflEx
    {
        class MutableMember;
    };
};

namespace XamlCPP::Core::Assembler
{
    enum class ValueProcessingMode
    {
        AssignToMember = 0,
        ConstructionParameter,
        Key,
        InitializationValue,
        Name,
        Class,
        SubClass
    };

    class StateCommuter
    {
    private:
        CurrentLevelWrapper m_Current;
        PreviousLevelWrapper m_Previous;
        ValueProcessingMode m_ValueProcessingMode = ValueProcessingMode::AssignToMember;
        std::shared_ptr<XamlCPP::Core::DeferredXamlLoader> m_deferLoader;
        System::Reflection::RefObject m_topLevel;

        Interfaces::IInstanceLifeCycleListener *const lifecycleListener = nullptr;
        Interfaces::IValueContext *const valueContext = nullptr;
        std::vector<XamlCPP::Core::Assembler::Level*>* stack = nullptr;
        bool isTemplateLoader = false;

    public:
        virtual ~StateCommuter()
        { }

        StateCommuter(std::vector<XamlCPP::Core::Assembler::Level*>* stack, Interfaces::IRuntimeTypeSource *typeSource, Interfaces::IInstanceLifeCycleListener *lifecycleListener, Interfaces::IValueContext *valueContext, bool isTemplateLoader = false);

        CurrentLevelWrapper *getCurrent() const;
        PreviousLevelWrapper *getPrevious() const;

        int getLevel() const;
        void SetTemplateLoader(bool on)
        {
            isTemplateLoader = on;
        }

        bool IsTemplateLoader() const
        {
            return isTemplateLoader;
        }

    private:
        bool getHasParentToAssociate() const;

    public:
        bool getWasAssociatedRightAfterCreation() const;

        Interfaces::IValueContext *getValueContext() const;

        ValueProcessingMode getValueProcessingMode() const;
        void setValueProcessingMode(ValueProcessingMode value);

        System::Reflection::Variant getValueOfPreviousInstanceAndItsMember() const;

        std::vector<XamlCPP::Core::Assembler::Level*> *getStack() const;
        void setStack(std::vector<XamlCPP::Core::Assembler::Level*> *value);

        bool getParentIsOneToMany() const;

        InstanceProperties *getInstanceProperties() const;
        bool getHasParent() const;

        Interfaces::ITopDownValueContext *getTopDownValueContext() const;

        void SetKey(System::Reflection::Variant const& value);
        void SetDeferLoader(std::shared_ptr<XamlCPP::Core::DeferredXamlLoader> const& value)
        {
            m_deferLoader = value;
        }

        std::shared_ptr<XamlCPP::Core::DeferredXamlLoader> const& GetDeferLoader() const
        {
            return m_deferLoader;
        }

        void SetTopLevel(System::Reflection::RefObject const& topLevel) { m_topLevel = topLevel; }
        System::Reflection::RefObject const& GetTopLevel() const { return m_topLevel; }

        void AssignChildToParentProperty();

        void RaiseLevel();

    private:
        void UpdateLevelWrappers();

    public:
        void DecreaseLevel();

        void CreateInstanceOfCurrentXamlTypeIfNotCreatedBefore();

    private:
        void MaterializeInstanceOfCurrentType();

    public:
        System::Reflection::Variant GetValueProvidedByMarkupExtension(Interfaces::IMarkupExtension *instance);

    private:
        void GetExtensionContext(MarkupExtensionContext& context);

        std::vector<System::Reflection::Variant> GatherConstructionArguments();

        void AddChildToCurrentCollection();

    public:
        void AddCtorArgument(const std::string &stringValue);

        void AssociateCurrentInstanceToParent();

        void RegisterInstanceNameToNamescope();

        void PutNameToCurrentInstanceIfAny();

    private:
        Interfaces::INameScope *FindNamescope();

        Interfaces::INameScope *FindNamescopeForInstanceWithNoName();

        Interfaces::INameScope *FindNamescopeForInstanceThatHadPreviousName();

        Interfaces::INameScope *FindNamescopeSkippingAncestor(int skip);

        void AddChildToHost();

        void AddChildToDictionary();

        void ClearKey();

        static System::Reflection::Variant GetValueTuple(System::Reflection::Object *instance, ReflEx::MutableMember *member);

    public:
        void SetNameForCurrentInstance(const std::string &value);

        void AssociateCurrentInstanceToParentForCreation();
    };
}


#endif	//#ifndef __XAMLCPP_STATECOMMUTER_H__
