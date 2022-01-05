#ifndef CLOUDMESSAGING_H
#define CLOUDMESSAGING_H

#include <map>
#include <vector>

#include "Arduino.h"

struct MessageUtil {
   protected:
    String jsonPairString(String key, String val) const {
        if (val.isEmpty()) {
            return "";
        } else {
            return "\"" + key + "\":" + "\"" + val + "\",";
        }
    }

    String jsonPairObject(String key, String val) const {
        if (val.isEmpty()) {
            return "";
        } else {
            return "\"" + key + "\":" + val + ",";
        }
    }

    String jsonArray(String key, std::vector<String> arr, bool isDuration = false) const {
        if (arr.empty()) {
            return "";
        } else {
            String str = "\"" + key + "\":[";
            bool isFirst = true;
            for (String s : arr) {
                if (isFirst) {
                    isFirst = false;
                    str += "\"" + s + (isDuration ? (s.endsWith("s") ? "" : "s") : "") + "\"";
                } else {
                    str += ",\"" + s + (isDuration ? (s.endsWith("s") ? "" : "s") : "") + "\"";
                }
            }
            str += "],";
            return str;
        }
    }

    String jsonMap(String key, std::map<String, String> mapString, bool first = false) const {
        if (mapString.empty()) {
            return "";
        } else {
            String str = "\"" + key + "\":{";
            bool isFirst = true;
            for (auto &m : mapString) {
                if (isFirst) {
                    isFirst = false;
                    str += "\"" + m.first + "\":" + "\"" + m.second + "\"";
                } else {
                    str += ",\"" + m.first + "\":" + "\"" + m.second + "\"";
                }
            }
            str += "},";
            return str;
        }
    }
};

struct NotificationConfig : protected MessageUtil {
    String title;
    String body;
    String image;

    NotificationConfig() {}
    NotificationConfig(String title, String body, String image = "")
        : title(title), body(body), image(image) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairString("title", title);
        str += jsonPairString("body", body);
        str += jsonPairString("image", image);
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return title.isEmpty() && body.isEmpty() && image.isEmpty();
    }
};

struct AndroidColor : protected MessageUtil {
    String red;
    String green;
    String blue;
    String alpha;

    AndroidColor() {}
    AndroidColor(String colorHex) {
        if (colorHex.startsWith("#")) {
            colorHex.replace("#", "");
            uint32_t color = strtoul(colorHex.c_str(), NULL, 16);
            alpha = (color >> 24) & 0xff;
            red = (color >> 16) & 0xff;
            green = (color >> 8) & 0xff;
            blue = (color >> 0) & 0xff;
        }
    }

    AndroidColor(String alpha, String red, String green, String blue)
        : red(red), green(green), blue(blue), alpha(alpha) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairObject("red", red);
        str += jsonPairObject("green", green);
        str += jsonPairObject("blue", blue);
        str += jsonPairObject("alpha", alpha);
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    String toHexString(String key = "", bool argb = false) const {
        String str = (!key.isEmpty() ? "\"" + key + "\":\"" : "") + "#";
        str += (argb ? ((alpha.toInt() < 16 ? "0" : "") + String(alpha.toInt(), HEX)) : "");
        str += (red.toInt() < 16 ? "0" : "") + String(red.toInt(), HEX);
        str += (green.toInt() < 16 ? "0" : "") + String(green.toInt(), HEX);
        str += (blue.toInt() < 16 ? "0" : "") + String(blue.toInt(), HEX);
        str += (!key.isEmpty() ? "\"," : "");
        return str;
    }

    bool isEmpty(bool argb = true) const {
        return argb ? (alpha.isEmpty() || red.isEmpty() || green.isEmpty() || blue.isEmpty()) : (red.isEmpty() || green.isEmpty() || blue.isEmpty());
    }
};

struct AndroidLightSettings : protected MessageUtil {
    AndroidColor color;
    String lightOnDuration;
    String lightOffDuration;

