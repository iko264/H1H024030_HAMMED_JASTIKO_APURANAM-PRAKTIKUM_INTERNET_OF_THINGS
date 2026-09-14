# Skematik/diagram rangkaian
![skematik rangkaian percobaan 1A](documentation/percobaan%201a.png)


![skematik rangkaian percobaan 2A](documentation/percobaan%202a.png)

# Library atau dependencies yang diperlukan
- Arduino IDE
- Library ESP8266 wifi 
- NodeMCU ESP8266
- Relay atau LED
- kanel jumper 
- resistor
- breadboard

# Penjelasan kode
## kode percobaan 1A
Program ini mengatur ESP32 sebagai klien WiFi menggunakan WiFi.mode(WIFI_STA).
Proses koneksi ke router diinisialisasi melalui WiFi.begin(ssid, password) dan ditunggu
menggunakan perulangan while hingga statusnya WL_CONNECTED. Setelah berhasil
terhubung, program mengambil dan menampilkan data jaringan seperti alamat IP, MAC
Address, dan RSSI (kekuatan sinyal).

## kode percobaan 2A
Program ini menjadikan ESP32 sebagai pemancar WiFi (hotspot) dengan perintah
WiFi.mode(WIFI_AP). Jaringan SSID dan password mandiri dibuat menggunakan
WiFi.softAP(ap_ssid, ap_password). Alamat IP jaringan lokal ditampilkan dengan
WiFi.softAPIP() (default 192.168.4.1). Pada perulangan utama, program secara aktif
menghitung dan menampilkan jumlah perangkat (klien) yang terhubung ke ESP32
tersebut.

# Penjelasan setiap fungsi


# Penjelasan percabangan/conditional


# Penjelasan singkat mengenai detail percobaan

## Percobaan 1A (Akuisisi Data): 

## Percobaan 2A (Kendali Aktuator): 


# Jawaban pertanyaan praktikum yang berkaitan dengan code
## A. A. Pertanyaan 1.5.4 No 4
```cpp

```



## Pertanyaan 1.6.4 No 4
```cpp

```


# dokumentasi
![percobaan 1 meggunaka led](documentation/percobaan%201a%20menggunakan%20esp.png)

![percobaan 1 dengan mencoba memegang sensor mendekat ke ac](documentation/percobaan%202a%20oleh%20mas%20refan.png)

![percobaan 2 rangkaian skematik](documentation/gambar%203%20percobaan%201.jpeg)