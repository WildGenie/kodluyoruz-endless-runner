# Kodluyoruz Tinkercad Atolyesi

# Arduino ile Endless Runner Oyunu

Bu workshop’ta Arduino, buton ve karakter LCD kullanarak endless runner tarzı bir oyun kodlayacağız. Bu oyun türü, karakterin sürekli olarak ileri doğru hareket ettiği ve butona basarak zıplayıp engellerden kaçındığı basit bir platform oyunudur.

## Gereken Malzemeler

- Arduino Uno
- Buton
- Karakter LCD (16x2)
- Potansiyometre
- Breadboard
- Jumper kablolar

## Eğitmen İçeriği

- **Arduino’nun temel bileşenlerini tanıtın**. Arduino’nun ne olduğunu, nasıl programlandığını, hangi pinlerin ne işe yaradığını anlatın. Arduino ile kullanabileceğiniz sensörler, motorlar, LED’ler gibi bileşenlerden bahsedin.
- **Oyunun mantığını açıklayın**. Oyunun nasıl çalıştığını, karakterin nasıl hareket ettiğini, engellerin nasıl oluşturulduğunu ve puan sisteminin nasıl işlediğini anlatın. Oyunun kodunu göstererek hangi kısımların ne işe yaradığını açıklayın.
- **Oyunu kurulumunu yapın**. Arduino’yu bilgisayara bağlayın ve kodu yükleyin. Butonu 7 numaralı pine, LCD’yi 8-13 numaralı pinlere bağlayın. LCD’nin kontrastını ayarlamak için potansiyometre kullanın.
- **Oyunu test edin**. Butona basarak oyunu başlatın ve karakteri zıplatın. Engellere çarpmamaya çalışın ve puanınızı arttırın. Oyunu bitirdiğinizde LCD’de puanınızı görün.

## Kurulum

Arduino’yu bilgisayara bağlayın ve ilgili kodu indirerek kodu yükleyin. Butonu 7 numaralı pine, LCD’yi 8-13 numaralı pinlere bağlayın. LCD’nin kontrastını ayarlamak için potansiyometre kullanabilirsiniz. Aşağıdaki şemaya göre bağlantıları yapın.

![Untitled](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/63934095-64be-49de-9752-746190ec90fa/Untitled.png)

## Oynanış

Butona basarak oyunu başlatın ve karakteri zıplatın. Engellere çarpmamaya çalışın ve puanınızı arttırın. Oyunu bitirdiğinizde LCD’de puanınızı görün.

## Örnek Proje Şablonu

```arduino
// LiquidCrystal kütüphanesini dahil et
#include <LiquidCrystal.h>

// Sabitleri tanımla
#define BTN_PIN 7
#define SPRITE_RUN1 1
#define SPRITE_RUN2 2
// ...

// LCD ekranı tanımla
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// Oyunun durumunu tutan değişkenleri tanımla
static char terrainUpper [TERRAIN_WIDTH + 1];
static char terrainLower [TERRAIN_WIDTH + 1];
static bool buttonPushed = false;
// ...

void setup() {
  // LCD ekranı başlat
  lcd.begin(16, 2);

  // Butonun bağlı olduğu pini giriş olarak ayarla
  pinMode(BTN_PIN, INPUT_PULLUP);

  // Grafikleri LCD'ye yükle
  initializeGraphics();
}

void loop() {
  // Butonun basılıp basılmadığını kontrol et
  if ( digitalRead(BTN_PIN) == LOW ) {
    buttonPushed = true;
  }

  // Oyun başlamadıysa başlatma ekranını göster
  if (!playing) {
    drawHero( (blink) ? HERO_POSITION_OFF : HERO_POSITION_RUN_LOWER_1 );
    drawTerrain();
    lcd.setCursor(0,0);
    lcd.print("Press to start");
    blink = !blink;
    delay(500);

    // Butona basılırsa oyunu başlat
    if (buttonPushed) {
      playing = true;
      buttonPushed = false;
      distance = 0;
      lcd.clear();
    }

    return;
  }

  // Oyun başladıysa karakterin hareketini güncelle
  updateHeroPosition();

  // Engelleri güncelle ve yeni engel oluştur
  updateTerrain();

  // Çarpışma kontrolü yap
  if ( checkCollision() ) {
    playing = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Game Over!");
    lcd.setCursor(0,1);
    lcd.print("Score: ");
    lcd.print(distance);

    return;
  }

  // Puanı arttır ve ekrana yazdır
  distance++;
  lcd.setCursor(12,0);
  lcd.print(distance);

}

```

## Kaynaklar

Benzer proje kaynaklarına aşağıdaki linklerden ulaşabilirsiniz

[Endless Runner Game - SparkFun Learn](https://learn.sparkfun.com/tutorials/endless-runner-game/all)

[Arduino Endless Run Game Using LCD Display & Push Button : 4 Steps - Instructables](https://www.instructables.com/Arduino-Endless-Run-Game-Using-LCD-Display-Push-Bu/)
