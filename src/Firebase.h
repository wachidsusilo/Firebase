#ifndef FIREBASE_H
#define FIREBASE_H

#include "Arduino.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "HTTPClient.h"
#include "FirebaseAuth.h"
#include "CloudMessaging.h"

class FirebaseWrapper : protected FCM {
  public:
    FirebaseWrapper() {}
    void begin(String projectId, String projectEmail, String privateKey);
    String getAccessToken();
    
    String getMessageBody(Message message, bool validateOnly = false);
    int sendMessage(Message message, bool validateOnly = false);
    int sendAndroidNotification(Message::MessageType type, String typeVal, String title, String body, String channelId);

  private:
    String projectId;
    int GET(String url);
    int POST(String url, String payload);
};

extern FirebaseWrapper Firebase;

#endif
