#include "Firebase.h"

void FirebaseWrapper::begin(String projectId, String projectEmail, String privateKey) {
  this->projectId = projectId;
  FirebaseAuth.begin(projectEmail, privateKey);
}

String FirebaseWrapper::getAccessToken() {
  return FirebaseAuth.getAccessToken();
}

String FirebaseWrapper::getMessageBody(Message message, bool validateOnly) {
  return FCM::buildMessage(message, validateOnly);
}

int FirebaseWrapper::sendMessage(Message message, bool validateOnly) {
  return POST("https://fcm.googleapis.com/v1/projects/" + projectId + "/messages:send", getMessageBody(message, validateOnly));
}

int FirebaseWrapper::sendAndroidNotification(Message::MessageType type, String typeVal, String title, String body, String channelId) {
  Message *message =  new Message(type, typeVal);
  AndroidNotification *androidNote =  new AndroidNotification();
  androidNote->title = title;
  androidNote->body = body;
  androidNote->channelId = channelId;
  AndroidConfig *android = new AndroidConfig();
  android->notification = *androidNote;
  message->android = *android;
  bool res = sendMessage(*message);
  delete message;
  delete androidNote;
  delete android;
  return res;
}


/*------ Firebase ------*/
int FirebaseWrapper::GET(String url) {
  HTTPClient *http = new HTTPClient();
  WiFiClientSecure *client = new WiFiClientSecure();
  http->begin(*client, url);
  http->addHeader("Authorization", "Bearer " + FirebaseAuth.getAccessToken(), false, true);
  int httpCode = http->GET();
  http->end();
  delete http;
  delete client;
  return httpCode;
}

int FirebaseWrapper::POST(String url, String payload) {
  HTTPClient *http = new HTTPClient();
  WiFiClientSecure *client = new WiFiClientSecure();
  http->begin(*client, url);
  http->addHeader("Content-Type", "application/json", false, true);
  http->addHeader("Authorization", "Bearer " + FirebaseAuth.getAccessToken(), false, true);
  int httpCode = http->POST(payload);
  http->end();
  delete http;
  delete client;
  return httpCode;
}

FirebaseWrapper Firebase;
