# Space Combat Arena
Note: In order to laod the sounds, the SoundPack.dat file should be in the same directory as the executable when run.

This is a game I created built off of using my 3d physics engine. The 3d graphics are also from that project, all running on the olc pixel game engine

Originally, I had the concept that the game would be about mining asteroids for resources, and then using those resources to purchase upgrades.
I wanted the enemies to be something to interrupt that main game loop to make things more interesting. I ended up really enjoying the combat,
And even after I ended up implmenting the rudimentery features for the original idea, including a space station, a whole shop UI to buy upgrades and to sell
the collected resources through, I found these mechanics weren't very fun in their current form, so rather then spending the effort to revise these features 
and make them more enjoyable I decided to cut them and make a much simpler game focused entirely on just fighting enemies.

The enemies you fight ended up being the best part of this project in my opinion, and I was quite pleased how they turned out. Their AI is pretty simple, if they detect
the player to their left or right, they will input pitch and yaw in their direction. To make it a little more interesting, if the player is a sufficent angular distance away,
they will roll to align their pitch axis, giving them the effect of turning into the player. They also attempt to lead their shots in order to hit the player, rather
than trying to orient themselves exactly towards the player, each frame they calculate the point they need to aim in order to hit the player given his current trajectory
and try to orient themselves towards that. Both the missiles and the player's lead gunsight use a similar calculation, however these also factor in the acceleration
of the target to be more accurate. I didn't update the enemies AI to use acceleration as I felt they were difficult enough already.

I wanted the user input to only use the mouse and keyboard, I considered trying to make it work with a joystick but decided against it to try to make it more accesible to
my friends. The linear movement is controlled using the keyboard, and the mouse controls the orientation. Watching other people try to play they do seem to struggle with it
a bit, which is a bit unfortunate as I was hoping for the controls to be more intuitive.

The Asteroids surronding the enviorment actually use a very simple procedural generation, this is more of a relic of when this would have been a feature that made sense
in the original game idea as I wouldn't want asteroids that had already been mined to re-appear, but the effect is that the same asteroids will pop in and out of existence
if the player gets sufficently far from them.

This is the second project where I have tried utilizing sounds, the first time being in my Arizona Adventure program in Java. The pixel game engine
provides a sound extension that was really simple to use. I had the idea of simply trying to record sounds myself rather than trying to find sounds online this time,
and I felt it had a pretty funny effect.

# Demonstration

Click the image below to view a demonstration of the program on youtube

[![Gameplay](https://img.youtube.com/vi/wegRKCN6xmk/0.jpg)](https://www.youtube.com/watch?v=wegRKCN6xmk)
