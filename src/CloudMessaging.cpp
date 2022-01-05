#include "CloudMessaging.h"

String FCM::buildMessage(const Message &message, bool validateOnly) {
    String str = "{";
    str += (validateOnly ? "\"validate_only\":true," : "\"validate_only\":false,");
    str += message.toString("message");
    str += "}";
    str.replace(",}", "}");
    return str;
}
