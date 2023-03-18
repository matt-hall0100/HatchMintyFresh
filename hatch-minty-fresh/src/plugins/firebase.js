// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyDl3rg1NaCUEg9g9l6VaDx1r19zWuCAbUE",
  authDomain: "hatchmintyfresh.firebaseapp.com",
  databaseURL: "https://hatchmintyfresh-default-rtdb.firebaseio.com",
  projectId: "hatchmintyfresh",
  storageBucket: "hatchmintyfresh.appspot.com",
  messagingSenderId: "862008539368",
  appId: "1:862008539368:web:7b176a64818fc9a5a1715d",
  measurementId: "G-3WKVDZ0HBV"
};

// Initialize Firebase
export const app = initializeApp(firebaseConfig);
export const db = getDatabase(app);
