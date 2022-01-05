#ifndef FIREBASE_H
#define FIREBASE_H

#include "Arduino.h"
#include "CloudMessaging.h"
#include "Database.h"
#include "FirebaseAuth.h"
#include "FirebaseClient.h"
#include "Firestore.h"
#include "Json.h"

class FirebaseWrapper : protected FCM {
   public:
    FirebaseWrapper() {}
    void begin(const String &projectId, const String &projectEmail, const String &privateKey);
    void attachDebugger(HardwareSerial &serial);
    void detachDebugger();
    bool isReady();

    String getMessageBody(const Message &message, bool validateOnly = false);
    bool sendMessage(const Message &message, bool validateOnly = false);
    bool sendAndroidNotification(Message::MessageType type, const String &typeVal, const String &title, const String &body, const String &channelId);

    RTDB database(const String &dbName);
    FF document(const String &docName);

   private:
    String projectId;
};

extern FirebaseWrapper Firebase;

#endif
