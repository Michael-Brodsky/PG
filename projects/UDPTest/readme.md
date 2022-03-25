UDPTest.exe is a Win32 app that can send/receive UDP packets to/from any open port on a connected WiFi network. It's just a simple app to test end-to-end connectivity 
and help troubleshoot network problems and will also be used to implement UDP/IP support in the next iteration of View. Download and extract UDPTestSetup.zip then run 
UDPTestSetup.msi. This will install UDPTest.exe along with the current Windows .NET runtime, if not already installed. Run UDPTest.exe to open the application. The UI is 
pretty self-explanatory.

UDPTest.exe now detects SSID and local IP, and prints the sender's IP and port of any received packets.