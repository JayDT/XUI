#include "MainUI.h"

#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Core/Assembler/Settings.h"
#include "XamlCPP/Core/ReflEx/TypeConverterRegistration.h"
#include "XUI/UI/Window.h"
#include "XUI/UI/Grid.h"
#include "XUI/UI/Button.h"
#include "XUI/UI/Border.h"
#include "XUI/UI/ListBox.h"
#include "XUI/UI/TreeView.h"
#include "XUI/UI/TreeViewItem.h"
#include "XUI/UI/DataGrid/DataGridControl.h"
#include "XUI/Core/Media/Brush.h"
#include "XUI/Platform/UIRenderer.h"
#include "XUI/Core/TypeConverter/TypeConverters.h"
#include "XUI/Core/Application.h"
#include "UI/DebugUI.h"

void WorldClient::MainWindow::InitializeComponent()
{
    auto window = this; //result.get()->Dynamic_As<XUI::UI::Window>();
    EXCEPT(window);

    window->Show();

    //auto button = window->Find("button");
    //if (button)
    //{
    //    auto bt = button->Static_As<XUI::UI::Button>();
    //    //bt->Click += System::Events::IEventHandler::Bind<XUI::Core::Observer::RoutedEventArgs>(&WorldClient::MainWindow::PART_CLOSE_Click, this);
    //}

	//auto dataView = window->Find("dataView");
	if (m_mainDataGrid)
	{
		auto dv = m_mainDataGrid->Static_As<XUI::UI::DataGrid>();
		auto observableCollection = std::make_shared<System::Collection::List<std::shared_ptr<DataElement>>>();
		observableCollection->Add(std::make_shared<DataElement>("Value1", 1, 1, DataElementType::Normal));
        observableCollection->Add(std::make_shared<DataElement>("Value2", 20, 0, DataElementType::Extended));
        observableCollection->Add(std::make_shared<DataElement>("Value3", 3, 0, DataElementType::Normal));
        observableCollection->Add(std::make_shared<DataElement>("Value4", 34, 1, DataElementType::Value));

        dv->Items = observableCollection;


		// { std::string("Value1"), 1, 1}
	}

    auto listbox = window->Find("lbItems");
    if (listbox)
    {
        auto lb = listbox->Static_As<XUI::UI::ListBox>();
        lb->Items->Add(std::make_shared<System::String>("test1"));
        lb->Items->Add(std::make_shared<System::String>("test2"));
        lb->Items->Add(std::make_shared<System::String>("test3"));
        lb->Items->Add(std::make_shared<System::String>("test4"));
        lb->Items->Add(std::make_shared<System::String>("test5"));
        lb->Items->Add(std::make_shared<System::String>("test6"));
        lb->Items->Add(std::make_shared<System::String>("test7"));
        lb->Items->Add(std::make_shared<System::String>("test8"));
        lb->Items->Add(std::make_shared<System::String>("test9"));
        lb->Items->Add(std::make_shared<System::String>("test10"));
        lb->Items->Add(std::make_shared<System::String>("test11"));
        lb->Items->Add(std::make_shared<System::String>("test12"));
        lb->Items->Add(std::make_shared<System::String>("test13"));
        lb->Items->Add(std::make_shared<System::String>("test14"));
        lb->Items->Add(std::make_shared<System::String>("test15"));
        lb->Items->Add(std::make_shared<System::String>("test16"));
        lb->Items->Add(std::make_shared<System::String>("test17"));
        lb->Items->Add(std::make_shared<System::String>("test18"));
        lb->Items->Add(std::make_shared<System::String>("test19"));

        lb->SelectionChanged += System::make_eventhandler(&WorldClient::MainWindow::PART_SELECTION_Changed, this);
    }

    auto treeviewobj = window->Find("tvItems");
    if (treeviewobj)
    {
        auto treeview = treeviewobj->Static_As<XUI::UI::TreeView>();
        treeview->BeginInit();
        for (int i = 0; i < 20; ++i)
        {
            auto treeviewitem = std::make_shared<XUI::UI::TreeViewItem>();
            //treeviewitem->BeginInit();
            treeviewitem->Header = std::string(System::String::format("1item%u", i));
            treeviewitem->GID = 202;
    
            for (int ii = 0; ii < 3; ++ii)
            {
                auto treeviewitem1 = std::make_shared<XUI::UI::TreeViewItem>();
                //treeviewitem1->BeginInit();
                treeviewitem1->Header = std::string(std::string(System::String::format("2item%u%u", i, ii)));
                treeviewitem->Items->Add(treeviewitem1);
    
                for (int iii = 0; iii < 3; ++iii)
                {
                    auto treeviewitem2 = std::make_shared<XUI::UI::TreeViewItem>();
                    treeviewitem2->Header = std::string(std::string(System::String::format("3item%u%u%u", i, ii, iii)));
                    treeviewitem1->Items->Add(treeviewitem2);
    
                    for (int iiii = 0; iiii < 3; ++iiii)
                    {
                        auto treeviewitem3 = std::make_shared<XUI::UI::TreeViewItem>();
                        treeviewitem3->GID = 303;
                        treeviewitem3->Header = std::string(std::string(System::String::format("dsfgdsfdgsdf__item%u%u%u%u", i, ii, iii, iiii)));
                        treeviewitem2->Items->Add(treeviewitem3);
                    }
                }
                //treeviewitem1->EndInit();
            }
            //treeviewitem->EndInit();
    
            treeview->Items->Add(treeviewitem);
        }
        treeview->EndInit();
    }
}

void WorldClient::MainWindow::Connect(int connectionId, System::Reflection::Object* target)
{
    switch (connectionId)
    {
        case 110: 
        {
            auto _mainDataGrid = target->Static_As<XUI::UI::DataGrid>();
            if (_mainDataGrid)
                m_mainDataGrid = _mainDataGrid;
            break;
        }
    }
}

void WorldClient::MainWindow::PART_CLOSE_Click(void * sender, XUI::Core::Observer::RoutedEventArgs const & e)
{
}

void WorldClient::MainWindow::PART_SELECTION_Changed(void * sender, XUI::Interfaces::SelectionChangedEventArgs const & e)
{
}

void WorldClient::MainWindow::EditorMainManager_ActiveContentChanged(void * sender, System::Events::EventArg & e)
{
}

void WorldClient::MainWindow::OnShowDebugWindow(void * sender, System::Events::EventArg & e)
{
    auto debugWindow = XUI::Platform::UIRender::LoadXaml(System::URI("pack://application:,,,/IrrXUI;/UI/DebugUI.xaml", true));
    static_cast<DebugWindow*>(debugWindow.get())->SetDebuggedWindow(static_cast<XUI::UI::Window*>(this));
}
