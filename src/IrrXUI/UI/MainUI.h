#pragma once

#include "XUI/UI/Window.h"

namespace XUI::UI
{
    class DataGrid;
}

namespace WorldClient
{
    class MainWindow : public XUI::UI::Window, public XUI::Interfaces::IComponentConnector
    {
    private:

        Meta(Disable) XUI::UI::DataGrid* m_mainDataGrid = nullptr;

    public:

        virtual ~MainWindow()
        {
        }

        void Connect(int connectionId, System::Reflection::Object* target) override;
        void InitializeComponent() override;

        void PART_CLOSE_Click(void* sender, XUI::Core::Observer::RoutedEventArgs const& e);
        void PART_SELECTION_Changed(void * sender, XUI::Interfaces::SelectionChangedEventArgs const & e);
		void EditorMainManager_ActiveContentChanged(void* sender, System::Events::EventArg& e);
        void OnShowDebugWindow(void* sender, System::Events::EventArg& e);
    };

    enum DataElementType
    {
        Normal,
        Extended,
        Value,
        Unknown
    };

	struct DataElement : System::Reflection::Object
	{
		DataElement()
		{

		}

		DataElement(std::string const& _name, int _level, int _mvp, DataElementType type)
			:Name(_name)
			,Level(_level)
            ,mvp(_mvp)
            ,Type(type)
		{

		}

        virtual ~DataElement()
        {

        }

		std::string Name;
		int Level = 0;
		bool mvp = false;
        DataElementType Type = DataElementType::Normal;

		bool operator==(DataElement const& rhs) const
		{
			return Name == rhs.Name && Level == rhs.Level && mvp == rhs.mvp;
		}
	};
}