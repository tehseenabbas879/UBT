const express = require("express");
const mongoose = require("mongoose");
const bodyParser = require("body-parser");
const cors = require("cors");

// Replace this with your actual MongoDB URI
const mongoURI = "mongodb+srv://211920:tehseen@ubt.lm2my.mongodb.net/";

// Connect to MongoDB
mongoose.connect(mongoURI, { useNewUrlParser: true, useUnifiedTopology: true })
  .then(() => console.log("Connected to MongoDB"))
  .catch(err => console.error("MongoDB connection error:", err));

// Define a schema for storing GPS data
const LocationSchema = new mongoose.Schema({
  latitude: String,
  longitude: String,
  timestamp: { type: Date, default: Date.now },
});

// Create a model for the schema
const Location = mongoose.model("Location", LocationSchema);

// Initialize Express
const app = express();
app.use(cors());
app.use(bodyParser.json());

// API endpoint to handle incoming GPS data
app.post("/api/location", async (req, res) => {
  try {
    const { latitude, longitude } = req.body;
    if (!latitude || !longitude) {
      return res.status(400).send("Latitude and longitude are required");
    }

    const location = new Location({ latitude, longitude });
    await location.save();
    res.status(200).send("Location saved successfully");
  } catch (error) {
    console.error("Error saving location:", error);
    res.status(500).send("Internal server error");
  }
});

// API endpoint to retrieve all GPS data
app.get("/api/location", async (req, res) => {
  try {
    const locations = await Location.find();
    res.status(200).json(locations);
  } catch (error) {
    console.error("Error retrieving locations:", error);
    res.status(500).send("Internal server error");
  }
});

// Start the server
const PORT = 3000; // You can change this port if needed
app.listen(PORT, () => console.log(`Server running on http://localhost:${PORT}`));
