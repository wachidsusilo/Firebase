#include "Firestore.h"

FF &FF::project(const String &projectName) {
    host = "firestore.googleapis.com";
    paths = "/v1/projects";
    queries = "";
    payloads = "";
    if (!projectName.startsWith("/")) {
        paths += "/";
    }
    paths += projectName;
    return *this;
}

FF &FF::database(const String &databaseName) {
    paths += "/databases";
    if (!databaseName.startsWith("/")) {
        paths += "/";
    }
    paths += databaseName + "/documents";
    return *this;
}

FF &FF::document(const String &docName) {
    if (!docName.startsWith("/")) {
        paths += "/";
    }
    paths += docName;
    return *this;
}

FF &FF::mask(const String &fieldName) {
    queries += queries.length() > 15 ? "&" : "?";
    queries += "mask.fieldPaths=" + fieldName;
    return *this;
}

FF &FF::updateMask(const String &fieldName) {
    queries += queries.length() > 15 ? "&" : "?";
    queries += "updateMask.fieldPaths=" + fieldName;
    return *this;
}

FF &FF::payload(Document &doc) {
    payloads = doc.toString();
    return *this;
}

bool FF::get(DBHandler handler) {
    return FirebaseClient.GET(host, paths + queries, handler);
}

bool FF::create(const String &docId, DBHandler handler) {
    if (docId.length() > 0) {
        queries += queries.length() > 15 ? "&" : "?";
        queries += "documentId=" + docId;
    }
    return FirebaseClient.POST(host, paths + queries, payloads, handler);
}

bool FF::update(DBHandler handler) {
    return FirebaseClient.PATCH(host, paths + queries, payloads, handler);
}

bool FF::remove(DBHandler handler) {
    return FirebaseClient.DELETE(host, paths + queries, payloads, handler);
}
