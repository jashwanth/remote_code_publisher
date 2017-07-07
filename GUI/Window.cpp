/////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application          //
//            - Runs Code Static Analysis                          //
// ver 3.2                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2016                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: Code Analyzer for SMA and OOD, S2016               //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////
/*
*  This note is here to show how to convert between console and window app.
*  ------------------------------------------------------------------------
*  To run as a Console Application - this is what you need for this project:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*  To run as a Windows Application - not very useful for this project:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*/
#include "Window.h"
#include <string>

using namespace CppCliWindows;
using namespace System::Windows;
using namespace System::Windows::Data;
using namespace System::Globalization;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Diagnostics;

WPFCppCliDemo::WPFCppCliDemo()
{
  // set event handlers

  this->Loaded += 
    gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
  this->Closing += 
    gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);

  hExecutionTab->GotFocus += gcnew RoutedEventHandler(this, &CppCliWindows::WPFCppCliDemo::executionTabSelected);
  hSetupTab->GotFocus += gcnew RoutedEventHandler(this, &CppCliWindows::WPFCppCliDemo::setupTabSelected);
  hProcModeTab->GotFocus += gcnew RoutedEventHandler(this, &CppCliWindows::WPFCppCliDemo::procModeTabSelected);

  hExecuteViewStartButton_->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::execute);
  hExecuteViewBrowseButton_->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
  hSetupBrowseButton_->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolderEx);
  hSetupExecuteButton_->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::execute);
  hExecuteViewTextBox_->LostFocus += gcnew RoutedEventHandler(this, &WPFCppCliDemo::OnExecuteViewTextChanged);
  hSetupViewTextBox_->LostFocus += gcnew RoutedEventHandler(this, &WPFCppCliDemo::OnSetupViewTextChanged);

  // set Window properties

  this->Title = "VisualCodeAnalyzer - Static Code Analysis";
  this->Width = 450;
  this->MinWidth = 450;
  this->Height = 320;
  this->MinHeight = 320;
  this->Name = "MainWindow";

  getUserSettings();

  // attach dock panel to Window

  this->Content = hDockPanel;
  hDockPanel->Children->Add(hStatusBar);
  hDockPanel->SetDock(hStatusBar, Dock::Bottom);
  hDockPanel->Children->Add(hGrid);

  // setup Window controls and views

  setUpTabControl();
  setUpStatusBar();
  createExecutionView();
  createSetupView();
  createDisplayModeView();

  hStatus->Text = "Browse to find analysis path or enter in textbox";
}

WPFCppCliDemo::~WPFCppCliDemo()
{
  
}
//----< local helper function for getting user settings >------------

bool stringToBool(String^ value)
{
  if (value == "true")
    return true;
  return false;
}
//----< retrieve user settings from last execution >-----------------
/*
*  Look for user settings file in directory where GUI was started.
*/
void WPFCppCliDemo::getUserSettings()
{
  String^ settingsPath = Path::GetFullPath(Assembly::GetExecutingAssembly()->Location);
  settingsPath = Path::GetDirectoryName(settingsPath);
  if (!System::IO::Directory::Exists(settingsPath))
  {
    throw gcnew Exception("invalid settings path");
  }
  settingsPath += "\\UserSettings.txt";

  try
  {
    System::IO::StreamReader^ sr = gcnew System::IO::StreamReader(settingsPath);
    
    userSettings_.path = sr->ReadLine();
    userSettings_.h = stringToBool(sr->ReadLine());
    userSettings_.cpp = stringToBool(sr->ReadLine());
    userSettings_.cs = stringToBool(sr->ReadLine());
    userSettings_.upatt = stringToBool(sr->ReadLine());

    String^ commaSeperatedList = sr->ReadLine();
    array<String^>^ patts = commaSeperatedList->Split(',');
    for (int i = 0; i < patts->Length; ++i)
      hUPattList_->Items->Add(patts[i]);

    userSettings_.cl = stringToBool(sr->ReadLine());

    userSettings_.metrics = stringToBool(sr->ReadLine());
    userSettings_.ast = stringToBool(sr->ReadLine());
    userSettings_.slocs = stringToBool(sr->ReadLine());
    userSettings_.rslt = stringToBool(sr->ReadLine());
    userSettings_.demo = stringToBool(sr->ReadLine());
    userSettings_.dbug = stringToBool(sr->ReadLine());
    userSettings_.logfile = stringToBool(sr->ReadLine());
    
    sr->Close();
  }
  catch (Exception^ /*e*/)
  {
    // use default values
    userSettings_.path = Environment::CurrentDirectory;
    userSettings_.h = true;
    userSettings_.cpp = true;
    userSettings_.cs = false;
    userSettings_.upatt = false;
    userSettings_.upattStrs = "";
    userSettings_.cl = false;
    userSettings_.metrics = true;
    userSettings_.ast = false;
    userSettings_.slocs = false;
    userSettings_.rslt = true;
    userSettings_.demo = false;
    userSettings_.dbug = false;
    userSettings_.logfile = true;
  }
  hExecuteViewTextBox_->Text = path_ = userSettings_.path;
  hSetupViewTextBox_->Text = path_;
}
//----< helper function for saving user settings >-------------------

