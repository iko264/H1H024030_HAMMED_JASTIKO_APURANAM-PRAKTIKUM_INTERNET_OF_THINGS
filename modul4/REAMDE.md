# Skematik/diagram rangkaian
![skematik rangkaian percobaan 4A](documentation/rangkaian%204a.png)

[skematik rangkaian percobaan 4A](documentation/rangkaian%204b.png)

# Library atau dependencies yang diperlukan
- ESP8266Wifi.h
- PubSubCliet.h
- ArduinoJson.h
- DHT.h

# Penjelasan kode
## kode percobaan 4A
mengimplementasikan penerimaan data secara pasif (subscribe) pada topik MQTT topicPerintah. Saat pesan JSON baru diterima via broker, fungsi ```callback()``` terdeteksi secara otomatis, mengonversi string JSON menjadi objek menggunakan ```deserializeJson()```, lalu membaca nilai kunci "perintah" untuk mengubah kondisi logika pin LED (HIGH atau LOW).
## kode percobaan 4B
implementasi komunikasi full duplex. Menggunakan non-blocking via ```millis()```, perangkat secara rutin mengirim data suhu sensor DHT11 tiap 5 detik ke topicData. Pada saat yang sama, perangkat tetap responsif menerima perintah eksekusi aktuator dari topicPerintah tanpa terhenti oleh fungsi ```delay()```.

# Penjelasan setiap fungsi
- ```setup()```: Menginisialisasi baud rate serial, arah mode pin LED, koneksi Wi-Fi, alamat broker/port MQTT, serta mendaftarkan fungsi callback.   
- ```loop()```: Memastikan koneksi MQTT tetap aktif dan terus mengeksekusi ```client.loop()``` untuk memproses pesan masuk serta menjalankan interval timer ```millis()```.   
- ```callback(topic, payload, length)```: Fungsi otomatis yang mengekstraksi array byte payload pesan masuk, mem-parsing format JSON-nya, dan mengendalikan aktuator sesuai status data.   
- ```hubungkanWiFi()```: Memulai koneksi ke SSID dan kata sandi Wi-Fi hingga status terhubung.   
- ```hubungkanMQTT()```: Menghubungkan perangkat ke broker MQTT menggunakan ID unik, lalu mendaftarkan ulang subscription ke topik perintah.

# Penjelasan percabangan/conditional
perulangan:
- ```while (WiFi.status() != WL_CONNECTED)```: Mengulang jeda pemanggilan sampai jaringan Wi-Fi berhasil terhubung.   
- ```while (!client.connected())```: Loop untuk terus mencoba melakukan reconnect ke broker MQTT jika koneksi terputus.   
- ```for (unsigned int i = 0; i < length; i++)```: Mengiterasi tiap byte dari payload MQTT untuk disusun menjadi satu kesatuan objek String. 

percabagan: 
- ```if (error) / if (deserializeJson(...))```: Memeriksa kegagalan konversi/parsing struktur JSON.   
- ```if (String(perintah) == "ON") ... else if ...```: Mengevaluasi isi instruksi perintah dari JSON untuk menentukan status nyala/mati pin LED.   
- ```if (millis() - waktuTerakhirPublish > intervalPublish)```: Memeriksa apakah selisih waktu sistem sudah memenuhi interval 5 detik untuk pengiriman data sensor berikutnya.
# Penjelasan singkat mengenai detail percobaan
### Percobaan 4A 
pengujian penerimaan instruksi kendali remote lewat mekanisme subscribe topik MQTT. Pesan berformat JSON diproses menggunakan fungsi deserialization untuk menyalakan atau mematikan LED secara real-time.

### Percobaan 4B 
eksekusi dua fungsi sekaligus (full-duplex) secara bersamaan. Mempublikasikan pemantauan suhu dari sensor DHT11 secara berkala sembari mempertahankan penerimaan pesan kontrol aktuator tanpa mengalami hambatan blocking.

# Jawaban pertanyaan praktikum yang berkaitan dengan code
## Pertanyaan 4.5.4 No 4
```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";
const char* mqttServer    = "broker.hivemq.com";
const int mqttPort        = 1883;
const char* topicPerintah = "unsoed/tk245004/kelompokAnda/perintah";

const int ledPin = 26; // Pin GPIO LED (mendukung PWM)

WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi callback dipanggil otomatis setiap ada pesan baru masuk
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  // Deserialisasi data JSON yang diterima
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);

  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Mengambil parameter perintah dan intensitas dari payload JSON
  const char* perintah = doc["perintah"];
  int intensitas = doc["intensitas"] | 0; // Mengambil nilai intensitas (default 0 jika kunci tidak ditemukan)

  // Validasi batas nilai PWM (0 - 255)
  if (intensitas < 0) intensitas = 0;
  if (intensitas > 255) intensitas = 255;

  if (String(perintah) == "ON") {
    // Mengatur kecerahan LED menggunakan PWM sesuai nilai intensitas yang diterima
    analogWrite(ledPin, intensitas);
    Serial.print("Aktuator: ON | Intensitas PWM: ");
    Serial.println(intensitas);
  } else if (String(perintah) == "OFF") {
    // Mematikan LED (PWM 0)
    analogWrite(ledPin, 0);
    Serial.println("Aktuator: OFF");
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      client.subscribe(topicPerintah);
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0); // Kondisi awal LED mati
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();
}
```

