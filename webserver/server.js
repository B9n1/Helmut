const express = require('express');
const bodyParser = require('body-parser');

const app = express();
const PORT = process.env.PORT || 3002;

// Parse JSON requests
app.use(bodyParser.json());

// Handle POST requests to /sensor-data
app.post('/sensor-data', (req, res) => {
  // Access the sensor data from the request body
  const sensorData = req.body;
  
  // Process the sensor data as needed
  console.log('Received Sensor Data:', sensorData);

  // Send a response (optional)
  res.status(200).send('Data received successfully');

  // Broadcast the sensor data to connected clients using SSE
  sendSse(sensorData);
});

// SSE clients storage
let clients = [];

// Handle SSE endpoint
app.get('/sse', (req, res) => {
  // Set headers for SSE
  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');
  res.setHeader('Access-Control-Allow-Origin', '*'); // Allow all origins

  // Add the client to the list
  clients.push(res);

  // Remove the client when the connection is closed
  req.on('close', () => {
    clients = clients.filter(client => client !== res);
  });
});

// Function to broadcast SSE to all connected clients
function sendSse(data) {
  const eventString = `data: ${JSON.stringify(data)}\n\n`;
  clients.forEach(client => client.write(eventString));
}

// Start the server
app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