String^ boolToString(bool pred)
{
  if (pred)
    return "true";
  return "false";
}
//----< persist user settings to file >------------------------------
/*
*  Look for user settings file in directory where GUI was started.
*/
void WPFCppCliDemo::saveUserSettings()
{
  String^ settingsPath = Path::GetFullPath(Assembly::GetExecutingAssembly()->Location);
  settingsPath = Path::GetDirectoryName(settingsPath);
  if (!System::IO::Directory::Exists(settingsPath))
  {
    throw gcnew Exception("invalid settings path path");
  }
  settingsPath += "\\UserSettings.txt";

  try
  {
    System::IO::StreamWriter^ sr = gcnew System::IO::StreamWriter(settingsPath);

    sr->WriteLine(path_);
    sr->WriteLine(boolToString((bool)hHdrs_->IsChecked));
    sr->WriteLine(boolToString((bool)hImpls_->IsChecked));
    sr->WriteLine(boolToString((bool)hCSharp_->IsChecked));
    sr->WriteLine(boolToString((bool)hUPatt_->IsChecked));

    String^ commaSeparatedList;
    size_t count = hUPattList_->Items->Count;
    for (size_t i = 0; i < count - 1; ++i)
      commaSeparatedList += hUPattList_->Items[i] + ",";
    commaSeparatedList += hUPattList_->Items[count - 1];
    sr->WriteLine(commaSeparatedList);

    sr->WriteLine(boolToString((bool)hClear_->IsChecked));
    sr->WriteLine(boolToString((bool)hMetrics_->IsChecked));
    sr->WriteLine(boolToString((bool)hAST_->IsChecked));
    sr->WriteLine(boolToString((bool)hSlocs_->IsChecked));
    sr->WriteLine(boolToString((bool)hRsltMode_->IsChecked));
    sr->WriteLine(boolToString((bool)hDemoMode_->IsChecked));
    sr->WriteLine(boolToString((bool)hDbugMode_->IsChecked));
    sr->WriteLine(boolToString((bool)hLogFileMode_->IsChecked));

    sr->Close();
  }
  catch (Exception^ e)
  {
    Console::Write("\n\n{0}\n", e->Message);
  }
}
//----< envent handlers for selecting Tabs >-------------------------

void WPFCppCliDemo::setupTabSelected(Object^ sender, RoutedEventArgs^ args)
{
  this->Width = 600;
  this->Height = 500;
  hStatus->Text = "Use pattern settings and browse for path, or browse for files.";
}

void WPFCppCliDemo::executionTabSelected(Object^ sender, RoutedEventArgs^ args)
{
  this->Width = 400;
  this->Height = 320;
  hStatus->Text = "Browse to find analysis path or enter in textbox, then start";
}

void WPFCppCliDemo::procModeTabSelected(Object^ sender, RoutedEventArgs^ args)
{
  this->Width = 400;
  this->Height = 320;
  hStatus->Text = "Typically set Results Mode and LogFile Mode - use either Results or Demo or Debug";
}
//----< setup status bar controls >----------------------------------

void WPFCppCliDemo::setUpStatusBar()
{
  hStatusBar->Items->Add(hStatusBarItem);
  //hStatus->FontWeight = FontWeights::Bold;
  hStatusBarItem->Content = hStatus;
  hStatusBar->Padding = Thickness(10, 2, 10, 2);
}
//----< setup application tab views >--------------------------------

