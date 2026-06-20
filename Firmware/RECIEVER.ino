/* ========= BIBLIOTEKI ========= */
#include <SPI.h>
#include <RF24.h>
#include <U8g2lib.h>

#include <stdlib.h>
#include <stdio.h>

/* ===== NRF ===== */
#define CE_PIN  12
#define CSN_PIN 13
RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "MBTD1"; // ADRES Pipe0 - TAKI SAM JAK ODBIORNIK

/* ===== OLED SSD1309 SPI ===== */
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(U8G2_R0, 30, /* CS*/  29, /* DC */  28 /* RST */); // Wytworzenie obiektu OLED - u8g2

/* ===== PAYLOAD ===== */
struct Payload {
  float temp;
  float press;
  uint8_t battery;
  uint8_t mail;
};

Payload rx; // Nazwa struktury Payload

/* ===== RYSOWANIE IKONY BATERII ===== */
void drawBatteryIcon(uint8_t percent) {
  char buf[6];              // "100%"
  snprintf(buf, sizeof(buf), "%u%%", percent);
  uint8_t x = 96;
  uint8_t y = 2;
  uint8_t w = 20;
  uint8_t h = 8;

  // obrys
  u8g2.drawFrame(x, y, w, h);
  u8g2.drawBox(x + w, y + 3, 2, h - 6); // styk

  // wypełnienie
  uint8_t fill = map(percent, 0, 100, 0, w - 2);
  u8g2.drawBox(x + 1, y + 1, fill, h - 2);

  uint8_t text_x = x + (w / 2) - (strlen(buf) * 3);
  uint8_t text_y = y + h + 12;
  u8g2.drawStr(text_x, text_y, buf);
}

/* ===== RYSOWANIE IKONY LISTU ===== */
void drawLetterIcon() {
  uint8_t x = 75;
  uint8_t y = 4;
  uint8_t w = 15;
  uint8_t h = 10;

  // obrys
  u8g2.drawFrame(x, y, w, h);
  u8g2.drawLine(x, y, x + w/2, y + h/2); 
  u8g2.drawLine(x + w - 1, y, x + w/2, y + h/2); 
}

/* ========= SETUP ========= */
void setup() {
  SPI.begin();

  u8g2.begin();
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.clearBuffer();
  u8g2.drawStr(0,12,"MBTD v1.0");
  u8g2.drawStr(0,24,"Czekam na dane");
  u8g2.sendBuffer();

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();
  radio.flush_rx();

}

/* ========= LOOP ========= */
void loop() {
  if (radio.available()) { // Czy dostępne są nowe dane w buforze radia
    radio.read(&rx, sizeof(rx)); // Odzcyt danych i zapisanie ich w strukturze 
    radio.flush_rx(); // Wyczyszczenie rejestru odbiorczego radia 


    u8g2.clearBuffer(); // Wyczyszczenie buforu SSD1309 (sterownika w wyśweitlaczu OLED)

    /* ===== STATUS POCZTY ===== */
    if (rx.mail) {
      u8g2.drawStr(0, 14, "NOWA POCZTA!");
      drawLetterIcon();
    } else {
      u8g2.drawStr(0, 14, "ODEBRANO!");
    }

    /* ===== IKONA BATERII ===== */
    drawBatteryIcon(rx.battery);

    /* ===== KOMUNIKATY BATERII ===== */
    if (rx.battery <= 10) {
      u8g2.drawStr(0, 54, "NALADUJ AKUMULATOR!");
    } else if (rx.battery <= 20) {
      u8g2.drawStr(0, 54, "Bateria < 20%");
    }
    
    u8g2.setCursor(0, 30);
    u8g2.print("Temp: ");
    u8g2.print(rx.temp);
    u8g2.print(" C");
    
    u8g2.setCursor(0, 42);
    u8g2.print("Cisn: ");
    u8g2.print(rx.press);
    u8g2.print(" hPa");

    u8g2.sendBuffer();
  }
}
