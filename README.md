# Arduino App - Control Web MQTT

Aplicación web para controlar dispositivos Arduino/ESP32 mediante protocolo MQTT.

## 🌐 Demo en línea

La aplicación está desplegada en: https://fantastic-meerkat-7dcc6a.netlify.app/

## 📋 Características

- **Panel de Login**: Interfaz de autenticación para acceder al sistema
- **Dashboard MQTT**: Control de 3 LEDs mediante protocolo MQTT
- **Configuración de Broker**: Configuración flexible para diferentes servidores MQTT
- **Página Quiénes Somos**: Información del equipo de desarrollo
- **Simulador**: Script Python para pruebas locales del sistema MQTT
- **Firmware ESP32**: Código completo para microcontroladores ESP32 con simulación Wokwi

## 📁 Estructura del proyecto

```
Arduino_app/
├── index.html                          # Página de login
├── dashboard.html                      # Panel de control MQTT
├── quienes-somos.html                  # Información del equipo
├── simulador_mqtt.py                   # Simulador MQTT para pruebas
├── assets/
│   ├── config_mqtt.html               # Configuración del broker MQTT
│   ├── css/                           # Estilos
│   ├── js/                            # Scripts JavaScript
│   ├── images/                        # Imágenes y recursos
│   ├── vendor/                        # Librerías de terceros
│   └── esp32_mqtt_complete/           # Firmware ESP32 + simulación Wokwi
├── netlify.toml                        # Configuración de Netlify
└── NETLIFY.md                          # Guía de despliegue
```

## 🚀 Despliegue en Netlify

El proyecto está configurado para desplegarse automáticamente en Netlify.

### Configuración de build:
- **Build command**: (dejar vacío)
- **Publish directory**: `.` (punto)

Para más detalles sobre la configuración y solución de problemas, consulta el archivo [NETLIFY.md](NETLIFY.md).

## 🔧 Requisitos

- Navegador web moderno
- Conexión a internet
- Servidor MQTT configurado
- Dispositivo ESP32 (opcional, para control real)

## 📖 Uso

1. Accede a la aplicación desde [https://fantastic-meerkat-7dcc6a.netlify.app/](https://fantastic-meerkat-7dcc6a.netlify.app/)
2. Inicia sesión con tus credenciales
3. Configura tu servidor MQTT en el panel de configuración
4. Controla los LEDs desde el dashboard

### Pruebas locales

Para realizar pruebas locales, puedes utilizar el simulador:

```bash
python3 simulador_mqtt.py
```

## 🛠️ Desarrollo

El firmware para ESP32 se encuentra en `assets/esp32_mqtt_complete/` e incluye:
- Código completo para el ESP32
- Configuración de simulación Wokwi
- Ejemplos de conexión MQTT

## 👥 Equipo

Información del equipo de desarrollo disponible en la sección "Quiénes Somos" de la aplicación.

## 📄 Licencia

Este proyecto es de código abierto y está disponible para uso educativo y de desarrollo.
