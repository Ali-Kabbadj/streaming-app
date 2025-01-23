# streaming-app

This is a codebase for a streaming app that i am working on.

The codebase has a frontend in Next.js with React and a backend in C++ handling media providers and WebView integration. The frontend currently shows movies using components like MoviesPage.tsx, and the backend has services for media, IPC communication, configuration, etc.

I want to add features like filtering, detailed views, torrent integration, user data (history, favorites), and dynamic provider management. We also need caching, both local and cloud, and video streaming with progress tracking.

Starting with the media providers, the existing code uses IMediaProvider and GenericProvider with manifests. I should consider how to extend this to support torrent providers similarly. Since media and torrent providers are separate, maybe creating a base Provider interface that both can extend, ensuring consistency in how their loaded and managed.

For the frontend , we need main section with categories (movies, TV shows), filtering options, and search. The filtering should be dynamic based on provider capabilities. This means the backend needs to expose provider-specific filters and catalogs through IPC so the frontend can render UI elements dynamically. these are just example views, i might go with them or try a diffrent approch with unified view of someitng, we'll see

For the detailed view, clicking a movie should fetch additional data from the provider. The MovieDetailsPage.tsx currently listens for messages, so expanding the IPC handlers to fetch details when a movie is selected makes sense. Similarly, TV shows would require fetching season and episode data (episode title, image, duration, etc), we might need new IPC endpoints for that.

Torrent integration is another major component, i don't even know how to approach it yet, Torrent providers would need their own interfaces, similar to media providers, possibly with methods to search for torrents based on media ID or title, or maybe if i could find a way to us IMDB id or something of that sort to find appropriate torrent sources from some sort of annoucers or someting, i dont khow, in any case, the frontend would then display available torrents, and selecting one would trigger the playback, this will requires a video player component in the frontend and backend handling of torrent streaming, mpv would be a good choise but its a headace to setup correcly, torrent streaming will for sure need integrating a torrent client library qbitorrent or web torrent?.

User data management (history, favorites) requires a persistent storage solution. The current code uses ConfigManager for configuration, but user-specific data might need a database. Local storage could be used initially, with cloud sync as a future feature. The backend services would need to handle CRUD operations for user data and expose them via IPC.

Caching is already partially implemented with LRUCache. Expanding this to cache provider responses, media details, and torrent results would improve performance. The CacheManager can be enhanced to support different cache tiers (in-memory, disk) and expiration policies based on data type.

Video streaming and progress tracking, another big one, it will require a player component that can handle streaming from torrent sources. The backend might need to manage torrent downloads, buffer content, and send progress updates to the frontend. Storing watch history would involve tracking the media ID, timestamp, and user ID, then saving this data periodically, localy, then cloud (ie local db for now)?

The frontend structure needs to be organized into routes for home, movies, TV shows, search, details, and settings (or depending or what view strucutre i might end up going with). Each route would fetch data via IPC calls. State management will be crucial to handle filters, user preferences, and media playback. Using React context or some other state management library.

I need dynamic UI components based on provider capabilities, which mean the frontend should render filters and options fetched from the backend. For example, if a provider supports filtering by year, or rating, or popularity, or any other sorting/filtering mechanist, the backend shoud annouce it to the frontend, the frontend displays a year selector. This requires the backend to send available filters when providers are loaded, then we need to handle these cases in the frontend with ease, there's gonna be a lot of abstraction to do damn it ahah.

Testing and error handling should not be forgoten either. Each provider integration should be tested for reliability, and the frontend should handle loading and error states gracefully. Utils::Logging, as seen in the current code, will help me diagnose issues.

Security considerations include validating provider manifests, securing IPC communication, and handling user data securely. OAuth or similar mechanisms might be needed for cloud sync (but this is surly for later)

Finally, documentation and future-proofing the architecture will ensure that adding new providers or features is straightforward. Using modular design patterns and keeping the backend and frontend decoupled will aid maintainability, and this is what i am trying to do, write stuff down so i dont foget about them.



### A relativly comprehensive Roadmap for Streaming Application Evolution (will update it regularly)

---

#### **1. Provider Ecosystem Expansion**
**Objective:** Create a unified provider framework supporting media metadata (movies/TV) and torrent sources, enabling dynamic integration via manifests.

##### **Media Providers**
- **Enhance Existing Framework:**
  - **Catalog Discovery:** Extend `IMediaProvider` to expose provider-specific catalogs (e.g., "trending," "top-rated") and filters (genre, year, rating). Return structured metadata for UI rendering.
  - **TV Show Support:** Modify `MediaMetadata` to include `season`/`episode` data. Add methods like `GetSeasons(mediaId)` and `GetEpisodes(seasonId)` to `IMediaProvider`.
  - **Provider-Specific UI Hooks:** Expose filter schemas (e.g., dropdowns, sliders) via IPC so the frontend can dynamically generate UI controls.

##### **Torrent Providers**
- **New Torrent Provider Interface:**
  - Define `ITorrentProvider` with methods like `SearchTorrents(query, mediaType)` and `GetStreamUrl(torrentId)`.
  - Use manifest files similar to media providers, specifying capabilities (e.g., direct streaming, magnet links).
- **Integration with Media Metadata:** Link torrent results to media IDs for seamless "Watch Now" functionality.