void WPFCppCliDemo::setUpTabControl()
{
  hGrid->Children->Add(hTabControl);
  hExecutionTab->Header = "Execution";
  hSetupTab->Header = "Applic Setup";
  hProcModeTab->Header = "Display Mode";
  hTabControl->Items->Add(hExecutionTab);
  hTabControl->Items->Add(hSetupTab);
  hTabControl->Items->Add(hProcModeTab);
}
//----< set properties of Execution View TextBox >-------------------
/*
*  Sets TextBox properties and adds to Execution view
*/
void WPFCppCliDemo::setTextBlockProperties()
{
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  hRow1Def->Height = GridLength(50);
  hExecutionGrid->RowDefinitions->Add(hRow1Def);
  hExecutionGrid->Margin = Thickness(40);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hBorder1->Height = 25;
  hBorder1->Child = hExecuteViewTextBox_;
  hExecuteViewTextBox_->Padding = Thickness(2);
  hExecuteViewTextBox_->Text = "";
  hExecuteViewTextBox_->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
  hExecuteViewTextBox_->FontWeight = FontWeights::Normal;
  hExecuteViewTextBox_->FontSize = 14;
  hExecutionGrid->SetRow(hBorder1, 0);
  hExecutionGrid->Children->Add(hBorder1);
}
//----< set properties of Execution View Buttons >-------------------
/*
*  Sets Buttons properties and adds them to Execution view
*/
void WPFCppCliDemo::setButtonsProperties()
{
  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(75);
  hExecutionGrid->RowDefinitions->Add(hRow2Def);
  hExecuteViewBrowseButton_->Content = "Browse";
  hExecuteViewBrowseButton_->Width = 90;
  hExecuteViewBrowseButton_->Height = 20;
  hExecuteViewStartButton_->Content = "Start Analysis";
  hExecuteViewStartButton_->Width = 90;
  hExecuteViewStartButton_->Height = 20;
  hExecuteViewStartButton_->IsEnabled = true;
  hStackPanel1->Children->Add(hExecuteViewBrowseButton_);
  Label^ hHorizontalSpacer = gcnew Label();
  hHorizontalSpacer->Width = 40;
  hStackPanel1->Children->Add(hHorizontalSpacer);
  hStackPanel1->Children->Add(hExecuteViewStartButton_);
  hStackPanel1->Orientation = Orientation::Horizontal;
  hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hExecutionGrid->SetRow(hStackPanel1, 1);
  hExecutionGrid->Children->Add(hStackPanel1);
}
//----< assemble Execution view >------------------------------------

void WPFCppCliDemo::createExecutionView()
{
  Border^ hExecutionBorder = gcnew Border();
  hExecutionBorder->BorderThickness = Thickness(1);
  hExecutionBorder->Margin = Thickness(15);
  hExecutionBorder->BorderBrush = Brushes::LightGray;
  hExecutionBorder->Child = hExecutionGrid;
  hExecutionTab->Content = hExecutionBorder;

  setTextBlockProperties();
  setButtonsProperties();
}
//----< helper class for binding ListBox in SetupView >--------------

ref class HeightConverter : IValueConverter
{
public:
  virtual Object^ Convert(Object^ value, Type^ typeTarget, Object^ param, CultureInfo^ culture)
  {
    return (Object^)((Double)value - 420);
  }
  virtual Object^ ConvertBack(Object^ value, Type^ typeTarget, Object^ param, CultureInfo^ culture)
  {
    return (Object^)((Double)value + 420);
  }
};
//----< setup grid for Setup View >----------------------------------

void WPFCppCliDemo::setUpGrid()
{
  hSetupGrid->Margin = Thickness(10);
  hSetupTab->Content = hSetupGrid;

  ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
  hCol1Def->Name = "ColumnLeft";
  hSetupGrid->ColumnDefinitions->Add(hCol1Def);
  ColumnDefinition^ hCol2Def = gcnew ColumnDefinition();
  hCol2Def->Name = "Column2";
  hCol2Def->Width = GridLength(250, GridUnitType::Pixel);
  hSetupGrid->ColumnDefinitions->Add(hCol2Def);
}
//----< setup file list binding of height to window height >---------

