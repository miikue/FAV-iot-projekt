# Meteostanice — FAV IoT projekt

Dvě Arduino jednotky měřící počasí uvnitř i venku, spojené přes MQTT. Data se ukládají do CSV a zobrazují v prohlížeči jako grafy.

---

## Architektura

```
[Venkovní Arduino]──┐
                    ├──MQTT──► [Python broker]──► CSV ──► [Web dashboard]
[Domácí Arduino]───┘
```

Obě jednotky se periodicky budí na výzvu brokera (topic `*/ov`), naměří hodnoty a odešlou je zpět. Broker data zapíše do CSV souborů, které webová stránka načítá a vykresluje.

---

## Struktura projektu

```
src/
├── venkovnijednotka/   — Arduino: teplota, vlhkost, srážky, napětí baterií
├── domacijednotka/     — Arduino: teplota, vlhkost, kvalita vzduchu
└── broker/             — Python: MQTT subscriber, zápis do CSV
web/
├── index.html          — dashboard s grafy
├── style.css
└── data/
    ├── dataVenku.csv   — záznamy venkovní jednotky
    └── dataDoma.csv    — záznamy domácí jednotky
```

---

## Jednotky

### Venkovní jednotka (`src/venkovnijednotka/`)
- **DHT22** — teplota a vlhkost vzduchu
- **Hallův senzor** — počítání impulzů srážkoměru (přepočet na mm)
- **Odporový dělič na A6** — měření napětí LiPo baterie
- Odesílá na topic `venku/hod`, poslouchá výzvu na `venku/ov`

### Domácí jednotka (`src/domacijednotka/`)
- **DHT11** — teplota a vlhkost vzduchu
- **MQ-5** — kvalita vnitřního vzduchu (analogové čtení na A0)
- Odesílá na topic `doma/hod`, poslouchá výzvu na `doma/ov`

---

## Broker (`src/broker/meteostanice.py`)

Python skript běžící na počítači/RPi ve stejné síti.

- Připojí se k MQTT brokeru (Mosquitto) na adrese zadané v `brokerip`
- Každých `perioda` sekund (výchozí 60 s) vyzve obě jednotky
- Přijatá data dekóduje a přidá jako řádek do příslušného CSV

**Spuštění:**
```bash
pip install -r requirements.txt
python meteostanice.py
```

> Před spuštěním nastav `brokerip` na IP adresu počítače s Mosquitto.

---

## Web dashboard (`web/`)

Statická HTML stránka — stačí ji otevřít ze stejného adresáře kde leží `data/`.

Zobrazuje 4 grafy:
| Graf | Data |
|------|------|
| Venkovní teplota & vlhkost | DHT22 |
| Vnitřní teplota & vlhkost | DHT11 |
| Srážky & znečištění vzduchu | srážkoměr + MQ-5 |
| Napětí baterií | dělič napětí |

Stránka se automaticky obnovuje každou minutu.

---

## Požadavky

### Arduino
Viz `libraries.txt` v každé složce jednotky. Knihovny nainstaluj přes **Arduino IDE → Tools → Manage Libraries**.

Před nahráním vyplň `arduino_secrets.h`:
```cpp
#define SECRET_SSID "nazev_site"
#define SECRET_PASS "heslo"
```

### Python
```
paho-mqtt >= 1.6.1
```
Na serveru musí běžet **Mosquitto** (nebo jiný MQTT broker) na portu 1883.
