/* eslint-disable @typescript-eslint/no-unsafe-call */
"use client";

import { useEffect, useState } from "react";
import { Card, CardContent, CardTitle, CardDescription } from "../ui/card";

import Image from "next/image";
const ipc = await import("@/lib/ipc").then((module) => module.ipc);
type Movie = {
  id: string;
  title: string;
  description: string;
  poster: string;
};

export const MoviesPage = () => {
  const [movies, setMovies] = useState<Movie[]>();
  const [initialLoad, setInitialLoad] = useState(false);

  const LoadMovies = async () => {
    if (!initialLoad) {
      const response = await ipc.send("movies");
      if (response) {
        setMovies(response.movies);
        console.log(movies);
      } else {
        console.log("error seting movies response data");
      }
      setInitialLoad(true);
    }
  };

  useEffect(() => {
    void LoadMovies();
  });

  if (!movies) {
    return (
      <div className="grid grid-cols-1 gap-4 p-4 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4">
        Loading...
      </div>
    );
  }

  return (
    <div className="grid grid-cols-1 gap-4 p-4 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4">
      {movies?.map((movie) => (
        <Card
          key={movie.id}
          className="transform cursor-pointer transition-transform duration-300 hover:border-spacing-10 hover:scale-105"
          // onClick={() => handleMovieClick(movie.id)}
        >
          <CardContent className="gap-0 border-0 p-0">
            <CardTitle></CardTitle>
            <CardDescription>
              <Image
                src={movie.poster}
                alt={movie.title}
                className="h-full w-64 object-cover"
                width={300}
                height={400}
                priority={true}
              />
            </CardDescription>
          </CardContent>
        </Card>
      ))}
    </div>
  );
};
