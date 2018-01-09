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
 * PushNotifier SDK Service
 * A bridge for using PushNotifier
 *
 * @author hackherz
 * @version 1.0 <January 2018>
*/
class PushNotifier
{
public:

	/**
	 * Initialize the PushNotifier instance
	 */
	PushNotifier();


	/**
	 * Initialize the PushNotifier instance
	 * Takes a config array with these options:
	 *
	 * @param TODO
	 */
	PushNotifier(std::string username, std::string appToken, std::time_t expires_at);

	~PushNotifier();


	/**
	 * A PushNotifier device
	 *
	 * TODO
	 */
	typedef struct {
		std::string title;
		std::string id;
		std::string model;
		std::string image;

		std::string toString() {
			return "Title: " + title + " ID: " + id + " Model: " + model;
		}
	} Device;


	/**
	 * Represents an AppToken
	 *
	 * TODO
	 */
	typedef struct {
		std::string token;
		std::time_t expires_at;

		bool isAboutToExpire() {
			return std::difftime(expires_at, std::time(nullptr)) < 48.0 * 3600;
		}
	} AppToken;


	/**
	 * Log in as a user
	 *
	 * @param string       $username Username
	 * @param string       $password Password
	 * @param bool         $apply    Automatically apply the AppToken to this instance
	 *
	 * @return AppToken
	 *
	 * @throws InvalidCredentialsException If the users couldn't be authenticated
	 * @throws PushNotifierException If an unknown error happened
	 */
	AppToken login(std::string username, std::string password, bool apply = true);


	/**
	 * Refresh the current AppToken
	 * This can be done any time but shouldn't be done until 48 hours before it expires
	 *
	 * @param bool apply Automatically apply the AppToken to this instance
	 *
	 * @return AppToken
	 *
	 * @throws InvalidAppTokenException If the token had expired already
	 * @throws PushNotifierException If an unknown error happened
	 */
	AppToken refreshToken(bool apply = true);


	/**
	 * Get all devices the current user has
	 *
	 * @return Device[]
	 *
	 * @throws InvalidAppTokenException If the AppToken has expired or is invalid
	 * @throws PushNotifierException If an unknown error happened
	 */
	std::vector<Device> getDevices();


	/**
	 * Set the AppToken that should be used
	 *
	 * @param string        $appToken AppToken
	 * @param DateTime|null $expiry   Expiration Date of the AppToken, set null to disable auto-renewal
	 *
	 * @return PushNotifier
	 */
	void setApptoken(std::string appToken, std::time_t expires_at);


	/**
	 * Get the current AppToken
	 * Renews itself automatically in case it is about to expire
	 *
	 * @return string
	 */
	std::string getAppToken();



	/**
	 * Send a text-only message
	 *
	 * @param array        $devices Devices to send to, either Device[] or string[] (IDs of the devices)
	 * @param string       $content Content to send
	 * @param bool         $silent  If true, no sound is played
	 *
	 * @return array                 [ 'success' => [ { device_id: ... }, ... ], 'error' => [ { device_id: ... }, ... ] ]
	 *
	 * @throws DeviceNotFoundException If at least one device could not be found
	 * @throws InvalidAppTokenException If the AppToken has expired or is invalid
	 * @throws PushNotifierException If an unknown error happened
	 */
	bool sendMessage(std::vector<std::string> deviceIDs, std::string content, bool silent = false);


	/// TODO
	bool sendMessage(std::string deviceID, std::string content, bool silent = false);


	/**
	 * Send a link
	 *
	 * @param array        $devices Devices to send to, either Device[] or string[] (IDs of the devices)
	 * @param string       $url     URL to send
	 * @param bool         $silent  If true, no sound is played
	 *
	 * @return array                 [ 'success' => [ { device_id: ... }, ... ], 'error' => [ { device_id: ... }, ... ] ]
	 *
	 * @throws DeviceNotFoundException If at least one device could not be found
	 * @throws InvalidAppTokenException If the AppToken has expired or is invalid
	 * @throws PushNotifierException If an unknown error happened
	 */
	bool sendURL(std::vector<std::string> deviceIDs, std::string url, bool silent = false);


	/// TODO
	bool sendURL(std::string deviceID, std::string url, bool silent = false);


	/**
	 * Send a notification with content and a URL
	 * The user will be taken to the URL after tapping on the notification
	 *
	 * @param array        $devices Devices to send to, either Device[] or string[] (IDs of the devices)
	 * @param string       $content Content to send
	 * @param string       $url     URL to send
	 * @param bool         $silent  If true, no sound is played
	 *
	 * @return array                 [ 'success' => [ { device_id: ... }, ... ], 'error' => [ { device_id: ... }, ... ] ]
	 *
	 * @throws DeviceNotFoundException If at least one device could not be found
	 * @throws InvalidAppTokenException If the AppToken has expired or is invalid
	 * @throws PushNotifierException If an unknown error happened
	 */
	bool sendNotification(std::vector<std::string> deviceIDs, std::string content, std::string url, bool silent = false);


	/// TODO
	bool sendNotification(std::string deviceID, std::string content, std::string url, bool silent = false);



private:
	/**
	 * TODO
	 */
	typedef enum {
		PUT,
		GET,
		POST
	} http_method;

	/**
	 * TODO
	 */
	nlohmann::json call(http_method method, std::string url, const char* xapptoken, nlohmann::json body);

	/// TODO
	std::string username;

	/// TODO
	AppToken appToken;

	/// TODO
	std::string authorization;
};

#endif
