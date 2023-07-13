#include <iostream>
#include <string>
#include <curl/curl.h>
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>

// Callback function for handling the server response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response)
{
    size_t totalSize = size * nmemb;
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Get the current timestamp as UTC time in the specified format
std::string getCurrentTimestamp()
{
    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);
    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm utcTime;
    gmtime_r(&nowTimeT, &utcTime);

    ss << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << nowMs.count() << "+00:00";

    return ss.str();
}

int main()
{
    // Initialize cURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();

    if (curl)
    {
        // Set the request URL
        std::string url = "https://localhost:9200/test-index/_doc/";

        // Set the user credentials for authentication
        std::string username = "elastic";
        std::string password = "changeme";
        std::string authString = username + ":" + password;

        // Create the document to be indexed
        std::string documentData = R"({
            "author": "cpp",
            "text": "Elasticsearch: cool. bonsai cool.",
            "timestamp": ")" + getCurrentTimestamp() + R"("
        })";

        // Set cURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, documentData.c_str());
        curl_easy_setopt(curl, CURLOPT_USERPWD, authString.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Skip SSL verification for localhost

        // Set the Content-Type header
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Create a string to hold the server response
        std::string response;

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Print the response
            std::cout << "Document indexed successfully!" << std::endl;
            std::cout << "Response: " << response << std::endl;
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // Cleanup global cURL resources
    curl_global_cleanup();

    return 0;
}

//Response: {"_index":"test-index","_id":"a6OQT4kBEuZGoYr9INhg","_version":1,"result":"created","_shards":{"total":2,"successful":1,"failed":0},"_seq_no":18,"_primary_term":3}
