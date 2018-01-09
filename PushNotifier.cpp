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
	this->appToken.token = appToken;
	this->appToken.expires_at = expires_at;

	PushNotifier();
}



// needed for handling curl output¬
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((string*) userp)->append((char*) contents, size * nmemb);
	return size * nmemb;
}



string PushNotifier::login(string username, string password, bool apply) {
	json j;
	j["username"] = username;
	j["password"] = password;

	json buf = call(POST, URL_PN_LOGIN, NULL, j);

	if(buf["status"] == "error") {
		throw runtime_error(buf["message"].dump());
	}

	if(apply) {
		this->appToken.token = buf["app_token"];
	}

	return buf["app_token"];
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





json PushNotifier::call(http_method method, string url, const char* xapptoken, json body) {

	CURL *curl;
	CURLcode res;
	struct curl_slist *chunk = NULL;
	string readBuffer;

	// We need to add an authentication header
	string auth = "Authorization: Basic " + this->authorization;
	chunk = curl_slist_append(chunk, auth.c_str()); // TODO
	chunk = curl_slist_append(chunk, "Accept: application/json");
	chunk = curl_slist_append(chunk, "Content-Type: application/json");

	// X-AppToken is set
	if(xapptoken != NULL) {
		string buf = (string) "X-AppToken: " + xapptoken;
		chunk = curl_slist_append(chunk, buf.c_str());
	}

	curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		string data = body.dump();

		if(method == POST) {
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
