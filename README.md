# Cube Wwise Integration
This repo improves the Audiokinetic Cube Demo (2019.1) adding more features and complexity

- In the original project, libraries were added inside a parent folder of the source code. Libraries are not added to the repository at this stage.
- **Makefile** has been rewriten and now works correctly on MacOs.

### Changelog
- Added and modified events for jumping and landing: ```jump```, ```land_big_jump```, ```land_small_jump```.
- A Switch with the correct material is now setted before launching the land event, the land sound can now be customised based on the surface.
- Added a new switch called ```Entity``` that sets the type of monster when it spawns.
