
const splashscreen = document.getElementById('splash-screen'); // pantalla de carga inicial
const loginscreen = document.getElementById('login-screen'); // pantalla de inicio de sesión
const dashboardscreen = document.getElementById('dashboard-screen'); // panel de control principal
const loginform = document.getElementById('login-form'); // formulario donde el usuario escribe su nombre
const userdisplay = document.getElementById('user-display'); // texto en la barra superior que muestra el usuario actual

// 1. transición automática del splash (5 segundos exactos)
setTimeout(() => {
    splashscreen.classList.add('hidden'); // oculta la pantalla de carga 
    loginscreen.classList.remove('hidden'); // muestra la pantalla de login 
}, 5000);

// 2. control de acceso libre (cualquiera entra)
loginform.addEventListener('submit', (e) => {
    e.preventDefault(); // evita que la página se recargue, que es el comportamiento por defecto de los formularios

    // se captura el nombre que escriba el usuario y le borramos los espacios vacíos al inicio y final
    const alias = document.getElementById('username').value.trim();

    // se guarda e imprime en la barra superior. si está vacío, pone "invitado"
    userdisplay.innerText = alias ? alias : "invitado";

    // cambiamos a la pantalla del dashboard
    loginscreen.classList.add('hidden'); // oculta el login
    dashboardscreen.classList.remove('hidden'); // muestra el panel principal

    // iniciamos la conexión al protocolo de comunicación de internet
    conectarmqtt();
});

// lógica de mqtt
let client; 
const statusbadge = document.getElementById('status-badge'); // etiqueta visual que muestra el estado de la conexión

function conectarmqtt() {
    // apunta a tu propio contenedor docker local usando websockets (ws) en el puerto 9001
    const brokerurl = 'ws://localhost:9001/mqtt'; 
    
    statusbadge.innerText = "conectando a docker local..."; // actualiza el texto del estado
    statusbadge.className = "badge disconnected"; // cambia el diseño visual a modo "desconectado" (gris/rojo)

    // intenta establecer la conexión con la librería mqtt externa
    client = mqtt.connect(brokerurl);

    // esto se ejecuta automáticamente cuando la conexión es exitosa
    client.on('connect', () => {
        statusbadge.innerText = "en línea (docker local)"; // avisa en la pantalla que ya está conectado
        statusbadge.className = "badge connected"; // cambia el diseño visual a modo "conectado" (verde)
        
        // ahora puedes usar un tópico simple sin miedo a que se mezcle con nadie más
        // se suscribe al canal de control para escuchar posibles cambios
        client.subscribe('casa/circuito/control');
    });

    // esto se ejecuta si ocurre algún fallo en la comunicación con docker
    client.on('error', (err) => {
        console.error("error en docker mosquitto: ", err); // muestra el error detallado en la consola del navegador
        statusbadge.innerText = "error local"; // cambia el texto en pantalla
        statusbadge.className = "badge disconnected"; // mantiene el estado visual en desconectado
    });
}

// acciones de los botones para enviar comandos mqtt al contenedor local de mosquitto
// botón encender
document.getElementById('btn-on').addEventListener('click', () => {
    // verifica que el cliente exista y que la conexión esté activa
    if (client && client.connected) {
        client.publish('casa/circuito/control', 'on'); // envía la palabra 'on' al canal de control
        console.log("comando mosquitto: on"); // deja un registro en la consola
    }
});

// botón apagar
document.getElementById('btn-off').addEventListener('click', () => {
    // verifica que el cliente exista y que la conexión esté activa
    if (client && client.connected) {
        client.publish('casa/circuito/control', 'off'); // envía la palabra 'off' al canal de control
        console.log("comando mosquitto: off"); // deja un registro en la consola
    }
});

// --- lógica de movimiento del carrusel ---
let currentindex = 0; // variable para saber en qué posición o tarjeta del carrusel estamos

function movecarousel(direction) {
    const track = document.getElementById('carouselTrack'); // la tira o contenedor largo que se mueve
    const cards = document.querySelectorAll('.dynamic-card'); // selecciona todas las tarjetas internas
    const cardwidth = cards[0].offsetWidth + 20; // calcula el ancho de una tarjeta más 20 píxeles de separación (gap)
    const maxscroll = track.scrollWidth - track.offsetWidth; // calcula el límite máximo que se puede deslizar el contenedor

    currentindex += direction; // suma o resta 1 a la posición actual según el botón presionado (-1 izquierda, 1 derecha)

    // evitar salir de los límites
    let targetx = currentindex * cardwidth; // calcula cuántos píxeles debemos mover el carrusel en el eje x
    
    if (targetx < 0) {
        targetx = 0; // si va más allá del inicio, se frena en el cero
        currentindex = 0; // reinicia el índice al primer elemento
    } else if (targetx > maxscroll) {
        targetx = maxscroll; // si va más allá del final, se frena en el máximo posible
        currentindex = Math.ceil(maxscroll / cardwidth); // calcula el índice máximo real basado en ese límite
    }

    // aplica el movimiento físico en la pantalla usando transformaciones css
    track.style.transform = `translateX(-${targetx}px)`;
}