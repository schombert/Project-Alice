# Province Tiles UI by SneakBug8

Every province gets a 8x8 grid of tiles to display all the province buildings. For now, the list is generated from existing feature structure:
  - [Done] A tile per every RGO commodity with max employment >1000.
  - [Done] A tile per every built factory (all levels into one).
  - [Done] A tile per every commodity that has free (unused) resource potential.
  - [Done] Every province building takes a tile.
  - [Done] Regiment barracks take tiles last.
  
In tooltips, we display the info relevant to this building:
  - [Done] RGO
  - [Done] Factory
  - [Done] Regiment
  - [Done] Province building

With context windows player can:
  - [Done] Upgrade factories if Rules allow
  - [To Do] Build new factories if Rules allow
  - [To Do] Subsidize/Desubsidize factories if Rules allow
  - [To Do] Delete factories if Rules allow
  - [To Do] Recruit regiments
  - [To Do] Build ships
  - [To Do] Delete regiments
