#include <ESP8266WiFi.h>

const char* sta_ssid = "hammed";
const char* sta_password = "kudalari";

const char* ap_ssid = "ESP32_AccessPoint";
const char* ap_password = "12345678"; // minimal 8 karakter

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Set mode WiFi menjadi Access Point
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Mengaktifkan mode AP+STA...");

  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("AP Lokal aktif. SSID: ");
  Serial.print(ap_ssid);
  Serial.print(", IP: ");
  Serial.println(WiFi.softAPIP());

  // Mulai koneksi mode STA (Klien Internet)
  Serial.print("Mencoba menghubungkan STA ke Internet via: ");
  Serial.println(sta_ssid);
  WiFi.begin(sta_ssid, sta_password);

  // Loop waiting untuk koneksi STA ke router Internet
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Jika berhasil terhubung STA
  Serial.println("");
  Serial.println("STA berhasil terhubung ke Router Internet!");
  Serial.print("IP STA (dari Router): ");
  Serial.println(WiFi.localIP());
  
  // Nyalakan LED indikator koneksi STA sukses
  digitalWrite(ledPin, HIGH);
}

void loop() {
  // Menampilkan jumlah perangkat yang terhubung setiap 5 detik
  // (perbaikan penambahan operator =)
  int jumlahClient = WiFi.softAPgetStationNum(); 
  
  Serial.print("Jumlah perangkat terhubung: ");
  Serial.println(jumlahClient);
  delay(5000);
}