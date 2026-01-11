
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>
#include <JPEGDEC.h>
#include <vector>

TFT_eSPI tft = TFT_eSPI();
JPEGDEC jpeg;

unsigned long lastSync = 0;

// -------- INI LÆSER --------
String readIni(const String& key) {
  File f = SPIFFS.open("/config.ini", "r");
  if (!f) return "";

  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.startsWith(key + "=")) {
      int p = line.indexOf('=');
      f.close();
      return line.substring(p + 1);
    }
  }
  f.close();
  return "";
}

// -------- JPEG TEGNER --------
void drawJPEG(JPEGDRAW *p) {
  tft.pushImage(p->x, p->y, p->iWidth, p->iHeight,
                (uint16_t*)p->pPixels);
}

void showImage(const char *name) {
  File f = SPIFFS.open(name);
  if (!f) return;

  jpeg.open(f, drawJPEG);
  jpeg.decode(0, 0, 0);
  f.close();
}

// -------- SLIDESHOW --------
void slideshowLoop() {
  File dir = SPIFFS.open("/pics");
  File f;

  int delayMs = readIni("delay").toInt();

  while ((f = dir.openNextFile())) {
    String name = f.name();
    if (name.endsWith(".jpg")) {
      showImage(name.c_str());
      delay(delayMs);
    }
    f.close();
  }
}

// -------- HTTP DOWNLOAD --------
void downloadFile(String url, String path) {
  HTTPClient http;
  http.begin(url);
  int code = http.GET();

  if (code == 200) {
    File f = SPIFFS.open(path, "w");
    WiFiClient *stream = http.getStreamPtr();

    uint8_t buf[512];
    while (stream->available()) {
      int len = stream->readBytes(buf, sizeof(buf));
      f.write(buf, len);
    }

    f.close();
  }
  http.end();
}

// -------- SYNC --------
String fetchList(String baseURL) {
  HTTPClient http;
  http.begin(baseURL + "list.txt");
  http.GET();
  String data = http.getString();
  http.end();
  return data;
}

void syncImages() {
  String baseURL = readIni("url");
  String list = fetchList(baseURL);

  std::vector<String> remote;

  while (list.length()) {
    int p = list.indexOf('\n');
    String file = list.substring(0, p);
    list.remove(0, p + 1);
    file.trim();

    if (file.endsWith(".jpg")) {
      remote.push_back(file);
      String path = "/pics/" + file;

      if (!SPIFFS.exists(path)) {
        downloadFile(baseURL + file, path);
      }
    }
  }

  // Slet lokale filer der ikke findes remote
  File dir = SPIFFS.open("/pics");
  File f;

  while ((f = dir.openNextFile())) {
    String name = String(f.name()).substring(6);
    if (std::find(remote.begin(), remote.end(), name) == remote.end()) {
      SPIFFS.remove("/pics/" + name);
    }
    f.close();
  }
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  SPIFFS.begin(true);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  WiFi.begin(readIni("ssid").c_str(), readIni("password").c_str());
  while (WiFi.status() != WL_CONNECTED) delay(500);

  syncImages();   // Første sync
}

// -------- LOOP --------
void loop() {
  unsigned long interval = readIni("interval").toInt() * 3600000UL;

  if (millis() - lastSync > interval) {
    syncImages();
    lastSync = millis();
  }

  slideshowLoop();
}
