const CACHE_NAME = 'nexus-iot-v1';

const ASSETS = [
  '/',
  '/index.html',
  '/dashboard.html',
  '/quienes-somos.html',
  '/manifest.json',
  '/assets/css/main.css',
  '/assets/css/util.css',
  '/assets/css/custom-style.css',
  '/assets/js/main.js',
  '/assets/js/jquery-3.2.1.min.js',
  '/assets/js/bootstrap.bundle.min.js',
  '/assets/images/icons/icon-192.png',
  '/assets/images/icons/icon-512.png'
];

self.addEventListener('install', event => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then(cache => cache.addAll(ASSETS))
      .then(() => self.skipWaiting())
  );
});

self.addEventListener('activate', event => {
  event.waitUntil(
    caches.keys().then(keys =>
      Promise.all(
        keys
          .filter(key => key !== CACHE_NAME)
          .map(key => caches.delete(key))
      )
    ).then(() => self.clients.claim())
  );
});

self.addEventListener('fetch', event => {
  // MQTT y WebSocket nunca se cachean
  if (
    event.request.url.includes('hivemq') ||
    event.request.url.includes('ws://') ||
    event.request.url.includes('wss://')
  ) {
    return;
  }

  event.respondWith(
    caches.match(event.request)
      .then(cached => cached || fetch(event.request))
      .catch(() => {
        if (event.request.destination === 'document') {
          return caches.match('/dashboard.html');
        }
      })
  );
});