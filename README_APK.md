# NEXUS IoT - APK Generada

Se ha generado exitosamente la APK de la aplicación NEXUS IoT con PWA.

## Archivos Generados

- **NEXUS_IoT_Debug.apk** (6.7 MB) - Versión de desarrollo para pruebas
- **NEXUS_IoT_Release.apk** (5.9 MB) - Versión de producción firmada

## Características de la APK

- **Nombre**: NEXUS IoT
- **Package ID**: com.nexus.iot
- **Versión**: 1.0
- **SDK Mínimo**: Android 7.0 (API 24)
- **SDK Objetivo**: Android 14 (API 34)
- **PWA**: Progressive Web App con soporte offline
- **MQTT**: Dashboard de control IoT con MQTT y ESP32

## Instalación

### Para instalar en un dispositivo Android:

1. Transfiere el archivo APK a tu dispositivo Android
2. Habilita "Fuentes desconocidas" en Configuración > Seguridad
3. Toca el archivo APK para instalar
4. Sigue los pasos de instalación

### Para instalar mediante ADB:

```bash
adb install NEXUS_IoT_Release.apk
```

## Configuración de Firma

- **Keystore**: nexus-release.keystore
- **Alias**: nexus
- **Validez**: 10,000 días

## Estructura del Proyecto

- **www/** - Archivos web de la PWA
- **android/** - Proyecto Android nativo con Capacitor
- **assets/** - Recursos de la aplicación
- **manifest.json** - Configuración PWA
- **sw.js** - Service Worker para soporte offline

## Notas Importantes

- La APK usa Capacitor 5.x para compatibilidad con Java 17
- La aplicación mantiene toda la funcionalidad PWA original
- Los iconos se generaron automáticamente desde el logo de Arduino
- El service worker permite funcionamiento offline

## Requisitos del Sistema

- Android 7.0 (API 24) o superior
- Conexión a internet para funcionalidad MQTT
- Permiso de acceso a internet

## Próximos Pasos

Para actualizar la APK en el futuro:

1. Modifica los archivos en el directorio www/
2. Ejecuta: `npx cap sync android`
3. Construye la APK: `cd android && ./gradlew assembleRelease`
4. La APK actualizada estará en: `android/app/build/outputs/apk/release/`

---
Generado el 4 de junio de 2026