#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <vector> //Simplier arrays - input handling
#include <sstream> //used for input handling
#include <sys/timeb.h>
#include <algorithm>
using namespace std;
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 4000
#define DEFAULT_PORT "80"

//Function prototypes

int sendRequest(SOCKET&, string);
int recvResponse(SOCKET&, char[DEFAULT_BUFLEN], string&);
string filterHostName(string, string&);
bool connectServer( SOCKET&, string);
void printToolInstructions();
void tokenize(string const&, const char, vector<string>&);
bool is_number(const std::string& s);
int getMilliCount();
int getMilliSpan(int nTimeStart);
void additionalLogic(const int, vector<int>&, vector<int>&, vector<int>&, vector<string>&, vector<bool>&);
int findMax(const vector<int>);
int findMin(const vector<int>);
double findMean(const vector<int>);
double findMedian(vector<int>);
double findSuccessfulPercent(const vector<bool>);
void errCodesPrint(const vector<string>);

struct myclass {
     bool operator() (int i, int j) { return (i < j); }
} sortAlg;
int __cdecl main(int argc, char** argv)
{
     SOCKET ConnectSocket = INVALID_SOCKET;
     string request = "";
     string path = "/";
     string address = "";
     string response = "";
     string options = "";
     string hostname = "";
     int numRequests = 1;
     char recvbuf[DEFAULT_BUFLEN];
     int iResult;
     int recvbuflen = DEFAULT_BUFLEN;
     vector<string> tokens;
     vector<int> bytesSentPerRequest;
     vector<int> bytesReceivedPerRequest;
     vector<int> timerPerRequest;
     vector<string> errCodesPerReq;
     vector<bool> errCodeOccurances;
     //Instructions
     printToolInstructions();

    // Get address from user
    while (options != "--exit") {
         cout << "\nEnter a command: ";
         getline(cin, options);

         tokenize(options, ' ', tokens); //split by ' '

         if (tokens.at(0) == "--help") {
              printToolInstructions();
         }
         else if (tokens.at(0) == "--url") {
              const int x = numRequests;
              for (int i = 0; i < x; i++) {
                   if (!is_number(tokens.at(1))) {
                        hostname = filterHostName(tokens.at(1), path);
                        connectServer(ConnectSocket, hostname);
                        request = "GET " + path + " HTTP/1.0\r\n\r\n";

                        /* SEND REQUEST STARTS*/
                        int start = getMilliCount(); //timer starts

                        int bytesSent = sendRequest(ConnectSocket, request);
                        if (bytesSent <= 0) {
                             cout << "Error: Unable to send the request" << endl;
                        }
                        else {
                             bytesSentPerRequest.push_back(bytesSent);
                        }
                        int bytesReceived = recvResponse(ConnectSocket, recvbuf, response);
                        int milliSecondsElapsed = getMilliSpan(start); //timer ends

                        size_t findStatusCode = response.find("HTTP/1.");
                        //All error codes
                        if (findStatusCode != string::npos) {
                             string subCode = response.substr(findStatusCode + 9, 3);
                             if (subCode != "200") {
                                  errCodesPerReq.push_back(subCode);
                                  errCodeOccurances.push_back(false); //errors then log it as false 
                             }
                             else {
                                  errCodeOccurances.push_back(true); //no errors, then log it as true
                             }
                        }
                        timerPerRequest.push_back(milliSecondsElapsed);

                        if (bytesReceived <= 0) {
                             cout << "Error: Unable to receive the response" << endl;
                        }
                        else {
                             bytesReceivedPerRequest.push_back(bytesReceived);
                        }
                   }
                   else {
                        cout << "Invalid arguement after --url. Try again!" << endl;
                   }
                   response.clear();
                   recvbuf[0] = '/0';
              }
              if (x > 1) {
                   additionalLogic(x, bytesSentPerRequest, bytesReceivedPerRequest, timerPerRequest, errCodesPerReq, errCodeOccurances);
              }

              //reset numRequests
              numRequests = 1;
         }
         else if (tokens.at(0) == "--profile") {
              if ((tokens.size() > 1) && is_number(tokens.at(1)) && stoi(tokens.at(1)) > 0) {
                   numRequests = stoi(tokens.at(1));
                   cout << numRequests << " HTTP requests will be made to the server. Please type the --url [URL] command next." << endl;
              }
              else {
                   cout << "A  positive number greator than 0 must be provided after --profile command. Try again!" << endl;
              }
         }
         else if (tokens.at(0) == "--exit") {
              cout << "***************Thanks for using this program! Have a great day!*********************" << endl;
         }
         else {
              cout << "Invalid Command. Try again or type --help" << endl;
         }
         bytesSentPerRequest.clear();
         bytesReceivedPerRequest.clear();
         timerPerRequest.clear();
         tokens.clear();
         errCodesPerReq.clear();
         errCodeOccurances.clear();
    }
    
     // cleanup
     recvbuf[0] = '/0';
     closesocket(ConnectSocket);
     WSACleanup();

     return 0;
}
/*
     Detail: Prints instructions about the program
     Pre: None
     Post: None
*/
void printToolInstructions() {
     
     cout << "________________________________________" << "Instructions" << "___________________________________________________" << endl;

     cout << setw(5) << "Use the following commands for a successful run of this program, after every command press ENTER to execute." << setw(5) << endl;

     cout << ".................................................................................................................." << endl;
     cout << "--url (lowercase): Using this command type the address of the page you want to fetch from, then press ENTER. Example below!" << setw(5) << endl;
     cout << "         " << "Example: --url http://www.google.com/page" << setw(5) << endl;

     cout << ".................................................................................................................." << endl;
     cout << "--profile (lowercase): Using this command type how many times you want to make a request to the server.\nYou must use this command before the --url command." << setw(5) << endl;
     cout << "         " << "Example: --profile 2" << setw(5) << endl;
     cout << "         " << "2 requests will be made to your specified URL, next use the --url command to send a request." << setw(5) << endl;
     cout << "         " << "--url http://www.google.com/page" << setw(5) << endl;
     cout << ".................................................................................................................." << endl;

     cout << "         " << "--help (lowercase): Using this command to display all commands of the tool! This will guide you when you're stuck" << setw(5) << endl;
     cout << "         " << "Example: --help" << setw(5) << endl;
     cout << ".................................................................................................................." << endl;

     cout << "--exit (lowercase): Using this command to exit or terminate the program" << setw(5) << endl;
     cout << "         " << "Example: --exit" << setw(5) << endl;
     cout << "         " << "Thanks for using this tool. Have a great day!" << setw(5) << endl;
     cout << ".................................................................................................................." << endl;

     cout <<  "_________________________________________________________________________________________________________" << endl;

}
void additionalLogic(const int numRequests, vector<int> &bytesSentPerRequest, vector<int> &bytesReceivedPerRequest, vector<int> &timerPerRequest, vector<string>& errCodesPerReq, vector<bool> &errCodeOccurances) {
     cout << "*******************************************************************************************************************" << endl;
     cout << "Number of requests: " << numRequests << endl;
     cout << "The fastest time: " << findMin(timerPerRequest) << "ms" <<endl;
     cout << "The slowest time: " << findMax(timerPerRequest) << "ms" << endl;
     cout << "The mean time: " << findMean(timerPerRequest) << "ms" << endl;
     cout << "The median time: " << findMedian(timerPerRequest) << "ms" << endl;
     cout << "Percent of successful requests: " << setprecision(2) << fixed << findSuccessfulPercent(errCodeOccurances) << "%" << endl;
     cout << "Unsuccessful Error Codes: ";
     errCodesPrint(errCodesPerReq);
     cout << "Size of smallest response: " << findMin(bytesReceivedPerRequest) << " bytes" << endl;
     cout << "Size of largest response: " << findMax(bytesReceivedPerRequest) << " bytes" << endl;
     cout << "*******************************************************************************************************************" << endl << endl;
}
int findMax(const vector<int> temp)
{
     int maxSoFar = temp.at(0);
     for (int i = 0; i < temp.size(); i++) {
          if (temp.at(i) > maxSoFar) {
               maxSoFar = temp.at(i);
          }
     }
     return maxSoFar;
}

