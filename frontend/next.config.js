/**
 * Run `build` or `dev` with `SKIP_ENV_VALIDATION` to skip env validation. This is especially useful
 * for Docker builds.
 */
import "./src/env.js";

/** @type {import("next").NextConfig} */
const config = {
  images: {
    domains: [
      "via.placeholder.com",
      "d1csarkz8obe9u.cloudfront.net",
      "resizing.flixster.com",
      "m.media-amazon.com",
    ],
  },
};

export default config;