- `int intensitas = doc["intensitas"] | 0;`: Membaca kunci `"intensitas"` dari objek JSON yang telah dideserialisasi. Karakter `| 0` berfungsi sebagai *fallback* nilai default (0) jika kunci `"intensitas"` tidak dikirim dalam payload JSON.
- `if (intensitas < 0) intensitas = 0;` & `if (intensitas > 255) intensitas = 255;`: Memastikan nilai intensitas berada pada rentang valid sinyal PWM 8-bit (0 hingga 255) agar tidak terjadi kesalahan sinyal pada keluaran GPIO.
- `analogWrite(ledPin, intensitas)`: Menggantikan fungsi `digitalWrite(ledPin, HIGH)` dengan sinyal Modulasi Lebar Pulsa (PWM). Fungsi ini mengatur duty cycle pin LED sesuai nilai intensitas yang diterima (misal `200` dari total `255`), sehingga kecerahan LED dapat diatur secara presisi.
- `analogWrite(ledPin, 0)`: Digunakan saat perintah bernilai `"OFF"` untuk menghentikan sinyal PWM dan mematikan LED secara total.

## Pertanyaan 4.5.4 No 4
```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";
const char* mqttServer    = "broker.hivemq.com";
const int mqttPort        = 1883;

// Topik Data dan Perintah
const char* topicData           = "unsoed/tk245004/kelompokAnda/data";
const char* topicPerintahLED    = "unsoed/tk245004/kelompokAnda/perintah/led";
const char* topicPerintahBuzzer = "unsoed/tk245004/kelompokAnda/perintah/buzzer";

#define DHTPIN 4
#define DHTTYPE DHT11

const int ledPin    = 26;
const int buzzerPin = 25; // Pin GPIO untuk Buzzer (Aktuator Kedua)

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000;

// Fungsi Callback dengan pembedaan topik penerima
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return; // Abaikan jika format JSON tidak valid

  String currentTopic = String(topic);

  // Pembedaan eksekusi berdasarkan topik yang menerima pesan
  if (currentTopic == topicPerintahLED) {
    const char* perintah = doc["perintah"];
    digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
    Serial.print("[LED] Perintah diterima -> Status: ");
    Serial.println(perintah);
  } 
  else if (currentTopic == topicPerintahBuzzer) {
    const char* perintah = doc["perintah"];
    digitalWrite(buzzerPin, String(perintah) == "ON" ? HIGH : LOW);
    Serial.print("[BUZZER] Perintah diterima -> Status: ");
    Serial.println(perintah);
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      // Subscribe ke kedua topik perintah
      client.subscribe(topicPerintahLED);
      client.subscribe(topicPerintahBuzzer);
      Serial.println("Terhubung dan subscribe ke topik LED & Buzzer");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  dht.begin();
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) hubungkanMQTT();
  client.loop();

  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();
    float suhu = dht.readTemperature();
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = suhu;
      char buffer[128];
      serializeJson(doc, buffer);
      client.publish(topicData, buffer);
      Serial.print("Data suhu terkirim: ");
      Serial.println(buffer);
    }
  }
}
```

- `const char* topicPerintahLED` & `const char* topicPerintahBuzzer`: Memisahkan topik kontrol menjadi dua jalur MQTT berbeda agar ESP32/ESP8266 dapat membedakan asal instruksi.
- `const int buzzerPin = 25`: Menentukan GPIO 25 sebagai output kontrol untuk aktuator kedua (buzzer).
- `pinMode(buzzerPin, OUTPUT)` & `digitalWrite(buzzerPin, LOW)`: Mengatur pin buzzer sebagai output digital dan mengeset kondisi awalnya pada status mati (LOW).
- `client.subscribe(topicPerintahLED)` & `client.subscribe(topicPerintahBuzzer)`: Mendaftarkan ESP ke broker untuk mendengarkan pesan dari kedua topik secara simultan saat berhasil terhubung.
- `String currentTopic = String(topic)`: Mengonversi parameter `topic` pesan masuk dari *array of char* menjadi objek `String` untuk mempermudah pembandingan logika.
- `if (currentTopic == topicPerintahLED)`: Mengecek apakah pesan dikirim ke topik LED. Jika ya, mengontrol `ledPin` sesuai isi payload JSON (`ON`/`OFF`).
- `else if (currentTopic == topicPerintahBuzzer)`: Mengecek apakah pesan dikirim ke topik Buzzer. Jika ya, mengontrol `buzzerPin` sesuai isi payload JSON (`ON`/`OFF`).

# dokumentasi
![percobaan 1 hasil di serial monitor](documentation/hasil%20serial%20montitor%20percobaan%201.jpeg)

![percobaan 2 mencoba eksplorasi mqtt eksplorer](documentation/mqtt%20explorer.jpeg)

![percobaan 2 pengiriman data ke broker ](documentation/pengiriman%20data.jpeg) 

![percobaan 2 hasil json ](documentation/json%20mqtt.jpeg)    