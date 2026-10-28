#include <math.h>

const int N = 5;
float buffer[N];
int indeks = 0;
int jumlah = 0;
float suhuMin = 1000.0;
float suhuMax = -1000.0;
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0;
    jumlahBaca++;

    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);
    float noise = random(-5, 6) * 0.1;
    float suhu = suhuDasar + noise;
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0;
    }
    return suhu;
}
float hitungMovingAverage(float nilaiBaru) {
    buffer[indeks] = nilaiBaru;
    indeks = (indeks + 1) % N;
    if (jumlah < N) {
        jumlah++;
    }
    float total = 0;
    for (int i = 0; i < jumlah; i++) {
        total += buffer[i];
    }
    return total / jumlah;
}

void setup() {
    Serial.begin(115200);
    randomSeed(micros());
    Serial.println("Simulasi Pengolahan Data Suhu");
}

void loop() {
    float suhu = bacaSuhuMock();
    float suhuFilter = hitungMovingAverage(suhu);
    if (suhu < suhuMin) {
        suhuMin = suhu;
    }
    if (suhu > suhuMax) {
        suhuMax = suhu;
    }

    Serial.print("Mentah: ");
    Serial.print(suhu, 2);
    Serial.print(" C | Filter: ");
    Serial.print(suhuFilter, 2);
    Serial.print(" C | Min: ");
    Serial.print(suhuMin, 2);
    Serial.print(" C | Max: ");
    Serial.print(suhuMax, 2);
    Serial.println(" C");

    delay(2000);
}