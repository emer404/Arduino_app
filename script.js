let arduinoStatus = false;
const statusElement = document.getElementById('status');
const btnOn = document.getElementById('btnOn');
const btnOff = document.getElementById('btnOff');
const logContainer = document.getElementById('logContainer');

// Función para agregar logs
function addLog(message) {
    const timestamp = new Date().toLocaleTimeString();
    const logEntry = document.createElement('div');
    logEntry.className = 'log-entry';
    logEntry.textContent = `[${timestamp}] ${message}`;
    logContainer.insertBefore(logEntry, logContainer.firstChild);
}

// Función para encender Arduino
function turnOn() {
    arduinoStatus = true;
    statusElement.textContent = 'Encendido';
    statusElement.classList.add('on');
    addLog('Arduino encendido');
    console.log('Arduino encendido');
    
    // Aquí agregarías la llamada a tu API o WebSocket para comunicarte con Arduino
    // Por ejemplo: fetch('/api/arduino/on', { method: 'POST' });
}

// Función para apagar Arduino
function turnOff() {
    arduinoStatus = false;
    statusElement.textContent = 'Apagado';
    statusElement.classList.remove('on');
    addLog('Arduino apagado');
    console.log('Arduino apagado');
    
    // Aquí agregarías la llamada a tu API o WebSocket para comunicarte con Arduino
    // Por ejemplo: fetch('/api/arduino/off', { method: 'POST' });
}

// Event listeners
btnOn.addEventListener('click', turnOn);
btnOff.addEventListener('click', turnOff);

// Log inicial
addLog('Aplicación iniciada');