int findMin(const vector<int> temp)
{
     int minSoFar = temp.at(0);
     for (int i = 0; i < temp.size(); i++) {
          if (temp.at(i) < minSoFar) {
               minSoFar = temp.at(i);
          }
     }
     return minSoFar;
}
double findMean(const vector<int> temp) {
     int sum = 0;
     for (int i = 0; i < temp.size(); i++) {
          sum += temp.at(i);
     }
     double mean = (sum / temp.size());
     return mean;
}
double findMedian( vector<int> temp) {
     double median = 0.0;
     //sorts the vector from lowest to highest
     sort(temp.begin(), temp.end(), sortAlg);

     //if its an even number, take avg of the two middle numbers
     if (temp.size() % 2 == 0) {
          int mid = temp.size() / 2;
          median = (temp.at(mid) + temp.at(mid - 1)) / 2;
     }
     else {
          int mid = (temp.size() / 2);
          median = temp.at(mid);
     }
     return median;
}
double findSuccessfulPercent(const vector<bool> temp) {
     int countSuccess = 0;
     for (int i = 0; i < temp.size(); i++) {
          if (temp.at(i)) {
               countSuccess++;
          }
     }
     double successPercent = (((double)countSuccess / temp.size()) * 100.0);

     return successPercent;
}
void errCodesPrint(const vector<string> temp) {
     for (int i = 0; i < temp.size(); i++) {
          if (i == temp.size() - 1) {
               cout << temp.at(i) << ".";
          }
          else {
               cout << temp.at(i) << ", ";
          }
     }
     cout << endl;
}

