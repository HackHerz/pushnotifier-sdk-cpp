#ifndef H_PUSHNOTIFIER
#define H_PUSHNOTIFIER

#include <string>
#include <vector>
#include <ctime>
#include <curl/curl.h>

#include "json/src/json.hpp"



// Uncomment and change these lines to your API-Token and Package
// #define API_TOKEN		"replaceme"
// #define APP_PACKAGE		"replaceme"



// URLs
#define URL_PN_LOGIN		"https://api.pushnotifier.de/v2/user/login"
#define URL_PN_DEVICES		"https://api.pushnotifier.de/v2/devices"

/*
 * https://api.pushnotifier.de/v2/user/refresh
 * https://api.pushnotifier.de/v2/notifications/text
 * https://api.pushnotifier.de/v2/notifications/url
 * https://api.pushnotifier.de/v2/notifications/notification
 */




class PushNotifier
{
public:
	PushNotifier();
	PushNotifier(std::string username, std::string appToken, std::time_t expires_at);

	~PushNotifier();


	typedef struct {
		std::string title;
		std::string id;
		std::string model;
		std::string image;
	} Device;

	typedef struct {
		std::string token;
		std::time_t expires_at;
	} AppToken;

	std::string login(std::string username, std::string password, bool apply = true);
	std::vector<Device> getDevices();


private:
	typedef enum {
		PUT,
		GET,
		POST
	} http_method;

	nlohmann::json call(http_method method, std::string url, const char* xapptoken, nlohmann::json body);

	std::string username;
	AppToken appToken;
	std::string authorization;
};

#endif
