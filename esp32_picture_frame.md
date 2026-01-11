# ESP32-C3 Digital Picture Frame

## Hardware

-   ESP32-C3
-   2.8" Touch-skærm (ST7789)
-   SD-kort eller SPIFFS (flash)
-   Wi-Fi
-   Eksisterende touch-UI (zoner, long-press, helpers)

## Formål

Bygge en digital billedramme, som automatisk viser JPEG-billeder i loop
og synkroniserer indhold fra en Debian-maskine via HTTP.

## Funktioner

-   Viser `/pics/*.jpg` i slideshow (loop)
-   Henter kun `.jpg`-filer
-   Synkroniserer 2 gange om dagen
-   Sletter lokale filer der ikke findes på serveren
-   Bruger `config.ini` i flash
-   Indstillinger kan ændres via touch-skærmen

## Eksempel på config.ini

``` ini
[sync]
url=http://192.168.1.97:8080/frame/
interval=12

[display]
delay=5000
```

## Backend (Debian + Docker)

``` bash
docker run -d \
  --name frame-server \
  -p 8080:80 \
  -v /home/shared/frame:/usr/share/nginx/html:ro \
  nginx
```

Filstruktur:

    /home/shared/frame/
      image1.jpg
      image2.jpg
      list.txt

Generér fil-liste:

``` bash
ls *.jpg > list.txt
```

## ESP32 Arkitektur

### Moduler

1.  Slideshow\
2.  HTTP Sync\
3.  INI Læs/Skriv\
4.  Touch Menu

### Sync-flow

1.  Hent `list.txt`\
2.  Download manglende `.jpg`\
3.  Slet forældede filer\
4.  Start slideshow

### Slideshow

-   Loop gennem `/pics/*.jpg`
-   Vis hvert billede i `delay` ms

### Touch-menu

-   Long-press åbner menu
-   Knapper ændrer værdier
-   Gem i `config.ini`
-   Genstart

## Fordele

-   Ingen SD-kort ud
-   Automatisk opdatering
-   Stabil løsning
-   Let at vedligeholde
-   Embedded-venlig

## Status

Projektet er teknisk realistisk, stabilt og klar til implementering.
