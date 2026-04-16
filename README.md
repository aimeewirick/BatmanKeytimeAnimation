
<p align="center">
  <a href="https://youtu.be/gTZcyyYLg8s">
    <img src="https://github.com/user-attachments/assets/f9621934-ee9d-4469-b59f-7bc1cedd490b" alt="description" width="622" height="576" />
  </a>
</p>
<p>
  3D Graphic:
My project is composed of several 3D graphics.  These include renderings of Lego Bat Man in a few different poses:  right leg forward, left leg forward, and standing straight.  Each of these poses is an object list composed of different parts of the character specified by color.  For instance, the belt is one object, the white part of the torso is one object, and the black part of the character is another.  The city scape is another list composed of two objects, the city without the lights and the white light bulbs.  The wall in the background, and the road are created using vertices and texture coordinates. It was an exciting exploration to figure out how to hand add texture coordinates by hand and where to put them.  The bat symbol is also a 3-D object that is illuminated by a spotlight. I added a yellow circle to make it a more noticeable visual effect.    The bats were initially one .obj file I found, but I used blender to add multiple bats to one .obj file.  Similarly to Batman, the  BatWing mobile is a 3-D obj file that I broke into black and red color sections and created a consolidated list.

Keytime Animation:
I have animated the little bats as one .obj file.  I would like to actually animate them separately and have them act more like a swarm of individuals, but they are side characters, and  I ran out of time.  They basically kind of track what Batman is doing, and when he flies away, they fly off.  Batman is several different obj files that I switch according to the keytime.  This was maybe the most tricky part to figure out, because the returned number for GetValue in keytime is actually a float of an average rather than the number set in AddTimeValue.  Because of this, I had to figure out what AddTimeValue and GetValues would be indicative of different leg positions every half-second. I ended up needing values far enough apart that their average showing GetValue would be recognizable when a change in the AddTimeValue happened.  I tried cutting out keytime all together, but that presented a whole different set of issues.  Once I figured this out, it was fairly simple to get the batWing animated.  I used keytime values for my program to detect when to have the symbol appear on the wall, and when to have Batman disappear (as he gets into the bat wing).

</p>
