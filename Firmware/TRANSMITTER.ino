/* ========= BIBLIOTEKI ========= */
#include <SPI.h>
#include <RF24.h>
#include <Adafruit_BME280.h>
#include <JC_Button.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

/* ========= NRF24 ========= */
#define NRF_CE   12   // Wyjście CE modułu nRF24L01+ na pinie 12
#define NRF_CSN  13   // Wyjście CSN modułu nRF24L01+ na pinie 13
RF24 radio(NRF_CE, NRF_CSN); // Tworzenie obiektu RF24 o nazwie radio na pinach sterujących NRF_CE i NRF_CSN

const byte address[6] = "MBTD1";  // Adres nadawczy - TAKI SAM JAK W ODBIORNIKU!

/* ========= BME280 ========= */
#define BME_CS 14 // Wyjście CS czujnika BME280 na pinie 14
Adafruit_BME280 bme(BME_CS);  // Tworzenie obiektu Adafruit_BME280 o nazwie bme na pinie BME_CS

/* ========= PPRZYCISKI ========= */
#define BTN_INT0 18   // Wejście przycisku (kontaktronu) na pinie 18 (PD0/INT0)
#define BTN_INT1 19   // Wejście przycisku (kontaktronu) na pinie 19 (PD1/INT1)

Button btnMail(BTN_INT0);   // Tworzenie obiektu Button o nazwie btnMail na pinie BTN_INT0
Button btnNoMail(BTN_INT1); // Tworzenie obiektu Button o nazwie btnNoMail na pinie BTN_INT1

/* ========= AKUMULATOR ========= */
#define BAT_ADC   A0    // Wejście analogowe przelnika akumulatora
#define BAT_MIN   764   // Surowa wartość ADC MIN
#define BAT_MAX   1002  // Surowa wartość ADC MAX

/* ========= FLAGI ========= */
volatile bool flagINT0  = false; // Volatile oznacza w locie/lotny tzn. możliwy do zmiany np w przerwaniu, kompilator nie może jej optymalizować!
volatile bool flagINT1  = false;
volatile bool flagTimer = false;
volatile bool flagWatcher = false;

/* ========= TIMER ========= */
uint8_t sec = 0;  // Zmienna określająca licznik sekund ustawiony w momencie startu na 0

/* ========= PAYLOAD ========= */
struct Payload {    // Strutura danych Payload zawiera informację o temperaturze, ciśnieniu, poziomie procentowym baterii oraz czy jest list czy nie
  float temp;
  float press;
  uint8_t battery;
  uint8_t mail;   // 1 = nowy list, 0 = brak
};

Payload tx; // Nazwa struktury Payload (ang. ładunek) w tym przypadku tx

/* ========= ISR ========= */
ISR(INT0_vect) {  // Ustawienie wektora przerwania 
  flagINT0 = true;
}

ISR(INT1_vect) {
  flagINT1 = true;
}

ISR(TIMER1_COMPA_vect) {
  static uint8_t sec_isr = 0;

  sec_isr++;
  if (sec_isr >= 60) {
    sec_isr = 0;
    flagTimer = true;   // tylko raz na minutę
  }
}

/* ========= INICJALIZACJA TIMER1 ========= */
void timer1_init() {
  cli();
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Tryb CTC, Preskaler 1024
  OCR1A  = 15624;   // Ustawienie TOP licznika TCNT1
  TIMSK1 |= (1 << OCIE1A); // Ustawienie bitu przerwania w rejestrze TIMSK1, włączenie porównania A dla Timera1 
}

/* ========= BODCZYT AKUMULATORA ========= */
uint8_t readBatteryPercent() {
  int adc = analogRead(BAT_ADC); // Odczyt wartości ADC z weścia analogowego

  if (adc <= BAT_MIN) return 0;
  if (adc >= BAT_MAX) return 100;

  return map(adc, BAT_MIN, BAT_MAX, 0, 100); // Mapowaie wartości ADC 
}