void WPFCppCliDemo::setUpBinding()
{
  // bind ListBox height to Window height minus offset
  // defined by HeightConverter

  Binding^ bnd = gcnew Binding("Value");
  bnd->Source = this;
  bnd->Converter = gcnew HeightConverter();
  bnd->Path = gcnew PropertyPath(hUPattList_->ActualHeightProperty);
  bnd->Mode = BindingMode::OneWay;
  bnd->UpdateSourceTrigger = UpdateSourceTrigger::PropertyChanged;
  BindingOperations::SetBinding(hUPattList_, ListBox::HeightProperty, bnd);
}
//----< initialize setup view buttons >------------------------------

void WPFCppCliDemo::setUpSearchButtons()
{
  hSetupBrowseButton_->Content = "Browse";
  hSetupBrowseButton_->Height = 20;
  hSetupBrowseButton_->Width = 90;

  hSetupExecuteButton_->Content = "Start Analysis";
  hSetupExecuteButton_->Height = 20;
  hSetupExecuteButton_->Width = 90;

  Label^ hHorizSpacer3 = gcnew Label();
  hHorizSpacer3->Width = 16;

  Label^ hHorizSpacer4 = gcnew Label();
  hHorizSpacer3->Width = 16;

  hButtonsDocker_->SetValue(DockPanel::HorizontalAlignmentProperty, System::Windows::HorizontalAlignment::Left);
  hButtonsDocker_->Children->Add(hHorizSpacer3);
  hButtonsDocker_->Children->Add(hSetupBrowseButton_);
  hButtonsDocker_->Children->Add(hHorizSpacer4);
  hButtonsDocker_->Children->Add(hSetupExecuteButton_);
}
//----< setup file list >--------------------------------------------

void WPFCppCliDemo::setupFileList()
{
  hUPatt_->IsChecked = userSettings_.upatt;
  hUPatt_->SetValue(DockPanel::HorizontalAlignmentProperty, System::Windows::HorizontalAlignment::Left);
  hUPattDock_->Children->Add(hUPatt_);
  hUPattList_->Height = 70;
  hUPattDock_->Children->Add(hUPattList_);

  hClearLabel_->Content = "clear file list on browse";
  hClear_->IsChecked = userSettings_.cl;
  hClearStack_ = gcnew StackPanel();
  hClearStack_->Orientation = Orientation::Horizontal;
  hClearStack_->Children->Add(hClear_);
  hClearStack_->Children->Add(hClearLabel_);
}
//----< setup path Grid panel >--------------------------------------

void WPFCppCliDemo::setUpPathPanel()
{
  hPathLabel_->Content = "analysis path";
  hSetupViewTextBox_->Padding = Thickness(2);
  hSetupViewTextBox_->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
  hSetupViewTextBox_->FontWeight = FontWeights::Normal;
  hSetupViewTextBox_->FontSize = 14;
  hPathPanel_->Children->Add(hSetupViewTextBox_);
}
//----< setup file selection for search panel >----------------------

void WPFCppCliDemo::setupFileSearch()
{
  setupFileList();
  setUpSearchButtons();
  setUpPathPanel();

  Label^ hVerticalSpacer1 = gcnew Label();
  hVerticalSpacer1->Content = "";
  hVerticalSpacer1->Height = 10;

  Label^ hVerticalSpacer2 = gcnew Label();
  hVerticalSpacer2->Height = 10;

  Label^ hVerticalSpacer3 = gcnew Label();
  hVerticalSpacer3->Height = 10;

  StackPanel^ hStackPanelLeft = gcnew StackPanel();
  hStackPanelLeft->Children->Add(hPattLabel_);
  hStackPanelLeft->Children->Add(hHdrsStack_);
  hStackPanelLeft->Children->Add(hImplsStack_);
  hStackPanelLeft->Children->Add(hCSharpStack_);
  hStackPanelLeft->Children->Add(hUPattDock_);
  hStackPanelLeft->Children->Add(hVerticalSpacer1);
  hStackPanelLeft->Children->Add(hClearStack_);
  hStackPanelLeft->Children->Add(hVerticalSpacer2);
  hStackPanelLeft->Children->Add(hButtonsDocker_);
  hStackPanelLeft->Children->Add(hVerticalSpacer3);
  hStackPanelLeft->Children->Add(hPathLabel_);
  hStackPanelLeft->Children->Add(hPathPanel_);
  hStackPanelLeft->Orientation = Orientation::Vertical;
  hStackPanelLeft->Margin = Thickness(10);
  hStackPanelLeft->Name = "StackPanelLeft";

  Border^ hBorderLeft = gcnew Border();
  hBorderLeft->Child = hStackPanelLeft;
  hBorderLeft->BorderBrush = Brushes::LightGray;
  hBorderLeft->BorderThickness = Thickness(1);
  hBorderLeft->MinWidth = 200;
  hBorderLeft->Margin = Thickness(5);

  hSetupGrid->SetColumn(hBorderLeft, 0);
  hSetupGrid->Children->Add(hBorderLeft);

  setUpBinding();
}
//----< setup patterns stacks >--------------------------------------

