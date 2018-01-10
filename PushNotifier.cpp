#include "PushNotifier.h"
#include "cppcodec/cppcodec/base64_default_rfc4648.hpp"

#include <iostream>

using namespace std;
using json = nlohmann::json;



PushNotifier::PushNotifier() {
	this->authorization = base64::encode((string) APP_PACKAGE + ":" + API_TOKEN);
	curl_global_init(CURL_GLOBAL_DEFAULT);
}



PushNotifier::~PushNotifier() {
	curl_global_cleanup();
}



PushNotifier::PushNotifier(string username, string appToken, time_t expires_at) {
	this->username = username;

	AppToken tok;
	tok.token = appToken;
	tok.expires_at = expires_at;

	this->appToken = tok;

	this->authorization = base64::encode((string) APP_PACKAGE + ":" + API_TOKEN);
	curl_global_init(CURL_GLOBAL_DEFAULT);
}


PushNotifier::AppToken PushNotifier::login(string username, string password, bool apply) {
	json j;
	j["username"] = username;
	j["password"] = password;

	json buf = call(POST, URL_PN_LOGIN, NULL, j);

	if(buf["status"] == "error") {
		throw runtime_error(buf["message"].dump());
	}

	AppToken tok;
	tok.token = buf["app_token"];
	tok.expires_at = buf["expires_at"].get<time_t>();

	if(apply) {
		this->appToken = tok;
	}

	return tok;
}



vector<PushNotifier::Device>PushNotifier::getDevices() {
	vector<Device> devices;
	json j = call(GET, URL_PN_DEVICES, this->appToken.token.c_str(), "{}"_json);

	if(!j.is_array() && j["status"] == "error") {
		throw runtime_error(j["message"].dump());
	}

	for(auto d : j) {
		Device buf;
		buf.id = d["id"];
		buf.image = d["image"];
		buf.model = d["model"];
		buf.title = d["title"];

		devices.push_back(buf);
	}

	return devices;
}



PushNotifier::AppToken PushNotifier::refreshToken(bool apply) {
	json j = call(GET, URL_PN_REFRESH, this->appToken.token.c_str(), "{}"_json);

	if(j["status"] == "error") {
		throw runtime_error(j["message"].dump());
	}

	AppToken tk;
	tk.token = j["app_token"];
	tk.expires_at = j["expires_at"].get<time_t>();

	if(apply) {
		this->appToken = tk;
	}

	return tk;
}



void PushNotifier::setApptoken(string appToken, time_t expires_at) {
	this->appToken.token = appToken;
	this->appToken.expires_at = expires_at;
}



string PushNotifier::getAppToken() {
	if(this->appToken.isAboutToExpire()) {
		refreshToken();
	}

	return this->appToken.token;
}



bool PushNotifier::sendMessage(vector<string> deviceIDs, string content, bool silent) {
	if(content.empty()) {
		throw new runtime_error("Content shall not be empty");
	}

	json request;
	request["devices"] = deviceIDs;
	request["content"] = content;
	request["silent"] = silent;

	json j = call(PUT, URL_PN_TXT, this->appToken.token.c_str(), request);

	return j["error"].empty();
}


bool PushNotifier::sendMessage(string deviceID, string content, bool silent) {
	vector<string> buf;
	buf.push_back(deviceID);

	return sendMessage(buf, content, silent);
}





bool PushNotifier::sendURL(vector<string> deviceIDs, string url, bool silent) {
	if(url.empty()) {
		throw new runtime_error("URL shall not be empty");
	}

	json request;
	request["devices"] = deviceIDs;
	request["url"] = url;
	request["silent"] = silent;

	json j = call(PUT, URL_PN_URL, this->appToken.token.c_str(), request);

	return j["error"].empty();
}


bool PushNotifier::sendURL(string deviceID, string url, bool silent) {
	vector<string> buf;
	buf.push_back(deviceID);

	return sendURL(buf, url, silent);
}





bool PushNotifier::sendNotification(vector<string> deviceIDs, string content, string url, bool silent) {
	if(url.empty() || content.empty()) {
		throw new runtime_error("Content and URL shall not be empty");
	}

	json request;
	request["devices"] = deviceIDs;
	request["url"] = url;
	request["content"] = content;
	request["silent"] = silent;

	json j = call(PUT, URL_PN_NOTIFICATION, this->appToken.token.c_str(), request);

	return j["error"].empty();
}


bool PushNotifier::sendNotification(string deviceID, string content, string url, bool silent) {
	vector<string> buf;
	buf.push_back(deviceID);

	return sendNotification(buf, content, url, silent);
}




// needed for handling curl output¬
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((string*) userp)->append((char*) contents, size * nmemb);
	return size * nmemb;
}



json PushNotifier::call(http_method method, string url, const char* xapptoken, json body) {

	CURL *curl;
	CURLcode res;
	struct curl_slist *chunk = NULL;
	string readBuffer;

	// We need to add an authentication header
	string auth = "Authorization: Basic " + this->authorization;
	chunk = curl_slist_append(chunk, auth.c_str());
	chunk = curl_slist_append(chunk, "Accept: application/json");
	chunk = curl_slist_append(chunk, "Content-Type: application/json");

	// X-AppToken is set
	if(xapptoken != NULL) {
		string buf = (string) "X-AppToken: " + xapptoken;
		chunk = curl_slist_append(chunk, buf.c_str());
	}

	curl = curl_easy_init();

	if(curl) {
		string data = body.dump();

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());


		if(method == POST) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		} else if(method == PUT) {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		}

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		// Perform the request, res will get the return code
		res = curl_easy_perform(curl);

		// Check for errors
		if(res != CURLE_OK) {
			throw runtime_error(curl_easy_strerror(res));
		}

		if(res == 500) {
			throw runtime_error("");
		}

		if(res == 401) {
			throw runtime_error("");
		}

		// always cleanup
		curl_easy_cleanup(curl);
	}

	return json::parse(readBuffer);
}