/* ========= WYSYŁKA DANYCH POGODOWYCH, STATUSU POCZTY I STANU AKUMULATORA ========= */
void sendWeather(uint8_t mailFlag) {
  radio.powerUp(); //Przejście radia z trybu uśpienia do oktywnego
  delayMicroseconds(2000);  // Wymagany czas w datasheet nRF24L01 (1.5ms)
    
  tx.temp  = bme.readTemperature(); // Pobranie temperatury z czujnika BME280 i przypisanie jej do zmiennej w strukturze
  tx.press = bme.readPressure() / 100.0;  // Pobranie ciśnienia z czujnika BME280 i przypisanie go do zmiennej w strukturze
  tx.mail  = mailFlag; // Pobranie argumentu z funkcji sendWeather i przypisanie go do zmiennej w trukturze
  tx.battery = readBatteryPercent(); // Pobranie wartości z funkcji odczytu akumulatora i przypisanie jej do zmiennej w trukturze

  radio.write(&tx, sizeof(tx)); // wysłanie struktury radiem 
  radio.powerDown(); // Przejście do trybu uśpienia
}

/* ========= SLEEP ========= */
void goSleep() {  // Funkcja usypiania mikrokontrolera
  set_sleep_mode(SLEEP_MODE_IDLE);  // Tryb IDLE - instrukcje cpu nie działają, liczniki i przerwania tak
  sleep_enable(); // Ustawienie bitu SE w rejestrze SMCR - Sleep Mode Control Register 
  sleep_cpu(); // Wykonuje instrukcję SLEEP gdy SE = 1 
  sleep_disable();
}


/* ========= SETUP ========= */
void setup() {
  SPI.begin(); // Uruchomienie SPI
  
  /* BME280 */
  if (!bme.begin()) { // Oczekiwanie na uruchomienie BME
    while (1);
  }

  /* NRF24 */
  radio.begin();
  radio.setPALevel(RF24_PA_MAX); // Ustawienie mocy radia MAX = 0dBm = 1mW 
  radio.setDataRate(RF24_250KBPS); // Ustawienie prędkości transmisji 250kb/s
  radio.openWritingPipe(address); // Uruchomienie nadawania na danym adresie 
  radio.stopListening(); // Przełączenie radia w tryb nadawnia

  /* Przyciski */
  pinMode(BTN_INT0, INPUT); //Ustawienie przycisków jako wejscia
  pinMode(BTN_INT1, INPUT);
  btnMail.begin();
  btnNoMail.begin(); 

  /* Przerwania zewnętrzne */
  EICRA = (1 << ISC01) | (1 << ISC00) | (1 << ISC10) | (1 << ISC11); // Zbocze narastające INT0, INT1
  EIMSK = (1 << INT0) | (1 << INT1); // Włączenie przerwań na INT0/INT1

  /* Timer */
  timer1_init();
  sei();

  delay(100); // Krótkie opóźnienie przez wysłaniem pierwszej informacji 
  sendWeather(0); // Wysłanie informacji pogodowych i akumulatora (TYLKO 1 RAZ ABY NIE BYŁO PUSTEGO EKRANU)
}

/* ========= LOOP ========= */
void loop() {

  btnMail.read(); // Odczyt w pętli stanu przycisku
  btnNoMail.read();

  /* === INT0 -> NOWY LIST === */
  if (flagINT0 && btnMail.wasPressed()) { // Warunek wysłania danych struktury w tym listu
    flagINT0 = false;
    flagWatcher = true;
    sendWeather(1);

    EIFR |= (1 << INTF0);
    flagWatcher = false;
  }

  /* === INT1 -> BRAK POCZTY === */
  if (flagINT1 && btnNoMail.wasPressed()) { // Warunek wysłania danych struktury bez listu
    flagINT1 = false;
    flagWatcher = true;
    sendWeather(0);
    
    EIFR |= (1 << INTF1);
    flagWatcher = false;
  }

  /* === TIMER -> CO 1 MIN === */
  if (flagTimer) {
    flagTimer = false;
    flagWatcher = true; // Flaga wybudzenia
    sendWeather(0); // Wysyłka danych bez inforamcji o liście argument w funckji to określa 0 = brak listu/ 1 = jest list
    flagWatcher = false;
  }

  /* JEŚŁI NIC NIE ROBIMY USYPIAMY MIKROKONTROLER */
  if (!flagINT0 && !flagINT1 && !flagWatcher) {
    goSleep();
  }
}