void WPFCppCliDemo::setUpPatternsStacks()
{
  hPattLabel_->Content = "Search Parameters";
  hPattLabel_->FontSize = 16;
  hPattLabel_->Margin = Thickness(10);

  hHdrsLabel_->Content = "*.h";
  hHdrs_->IsChecked = userSettings_.h;
  hHdrsStack_->Orientation = Orientation::Horizontal;
  hHdrsStack_->Children->Add(hHdrs_);
  hHdrsStack_->Children->Add(hHdrsLabel_);

  hImplsLabel_->Content = "*.cpp";
  hImpls_->IsChecked = userSettings_.cpp;
  hImplsStack_->Orientation = Orientation::Horizontal;
  hImplsStack_->Children->Add(hImpls_);
  hImplsStack_->Children->Add(hImplsLabel_);

  hCSharpLabel_->Content = "*.cs";
  hCSharp_->IsChecked = userSettings_.cs;
  hCSharpStack_->Orientation = Orientation::Horizontal;
  hCSharpStack_->Children->Add(hCSharp_);
  hCSharpStack_->Children->Add(hCSharpLabel_);
}
//----< setup search panel for Setup View >--------------------------

void WPFCppCliDemo::setUpSearchPanel()
{
  setUpPatternsStacks();
  setupFileSearch();
}
//----< setup Detailed Displays Panel for Setup View >---------------

void WPFCppCliDemo::setUpDisplaysPanel()
{
  Label^ hDisplaysLabel = gcnew Label();
  hDisplaysLabel->Content = "Detailed Displays";
  hDisplaysLabel->FontSize = 16;
  hDisplaysLabel->Margin = Thickness(10);

  hMetricsLabel_->Content = "Metrics display";
  hMetrics_->IsChecked = userSettings_.metrics;
  StackPanel^ hMetricsStack = gcnew StackPanel();
  hMetricsStack->Orientation = Orientation::Horizontal;
  hMetricsStack->Children->Add(hMetrics_);
  hMetricsStack->Children->Add(hMetricsLabel_);

  hASTLabel_->Content = "AST display";
  hAST_->IsChecked = userSettings_.ast;
  StackPanel^ hASTStack = gcnew StackPanel();
  hASTStack->Orientation = Orientation::Horizontal;
  hASTStack->Children->Add(hAST_);
  hASTStack->Children->Add(hASTLabel_);

  hSlocsLabel_->Content = "Slocs display";
  hSlocs_->IsChecked = userSettings_.slocs;
  StackPanel^ hSlocsStack = gcnew StackPanel();
  hSlocsStack->Orientation = Orientation::Horizontal;
  hSlocsStack->Children->Add(hSlocs_);
  hSlocsStack->Children->Add(hSlocsLabel_);

  Label^ hDisplaysCommentLabel = gcnew Label();
  hDisplaysCommentLabel->Content = "Metric Summary always shown";
  hDisplaysCommentLabel->Margin = Thickness(5);
  hDisplaysCommentLabel->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

  StackPanel^ hStackPanelRight = gcnew StackPanel();
  hStackPanelRight->Children->Add(hDisplaysLabel);
  hStackPanelRight->Children->Add(hMetricsStack);
  hStackPanelRight->Children->Add(hASTStack);
  hStackPanelRight->Children->Add(hSlocsStack);
  hStackPanelRight->Children->Add(hDisplaysCommentLabel);

  hStackPanelRight->Orientation = Orientation::Vertical;
  hStackPanelRight->Margin = Thickness(10);

  Border^ hBorderRight = gcnew Border();
  hBorderRight->Child = hStackPanelRight;
  hBorderRight->BorderBrush = Brushes::LightGray;
  hBorderRight->BorderThickness = Thickness(1);
  hBorderRight->MinWidth = 200;
  hBorderRight->Margin = Thickness(5);
  //hBorderRight->Width = 200;

  hSetupGrid->SetColumn(hBorderRight, 1);
  hSetupGrid->Children->Add(hBorderRight);
}

