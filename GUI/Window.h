#ifndef WINDOW_H
#define WINDOW_H
/////////////////////////////////////////////////////////////////////
// Window.h - C++\CLI implementation of WPF Application            //
//          - Runs Code Static Analysis                            //
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
*  Package Operations:
*  -------------------
*  This package demonstrates how to build a C++\CLI WPF application.  It 
*  provides a class, WPFCppCliDemo, derived from System::Windows::Window
*  that is compiled with the /clr option to run in the Common Language
*  Runtime, and another class HeightConverter used for binding a ListBox
*  to a window view.
*
*  The window class hosts, in its window, a tab control with three views for:
*  starting code analysis execution, setting up analysis parameters, and
*  determining what will be displayed and logged.
*
*  This application provides a GUI that expects to find an analyzer program
*  and execute it with command line parameters derived from GUI selections.
*
*  Required Files:
*  ---------------
*  Window.h, Window.cpp, Analyzer.exe
*
*  Build Command:
*  --------------
*  devenv CppCli-WPF-App.sln
*  - this builds C++\CLI client application
*
*  Maintenance History:
*  --------------------
*  ver 3.2 : 27 Oct 2016
*  - fixed bug passing directory name to Analyzer.  Wrapped the path in
*    quotes so Analyzer will handle directory names with spaces.
*  ver 3.1 : 23 Aug 2016
*  - refactored functions by mapping function content into several
*    smaller functions
*  - added additional controls and processing for Setup View
*  ver 3.0 : 22 Aug 2016
*  - repurposed from its original demonstration model to become a useful
*    tool for software analysis
*  - many changes from the earlier version
*  ver 2.0 : 15 Apr 2015
*  - completed message passing demo with moch channel
*  - added FileBrowserDialog to show files in a selected directory
*  ver 1.0 : 13 Apr 2015
*  - incomplete demo with GUI but not connected to mock channel
*/
/*
* Implementation:
* ---------------
* This was implemented as a C++/CLI console application.  All of the needed
* Windows Presentation Foundation Framework is added through references.
*
* To do this one must:
*   Create C++/CLI Console Application
*   Right-click project > Properties > Common Language Runtime Support > /clr
*   Right-click project > Add > References
*     add references to :
*       System
*       System.Windows.Presentation
*       System.Windows.Forms
*       System.Xaml
*       WindowsBase
*       PresentatioCore
*       PresentationFramework
*/
using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;

#include <string>

/////////////////////////////////////////////////////////////////////
// Settings struct:
//   - holds settings that will be retrieved at startup 
//     and persisted at shutdown

using UserSettings = ref struct Settings { 
  String^ path;                  // analysis path
  bool h; bool cpp; bool cs;     // search file patterns
  bool upatt; String^ upattStrs; // selected files
  bool cl;                       // clear file list
  bool metrics;                  // show metrics details
  bool ast;                      // show abstract syntax tree
  bool slocs;                    // show souce lines of code
  bool logfile;                  // create log file showing all output
  bool rslt;                     // show normal program results
  bool demo;                     // demonstrate program operations
  bool dbug;                     // show debugging output
};

