"""
==========================================================
  Simulador MQTT — ESP32/Arduino  (Python)
  Emula el comportamiento del microcontrolador real.
  Suscribe a LED1/control, LED2/control, LED3/control
  y publica respuestas en LED1/status, LED2/status, LED3/status
==========================================================

  REQUISITO: pip install paho-mqtt

  USO:
    1. Abrir una terminal y ejecutar:  python3 simulador_mqtt.py
    2. Abrir el dashboard en el navegador
    3. En el dashboard: hacer clic en "Conectar"
    4. Encender/apagar los LEDs desde el dashboard
    5. Ver en la terminal cómo responde el simulador

  NOTAS:
    - El broker debe coincidir con el que configures en el dashboard.
    - La web usa WebSockets (puerto 8884 con SSL en HiveMQ).
    - Este simulador usa TCP directo (puerto 1883), ambos llegan al mismo broker.
==========================================================
"""

import paho.mqtt.client as mqtt
import time
import sys
import random
import string
from datetime import datetime

# ──────────────────────────────────────────────
# CONFIGURACIÓN — debe coincidir con el dashboard
# ──────────────────────────────────────────────
MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT   = 1883          # TCP directo (Python usa este; la web usa WSS 8884)
KEEPALIVE   = 60

TOPICS_CONTROL = {
    "LED1/control": 1,
    "LED2/control": 2,
    "LED3/control": 3,
}
TOPICS_STATUS = {
    1: "LED1/status",
    2: "LED2/status",
    3: "LED3/status",
}

# ──────────────────────────────────────────────
# ESTADO GLOBAL DE LOS LEDs
# ──────────────────────────────────────────────
led_state = {1: False, 2: False, 3: False}   # False = OFF, True = ON
is_connected = False

# ──────────────────────────────────────────────
# HELPERS VISUALES
# ──────────────────────────────────────────────
COLORS = {
    "green":  "\033[92m",
    "red":    "\033[91m",
    "yellow": "\033[93m",
    "cyan":   "\033[96m",
    "white":  "\033[97m",
    "gray":   "\033[90m",
    "reset":  "\033[0m",
    "bold":   "\033[1m",
}

LED_COLOR = {
    1: "red",    # Rojo  (LED1)
    2: "green",  # Verde (LED2)
    3: "cyan",   # Azul  (LED3)
}

def c(text, color):
    return f"{COLORS[color]}{text}{COLORS['reset']}"

def now():
    return datetime.now().strftime("%H:%M:%S")

def print_header():
    print(c("\n╔══════════════════════════════════════════════════╗", "cyan"))
    print(c("║    🔌  Simulador MQTT — ESP32/Arduino            ║", "cyan"))
    print(c("╚══════════════════════════════════════════════════╝", "cyan"))
    print(f"   Broker : {c(MQTT_BROKER, 'white')} : {c(str(MQTT_PORT), 'white')}")
    print(f"   Tópicos: {c('LED1,LED2,LED3 /control  →  /status', 'gray')}")
    print(c("────────────────────────────────────────────────────", "cyan"))

def print_led_dashboard():
    """Muestra el estado actual de los 3 LEDs en la terminal."""
    led_labels = {1: "LED 1 (D3) — Rojo ", 2: "LED 2 (D5) — Verde", 3: "LED 3 (D7) — Azul "}
    print()
    print(c("  ┌─────────────────────────────────────────────┐", "cyan"))
    print(c("  │           Estado actual de LEDs             │", "cyan"))
    print(c("  ├─────────────────────────────────────────────┤", "cyan"))
    for num in [1, 2, 3]:
        state = led_state[num]
        icon  = "●" if state else "○"
        label = "ON " if state else "OFF"
        color = LED_COLOR[num] if state else "gray"
        print(f"  │  {c(icon, LED_COLOR[num])}  {led_labels[num]}  →  {c(label, color)}           │")
    print(c("  └─────────────────────────────────────────────┘", "cyan"))
    print()

def log(msg, level="info"):
    ts = c(f"[{now()}]", "gray")
    icons = {"info": c("ℹ", "cyan"), "ok": c("✔", "green"), "err": c("✘", "red"), "msg": c("↓", "yellow")}
    icon = icons.get(level, "·")
    print(f"  {ts} {icon}  {msg}")