//----< assemble Setup view >----------------------------------------

void WPFCppCliDemo::createSetupView()
{
  setUpGrid();
  setUpSearchPanel();
  setUpDisplaysPanel();
}
//----< assemble Display Mode view >---------------------------------

void WPFCppCliDemo::createDisplayModeView()
{
  Border^ hProcBorder = gcnew Border();
  hProcBorder->BorderThickness = Thickness(1);
  hProcBorder->Margin = Thickness(15);
  hProcBorder->BorderBrush = Brushes::LightGray;
  hProcBorder->Child = hModeGrid;
  hProcModeTab->Content = hProcBorder;

  ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
  hModeGrid->ColumnDefinitions->Add(hCol1Def);

  Label^ hRsltModeLabel = gcnew Label();
  hRsltModeLabel->Content = "Results mode";
  hRsltMode_->IsChecked = userSettings_.rslt;
  StackPanel^ hRsltStack = gcnew StackPanel();
  hRsltStack->Orientation = Orientation::Horizontal;
  hRsltStack->Children->Add(hRsltMode_);
  hRsltStack->Children->Add(hRsltModeLabel);

  Label^ hDemoModeLabel = gcnew Label();
  hDemoModeLabel->Content = "Demo mode";
  hDemoMode_->IsChecked = userSettings_.demo;
  StackPanel^ hDemoStack = gcnew StackPanel();
  hDemoStack->Orientation = Orientation::Horizontal;
  hDemoStack->Children->Add(hDemoMode_);
  hDemoStack->Children->Add(hDemoModeLabel);

  Label^ hDbugModeLabel = gcnew Label();
  hDbugModeLabel->Content = "DBug mode";
  hDbugMode_->IsChecked = userSettings_.dbug;
  StackPanel^ hDbugStack = gcnew StackPanel();
  hDbugStack->Orientation = Orientation::Horizontal;
  hDbugStack->Children->Add(hDbugMode_);
  hDbugStack->Children->Add(hDbugModeLabel);

  Label^ hLogFileModeLabel = gcnew Label();
  hLogFileModeLabel->Content = "Log file mode";
  hLogFileMode_->IsChecked = userSettings_.logfile;
  StackPanel^ hLogFileStack = gcnew StackPanel();
  hLogFileStack->Orientation = Orientation::Horizontal;
  hLogFileStack->Children->Add(hLogFileMode_);
  hLogFileStack->Children->Add(hLogFileModeLabel);

  StackPanel^ hStackPanel = gcnew StackPanel();
  hStackPanel->Children->Add(hRsltStack);
  hStackPanel->Children->Add(hDemoStack);
  hStackPanel->Children->Add(hDbugStack);
  hStackPanel->Children->Add(hLogFileStack);
  hStackPanel->Orientation = Orientation::Vertical;
  hStackPanel->Margin = Thickness(30);

  hModeGrid->SetColumn(hStackPanel, 0);
  hModeGrid->Children->Add(hStackPanel);
}
//----< set up command line for Analyzer.exe >-----------------------

