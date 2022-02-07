# Tankz General Todo

## High Level

A list of all the high level short term goals 

- [x] Convert Server browser instance into ServerBrowserResult.cpp 
- [x] Convert ServerBrowser BP into ServerBrowser.cpp
- [ ] Start GameplayAbilities conversion and remove HealthComponent
- [ ] Redesign associated ui's
- [ ] Look into using GameplayAbilities/Effects to modify the player's move speed and other attributes (PICKUPS BABY)
- [ ] Allow for some elevation change in aiming
- [ ] Clamp projectile power by a changable variable in char
- [ ] Implement some way for the final trajectory to be displayed (Some sort of aim marker)
- [ ] Implement sounds and VFX for projectiles
- [ ] Investigate camera shake to go along with those sounds and VFX


#### Allow for some elevation change in aiming
- [ ] Add Mouse Y Input Axis event
- [ ] Bind that event to a function that aggregates a mouse aim variable
- [ ] Clamp said variable to a min/max value in the Char class
- [ ] Take this Y value and add it to Pitch on Rotator before applying it to the bullet's spawn rotation