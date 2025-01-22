/* eslint-disable @typescript-eslint/no-unsafe-call */
"use client";

import { ipc } from "@/lib/ipc";

import { Card, CardContent, CardTitle, CardDescription } from "../ui/card";
import { useRouter } from "next/navigation";

import Image from "next/image";
import { useEffect } from "react";

export const MoviesPage = () => {
  const router = useRouter();

  const movies = [
    {
      id: 1,
      title: "The Space Adventure",
      description: "An intergalactic journey.",
      image:
        "https://d1csarkz8obe9u.cloudfront.net/posterpreviews/cosmos-conference-design-template-b65a6452f49134129091e2dead251a78_screen.jpg",
    },
    {
      id: 2,
      title: "Mystery in the Woods",
      description: "A thrilling mystery.",
      image:
        "https://resizing.flixster.com/0MdjFJqXPqW6_F6LYQXUt9cxUDo=/ems.cHJkLWVtcy1hc3NldHMvbW92aWVzLzk3N2I5OTM2LWE4MzUtNGNmNi1iNmNjLWRhZWVjZDJkOTk4YS5qcGc=",
    },
    {
      id: 3,
      title: "Laugh Out Loud",
      description: "A hilarious comedy.",
      image:
        "https://m.media-amazon.com/images/I/81aOoOsDTRL._AC_UF894,1000_QL80_.jpg",
    },
  ];

  // Add this console.log to your handleMovieClick function
  useEffect(() => {
  const handleMessage = (event: MessageEvent) => {
    console.log("Message received:", event.data);

    try {
      // The data is already an object, no need to parse
      const { data } = event.data;
      
      if (data?.id) {
        console.log("Navigating to movie with ID:", data.id);
        router.push(`/movie-details?id=${data.id}`);
      }
    } catch (err) {
      console.error("Error processing message:", err);
    }
  };

  if (window.chrome?.webview) {
    window.chrome.webview.addEventListener("message", handleMessage);
  }

  return () => {
    if (window.chrome?.webview) {
      window.chrome.webview.removeEventListener("message", handleMessage);
    }
  };
}, [router]);

  // Update handleMovieClick to include more logging
  const handleMovieClick = async (movieId: number) => {
    console.log("Movie clicked:", movieId);

    if (window.chrome?.webview) {
      const message = {
        action: "movieSelected",
        movieId: movieId,
      };
      console.log("Sending webview message:", message);
      window.chrome.webview.postMessage(message);

      console.log("Sending IPC navigate:", { id: movieId.toString() });
      await ipc.send("navigate", {
        id: movieId.toString(),
        action: "navigate",
      });
    } else {
      console.warn("WebView2 environment not detected.");
    }
  };

  const handleNavigate = async (url: string) => {
    try {
      await ipc.send("navigate", { url });
    } catch (error) {
      console.error("Navigation failed:", error);
    }
  };

  return (
    <div className="grid grid-cols-1 gap-4 p-4 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4">
      {movies.map((movie) => (
        <Card
          key={movie.id}
          className="transform cursor-pointer transition-transform duration-300 hover:border-spacing-10 hover:scale-105"
          onClick={() => handleMovieClick(movie.id)}
        >
          <CardContent className="gap-0 border-0 p-0">
            <CardTitle></CardTitle>
            <CardDescription>
              <Image
                src={movie.image}
                alt={movie.title}
                className="h-full w-64 object-cover"
                width={300}
                height={400}
              />
            </CardDescription>
          </CardContent>
        </Card>
      ))}
      <button onClick={() => handleNavigate("http://localhost:3000")}>
        Navigate
      </button>
    </div>
  );
};
