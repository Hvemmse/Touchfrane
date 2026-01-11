
# Build Guide

## 1. Backend (Debian + Docker)

```bash
docker run -d \
  --name frame-server \
  -p 8080:80 \
  -v /home/shared/frame:/usr/share/nginx/html:ro \
  nginx
```

Generate file list:
```bash
cd /home/shared/frame
ls *.jpg > list.txt
```

## 2. ESP32 Setup
- Flash SPIFFS/LittleFS
- Upload Arduino sketch
- Insert config.ini

## 3. config.ini Example

```ini
[sync]
url=http://192.168.1.97:8080/frame/
interval=12

[display]
delay=5000
```