    AndroidLightSettings() {
        color = AndroidColor();
    }
    AndroidLightSettings(AndroidColor color, String lightOnDuration = "", String lightOffDuration = "")
        : color(color), lightOnDuration(lightOnDuration), lightOffDuration(lightOffDuration) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += color.toString("color");
        str += jsonPairString("light_on_duration", lightOnDuration.isEmpty() ? "" : (lightOnDuration.endsWith("s") ? lightOnDuration : lightOnDuration + "s"));
        str += jsonPairString("light_off_duration", lightOffDuration.isEmpty() ? "" : (lightOffDuration.endsWith("s") ? lightOffDuration : lightOffDuration + "s"));
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return color.isEmpty() && lightOnDuration.isEmpty() && lightOffDuration.isEmpty();
    }
};

struct AndroidNotification : protected MessageUtil {
    enum AndroidNotificationPriority {
        PRIORITY_UNSPECIFIED,
        PRIORITY_MIN,
        PRIORITY_LOW,
        PRIORITY_DEFAULT,
        PRIORITY_HIGH,
        PRIORITY_MAX
    };

    enum AndroidVisibility {
        VISIBILITY_UNSPECIFIED,
        PRIVATE,
        PUBLIC,
        SECRET
    };

    String notificationPriorityToString(AndroidNotificationPriority notificationPriority) const {
        switch (notificationPriority) {
            case PRIORITY_UNSPECIFIED:
                return "";
            case PRIORITY_MIN:
                return "priority_min";
            case PRIORITY_LOW:
                return "priority_low";
            case PRIORITY_DEFAULT:
                return "priority_default";
            case PRIORITY_HIGH:
                return "priority_high";
            case PRIORITY_MAX:
                return "priority_max";
        }
    }

    String visibilityToString(AndroidVisibility visibility) const {
        switch (visibility) {
            case VISIBILITY_UNSPECIFIED:
                return "";
            case PRIVATE:
                return "private";
            case PUBLIC:
                return "public";
            case SECRET:
                return "secret";
        }
    }

    String tag;
    String body;
    String icon;
    String sound;
    String image;
    String title;
    String ticker;
    String sticky;
    String eventTime;
    String channelId;
    String localOnly;
    String bodyLocKey;
    String titleLocKey;
    String clickAction;
    String defaultSound;
    String notificationCount;
    String defaultLightSettings;
    String defaultVibrateTimings;
    std::vector<String> bodyLocArgs;
    std::vector<String> titleLocArgs;
    std::vector<String> vibrateTimings;
    AndroidColor color;
    AndroidVisibility visibility;
    AndroidLightSettings lightSettings;
    AndroidNotificationPriority notificationPriority;

    AndroidNotification() {
        color = AndroidColor();
        lightSettings = AndroidLightSettings();
        visibility = VISIBILITY_UNSPECIFIED;
        notificationPriority = PRIORITY_UNSPECIFIED;
    }

