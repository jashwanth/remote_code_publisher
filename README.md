The GUI contains the buttons browse and send the files from the client to Repository.
To get the files on the client side first click get categories.
The combobox is populated with list of categories

Now after the catgories are fetched click on getfile to get the files in that category.
We can download the file now by clicking the download button.
The downloaded files are stored at location remote_code_publisher/clientStorage/FileStorage

Initially after the connection setup we will download both js and css files into the clientStorage directory.
We can also delete the file using delete button.

The button for code Analyzer to run on the server is also present.

For the first download of file , if the Code Analysis results are absent, repository automatically triggers the code Analyzer.

Also implemented lazy loading to load the dependent files along with original file in the clientStorage/FileStorage location.

Also implemented IIS service . The last two arguments for TestExecutive contains the directory mapping and the localhost on which IIS is hosted.

Create a run.bat file outside the directory with the following content
cd remote_code_publisher\x64\Debug 
START TestExectutive.exe ..\..\Repository  *.h  *.cpp  /f /r ..\..\StoreHTMFiles C:\HostWebServer http://localhost:8090/CodePublisher
cd ..\..\..\remote_code_publisher\WPFTest\bin\Debug
START WPFTest.exe


Create a compile.bat file outside the directory with the following contents
devenv remote_code_publisher\CodeAnalyzerEx.sln /rebuild debug
@pause

