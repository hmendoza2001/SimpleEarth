<img src="http://www.huesoftllc.com/oldPage/slider1.jpg" width=400>


SimpleEarth
=========

[SimpleEarth][1] is a simplistic multi-platform virtual globe. It works on Windows, Linux and OS X. SimpleEarth is Fast, Free and super Easy to use.


### Build and run SimpleEarth ###

[See Installation Instructions][2]


Main features
-------------
   * Simple code you can learn and leverage
   * Easy to load your own maps and icons
   * Measuring, Volumes and Paths
   * Supports digital elevation data
   * Supports satellite imagery
   * Supports 3D model loading


Build Requirements
------------------

* Qt5 or Qt4 (MinGW under Windows)
* Other optional libraries if using extra features [See Installation Instructions][2]


Runtime Requirements
--------------------
  * Graphics card with OpenGL support


Hello World
-----------

To start understanding the code and API, look at WorldObject and WorldObjectManager classes and the globals.h header file. The hello world example emplaces a label in the world with the following code:

// instantiate world object and set label<br>
WorldObject* worldObject = new WorldObject();<br>
worldObject->setLabel("Hello World!");<br>

// set world object position<br>
GeodeticPosition position;<br>
position.latitude = 0.0;  // in decimal degrees<br>
position.longitude = 0.0;<br>
position.altitude = 1.0;  // in Km<br>
worldObject->setGeodeticPosition(position);<br>

// add world object to manager so that it gets rendered<br>
WorldObjectManager::getInstance()->addWorldObject(worldObject);<br>


Contact us
----------

   * [Web][1]
   * [YouTube][2]
   * [Twitch][3]
   * [LinkedIn][4]

[1]: http://www.huesoftllc.com
[2]: https://www.youtube.com/hectormendoza2001
[3]: https://www.twitch.tv/hmendoza2001
[4]: https://www.linkedin.com/in/hector-v-mendoza-940b5149/
