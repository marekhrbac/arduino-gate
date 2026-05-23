# arduino-gate

**Open gate with Siri using D1 Mini (ESP8266) + 5V Relay + Cloudflare Workers**

---

## How It Works

```
"Hey Siri, open gate"
        ↓
iPhone Siri Shortcut
        ↓
HTTP GET → Cloudflare Worker (sets status = "open" in KV)
        ↓
D1 Mini polls Worker every 2 seconds
        ↓
Detects "open" → triggers relay for 500ms
        ↓
Relay closes IMP + GND terminals on gate controller
        ↓
Gate opens 
        ↓
D1 Mini calls action=done → Worker resets status to "idle"
```

---

## Hardware Required

| Component | Description |
|---|---|
| Wemos D1 Mini (ESP8266) | Main controller with WiFi |
| 5V Relay Module | Controls gate terminal |
| Micro USB cable (data) | For flashing firmware |
| 5V USB power adapter | For permanent installation |
| Jumper wires | For connections |

---

## Wiring Diagram

```
┌─────────────────────────────────────────────────────┐
│                                                     │
│   WEMOS D1 MINI          RELAY MODULE               │
│   ┌──────────┐           ┌──────────┐               │
│   │       3V3├───────────┤VCC       │               │
│   │       GND├───────────┤GND       │               │
│   │        D2├───────────┤IN        │               │
│   │          │           │          │               │
│   │  [ESP8266]│           │ COM  NO  │               │
│   └──────────┘           └──┬───┬──┘               │
│                             │   │                   │
│                    ┌────────┘   └────────┐          │
│                    │                     │          │
│              ┌─────┴─────────────────────┴─────┐   │
│              │   GATE CONTROLLER                │   │
│              │                                  │   │
│              │   [ IMP / START ]  [ GND / COM ] │   │
│              └──────────────────────────────────┘   │
└─────────────────────────────────────────────────────┘
```

> ⚠️ **Important:** Use 3.3V (not 5V) to power the relay VCC – this is required for reliable operation with D1 Mini's 3.3V logic level.

---

## Software Setup

### 1. Arduino IDE – Install ESP8266 Board

1. Open **File → Preferences**
2. Add to Additional Board Manager URLs:
   ```
   https://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Go to **Tools → Board → Board Manager**
4. Search `esp8266` → Install **esp8266 by ESP8266 Community**
5. Select board: **Tools → Board → LOLIN(WEMOS) D1 R2 & mini**

### 2. Configure the Sketch

Open `gate-arduino.ino` and update these values:

```cpp
const char* ssid      = "YOUR_WIFI_SSID";
const char* password  = "YOUR_WIFI_PASSWORD";
const char* statusUrl = "https://YOUR-WORKER.YOUR-NAME.workers.dev/?key=YOUR_SECRET";
const char* doneUrl   = "https://YOUR-WORKER.YOUR-NAME.workers.dev/?key=YOUR_SECRET&action=done";
```

### 3. Upload to D1 Mini

Connect via micro USB and click **Upload** in Arduino IDE.

---

## Cloudflare Worker Setup

### 1. Create Worker

1. Go to [dash.cloudflare.com](https://dash.cloudflare.com)
2. Click **Build → Workers & Pages → Create Application → Start with Hello World**
3. Name it `gate-worker` → Deploy
4. Click **Edit code** and paste the contents of `gate-worker`
5. Click **Deploy**

### 2. Create KV Namespace

1. Go to **Storage & databases → Workers KV**
2. Click **Create instance** → name it `gate-kv` → Key:status and value:idle -> Connect
3. Go back to your Worker → **Bindings → Add a binding**
4. Select **KV Namespace**
5. Variable name: `GATE`, KV Namespace: `gate-kv`
6. Save and redeploy

### 3. Add Secret Key

1. In Worker **Settings → Variables and Secrets → Add**
2. Select **Secret**
3. Name: `SECRET_KEY`, Value: your secret password
4. Deploy

### 4. Test the Worker

Open in browser:
```
# Check status (should return "idle")
https://gate.YOUR-NAME.workers.dev/?key=YOUR_SECRET

# Trigger gate open
https://gate.YOUR-NAME.workers.dev/?key=YOUR_SECRET&action=open

# Reset status
https://gate.YOUR-NAME.workers.dev/?key=YOUR_SECRET&action=done
```

---

## Siri Shortcut Setup

1. Open **Shortcuts** app on iPhone
2. Tap **+** → **Add Action**
3. Search for **Get contents of URL**
4. Set URL to:
   ```
   https://gate.YOUR-NAME.workers.dev/?key=YOUR_SECRET&action=open
   ```
5. Rename shortcut to **open gate**
6. Say **"Hey Siri, open gate"** ✅

---

## Serial Monitor Output

When working correctly you should see:

```
Connecting WiFi.....
WiFi connected!
IP address: 192.168.1.xxx
Status: idle
Status: idle
Status: open
>>> Opening gate!
>>> Done, status reset
Status: idle
```

---