    AndroidNotification(String title, String body)
        : title(title), body(body) {
        color = AndroidColor();
        lightSettings = AndroidLightSettings();
        visibility = VISIBILITY_UNSPECIFIED;
        notificationPriority = PRIORITY_UNSPECIFIED;
    }

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairString("title", title);
        str += jsonPairString("body", body);
        str += jsonPairString("tag", tag);
        str += jsonPairString("icon", icon);
        str += jsonPairString("sound", sound);
        str += jsonPairString("image", image);
        str += jsonPairString("ticker", ticker);
        str += jsonPairString("sticky", sticky);
        str += jsonPairString("event_time", eventTime);
        str += jsonPairString("channel_id", channelId);
        str += jsonPairString("local_only", localOnly);
        str += jsonPairString("body_loc_key", bodyLocKey);
        str += jsonPairString("title_loc_key", titleLocKey);
        str += jsonPairString("click_action", clickAction);
        str += jsonPairObject("default_sound", defaultSound);
        str += jsonPairObject("default_light_settings", defaultLightSettings);
        str += jsonPairObject("default_vibrate_timings", defaultVibrateTimings);
        str += jsonPairObject("notification_count", notificationCount);
        str += jsonArray("body_loc_args", bodyLocArgs);
        str += jsonArray("title_loc_args", titleLocArgs);
        str += jsonArray("vibrate_timings", vibrateTimings, true);
        str += color.isEmpty(false) ? "" : color.toHexString("color");
        str += jsonPairString("visibility", visibilityToString(visibility));
        str += jsonPairString("notification_priority", notificationPriorityToString(notificationPriority));
        str += lightSettings.isEmpty() ? "" : lightSettings.toString("light_settings");
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return tag.isEmpty() && body.isEmpty() && icon.isEmpty() && sound.isEmpty() && image.isEmpty() &&
               title.isEmpty() && ticker.isEmpty() && sticky.isEmpty() && eventTime.isEmpty() && channelId.isEmpty() &&
               localOnly.isEmpty() && bodyLocKey.isEmpty() && titleLocKey.isEmpty() && clickAction.isEmpty() &&
               defaultSound.isEmpty() && notificationCount.isEmpty() && defaultLightSettings.isEmpty() &&
               defaultVibrateTimings.isEmpty() && bodyLocArgs.empty() && titleLocArgs.empty() &&
               vibrateTimings.empty() && color.isEmpty(false) && visibility == 0 && lightSettings.isEmpty() &&
               notificationPriority == 0;
    }
};

struct AndroidFcmOptions : protected MessageUtil {
    String analyticsLabel;

    AndroidFcmOptions() {}
    AndroidFcmOptions(String analyticsLabel)
        : analyticsLabel(analyticsLabel) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairString("analytics_label", analyticsLabel);
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return analyticsLabel.isEmpty();
    }
};

struct AndroidConfig : protected MessageUtil {
    enum AndroidMessagePriority {
        PRIORITY_UNSPECIFIED,
        PRIORITY_HIGH,
        PRIORITY_NORMAL
    };

    String MessagePriorityToString(AndroidMessagePriority priority) const {
        switch (priority) {
            case PRIORITY_UNSPECIFIED:
                return "";
            case PRIORITY_HIGH:
                return "high";
            case PRIORITY_NORMAL:
                return "normal";
        }
    }

    String ttl;
    String collapseKey;
    String directBootOk;
    String restrictedPackageName;
    std::map<String, String> data;
    AndroidFcmOptions fcmOptions;
    AndroidMessagePriority priority;
    AndroidNotification notification;

    AndroidConfig() {
        fcmOptions = AndroidFcmOptions();
        notification = AndroidNotification();
        priority = PRIORITY_UNSPECIFIED;
    }

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairString("ttl", ttl.isEmpty() ? "" : (ttl.endsWith("s") ? ttl : ttl + "s"));
        str += jsonPairString("collapse_key", collapseKey);
        str += jsonPairObject("direct_boot_ok", directBootOk);
        str += jsonPairString("restricted_package_name", restrictedPackageName);
        str += jsonMap("data", data);
        str += fcmOptions.isEmpty() ? "" : fcmOptions.toString("fcm_options");
        str += notification.isEmpty() ? "" : notification.toString("notification");
        str += jsonPairString("priority", MessagePriorityToString(priority));
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return ttl.isEmpty() && collapseKey.isEmpty() && directBootOk.isEmpty() && restrictedPackageName.isEmpty() && data.empty() && fcmOptions.isEmpty() && notification.isEmpty() && priority == 0;
    }
};

struct WebpushFcmOptions : protected MessageUtil {
    String link;
    String analyticsLabel;

    WebpushFcmOptions() {}
    WebpushFcmOptions(String analyticsLabel, String link)
        : analyticsLabel(analyticsLabel), link(link) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairString("analytics_label", analyticsLabel);
        str += jsonPairString("link", link);
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return analyticsLabel.isEmpty() && link.isEmpty();
    }
};

struct WebpushConfig : protected MessageUtil {
    std::map<String, String> headers;
    std::map<String, String> data;
    String notification;
    WebpushFcmOptions fcmOptions;

