#include "App.h"
#include "IrrXUI/XUIIrrRenderer.h"
#include "Log/Log.h"
#include "standard/Timer.h"
#include "standard/String/atoi_imp.h"
#include "Threading/Threading.h"

#include "IrrLicht/include/irrlicht.h"

#include "XUI/Core/Dependencies/DependencyObject.h"
#include "XamlCPP/DataBind/Binding.h"
#include "XamlCPP/Core/NotifyPropertyChanged.h"
#include "XUI/UI/TextBlock.h"
#include "standard/BasicPrimities.h"

#include "Reflected/DataObject.h"
#include "Reflection/Runtime/ReflectionModule.h"

#include "XUI/Core/GlobalInitializerScheduler.h"

#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Core/Assembler/Settings.h"
#include "XamlCPP/Core/ReflEx/TypeConverterRegistration.h"
#include "XUI/UI/Window.h"
#include "XUI/UI/Grid.h"
#include "XUI/UI/Button.h"
#include "XUI/UI/Border.h"
#include "XUI/Core/Media/Brush.h"
#include "XUI/Core/TypeConverter/TypeConverters.h"

std::string ControlTemplateTest = R"(
<Window x:Class="WpfApp2.MainWindow"                                             
        xmlns="http://git.tauri.hu/XUI"                                          
        xmlns:x="http://git.tauri.hu/Xaml"                   
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"             
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"   
        xmlns:local="clr-namespace:WpfApp2"                                      
        mc:Ignorable="d"                                                         
        Title="MainWindow" Height="350" Width="525">
    <Window.Resources>

        <!-- Fill Brushes -->

        <LinearGradientBrush x:Key="NormalBrush" StartPoint="0,0" EndPoint="0,1">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#FFF" Offset="0.0"/>
                    <GradientStop Color="#CCC" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <LinearGradientBrush x:Key="HorizontalNormalBrush" StartPoint="0,0" EndPoint="1,0">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#FFF" Offset="0.0"/>
                    <GradientStop Color="#CCC" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <LinearGradientBrush x:Key="LightBrush" StartPoint="0,0" EndPoint="0,1">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#FFF" Offset="0.0"/>
                    <GradientStop Color="#EEE" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <LinearGradientBrush x:Key="HorizontalLightBrush" StartPoint="0,0" EndPoint="1,0">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#FFF" Offset="0.0"/>
                    <GradientStop Color="#EEE" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <LinearGradientBrush x:Key="DarkBrush" StartPoint="0,0" EndPoint="0,1">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#FFF" Offset="0.0"/>
                    <GradientStop Color="#AAA" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <LinearGradientBrush x:Key="PressedBrush" StartPoint="0,0" EndPoint="0,1">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#BBB" Offset="0.0"/>
                    <GradientStop Color="#EEE" Offset="0.1"/>
                    <GradientStop Color="#EEE" Offset="0.9"/>
                    <GradientStop Color="#FFF" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <SolidColorBrush x:Key="DisabledForegroundBrush" Color="#888" />

        <SolidColorBrush x:Key="DisabledBackgroundBrush" Color="#EEE" />

        <SolidColorBrush x:Key="WindowBackgroundBrush" Color="#FFF" />

        <SolidColorBrush x:Key="SelectedBackgroundBrush" Color="#DDD" />

        <!-- Border Brushes -->

        <LinearGradientBrush x:Key="NormalBorderBrush" StartPoint="0,0" EndPoint="0,1">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#CCC" Offset="0.0"/>
                    <GradientStop Color="#444" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <LinearGradientBrush x:Key="HorizontalNormalBorderBrush" StartPoint="0,0" EndPoint="1,0">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#CCC" Offset="0.0"/>
                    <GradientStop Color="#444" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <LinearGradientBrush x:Key="DefaultedBorderBrush" StartPoint="0,0" EndPoint="0,1">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#777" Offset="0.0"/>
                    <GradientStop Color="#000" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <LinearGradientBrush x:Key="PressedBorderBrush" StartPoint="0,0" EndPoint="0,1">
            <GradientBrush.GradientStops>
                <GradientStopCollection>
                    <GradientStop Color="#444" Offset="0.0"/>
                    <GradientStop Color="#888" Offset="1.0"/>
                </GradientStopCollection>
            </GradientBrush.GradientStops>
        </LinearGradientBrush>

        <SolidColorBrush x:Key="DisabledBorderBrush" Color="#AAA" />

        <SolidColorBrush x:Key="SolidBorderBrush" Color="#888" />

        <SolidColorBrush x:Key="LightBorderBrush" Color="#AAA" />

        <!-- Miscellaneous Brushes -->
        <SolidColorBrush x:Key="GlyphBrush" Color="#444" />

        <SolidColorBrush x:Key="LightColorBrush" Color="#DDD" />

        <Style x:Key="ButtonFocusVisual">
          <Setter Property="Control.Template">
            <Setter.Value>
              <ControlTemplate>
                <Border>
                  <Rectangle 
                    Margin="2"
                    StrokeThickness="1"
                    Stroke="#60000000"
                    StrokeDashArray="1 2"/>
                </Border>
              </ControlTemplate>
            </Setter.Value>
          </Setter>
        </Style>

        <Style TargetType="Button">
            <Setter Property="SnapsToDevicePixels" Value="true"/>
            <Setter Property="OverridesDefaultStyle" Value="true"/>
            <Setter Property="FocusVisualStyle" Value="{x:StaticResource ButtonFocusVisual}"/>
            <Setter Property="MinHeight" Value="23"/>
            <Setter Property="MinWidth" Value="75"/>
            <Setter Property="Template">
                <Setter.Value>
                    <ControlTemplate TargetType="Button">
                        <Border 
                            x:Name="Border"  
                            CornerRadius="2" 
                            BorderThickness="1"
                            Background="{x:StaticResource NormalBrush}"
                            BorderBrush="{x:StaticResource NormalBorderBrush}">
                            <ContentPresenter 
                                Margin="2"
                                HorizontalAlignment="Center"
                                VerticalAlignment="Center"
                                RecognizesAccessKey="True"/>
                        </Border>
                        <ControlTemplate.Triggers>
                            <Trigger Property="IsKeyboardFocused" Value="true">
                                <Setter TargetName="Border" Property="BorderBrush" Value="{x:StaticResource DefaultedBorderBrush}" />
                            </Trigger>
                            <Trigger Property="IsDefaulted" Value="true">
                                <Setter TargetName="Border" Property="BorderBrush" Value="{x:StaticResource DefaultedBorderBrush}" />
                            </Trigger>
                            <Trigger Property="IsMouseOver" Value="true">
                                <Setter TargetName="Border" Property="Background" Value="{x:StaticResource DarkBrush}" />
                            </Trigger>
                            <Trigger Property="IsPressed" Value="true">
                                <Setter TargetName="Border" Property="Background" Value="{x:StaticResource PressedBrush}" />
                                <Setter TargetName="Border" Property="BorderBrush" Value="{x:StaticResource PressedBorderBrush}" />
                            </Trigger>
                            <Trigger Property="IsEnabled" Value="false">
                                <Setter TargetName="Border" Property="Background" Value="{x:StaticResource DisabledBackgroundBrush}" />
                                <Setter TargetName="Border" Property="BorderBrush" Value="{x:StaticResource DisabledBorderBrush}" />
                                <Setter Property="Foreground" Value="{x:StaticResource DisabledForegroundBrush}"/>
                            </Trigger>
                        </ControlTemplate.Triggers>
                    </ControlTemplate>
                </Setter.Value>
            </Setter>
            <Setter Property="ContentTemplate">
                <Setter.Value>
                    <DataTemplate>
                        <TextBlock Text="{x:TemplateBinding Content}"/>
                    </DataTemplate>
                </Setter.Value>
            </Setter>
        </Style>
    </Window.Resources>
    <Grid>
        <Button Margin="10,10,377,285">cool</Button>
    </Grid>
