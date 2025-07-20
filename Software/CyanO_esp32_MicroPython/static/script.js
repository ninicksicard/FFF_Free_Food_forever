function updateData() {
    // Fetch data from the server
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            // Update the paragraphs with the new data
            document.getElementById('sensorStatus').textContent = 'Sensor status: ' + JSON.stringify(data.sensors);
            document.getElementById('currentMode').textContent = 'Current mode: ' + data.mode;
            document.getElementById('camRelayValues').textContent = 'Cam and relay values: ' + JSON.stringify(data.camRelayValues);
            document.getElementById('currentTime').textContent = 'Current time: ' + data.time;
        });
}

function extract() {
    // Send an HTTP POST request to the server when the button is clicked
    fetch('/extract', {
        method: 'POST',
        body: new URLSearchParams('duration=' + document.getElementById('duration').value)
    });
}

// Call updateData every second
setInterval(updateData, 1000);
