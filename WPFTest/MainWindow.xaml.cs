using System;
using System.IO;
using System.Windows;
using Microsoft.Win32;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Threading;

namespace WpfTutorialSamples.Dialogs
{
    public partial class OpenFileDialogMultipleFilesSample : Window
    {
        Shim shim = new Shim();
        Thread tRcv = null;
        String filecatselection;
        String fileselected;
        List<String> fileList= new List<string>();

        public OpenFileDialogMultipleFilesSample()
        {
            Console.WriteLine("\nRequirement 1 is met: ");
            Console.WriteLine("\n Using Visual Studio 2015 and its C++ Windows console projects. ");
            Console.WriteLine(" \n Also using WPF to provide a required client Graphical User Interface (GUI). \n");
            InitializeComponent();
            //fileList = new List<string>();
        }

        private void btnOpenFiles_Click(object sender, RoutedEventArgs e)
        {
            lbFilesList.Items.Clear();
            fileList.Clear();
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Multiselect = true;
            // openFileDialog.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*";
            openFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            if (openFileDialog.ShowDialog() == true)
            {
                foreach (string filename in openFileDialog.FileNames)
                {

                    Console.WriteLine("I have selected the file " + Path.GetFullPath(filename));
                    lbFilesList.Items.Add(Path.GetFileName(filename));
                    String filetoBeSent = Path.GetFileName(filename);
                    //Pushing file to the shim
                    fileList.Add(Path.GetFullPath(filename));
                  //  shim.PostMessage(Path.GetFullPath(filename));
                }
            }
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cmb = sender as ComboBox;
            filecatselection = cmb.SelectedItem.ToString();
            Console.WriteLine("The user has selected the category: " + filecatselection);
        }

        private void ComboBoxForFiles_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cmb = sender as ComboBox;
            fileselected = cmb.SelectedItem.ToString();
            Console.WriteLine("The user has selected the file: " + fileselected);
        }


        void Add(String msg)
        {
            //    filesfromrepository.Items.Add(msg);
            try
            {

                List<string> listmaster = msg.Split('$').ToList();

                if (listmaster[0] == "getCategoriesReply")
                {
                    comboBox.Items.Clear();
                    List<string> list1 = listmaster[1].Split(',').ToList();
                    foreach (String str in list1)
                    {
                        Console.WriteLine("Add to Combobox: " + str);
                        comboBox.Items.Add(str);
                    }
                    Console.WriteLine("Requirement 5 is met: populated the Combo box with Categories");
                }
                else if(listmaster[0] == "getFileNamesReply")
                {
                    comboBox1.Items.Clear();
                    List<string> list1 = listmaster[1].Split(',').ToList();
                    foreach (String str in list1)
                    {
                        Console.WriteLine("Add to Combobox1: " + str);
                        comboBox1.Items.Add(str);
                    }
                    Console.WriteLine("Requirement 5 is met: populated the Combo box with Files");
                }
            }
            catch(Exception e)
            {
                Console.WriteLine("Exception caught in comboxbox: " + e.ToString());
            }
        }

        //Upload files button
        private void upload_Click(object sender, RoutedEventArgs e)
        {
        //    String combo = comboBox.Text;
            Console.WriteLine("Total number of files to be uploaded " + fileList.Count);
            foreach(string str in fileList)
            { 
                String message = "uploadFile" + str;
                Console.WriteLine("Demonstrating Requirement5: Uploading the files to the Repository");
                shim.PostMessage("uploadFile"+str);
            }
            //Console.WriteLine("I have selected this in my comobobox " + combo);
        }

        //Upload files button
        private void getcategoriesbutton(object sender, RoutedEventArgs e)
        {
            //    filesfromrepository.Items.Clear();
            // comboBox.Items.Clear();
            Console.WriteLine("getcategoriesbutton button is clicked");
            Console.WriteLine("Demonstarting Req 5: Sending the get categories request to the client");
            shim.PostMessage("getcategories");
        }

        private void getFilesbutton(object sender, RoutedEventArgs e)
        {
            //    filesfromrepository.Items.Clear();
          //  comboBox.Items.Clear();
            Console.WriteLine("getFiles Button is clicked");
            Console.WriteLine("Demonstrating Requirement5: sending the getFile Request based on category");
            if (filecatselection.Length != 0)
            {
                Console.WriteLine("Demonstrating Requirement5: sending the getFile Request based on category");
                shim.PostMessage("getFileNamesBasedOnCategory"+"$"+filecatselection);
            }
            else
            {
                Console.WriteLine("Please select the category to get the files");
            }
        }

        //download files button
        private void downloadFilesbutton(object sender, RoutedEventArgs e)
        {
            if ((filecatselection != null) && (fileselected != null))
            {
                Console.WriteLine("Demonstrating Requirement 5: Download Html file request : " + fileselected);
                shim.PostMessage("downloadFileHtmlContent" + "$" + filecatselection + "$" + fileselected);
            }
            else
            {
                Console.WriteLine("Please select both the category and file name for download..");
            }
        }

        // delete files button
        private void deleteFilesbutton(object sender, RoutedEventArgs e)
        {
            if ((filecatselection != null) && (fileselected != null))
            {
                Console.WriteLine("Requirement 5 is met: Sending the request to delete the file: " + fileselected);
                shim.PostMessage("deleteFileFromServer" + "$" + filecatselection + "$" + fileselected);
            }
            else
            {
                Console.WriteLine("Please select both the category and file name for deletion..");
            }
        }

        //Analyze files button
        private void analyze_Click(object sender, RoutedEventArgs e)
        {

            Console.WriteLine("Requesting Repo for the code analysis: " + fileselected);
            shim.PostMessage("RunCodeAnalyzerOnServer");
        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            Console.WriteLine(" \n Window_Loaded::Starting ofWindow_Loaded method");
            tRcv = new Thread(
              () =>
              {
                  try
                  {
                      while (true)
                      {
                          String msg = shim.GetMessage();

                          Console.WriteLine(" \n Window_Loaded::Received message from the Shim ");
                          Console.WriteLine("\n Message Body is " + msg);
                          Action<String> act = new Action<string>(Add);
                          Object[] args = { msg };
                          Dispatcher.Invoke(act, args);
                          //  Console.WriteLine("Inside thread of Window.xaml.cs file message frm getMessage is " + msg);
                      }
                  }
                  catch(Exception exp)
                  {
                      Console.WriteLine("Exception caught in Window_Loaded: thread " + exp.ToString());
                  }
              }
            );
            tRcv.Start();
        }
    }
}