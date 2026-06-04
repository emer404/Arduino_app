# Configuración para despliegue en Netlify

## Estructura del proyecto
- `index.html` — login (redirige al dashboard)
- `dashboard.html` — control MQTT de 3 LEDs
- `assets/config_mqtt.html` — configuración del broker
- `quienes-somos.html` — información del equipo
- `simulador_mqtt.py` — simulador del dispositivo (pruebas locales)
- `assets/esp32_mqtt_complete/` — firmware ESP32 + simulación Wokwi

## Configuración en Netlify

1. **Conectar repositorio:**
   - Ve a Netlify → "Add new site" → "Import an existing project"
   - Conecta tu repositorio de GitHub: emer404/Arduino_app

2. **Configuración de Build:**
   - **Build command:** (dejar vacío)
   - **Publish directory:** `.` (punto) o dejar vacío

3. **Configuración avanzada (si es necesario):**
   - Ve a "Site configuration" → "Build & deploy"
   - Asegúrate de que "Base directory" esté vacío
   - "Publish directory" debe ser `.`

## Solución de problemas

Si recibes "Page not found":

1. **Verifica el directorio de publicación:**
   - En Netlify: Site settings → Build & deploy → Build settings
   - "Publish directory" debe ser `.` (punto)

2. **Verifica los archivos:**
   - Asegúrate de que index.html esté en la raíz del repositorio
   - No debe estar en una subcarpeta

3. **Clear cache y redeploy:**
   - Ve a Deploys → Trigger deploy → Clear cache and retry

4. **Verifica los logs:**
   - Revisa los "Deploy logs" para ver si hay errores específicos