# ──────────────────────────────────────────────
# CALLBACKS MQTT
# ──────────────────────────────────────────────
def on_connect(client, userdata, flags, rc):
    global is_connected
    if rc == 0:
        is_connected = True
        log(c(f"Conectado al broker  {MQTT_BROKER}:{MQTT_PORT}", "green"), "ok")
        # Suscribirse a todos los tópicos de control
        for topic in TOPICS_CONTROL:
            client.subscribe(topic)
            log(f"Suscrito a  {c(topic, 'yellow')}", "info")
        # Publicar estado inicial para sincronizar con el dashboard
        publish_all_states(client)
        print_led_dashboard()
    else:
        errores = {
            1: "Protocolo incorrecto",
            2: "ID de cliente rechazado",
            3: "Servidor no disponible",
            4: "Usuario/contraseña incorrectos",
            5: "No autorizado",
        }
        log(c(f"Error de conexión: {errores.get(rc, f'código {rc}')}", "red"), "err")

def on_disconnect(client, userdata, rc):
    global is_connected
    is_connected = False
    if rc != 0:
        log(c(f"Desconectado inesperadamente (código {rc}). Reconectando...", "yellow"), "err")

def on_message(client, userdata, msg):
    topic   = msg.topic
    payload = msg.payload.decode().strip().upper()

    log(f"Recibido en {c(topic, 'yellow')} → {c(payload, 'white')}", "msg")

    if topic not in TOPICS_CONTROL:
        log(f"Tópico no reconocido: {topic}", "err")
        return

    led_num = TOPICS_CONTROL[topic]

    if payload == "ON":
        led_state[led_num] = True
    elif payload == "OFF":
        led_state[led_num] = False
    elif payload == "TOGGLE":
        led_state[led_num] = not led_state[led_num]
    else:
        log(c(f"Comando no reconocido: '{payload}' (usa ON, OFF o TOGGLE)", "red"), "err")
        return

    # Publicar el nuevo estado de vuelta al dashboard
    new_state_str = "ON" if led_state[led_num] else "OFF"
    status_topic  = TOPICS_STATUS[led_num]
    client.publish(status_topic, new_state_str, retain=True)

    color = "green" if led_state[led_num] else "red"
    log(f"LED{led_num} → {c(new_state_str, color)}  (publicado en {c(status_topic, 'yellow')})", "ok")

    print_led_dashboard()

def publish_all_states(client):
    """Publica el estado actual de todos los LEDs (para sincronizar el dashboard)."""
    for num in [1, 2, 3]:
        state_str = "ON" if led_state[num] else "OFF"
        client.publish(TOPICS_STATUS[num], state_str, retain=True)
    log("Estados iniciales publicados (todos OFF)", "ok")

# ──────────────────────────────────────────────
# CLIENTE MQTT
# ──────────────────────────────────────────────
# ID único para evitar conflictos de sesión en el broker
unique_suffix = ''.join(random.choices(string.ascii_lowercase + string.digits, k=6))
client_id = f"sim-esp32-{unique_suffix}"

client = mqtt.Client(client_id=client_id, clean_session=True)
client.on_connect    = on_connect
client.on_message    = on_message
client.on_disconnect = on_disconnect

# ──────────────────────────────────────────────
# MAIN
# ──────────────────────────────────────────────
def main():
    print_header()
    print(f"  Client ID: {c(client_id, 'gray')}")
    log(f"Conectando a {c(MQTT_BROKER, 'white')}...", "info")

    try:
        client.connect(MQTT_BROKER, MQTT_PORT, KEEPALIVE)
        client.loop_start()

        print(c("\n  Instrucciones:", "bold"))
        print(f"    1. Ejecuta el dashboard en el navegador")
        print(f"    2. Haz clic en {c('Conectar', 'green')} en el navbar")
        print(f"    3. Enciende/apaga los LEDs → verás los cambios aquí")
        print(f"    4. Presiona {c('Ctrl+C', 'yellow')} para detener\n")

        while True:
            time.sleep(1)

    except KeyboardInterrupt:
        print(c("\n\n  🛑 Simulación detenida por el usuario.", "yellow"))
    except Exception as e:
        print(c(f"\n  ✘ Error: {e}", "red"))
        print(c("  Verifica que el broker sea accesible y el puerto 1883 no esté bloqueado.", "gray"))
        sys.exit(1)
    finally:
        if is_connected:
            client.loop_stop()
            client.disconnect()
        print(c("  👋 Desconectado correctamente.\n", "gray"))

if __name__ == "__main__":
    main()