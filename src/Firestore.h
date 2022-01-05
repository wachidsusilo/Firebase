#ifndef FIRESTORE_H
#define FIRESTORE_H

#include "Arduino.h"
#include "Document.h"
#include "FirebaseClient.h"
#include "FirebaseConstant.h"
#include "Json.h"

class FF {
   public:
    using DBHandler = void (*)(Json);
    FF() {}
    FF &project(const String &projectName);
    FF &database(const String &databaseName);
    FF &document(const String &docName);
    FF &payload(Document &doc);
    FF &mask(const String &fieldName);
    FF &updateMask(const String &fieldName);
    bool get(DBHandler handler = NULL);
    bool create(const String &docId = "", DBHandler handler = NULL);
    bool update(DBHandler handler = NULL);
    bool remove(DBHandler handler = NULL);

   private:
    String host;
    String paths;
    String queries;
    String payloads;
};

#endif