void WPFCppCliDemo::setCmdLineArgs()
{
  cmdLine_ = "\"" + path_ + "\"";
  if (hHdrs_->IsChecked)
    cmdLine_ += " *.h";
  if (hImpls_->IsChecked)
    cmdLine_ += " *.cpp";
  if (hCSharp_->IsChecked)
    cmdLine_ += " *.cs";
  if (hUPatt_->IsChecked)
  {
    for (size_t i = 0; i < hUPattList_->Items->Count; ++i)
    {
      if(hUPattList_->Items[i]->ToString() != "FolderSelection")
        cmdLine_ += " " + hUPattList_->Items[i];
    }
  }
  if (hSlocs_->IsChecked)
    cmdLine_ += " /s";
  if (hAST_->IsChecked)
    cmdLine_ += " /a";
  if (hMetrics_->IsChecked)
    cmdLine_ += " /m";
  if (hRsltMode_->IsChecked)
    cmdLine_ += " /r";
  if (hDemoMode_->IsChecked)
    cmdLine_ += " /d";
  if (hDbugMode_->IsChecked)
    cmdLine_ += " /b";
  if (hLogFileMode_->IsChecked)
    cmdLine_ += " /f";
}
//----< execute Analyzer.exe as a separate process >----------------0

void WPFCppCliDemo::doExecute()
{
  Console::Clear();
  ProcessStartInfo^ psi = gcnew ProcessStartInfo();
  psi->UseShellExecute = false;
  /*
  *  Look for Analyzer executable in directory where GUI was started.
  */
  String^ analyzerPath = Path::GetFullPath(Assembly::GetExecutingAssembly()->Location);
  analyzerPath = Path::GetDirectoryName(analyzerPath);
  if (!System::IO::Directory::Exists(analyzerPath))
  {
    throw gcnew Exception("invalid analyzer path");
  }
  analyzerPath += "\\CodeAnalyzer.exe";
  try {
    analyzerPath = System::IO::Path::GetFullPath(analyzerPath);
    psi->FileName = analyzerPath;
    setCmdLineArgs();
    psi->Arguments = cmdLine_;
    Process^ p = Process::Start(psi);
  }
  catch (Exception^ ex)
  {
    // we get here if file spec is invalid

    Console::Write("\n{0} = {1}", ex->Message, analyzerPath);
    Console::Write("\n{0}\n", "try rebuilding CodeAnalyzer");
  }
  saveUserSettings();
}
//----< event handler for Start Analysis Button >--------------------