</Window>
)";


void UnitTest()
{
    XamlCPP::Xaml::UIServiceContext = XUI::Platform::UIService::Instance();

    std::shared_ptr<WorldClient::UnitTestDataContext> dataContext = std::make_shared<WorldClient::UnitTestDataContext>();

    std::shared_ptr<XUI::UI::TextBlock> first = std::make_shared<XUI::UI::TextBlock>();
    std::shared_ptr<XUI::UI::TextBlock> second = std::make_shared<XUI::UI::TextBlock>();

    first->DataContext = dataContext;
    auto _datacontext = first->DataContext;
    EXCEPT(_datacontext.GetBase());

    //StringValue
    std::shared_ptr<XamlCPP::DataBind::Binding> bindDataContext = std::make_shared<XamlCPP::DataBind::Binding>("StringValue", XUI::Core::Dependency::BindingMode::TwoWay);
    std::shared_ptr<XamlCPP::DataBind::Binding> bind = std::make_shared<XamlCPP::DataBind::Binding>("#first.Text", XUI::Core::Dependency::BindingMode::TwoWay);

    XUI::Core::Dependency::DependencyObject::Bind(first.get(), XUI::UI::TextBlock::TextProperty.get(), bindDataContext.get(), nullptr);
    XUI::Core::Dependency::DependencyObject::Bind(second.get(), XUI::UI::TextBlock::TextProperty.get(), bind.get(), nullptr);

    first->SetName("first");
    IrrBackend::XUIIrrPlatformDriver::Instance()->Register("first", first);

    second->Text = L"teszt";

    EXCEPT(first->Text == L"teszt");

    dataContext->StringValue = L"data text";

    EXCEPT(first->Text == L"data text");
    EXCEPT(second->Text == L"data text");

    IrrBackend::XUIIrrPlatformDriver::Instance()->Unregister("first");
}