    WebpushConfig() {
        fcmOptions = WebpushFcmOptions();
    }

    WebpushConfig(std::map<String, String> headers, std::map<String, String> data, String notification, WebpushFcmOptions fcmOptions)
        : headers(headers), data(data), notification(notification), fcmOptions(fcmOptions) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonMap("headers", headers);
        str += jsonMap("data", data);
        str += jsonPairObject("notification", notification);
        str += fcmOptions.isEmpty() ? "" : fcmOptions.toString("fcm_options");
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return headers.empty() && data.empty() && notification.isEmpty() && fcmOptions.isEmpty();
    }
};

struct ApnsFcmOptions : protected MessageUtil {
    String analyticsLabel;
    String image;

    ApnsFcmOptions() {}
    ApnsFcmOptions(String analyticsLabel, String image = "")
        : analyticsLabel(analyticsLabel), image(image) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairString("analytics_label", analyticsLabel);
        str += jsonPairString("image", image);
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return analyticsLabel.isEmpty() && image.isEmpty();
    }
};

struct ApnsConfig : protected MessageUtil {
    std::map<String, String> headers;
    String payload;
    ApnsFcmOptions fcmOptions;

    ApnsConfig() {
        fcmOptions = ApnsFcmOptions();
    }

    ApnsConfig(std::map<String, String> headers, String payload, ApnsFcmOptions fcmOptions)
        : headers(headers), payload(payload), fcmOptions(fcmOptions) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonMap("headers", headers);
        str += jsonPairObject("payload", payload);
        str += fcmOptions.isEmpty() ? "" : fcmOptions.toString("fcm_options");
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return headers.empty() && payload.isEmpty() && fcmOptions.isEmpty();
    }
};

struct FcmOptions : protected MessageUtil {
    String analyticsLabel;

    FcmOptions() {}
    FcmOptions(String analyticsLabel)
        : analyticsLabel(analyticsLabel) {}

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairString("analytics_label", analyticsLabel);
        str += "},";
        str.replace(",}", "}");
        return str;
    }

    bool isEmpty() const {
        return analyticsLabel.isEmpty();
    }
};

struct Message : protected MessageUtil {
   public:
    enum MessageType {
        TOPIC,
        TOKEN,
        CONDITION
    };

    MessageType type;
    String typeVal;
    String name;
    std::map<String, String> data;
    NotificationConfig notification;
    AndroidConfig android;
    WebpushConfig webpush;
    ApnsConfig apns;
    FcmOptions fcmOptions;

    Message(MessageType type, String typeVal)
        : type(type), typeVal(typeVal) {
        notification = NotificationConfig();
        android = AndroidConfig();
        webpush = WebpushConfig();
        apns = ApnsConfig();
        fcmOptions = FcmOptions();
    }

    String toString(String key = "") const {
        String str = (!key.isEmpty() ? "\"" + key + "\":" : "") + "{";
        str += jsonPairString("name", name);
        str += type == TOPIC ? jsonPairString("topic", typeVal) : "";
        str += type == TOKEN ? jsonPairString("token", typeVal) : "";
        str += type == CONDITION ? jsonPairString("condition", typeVal) : "";
        str += jsonMap("data", data);
        str += notification.isEmpty() ? "" : notification.toString("notification");
        str += android.isEmpty() ? "" : android.toString("android");
        str += webpush.isEmpty() ? "" : webpush.toString("webpush");
        str += apns.isEmpty() ? "" : apns.toString("apns");
        str += fcmOptions.isEmpty() ? "" : fcmOptions.toString("fcm_options");
        str += "},";
        str.replace(",}", "}");
        return str;
    }

   protected:
    Message() {
        notification = NotificationConfig();
        android = AndroidConfig();
        webpush = WebpushConfig();
        apns = ApnsConfig();
        fcmOptions = FcmOptions();
    }
};

class FCM {
   protected:
    String buildMessage(const Message &message, bool validateOnly = false);
};

#endif