namespace CppCliWindows
{
  ref class WPFCppCliDemo : Window
  {
    // Controls for Window

    DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
    Grid^ hGrid = gcnew Grid();                    
    TabControl^ hTabControl = gcnew TabControl();
    TabItem^ hExecutionTab = gcnew TabItem();
    TabItem^ hSetupTab = gcnew TabItem();
    TabItem^ hProcModeTab = gcnew TabItem();
    StatusBar^ hStatusBar = gcnew StatusBar();
    StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
    TextBlock^ hStatus = gcnew TextBlock();

    // Controls for Execution View

    Grid^ hExecutionGrid = gcnew Grid();
    Button^ hExecuteViewBrowseButton_ = gcnew Button();
    Button^ hExecuteViewStartButton_ = gcnew Button();
    TextBox^ hExecuteViewTextBox_ = gcnew TextBox();
    //Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
    Forms::OpenFileDialog^ hOpenFileDialog = gcnew Forms::OpenFileDialog();
    StackPanel^ hStackPanel1 = gcnew StackPanel();

    // Controls for SetupView View

    Grid^ hSetupGrid = gcnew Grid();
    StackPanel^ hHdrsStack_ = gcnew StackPanel();
    CheckBox^ hHdrs_ = gcnew CheckBox();
    StackPanel^ hImplsStack_ = gcnew StackPanel();
    CheckBox^ hImpls_ = gcnew CheckBox();
    StackPanel^ hCSharpStack_ = gcnew StackPanel();
    CheckBox^ hCSharp_ = gcnew CheckBox();
    CheckBox^ hUPatt_ = gcnew CheckBox();
    Label^ hPattLabel_ = gcnew Label();
    StackPanel^ hClearStack_ = gcnew StackPanel();
    CheckBox^ hClear_ = gcnew CheckBox();
    Label^ hPathLabel_ = gcnew Label();
    TextBox^ hSetupViewTextBox_ = gcnew TextBox();
    Grid^ hPathPanel_ = gcnew Grid();
    CheckBox^ hMetrics_ = gcnew CheckBox();
    CheckBox^ hAST_ = gcnew CheckBox();
    CheckBox^ hSlocs_ = gcnew CheckBox();
    Label^ hHdrsLabel_ = gcnew Label();
    Label^ hImplsLabel_ = gcnew Label();
    Label^ hCSharpLabel_ = gcnew Label();
    Label^ hClearLabel_ = gcnew Label();
    DockPanel^ hUPattDock_ = gcnew DockPanel();
    ListBox^ hUPattList_ = gcnew ListBox();
    DockPanel^ hButtonsDocker_ = gcnew DockPanel();
    Button^ hSetupBrowseButton_ = gcnew Button();
    Button^ hSetupExecuteButton_ = gcnew Button();
    Label^ hMetricsLabel_ = gcnew Label();
    Label^ hASTLabel_ = gcnew Label();
    Label^ hSlocsLabel_ = gcnew Label();

    // Controls for DisplayModeView View

    Grid^ hModeGrid = gcnew Grid();
    CheckBox^ hRsltMode_ = gcnew CheckBox();
    CheckBox^ hDemoMode_ = gcnew CheckBox();
    CheckBox^ hDbugMode_ = gcnew CheckBox();
    CheckBox^ hLogFileMode_ = gcnew CheckBox();

  public:
    WPFCppCliDemo();
    ~WPFCppCliDemo();

    void getUserSettings();
    void saveUserSettings();

    void setUpStatusBar();
    void setUpTabControl();
    void createExecutionView();
    void createSetupView();
    void createDisplayModeView();

    void setCmdLineArgs();
    void doExecute();
    void execute(Object^ sender, RoutedEventArgs^ args);
    void browseForFolder(Object^ sender, RoutedEventArgs^ args);
    void browseForFolderEx(Object^ sender, RoutedEventArgs^ args);
    void OnLoaded(Object^ sender, RoutedEventArgs^ args);
    void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);
    void OnExecuteViewTextChanged(Object^ sender, RoutedEventArgs^ args);
    void OnSetupViewTextChanged(Object^ sender, RoutedEventArgs^ args);

  private:
    void setTextBlockProperties();
    void setButtonsProperties();
    void WPFCppCliDemo::setUpGrid();
    void WPFCppCliDemo::setUpBinding();
    void WPFCppCliDemo::setUpSearchButtons();
    void WPFCppCliDemo::setupFileList();
    void WPFCppCliDemo::setupFileSearch();
    void WPFCppCliDemo::setUpPathPanel();
    void WPFCppCliDemo::setUpPatternsStacks();
    void WPFCppCliDemo::setUpSearchPanel();
    void WPFCppCliDemo::setUpDisplaysPanel();
    void executionTabSelected(Object^ sender, RoutedEventArgs^ args);
    void setupTabSelected(Object^ sender, RoutedEventArgs^ args);
    void procModeTabSelected(Object^ sender, RoutedEventArgs^ args);
    String^ path_;
    String^ cmdLine_;
    UserSettings userSettings_;

    ///////////////////////////////////////////////////////
    // Not used now:
    //   std::string toStdString(String^ pStr);
    //   String^ toSystemString(std::string& str);
  };
}


#endif