void WPFCppCliDemo::execute(Object^ sender, RoutedEventArgs^ args)
{
  setCmdLineArgs();
  doExecute();
}
//----< event handler for Browse Button >----------------------------
/*
*  - Used in both Execution and Setup Views.
*  - Uses an OpenFileDialog so we can select either files or folders.
*  - Always clears file list on browse.
*/
void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
  String^ defaultFileName = "FolderSelection";
  System::Windows::Forms::DialogResult result;

  hOpenFileDialog->CheckPathExists = true;
  hOpenFileDialog->CheckFileExists = false;
  hOpenFileDialog->ShowReadOnly = false;
  hOpenFileDialog->ReadOnlyChecked = true;
  hOpenFileDialog->ValidateNames = false;
  hOpenFileDialog->Multiselect = true;
  hOpenFileDialog->FileName = defaultFileName;
  hOpenFileDialog->InitialDirectory = hExecuteViewTextBox_->Text;
  result = hOpenFileDialog->ShowDialog();

  if (result == System::Windows::Forms::DialogResult::OK)
  {
    hUPattList_->Items->Clear();
    array<String^>^ fileNames = hOpenFileDialog->FileNames;
    String^ path = System::IO::Path::GetDirectoryName(fileNames[0]);

    for (int i = 0; i < fileNames->Length; ++i)
    {
      String^ name = System::IO::Path::GetFileName(fileNames[i]);
      hUPattList_->Items->Add(name);
    }
    hExecuteViewTextBox_->Text = path;
    hSetupViewTextBox_->Text = path;
    path_ = path;
    hExecuteViewStartButton_->IsEnabled = true;
    if (hUPattList_->Items->Count == 0)
      return;

    // change settings depending on file or folder selection

    String^ debug = hUPattList_->Items[0]->ToString();

    if (hUPattList_->Items[0]->ToString() == defaultFileName)
    {
      hHdrs_->IsChecked = true;
      hImpls_->IsChecked = true;
      hCSharp_->IsChecked = true;
      hUPatt_->IsChecked = false;
    }
    else
    {
      hHdrs_->IsChecked = false;
      hImpls_->IsChecked = false;
      hCSharp_->IsChecked = false;
      hUPatt_->IsChecked = true;
    }
  }
}
//----< event handler for Setup Browse Button >----------------------
/*
*  - Used in both Execution and Setup Views.
*  - Uses an OpenFileDialog so we can select either files or folders.
*  - Clears file list only if clear CheckBox on Setup View is checked.
*/
void WPFCppCliDemo::browseForFolderEx(Object^ sender, RoutedEventArgs^ args)
{
  String^ defaultFileName = "FolderSelection";
  System::Windows::Forms::DialogResult result;

  hOpenFileDialog->CheckPathExists = true;
  hOpenFileDialog->CheckFileExists = false;
  hOpenFileDialog->ShowReadOnly = false;
  hOpenFileDialog->ReadOnlyChecked = true;
  hOpenFileDialog->ValidateNames = false;
  hOpenFileDialog->Multiselect = true;
  hOpenFileDialog->FileName = defaultFileName;
  hOpenFileDialog->InitialDirectory = hExecuteViewTextBox_->Text;
  result = hOpenFileDialog->ShowDialog();

  if (result == System::Windows::Forms::DialogResult::OK)
  {
    if(hClear_->IsChecked)
      hUPattList_->Items->Clear();
    array<String^>^ fileNames = hOpenFileDialog->FileNames;
    String^ path = System::IO::Path::GetDirectoryName(fileNames[0]);

    for (int i = 0; i < fileNames->Length; ++i)
    {
      String^ name = System::IO::Path::GetFileName(fileNames[i]);
      hUPattList_->Items->Add(name);
    }
    if (hClear_->IsChecked)
    {
      hExecuteViewTextBox_->Text = path;
      hSetupViewTextBox_->Text = path;
      path_ = path;
    }
    hExecuteViewStartButton_->IsEnabled = true;

    // change settings depending on file or folder selection

    String^ debug = hUPattList_->Items[0]->ToString();

    if (hClear_->IsChecked && hUPattList_->Items[0]->ToString() == defaultFileName)
    {
      hHdrs_->IsChecked = true;
      hImpls_->IsChecked = true;
      hCSharp_->IsChecked = true;
      hUPatt_->IsChecked = false;
    }
    else
    {
      hHdrs_->IsChecked = false;
      hImpls_->IsChecked = false;
      hCSharp_->IsChecked = false;
      hUPatt_->IsChecked = true;
    }
  }
}
//----< event handler for Window Loaded event >----------------------

void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
  hExecuteViewTextBox_->Text = path_;
  hSetupViewTextBox_->Text = path_;
}
//----< event handler for Window unloading event >-------------------

void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
  saveUserSettings();
}
//----< event handler for manual changes to path in textbox >--------

void WPFCppCliDemo::OnExecuteViewTextChanged(Object^ sender, RoutedEventArgs^ args)
{
  path_ = hExecuteViewTextBox_->Text;
  hSetupViewTextBox_->Text = path_;
}
//----< event handler for manual changes to path in textbox >--------

void WPFCppCliDemo::OnSetupViewTextChanged(Object^ sender, RoutedEventArgs^ args)
{
  path_ = hSetupViewTextBox_->Text;
  hExecuteViewTextBox_->Text = path_;
}

/////////////////////////////////////////////////////////////////////
// Not used now:
//
//String^ WPFCppCliDemo::toSystemString(std::string& str)
//{
//  StringBuilder^ pStr = gcnew StringBuilder();
//  for (size_t i = 0; i < str.size(); ++i)
//    pStr->Append((Char)str[i]);
//  return pStr->ToString();
//}
//
//std::string WPFCppCliDemo::toStdString(String^ pStr)
//{
//  std::string dst;
//  for (int i = 0; i < pStr->Length; ++i)
//    dst += (char)pStr[i];
//  return dst;
//}
//
// Note:
// The comments, below, are placed here to show you how to convert to a
// Windows application.  You don't want to do that for this project, but
// it is useful to know how to do this.

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
  //MSG msg;
  //Console::WriteLine(L"\n Starting WPFCppCliDemo");
  //while ((GetMessage(&msg, NULL, 0, 0)) != 0)
  //{
  //  TranslateMessage(&msg);
  //  DispatchMessage(&msg);
  //}

  Application^ app = gcnew Application();
  app->Run(gcnew WPFCppCliDemo());
  Console::WriteLine(L"\n\n");
}