## Firebase Client Library for ESP32
This library comes with a complete feature of firebase database, firestore, and messaging with simple yet powerful API. You need a service account to be able to use this library. No other authentication methods are supported. Every response value (from callback) is represented as `Json` object. The `Json` library is currently maintained on different repository. You can download it [here](https://github.com/wachidsusilo/Json).

## Examples
The `FirebaseWrapper` object is provided by this library with the name of `Firebase`. You don't need to create a new object on your own.

### Initialization
The initialization process needs `project_id`, `client_email`, and `private_key` from your service account. You can generate a service account from _Project Setting_ in your firebase console.
```c++
#include "Firebase.h"

const String projectId(F("project-bcd12"));
const String clientEmail(F("firebase-adminsdk-abcde@project-bcd12.iam.gserviceaccount.com"));
const String privateKey(F("-----BEGIN PRIVATE KEY-----\n YOUR_PRIVATE_KEY \n-----END PRIVATE KEY-----\n"));

void setup() {
    Serial.begin(115200);

    //Initialize WiFi
    WiFi.begin("SSID", "PASSWORD");
    WiFi.setAutoReconnect(true);
    Serial.println("Connecting to WiFi...");
    while (!WiFi.isConnected()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi is connected");

    //Initialize Firebse
    Firebase.setDebug(Serial);
    Firebase.begin(projectId, clientEmail, privateKey);
    Serial.println("Connecting to Firebase service...");
    while (!Firebase.isReady()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Firebase is ready");
}
```
### Realtime Database
Firebase Realtime Database API needs _database url_ to work. You can get the _database url_ from the Realtime Database tab in your Firebase console. The data of Realtime Database is represented as a `Json` object. You can read the documentation [here](https://github.com/wachidsusilo/Json).
```c++
//The url should looks like this:
const String dbUrl(F("https://project-bcd12-default-rtdb.firebaseio.com"));

//or like this:
const String dbUrl(F("https://project-bcd12-default-rtdb.asia-southeast1.firebasedatabase.app"));

//Get a value from the database
Firebase.database(dbUrl).path("users").get(callback);
```

#### 1. Create a callback
The callback is needed as this library runs asynchronously.
```c++
void callback(Json payload) {
    JSUtil.prettyPrint(Serial, payload);
}
```

#### 2. Get a value from the database
```c++
Firebase.database(dbUrl).path("parent/child/grandchild").get(callback);
```

#### 3. Set the value of a specific path
```c++
Json json;
json.add("id", "EX4MP73")
    .add("name", "John")
    .add("age", 35)
    .add("email", "john@example.com");

Firebase.database(dbUrl).path("users/EX4MP73").payload(json).set(callback);
```

#### 4. Push a value to a specific path
```c++
Json json;
json.add("name", "Diana")
    .add("age", 27)
    .add("email", "diana@example.com");

Firebase.database(dbUrl).path("users").payload(json).push(callback);
```

#### 5. Update an existing value
```c++
Json json;
json.add("age", 37);

Firebase.database(dbUrl).path("users/EX4MP73").payload(json).update(callback);
```

#### 6. Delete an existing value
```c++
Firebase.database(dbUrl).path("users/EX4MP73").remove(callback);
```

#### 6. Listen to a specific path
Please be aware that the `listen()` API is using _Server Sent Events_ technology, it allocates some resources to keep the connection open. You only need to call it once and you will be notified everytime the value changed.
```c++
Firebase.database(dbUrl).path("users/EX4MP73").listen(callback);
```

### Firestore
The firestore API is more or less the same as Realtime Database. The main different is that the data is represented as a `Document` instead of a `Json` object. 

#### 1. Create a document
```c++
Firebase.document("users").create("history", callback);
```

#### 2. Get a value from a document
```c++
//retrieve all fields:
Firebase.document("users").get(callback);

//or just a specific field:
Firebase.document("users").mask("john").get(callback);
Firebase.document("users").mask("[john, diana]").get(callback);
```

#### 3. Update a document
The `Document` class is constructed based on the official [documentation](https://firebase.google.com/docs/firestore/reference/rest/Shared.Types/ArrayValue#Value) of Firebase Firestore REST API.
```c++
Document doc;
doc.addField("name", "john");
doc.addField("tag", Document::Array()
             .push("a tag")
             .push(true)
             .push(47));
doc.addField("age", 32);
doc.addField("location", Document::GeoPoint(11.23, 112.43));
doc.addField("magic_number", 23.45);
doc.addField("interest", Document::Map()
             .add("social", "anything")
             .add("number", 27)
             .add("economy", "money"));
doc.addField("is_active", true);
doc.addField("previous_data", (void*)NULL);
doc.addField("ref", Document::Reference("projects/project-bcd12/databases/(default)/documents/users/history"));
doc.addField("date_created", Document::Timestamp(2021, 6, 15, 7, 15, 23));

//update the document and retrieve the value
Firebase.document("users/history").payload(doc).update(callback);

//update the document but do not retrieve the value
Firebase.document("users/history").payload(doc).mask("nulls").update(callback);

//update specific field of a document
Firebase.document("users/history").payload(doc).updateMask("name").update(callback);
Firebase.document("users/history").payload(doc).updateMask("[name, tag, age]").update(callback);
```

#### 4. Delete a document
Please be careful when using this API, deleting a document is an irreversible process and you may lose all of your valuable fields. If you want to delete a field, use `update()` method above.
```c++
Firebase.document("users").remove(callback);
```

### Firebase Cloud Messaging
The Firebase Cloud Messaging API allows you to push a message or notification into your Web or Mobile Application. The `Message` class is constructed based on the official [documentation](https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages#Message) of the Firebase Cloud Messaging REST API.

#### 1. Send a message or notification
You can construct your own message according to your needs.
```c++
//Create an AndroidNotification object
AndroidNotification androidNote;
androidNote.title = "title";
androidNote.body = "i am a body";
androidNote.channelId = "channelId";

//Create an AndroidConfig object
AndroidConfig android;
android.notification = androidNote;

//Create a Message object
Message message(Message::TOPIC, "topic-name");
message.android = android;

//Send the message
Firebase.sendMessage(message);
```

#### 2. Send an  Android Notification
This library also comes with predefined function to send a simple notification to your android apps.
```c++
Firebase.sendAndroidNotification(Message::TOPIC, "topic-name", "Title", "I am body", "channelId");
```

### Miscellaneous
There is so much thing going on in the background in order to provide all of the convenient methods described above. This library is constantly renewing access token for roughly once every ten minutes in the background, so the access token would theoretically never be expired. The _Server Sent Events_ method is polled constantly, and would be automatically reconnected whenever the connection is dropped. There is also an NTP service running in the background, you can access it using the APIs below.

#### 1. Set Timezone (Time Offset)
By default, the time provided by NTP is used `GMT+0` as it's timezone. You need to set the offset according to your own timezone.
```c++
NTP.setOffset(7);
```

#### 2. Get Current Unix Time
```c++
uint32_t unix = NTP.getUnixTime();
```

#### 3. Get Formatted Date and Time
```c++
String date = NTP.getFormattedDate("/");
String time = NTP.getFormattedTime(":");
```

#### 4. Get individual value of Date and Time
```c++
//DateTime object
NTPUDP::DateTime now = NTP.now();

//or using auto
auto now = NTP.now();

//Date
uint16_t year     = NTP.getYear();
uint8_t  month    = NTP.getMonth();
uint8_t  date     = NTP.getDate();

//Time
uint8_t  hour     = NTP.getHour();
uint8_t  minute   = NTP.getMinute();
uint8_t  second   = NTP.getSecond();

//Day of the week
uint8_t dayOfWeek = NTP.getDayOfTheWeek();

//Others
uint32_t minutesOfDay = NTP.getMinutes();
uint32_t secondsOfDay = NTP.getSeconds();
```