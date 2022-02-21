# Tankz General Todo

## High Level

A list of all the high level short term goals 

- [x] Convert Server browser instance into ServerBrowserResult.cpp 
- [x] Convert ServerBrowser BP into ServerBrowser.cpp
- [ ] Start GameplayAbilities conversion and remove HealthComponent
- [ ] Redesign associated ui's
- [x] Look into using GameplayAbilities/Effects to modify the player's move speed and other attributes (PICKUPS BABY)
- [x] Allow for some elevation change in aiming
- [x] Clamp projectile power by a changable variable in char
- [ ] Implement some way for the final trajectory to be displayed (Some sort of aim marker)
- [ ] Implement sounds and VFX for projectiles
- [ ] Investigate camera shake to go along with those sounds and VFX
- [?] Investigate tanks not being cleared properly


#### Allow for some elevation change in aiming
- [ ] Add Mouse Y Input Axis event
- [ ] Bind that event to a function that aggregates a mouse aim variable
- [ ] Clamp said variable to a min/max value in the Char class
- [ ] Take this Y value and add it to Pitch on Rotator before applying it to the bullet's spawn rotation

## Issues with current GAS implementation
- [x] No projectile speed in GameplayAbility
- [ ] No vertical aiming in Fire GameplayAbility
- [ ]