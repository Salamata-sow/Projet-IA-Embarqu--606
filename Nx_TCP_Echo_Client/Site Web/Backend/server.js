const express = require('express');
const axios = require('axios');
const cors = require('cors');
const app = express();
const PORT = 3000;

app.use(cors());
app.use(express.static('../Frontend'));

// Configuration ThingSpeak - Station Météo STM32 avec capteur LPS22HH
const CHANNEL_ID = "3355322";
const READ_API_KEY = '2KGT10UVSI343ZUJ';
const THINGSPEAK_BASE_URL = 'https://api.thingspeak.com';

// Route API pour récupérer les données météo
app.get('/api/weather', async (req, res) => {
    try {
        const url = `${THINGSPEAK_BASE_URL}/channels/${CHANNEL_ID}/feeds.json?api_key=${READ_API_KEY}&results=1`;
        const response = await axios.get(url);
        
        if (response.data.feeds.length === 0) {
            return res.status(404).json({ error: "Aucune donnée disponible" });
        }

        const channel = response.data.channel;
        const lastFeed = response.data.feeds[0];
        
        // Formater les données pour le frontend
        res.json({
            location: channel.name,
            latitude: channel.latitude,
            longitude: channel.longitude,
            lastUpdate: lastFeed.created_at,
            temperature: parseFloat(lastFeed.field1) || 0,
            humidity: parseFloat(lastFeed.field2) || 0,
            pressure: parseFloat(lastFeed.field3) || 0,
            raw: lastFeed
        });
    } catch (error) {
        console.error('Erreur ThingSpeak:', error.message);
        res.status(500).json({ 
            error: "Erreur lors de la récupération des données ThingSpeak",
            details: error.message 
        });
    }
});

// Route de santé
app.get('/health', (req, res) => {
    res.json({ status: 'ok', message: 'Station météo en ligne' });
});

app.listen(PORT, () => {
    console.log(`🌤️  Serveur Station Météo lancé sur http://localhost:${PORT}`);
    console.log(`📡 ThingSpeak Channel: ${CHANNEL_ID}`);
    console.log(`🌐 Frontend accessible à http://localhost:${PORT}`);
});