void UnitTextXaml()
{
    //std::string misc = typeid(System::Reflection::CleanedType<System::Reflection::RefObject>).name();
    XamlCPP::Core::Assembler::Settings settings;

    std::ostringstream stream;
    stream << "<Window x:Class=\"WpfApp2.MainWindow\""
        "          xmlns=\"http://git.tauri.hu/XUI\""
        "          xmlns:x=\"http://git.tauri.hu/Xaml\""
        "          xmlns:d=\"http://schemas.microsoft.com/expression/blend/2008\""
        "          xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\""
        "          xmlns:local=\"clr-namespace:WpfApp2\""
        "          mc:Ignorable=\"d\""
        "          Title=\"MainWindow\" Height=\"350\" Width=\"525\">"
        "      <Grid Name=\"MainGrid\">"
        "          <Button Content=\"Button\" HorizontalAlignment=\"Left\" Height=\"48\" Margin=\"105,50,0,0\" VerticalAlignment=\"Top\" Width=\"152\"/>"
        "          <Button Content=\"Button2\" HorizontalAlignment=\"Right\" Height=\"48\" Margin=\"106,50,0,0\" VerticalAlignment=\"Bottom\" Width=\"152\"/>"
        "          <TextBlock HorizontalAlignment=\"Left\" Margin=\"155,149,0,0\" TextWrapping=\"Wrap\" Text=\"{x:Binding Path=StringValue, Mode=TwoWay}\" VerticalAlignment=\"Top\" Width=\"91\" Name=\"first\"/>"
        "          <TextBlock Name=\"second\" HorizontalAlignment=\"Left\" Margin=\"251,149,0,0\" TextWrapping=\"Wrap\" Text=\"{x:Binding ElementName=first, Path=Text, Mode=TwoWay}\" VerticalAlignment=\"Top\" Width=\"103\"/>"
        "      </Grid>"
        "</Window>";

    std::shared_ptr<XamlCPP::Interfaces::ILoader> xamlLoader = XamlCPP::Xaml::DefaultLoader(XUI::Platform::UIService::Instance());
    System::Reflection::RefObject result = xamlLoader->Load(stream, &settings);
    std::shared_ptr<WorldClient::UnitTestDataContext> dataContext = std::make_shared<WorldClient::UnitTestDataContext>();

    // Xaml Builder Test

    auto window = result.get()->Dynamic_As<XUI::UI::Window>();
    EXCEPT(window);
    window->DataContext = dataContext;

    std::wstring title = window->Title;
    auto h = window->Height;
    auto w = window->Width;

    auto grid = window->Content.ToObject()->Dynamic_As<XUI::UI::Grid>();
    EXCEPT(grid);

    std::string gname = grid->Name;

    EXCEPT(title == L"MainWindow");
    EXCEPT(h == 350.0);
    EXCEPT(w == 525.0);
    EXCEPT(gname == "MainGrid");
    EXCEPT(!grid->Children.empty());

    auto button = grid->Children[0]->Dynamic_As<XUI::UI::Button>();
    EXCEPT(button);

    auto halign = button->HorizontalAlignment;

    auto button2 = grid->Children[1]->Dynamic_As<XUI::UI::Button>();
    EXCEPT(button2);

    // Binding Test

    auto FirstTextBlock = grid->Children[2]->Dynamic_As<XUI::UI::TextBlock>();
    EXCEPT(FirstTextBlock);
    auto SecondTextBlock = grid->Children[3]->Dynamic_As<XUI::UI::TextBlock>();
    EXCEPT(SecondTextBlock);

    SecondTextBlock->Text = L"teszt";

    EXCEPT(FirstTextBlock->Text == L"teszt");
    EXCEPT(dataContext->StringValue == L"teszt");

    dataContext->StringValue = L"data text";

    EXCEPT(FirstTextBlock->Text == L"data text");
    EXCEPT(SecondTextBlock->Text == L"data text");
}