##### **Unified Provider Registry**
- **Central `ProviderManager`:** Combine media/torrent providers into a single registry. Track provider health, rate limits, and user preferences (e.g., prioritize certain providers).
- **Central `ProviderManager` Manager:** this will allow the managment of the Providers by the user, maybe adding comunity providers down the line if i can fugure out how to create an SDK. 

---

#### **2. Frontend Architecture Overhaul**
**Objective:** Build a dynamic, filter-driven UI with multi-view navigation and detailed media exploration.

##### **Routing & Views (to rethink)**
- **Routes:**
  - `/`: Home (trending media grid, provider selector, global search).
  - `/movies`: Filterable movie grid (provider, genre, year, rating).
  - `/tv`: TV show grid with season/episode previews.
  - `/details/:mediaId`: Media details + torrent sources.
  - `/watch/:torrentId`: Video player with progress tracking.
  - `/user`: History, favorites, followed media.
- **Dynamic Filtering:**
  - Fetch filter options from providers via IPC (e.g., `/ipc/providers/filters`). Render dropdowns/sliders conditionally.

##### **Media Details Page**
- **Data Fetching:**
  - On navigation, send `getMediaDetails` IPC request. Handle TV shows by fetching seasons/episodes asynchronously.
  - Display torrent availability from registered torrent providers.
- **UI Components:**
  - **TV Show Navigation:** Season selector → Episode grid with thumbnails/descriptions.
  - **Torrent Listings:** Sort by quality, seeders, or provider trust score.

##### **State Management**
- **Context API/Zustand:** Centralize provider states (active providers, filters), user preferences, and playback progress.

---

#### **3. User Data & Personalization**
**Objective:** Implement cross-device user profiles with watch history, favorites, and notifications.

##### **Data Storage**
- **Local Storage:** Use `ConfigManager` for basic preferences. Extend with SQLite for structured data (history, favorites).
- **Cloud Sync (Phase 2):** Add OAuth2/auth hooks. Sync data via REST/GraphQL to a cloud service (e.g., Firebase).

##### **Features**
- **Watch History:** Track timestamps and progress. Expose IPC methods like `savePlaybackProgress(mediaId, time)`.
- **Follow System:** Notify users via WebView messaging when new episodes/seasons of followed media are added.
- **Calendar Integration:** Highlight release dates for followed media on `/user` view.

---

#### **4. Streaming & Torrent Integration**
**Objective:** Enable seamless video playback from torrent sources.

##### **Backend Torrent Handling**
- **Torrent Client Integration:** Use libraries like `libtorrent` to manage downloads. Prioritize streaming-friendly files (e.g., `.mp4`).
- **Streaming Server:** Implement a local HTTP server to proxy torrent content to the WebView (e.g., serve chunks via `localhost:3000/stream?torrentId=...`).

##### **Frontend Player**
- **Video.js/React Player:** Embed a player component in `/watch`. Send progress updates to backend every 10 seconds.
- **Quality Selection:** Allow users to choose between torrents (e.g., 1080p vs. 4K) based on metadata from `ITorrentProvider`.

---

#### **5. Performance & Caching**
**Objective:** Reduce latency and provider API calls.

##### **Caching Layers**
- **Metadata Cache:** Extend `LRUCache` to store media details, using `mediaId` as the key. Invalidate on provider updates.
- **Torrent Cache:** Cache magnet links/stream URLs with short TTLs (30 mins) to avoid stale links.
- **Image Prefetching:** Use `next/image` with lazy loading, but preload posters for trending media.

##### **WebView Optimization**
- **Static Assets:** Serve CSS/JS from CDN or embed in binary for offline access.
- **DOM Virtualization:** Use `react-virtualized` for large media grids.

---

#### **6. Security & Error Handling**
**Objective:** Ensure safe provider integration and data integrity.

- **Manifest Validation:** Sandbox provider DLLs/scripts. Validate JSON schemas rigorously (e.g., disallow `eval` in manifests).
- **IPC Security:** Sign messages between C++/WebView to prevent injection. Use `JSON Schema` validation for all IPC payloads.
- **Error Boundaries:** Wrap provider calls in try/catch. Expose `error` field in IPC responses for frontend handling.

---

#### **7. Future-Proofing & Extensibility**
- **Plugin System:** Allow third-party providers via signed packages. Use isolated processes for untrusted code.
- **Theming Engine:** Let users customize UI themes stored in `ConfigManager`.
- **Machine Learning:** Add a recommendation engine (e.g., suggest similar media based on watch history).

---

### Execution Milestones
1. **Phase 1 (2 Months):** 
   - Extend `IMediaProvider` for TV shows. 
   - Build `/details` page with season navigation. 
   - Integrate 1-2 torrent providers.

2. **Phase 2 (3 Months):** 
   - Implement user profiles + history. 
   - Develop torrent streaming backend. 
   - Add advanced filtering.

3. **Phase 3 (1 Month):** 
   - Optimize caching/performance. 
   - Release beta with cloud sync.

---

### Main goals
- **Decouple Frontend/Backend:** Maintain clear IPC contracts (e.g., `ipcTypes.hpp`) to allow future UI framework swaps.
- **Use Dependency Injection:** Avoiding singletons where possible (e.g., pass `MediaService` to components via interfaces).
- **Profile Performance:** Audit IPC message throughput and WebView memory usage early for less headache down the line .