/*
     Detail: Given a URL, this function will filter the hostname & path out of the address
     Pre: Pass the hostname, path variable (to be modified - pass by ref)
     Post: returns the hostname (string)
*/
string filterHostName(string hostname, string &path) {

     char      proto[6] = "";
     char hostname2[299] = "";
     /* ---------------------------------------------------------- *
      * Remove the final / from url_str, if there is one           *
      * ---------------------------------------------------------- */

     if (hostname[hostname.length() - 1] == '/')
          hostname[hostname.length() - 1] = '\0';
     /* ---------------------------------------------------------- *
      * the first : ends the protocol string, i.e. http            *
      * ---------------------------------------------------------- */
     strncpy_s(proto, hostname.c_str(), (strchr(hostname.c_str(), ':') - hostname.c_str()));


     /* ---------------------------------------------------------- *
      * the hostname starts after the "://" part                   *
      * ---------------------------------------------------------- */
     size_t pos1 = hostname.find("://");
     hostname = hostname.substr(pos1 + 3);
     
     size_t pos2 = hostname.find("/");

     if (pos2 != string::npos) {
          path = hostname.substr(pos2);
     }


     return hostname;
}
/*
     Detail: Connects to the server given the hostname
     Pre: Pass the socket, wsaData, hostname, addrinfo(hinfs, *ptr, *result)
     Post: Returns true if connection was successful, else false
*/
bool connectServer(SOCKET &ConnectSocket, string hostname) {
     int res = 0;
     WSADATA wsaData;
     struct addrinfo* result = NULL,
          * ptr = NULL,
          hints;

     // Initialize Winsock
     res = WSAStartup(MAKEWORD(2, 2), &wsaData);
     if (res != 0) {
          printf("WSAStartup failed with error: %d\n", res);
          exit(0);
          return false;
     }

     ZeroMemory(&hints, sizeof(hints));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_STREAM;
     hints.ai_protocol = IPPROTO_TCP;

     // Resolve the server address and port
     res = getaddrinfo(hostname.c_str(), DEFAULT_PORT, &hints, &result);
     if (res != 0) {
          printf("getaddrinfo failed with error: %d\n", res);
          WSACleanup();
          return false;
     }

     // Attempt to connect to an address until one succeeds
     for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

          // Create a SOCKET for connecting to server
          ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
               ptr->ai_protocol);
          if (ConnectSocket == INVALID_SOCKET) {
               printf("socket failed with error: %ld\n", WSAGetLastError());
               WSACleanup();
               exit(0);
               return false;
          }

          // Connect to server.
          res = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
          if (res == SOCKET_ERROR) {
               closesocket(ConnectSocket);
               ConnectSocket = INVALID_SOCKET;
               continue;
          }
          break;
     }

     freeaddrinfo(result);

     if (ConnectSocket == INVALID_SOCKET) {
          printf("Unable to connect to server!\n");
          WSACleanup();
          return false;
     }
     else {
          return true;
     }
}
/*
     Detail: Send your HTTP request using this method
     Pre: Pass the socket, and the actual request string as parameters
     Post: Returns the amount of bytes sent to the server as a req.
*/
int sendRequest(SOCKET &ConnectSocket, string request) {
     int bytesSent = 0;
     cout << "HTTP REQ: " << request << endl;
     //Send the actual request, the amount of bytes sent is the return statement received from the send method
     bytesSent = send(ConnectSocket, request.c_str(), (int)strlen(request.c_str()), 0);
     if (bytesSent == SOCKET_ERROR) {
          printf("send failed with error: %d\n", WSAGetLastError());
          closesocket(ConnectSocket);
          WSACleanup();
          exit(0);
     }
     else {
          return bytesSent;
     }
}
/*
     Detail: Receive your response back from the server
     Pre: Pass the socket, receive buffer, and the response string (pass by reference, to be modified)
     Post: Returns the amount of bytes received from the server as a response to your request.
*/
int recvResponse(SOCKET &ConnectSocket, char recvbuf[DEFAULT_BUFLEN], string &response) {
     int bytesReceived = 0;
     int tempReceived = 0;
     // Receive until the peer closes the connection
     do {

          tempReceived = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
          if (tempReceived > 0) {
               bytesReceived += tempReceived;
               response += string(recvbuf).substr(0, bytesReceived);
          }
          else if (tempReceived == 0) {
               response += string(recvbuf).substr(0, bytesReceived);
               response += "\n";
               printf(response.c_str());
               break;
          }
          else {
               printf("recv failed with error: %d\n", WSAGetLastError());
          }

     } while (true);

     return bytesReceived;
}

void tokenize(string const& str, const char delim, vector<string>& out) {
     stringstream ss(str);
     string temp;

     while (getline(ss, temp, delim)) {
          out.push_back(temp); //tokens are pushed into this array.
     }
}
bool is_number(const std::string& s)
{
     return !s.empty() && std::find_if(s.begin(),
          s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}
int getMilliCount() {
     timeb tb;
     ftime(&tb);
     int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
     return nCount;
}
int getMilliSpan(int nTimeStart) {
     int nSpan = getMilliCount() - nTimeStart;
     if (nSpan < 0)
          nSpan += 0x100000 * 1000;
     return nSpan;
}