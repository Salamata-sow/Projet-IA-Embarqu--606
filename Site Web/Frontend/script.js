// Fonction pour deviner la météo basée sur la température et l'humidité
function devinerMeteo(temp, hum) {
    // Logique météo AI simple
    if (hum > 85) return { texte: "Très Humide", icone: "ph-cloud-rain", couleur: "text-blue-400" };
    if (hum > 75 && temp < 10) return { texte: "Brouillard Froid", icone: "ph-cloud-fog", couleur: "text-gray-300" };
    if (hum > 75) return { texte: "Humide", icone: "ph-cloud", couleur: "text-gray-300" };
    if (hum > 60 && temp < 15) return { texte: "Nuageux Froid", icone: "ph-cloud", couleur: "text-gray-300" };
    if (hum > 50) return { texte: "Partiellement Nuageux", icone: "ph-cloud-sun", couleur: "text-yellow-200" };
    if (temp > 28) return { texte: "Ensoleillé & Chaud", icone: "ph-sun", couleur: "text-yellow-300" };
    if (temp > 20) return { texte: "Ensoleillé", icone: "ph-sun", couleur: "text-yellow-300" };
    if (temp > 10) return { texte: "Dégagé", icone: "ph-sun-dim", couleur: "text-gray-200" };
    return { texte: "Froid", icone: "ph-snowflake", couleur: "text-blue-200" };
}

// Fonction pour obtenir l'URL de l'API (compatible localhost et déploiements)
function getApiUrl() {
    // Utiliser localhost en développement
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        return 'http://localhost:3000/api/weather';
    }
    // Sinon utiliser l'URL relative
    return '/api/weather';
}

async function updateWeather() {
    try {
        const apiUrl = getApiUrl();
        console.log('📡 Récupération données depuis:', apiUrl);
        
        const response = await fetch(apiUrl);
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }
        
        const data = await response.json();
        console.log('✅ Données reçues:', data);
        
        // Récupérer les valeurs
        const temp = data.temperature || 0;
        const hum = data.humidity || 0;
        const pressure = data.pressure || 0;
        const location = data.location || "Station Météo";
        const lastUpdate = new Date(data.lastUpdate);
        
        // 1. Mettre à jour le nom du lieu
        document.getElementById('location-name').innerText = location;
        
        // 2. Mettre à jour les valeurs principales
        document.getElementById('temp-val').innerText = temp.toFixed(1);
        document.getElementById('hum-val').innerText = `${hum.toFixed(0)}%`;
        document.getElementById('pressure-val').innerText = `${pressure.toFixed(0)} hPa`;
        
        // 3. Mettre à jour l'heure
        const timeStr = lastUpdate.toLocaleTimeString('fr-FR', { 
            hour: '2-digit', 
            minute: '2-digit',
            second: '2-digit'
        });
        document.getElementById('last-update').innerText = `Mise à jour: ${timeStr}`;
        
        // 4. Mettre à jour l'interface météo (Prédiction IA)
        const meteo = devinerMeteo(temp, hum);
        document.getElementById('weather-desc').innerText = meteo.texte;
        
        const iconElement = document.getElementById('weather-icon');
        iconElement.className = `ph ${meteo.icone} text-9xl drop-shadow-lg animate-pulse ${meteo.couleur}`;
        
        // 5. Mettre à jour le statut
        document.getElementById('status-display').innerText = "✅ Connecté à ThingSpeak";
        document.getElementById('status-display').className = "text-green-300 text-sm mt-2";
        
        // 6. Animation du point d'état
        document.getElementById('signal-val').className = "text-3xl font-bold mt-2 text-green-400 animate-pulse";

    } catch (error) { 
        console.error("❌ Erreur de connexion:", error);
        document.getElementById('status-display').innerText = `⚠️ Erreur: ${error.message}`;
        document.getElementById('status-display').className = "text-red-300 text-sm mt-2";
        document.getElementById('signal-val').className = "text-3xl font-bold mt-2 text-red-400";
        document.getElementById('last-update').innerText = "Tentative de reconnexion...";
    }
}

// Lancer au démarrage puis toutes les 15 secondes
console.log('🚀 Démarrage de la station météo...');
updateWeather();
setInterval(updateWeather, 15000);
