#include "Firebase.h"

void FirebaseWrapper::begin(const String &projectId, const String &projectEmail, const String &privateKey) {
  this->projectId = projectId;
  FirebaseAuth.begin(projectEmail, privateKey);
}

void FirebaseWrapper::attachDebugger(HardwareSerial &serial) {
  fb_log.attachSerial(serial);
}

void FirebaseWrapper::detachDebugger() {
  fb_log.detachSerial();
}

bool FirebaseWrapper::isReady() {
  return !FirebaseAuth.isExpired();
}

String FirebaseWrapper::getMessageBody(const Message &message, bool validateOnly) {
  return FCM::buildMessage(message, validateOnly);
}

bool FirebaseWrapper::sendMessage(const Message &message, bool validateOnly) {
  return FirebaseClient.POST("fcm.googleapis.com", "/v1/projects/" + projectId + "/messages:send", getMessageBody(message, validateOnly));
}

bool FirebaseWrapper::sendAndroidNotification(Message::MessageType type, const String &typeVal, const String &title, const String &body, const String &channelId) {
  Message message(type, typeVal);
  AndroidNotification androidNote;
  androidNote.title = title;
  androidNote.body = body;
  androidNote.channelId = channelId;
  AndroidConfig android;
  android.notification = androidNote;
  message.android = android;
  return sendMessage(message);
}

RTDB FirebaseWrapper::database(const String &dbName) {
  return RTDB().database(dbName);
}

FF FirebaseWrapper::document(const String &docName) {
  return FF().project(projectId).database("(default)").document(docName);
}


/*------ Firebase ------*/

FirebaseWrapper Firebase;
