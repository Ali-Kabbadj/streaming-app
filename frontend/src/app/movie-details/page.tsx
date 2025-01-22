"use client";
import React, { useEffect, useState } from "react";

interface Movie {
  image: string;
  title: string;
  genre: string;
  releaseDate: string;
  rating: string;
  description: string;
}

export default function MovieDetailsPage() {
  const [movie, setMovie] = useState<Movie | null>(null);

  useEffect(() => {
    const handleMessage = (event: MessageEvent) => {
      console.log("Received message from WebView:", event.data);

      try {
        // Parse JSON if the data is a string
        const movieData =
          typeof event.data === "string" ? JSON.parse(event.data) : event.data;

        console.log("Parsed movie data:", movieData);
        setMovie(movieData);
      } catch (error) {
        console.error("Failed to parse message data:", error);
      }
    };

    if (window.chrome?.webview) {
      // Set up the message listener
      window.chrome.webview.addEventListener("message", handleMessage);
    } else {
      console.warn("WebView messaging is not available.");
    }

    // Cleanup listener on unmount
    return () => {
      if (window.chrome?.webview) {
        window.chrome.webview.removeEventListener("message", handleMessage);
      }
    };
  }, []);

  if (!movie) {
    return (
      <div className="flex h-screen items-center justify-center">
        <div className="text-lg">Loading movie details...</div>
      </div>
    );
  }

  return (
    <div className="mx-auto max-w-3xl p-6">
      <img
        src={movie.image}
        alt={movie.title}
        className="mb-4 h-auto w-full rounded-md object-cover"
      />
      <h1 className="mb-2 text-3xl font-bold">{movie.title}</h1>
      <p className="mb-4 text-sm text-gray-500">
        Genre: {movie.genre} | Release Date: {movie.releaseDate}
      </p>
      <p className="mb-4 text-sm text-gray-500">Rating: {movie.rating}</p>
      <p className="mb-6 text-lg text-gray-700">{movie.description}</p>
    </div>
  );
}
