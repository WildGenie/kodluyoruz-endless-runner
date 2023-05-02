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

![Kurulum Şeması](https://user-images.githubusercontent.com/39780/235758273-b6d4a8da-577d-455c-9889-0cc2ba986c2d.png)

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

Fonksiyon örnek kodları:

```c
// LCD ekran için özel grafikler yükler
void initializeGraphics() {
  // LCD ekranı başlat
  lcd.begin();
  // Koşan karakterin iki farklı sprite'ını tanımla
  byte SPRITE_RUN1[] = {
    // Buraya karakterin ilk sprite'ını yaz
  };
  byte SPRITE_RUN2[] = {
    // Buraya karakterin ikinci sprite'ını yaz
  };
}

// Karakterin konumuna göre LCD ekran üzerinde sprite'ını çizer
void drawHero() {
  // Karakterin konumunu temizle
  lcd.setCursor(heroX, heroY);
  lcd.write(" ");
  // Karakterin yeni konumunu belirle
  if (buttonPressed) {
    // Butona basılırsa karakter zıplar
    heroY = 0;
  } else {
    // Butona basılmazsa karakter düşer
    heroY = 1;
  }
  // Karakterin yeni konumunu çiz
  lcd.setCursor(heroX, heroY);
  if (heroY == 0) {
    // Karakter zıplıyorsa ilk sprite'ını göster
    lcd.write(SPRITE_RUN1);
  } else {
    // Karakter zıplamıyorsa ikinci sprite'ını göster
    lcd.write(SPRITE_RUN2);
  }
}

// Engelleri LCD ekran üzerinde çizer
void drawTerrain() {
  // Engellerin konumlarını temizle
  for (int i = 0; i < LCD_WIDTH; i++) {
    lcd.setCursor(i, terrainY);
    lcd.write(" ");
    lcd.setCursor(i, terrainY + 1);
    lcd.write(" ");
  }
  // Engellerin yeni konumlarını belirle
  for (int i = LCD_WIDTH - 1; i >= 0; i--) {
    if (i == LCD_WIDTH - 1) {
      // Eğer engel dizisi sonuna gelinirse yeni bir engel dizisi oluştur
      if (terrainIndex == TERRAIN_LENGTH - LCD_WIDTH) {
        generateTerrain();
      }
      // Engelleri bir karakter sola kaydır
      terrainIndex++;
    }
    // Engelleri ekrana çiz
    lcd.setCursor(i, terrainY);
    lcd.write(typea[terrainIndex + i]);
    lcd.setCursor(i, terrainY + 1);
    lcd.write(typeaa[terrainIndex + i]);
  }
}

// Karakterin konumunu butona basılıp basılmadığına göre günceller
void updateHeroPosition() {
  // Butonun durumunu oku
  buttonPressed = digitalRead(BUTTON_PIN);
}

// Engellerin konumunu günceller
void updateTerrain() {
  // Her döngüde engeller bir karakter sola kaydırılır
}

// Karakterin engellere çarpıp çarpmadığını kontrol eder
bool checkCollision() {
  // Eğer karakterin konumu ve engelin konumu aynı ise çarpışma olmuş demektir
  if (typea[terrainIndex + heroX] != ' ' || typeaa[terrainIndex + heroX] != ' ') {
    return true;
  } else {
    return false;
  }
}
```

## Kaynaklar

Benzer proje kaynaklarına aşağıdaki linklerden ulaşabilirsiniz

[Endless Runner Game - SparkFun Learn](https://learn.sparkfun.com/tutorials/endless-runner-game/all)

[Arduino Endless Run Game Using LCD Display & Push Button : 4 Steps - Instructables](https://www.instructables.com/Arduino-Endless-Run-Game-Using-LCD-Display-Push-Bu/)
