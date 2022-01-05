#ifndef REALTIMEDATABASE_H
#define REALTIMEDATABASE_H

#include "Arduino.h"
#include "FirebaseAuth.h"
#include "FirebaseClient.h"
#include "FirebaseConstant.h"
#include "Json.h"

class RTDB {
   public:
    using DBHandler = void (*)(Json);
    struct db_stream_t {
        DBHandler handler;
        String server;
        String path;

        db_stream_t() : handler(NULL), server(String()), path(String()) {}
        db_stream_t(const String &server, const String &path, DBHandler &handler)
            : server(server), path(path), handler(handler){};
    };

    RTDB() {}
    RTDB &database(const String &dbName);
    RTDB &path(const String &pathName);
    RTDB &payload(Json &json);
    bool get(DBHandler handler = NULL);
    bool set(DBHandler handler = NULL);
    bool push(DBHandler handler = NULL);
    bool update(DBHandler handler = NULL);
    bool remove(DBHandler handler = NULL);
    void listen(DBHandler handler);

   private:
    String host;
    String paths;
    String payloads;
    static void streamTask(void *);
};

#endif
