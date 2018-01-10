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
#define URL_PN_REFRESH		"https://api.pushnotifier.de/v2/user/refresh"
#define URL_PN_TXT			"https://api.pushnotifier.de/v2/notifications/text"
#define URL_PN_URL			"https://api.pushnotifier.de/v2/notifications/url"
#define URL_PN_NOTIFICATION	"https://api.pushnotifier.de/v2/notifications/notification"



/**
 * @brief PushNotifier SDK Service
 *
 * A bridge for using PushNotifier
 *
 * @author hackherz
 * @version 1.0 <January 2018>
*/
class PushNotifier
{
public:

	/**
	 * @brief Initialize the PushNotifier instance
	 */
	PushNotifier();


	/**
	 * @brief Initialize the PushNotifier instance
	 *
	 * @param username Username of the user
	 * @param appToken App-Token to user
	 * @param expires_at Timestamp of the expire date of the token. Set to NULL if you want to disable automatical renewal
	 */
	PushNotifier(std::string username, std::string appToken, std::time_t expires_at);

	/**
	 * @brief Take care of CURL
	 */
	~PushNotifier();


	/**
	 * @brief A PushNotifier device
	 */
	typedef struct {
		/// Title of the Device
		std::string title;

		/// ID of the Device
		std::string id;

		/// Model of the Device
		std::string model;

		/// URL to Device-image
		std::string image;

		/// String representation of the device (without image)
		std::string toString() {
			return "Title: " + title + " ID: " + id + " Model: " + model;
		}
	} Device;


	/**
	 * @brief Representation of an AppToken
	 */
	typedef struct {
		/// The token itself
		std::string token;

		/// Timestamp of the expiration date
		std::time_t expires_at;

		/// Checks if the token expires within the next 48 hours
		bool isAboutToExpire() {
			return std::difftime(expires_at, std::time(nullptr)) < 48.0 * 3600;
		}
	} AppToken;


	/**
	 * @brief Log in as a user
	 *
	 * @param username Username to login with
	 * @param  password Passwordof the user
	 * @param apply Automatically apply the AppToken or just return it
	 *
	 * @return AppToken
	 */
	AppToken login(std::string username, std::string password, bool apply = true);


	/**
	 * @brief Refresh the current AppToken
	 *
	 * This can be done any time but shouldn't be done until 48 hours before it expires
	 *
	 * @param apply Automatically apply the AppToken to this instance
	 *
	 * @return AppToken
	 *
	 * @throws runtime_error If an unknown error happened
	 */
	AppToken refreshToken(bool apply = true);


	/**
	 * @brief Get all devices the current user has
	 *
	 * @return List of Device
	 *
	 * @throws runtime_error If an unknown error happened
	 */
	std::vector<Device> getDevices();


	/**
	 * @brief Set the AppToken that should be used
	 *
	 * @param appToken AppToken
	 * @param expires_at Expiration Date of the AppToken, set null to disable auto-renewal
	 *
	 * @return PushNotifier
	 */
	void setApptoken(std::string appToken, std::time_t expires_at);


	/**
	 * @brief Get the current AppToken
	 *
	 * Renews itself automatically in case it is about to expire
	 *
	 * @return string representation of the token
	 */
	std::string getAppToken();



	/**
	 * @brief Send a text-only message
	 *
	 * @param deviceIDs List of Device-IDs to send to
	 * @param content Message to send
	 * @param silent If true, no sound is played
	 *
	 * @return Whether the message has been successfully delivered to all devices or not
	 *
	 * @throws runtime_error If content is empty
	 */
	bool sendMessage(std::vector<std::string> deviceIDs, std::string content, bool silent = false);


	/**
	 * @brief Send a message
	 *
	 * @param deviceID Device-ID to send to
	 * @param content Message to send
	 * @param silent If true, no sound is played
	 *
	 * @return Whether the message has been successfully delivered to all devices or not
	 *
	 * @throws runtime_error If content is empty
	 */
	bool sendMessage(std::string deviceID, std::string content, bool silent = false);


	/**
	 * @brief Send a link
	 *
	 * @param deviceIDs List of Device-IDs to send to
	 * @param url URL to send
	 * @param silent If true, no sound is played
	 *
	 * @return Whether the url has been successfully delivered to all devices or not
	 *
	 * @throws runtime_error If url is empty
	 */
	bool sendURL(std::vector<std::string> deviceIDs, std::string url, bool silent = false);



	/**
	 * @brief Send a link
	 *
	 * @param deviceID Device-ID to send to
	 * @param url URL to send
	 * @param silent If true, no sound is played
	 *
	 * @return Whether the url has been successfully delivered to all devices or not
	 *
	 * @throws runtime_error If url is empty
	 */
	bool sendURL(std::string deviceID, std::string url, bool silent = false);


	/**
	 * @brief Send a notification with content and a URL
	 *
	 * The user will be taken to the URL after tapping on the notification
	 *
	 * @param deviceIDs List of Device-IDs to send to
	 * @param content Content to send
	 * @param url URL to send
	 * @param silent If true, no sound is played
	 *
	 * @return Whether the notification has been successfully delivered to all devices or not
	 *
	 * @throws runtime_error If url or content are empty
	 */
	bool sendNotification(std::vector<std::string> deviceIDs, std::string content, std::string url, bool silent = false);


	/**
	 * @brief Send a notification with content and a URL
	 *
	 * The user will be taken to the URL after tapping on the notification
	 *
	 * @param deviceID Device-ID to send to
	 * @param content Content to send
	 * @param url URL to send
	 * @param silent If true, no sound is played
	 *
	 * @return Whether the notification has been successfully delivered to all devices or not
	 *
	 * @throws runtime_error If url or content are empty
	 */
	bool sendNotification(std::string deviceID, std::string content, std::string url, bool silent = false);



private:
	/**
	 * Enum Class to specify the Http method for call
	 */
	typedef enum {
		PUT,
		GET,
		POST
	} http_method;

	/**
	 * @brief CURL wrapper function
	 *
	 * @param method Http Method to use
	 * @param url The Url to invoke
	 * @param xapptoken X-AppToken for Api Authentication
	 * @param body JSON-Request data
	 *
	 * @return JSON-Object of the response
	 *
	 * @throws runtime_error If the Api response indicates an error or CURL failed
	 */
	nlohmann::json call(http_method method, std::string url, const char* xapptoken, nlohmann::json body);

	/// Currently used Username
	std::string username;

	/// Currently used AppToken
	AppToken appToken;

	/// Authorization Header Value
	std::string authorization;
};

#endif