void UnitTestSytleXaml()
{
    XamlCPP::Core::Assembler::Settings settings;

    std::ostringstream stream;
    stream << R"(
<Window x:Class="WpfApp2.MainWindow"                                             
        xmlns="http://git.tauri.hu/XUI"                                          
        xmlns:x="http://git.tauri.hu/Xaml"                   
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"             
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"   
        xmlns:local="clr-namespace:WpfApp2"                                      
        mc:Ignorable="d"                                                         
        Title="MainWindow" Height="350" Width="525">
    <Window.Resources>
        <Style x:Key="myStyle" TargetType="Button">
            <Setter Property="Control.Background" Value="Orange" />
            <Setter Property="FontStyle" Value="Italic" />
            <Setter Property="Padding" Value="8,4" />
            <Setter Property="Margin" Value="4" />
        </Style>
    </Window.Resources>
    <Grid>
        <Button Style="{x:StaticResource myStyle}" Margin="4,288,4,4">Styles</Button>
        <Button Style="{x:StaticResource myStyle}" Margin="19,105,262,180">are</Button>
        <Button Style="{x:StaticResource myStyle}" Margin="30,59,290,235">cool</Button>
    </Grid>
</Window>
)";

    std::shared_ptr<XamlCPP::Interfaces::ILoader> xamlLoader = XamlCPP::Xaml::DefaultLoader(XUI::Platform::UIService::Instance());
    System::Reflection::RefObject result = xamlLoader->Load(stream, &settings);
    std::shared_ptr<WorldClient::UnitTestDataContext> dataContext = std::make_shared<WorldClient::UnitTestDataContext>();

    auto window = result.get()->Dynamic_As<XUI::UI::Window>();
    EXCEPT(window);

    auto grid = window->Content.ToObject()->Dynamic_As<XUI::UI::Grid>();
    EXCEPT(grid);
    EXCEPT(!grid->Children.empty());

    auto button = grid->Children[0]->Dynamic_As<XUI::UI::Button>();
    EXCEPT(button);

    {
        auto bg = button->Background;
        auto fontStyle = button->FontStyle;
        auto padding = button->Padding;
        auto margin = button->Margin;

        auto brush = bg->shared_from_base_dynamic<XUI::Core::Media::SolidColorBrush>();
        uint32 color = brush->Color.ToUInt32();

        EXCEPT(int(fontStyle) == 2);
        EXCEPT(color == 0XFFFFA500);
    }

    auto button2 = grid->Children[1]->Dynamic_As<XUI::UI::Button>();
    EXCEPT(button2);

    {
        auto bg = button2->Background;
        auto fontStyle = button2->FontStyle;
        auto padding = button2->Padding;
        auto margin = button2->Margin;

        auto brush = bg->shared_from_base_dynamic<XUI::Core::Media::SolidColorBrush>();
        uint32 color = brush->Color.ToUInt32();

        EXCEPT(int(fontStyle) == 2);
        EXCEPT(color == 0XFFFFA500);
    }

    auto button3 = grid->Children[2]->Dynamic_As<XUI::UI::Button>();
    EXCEPT(button3);

    {
        auto bg = button3->Background;
        auto fontStyle = button3->FontStyle;
        auto padding = button3->Padding;
        auto margin = button3->Margin;

        auto brush = bg->shared_from_base_dynamic<XUI::Core::Media::SolidColorBrush>();
        uint32 color = brush->Color.ToUInt32();

        EXCEPT(int(fontStyle) == 2);
        EXCEPT(color == 0XFFFFA500);
    }
}

std::shared_ptr<XUI::UI::Window> UnitTestControlTemplateXaml()
{
    XamlCPP::Core::Assembler::Settings settings;

    std::ostringstream stream;
    stream << ControlTemplateTest;

    std::shared_ptr<XamlCPP::Interfaces::ILoader> xamlLoader = XamlCPP::Xaml::DefaultLoader(XUI::Platform::UIService::Instance());
    System::Reflection::RefObject result = xamlLoader->Load(stream, &settings);
    std::shared_ptr<WorldClient::UnitTestDataContext> dataContext = std::make_shared<WorldClient::UnitTestDataContext>();

    auto window = result.get()->Dynamic_As<XUI::UI::Window>();
    EXCEPT(window);

    auto grid = window->Content.ToObject()->Dynamic_As<XUI::UI::Grid>();
    EXCEPT(grid);
    EXCEPT(!grid->Children.empty());

    auto button = grid->Children[0]->Dynamic_As<XUI::UI::Button>();
    EXCEPT(button);

    auto border = button->Template->FindName("Border", button)->Dynamic_As<XUI::UI::Border>();
    auto borderBGPtr = border->Background.get();

    button->IsMouseOver = true;

    auto borderBGTriggerPtr = border->Background.get();
    EXCEPT(borderBGTriggerPtr != borderBGPtr);

    button->IsMouseOver = false;

    auto borderBGRestoreDefaultPtr = border->Background.get();
    EXCEPT(borderBGRestoreDefaultPtr == borderBGPtr);

    //button->Content = std::make_shared<System::String>("afsafas");

    return window->shared_from_base_static<XUI::UI::Window>();
}
