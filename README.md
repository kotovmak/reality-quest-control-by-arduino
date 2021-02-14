[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/kotovmak/100)

## reality-quest-control-by-arduino
**reality-quest-control-by-arduino** is a Reality Quest control App
written in Java. The App allows controlling Arduino (or another board) which
use Sockets from your java program.
You need W5100 Ethernet Arduino shield for setup network interface

## Capabilities
- Interaction with a board and its pins in object-oriented style
- Communication over network
- Provides an UI component that visualize the current state of every quest item relay and
allows changing their mode and state

## Incoming communication between Arduino board and App
For communication used a special string like `TTFFFTTFFFTFF`
Each symbol in string is a one relay condition `T = enabled` and `F = disabled`
First symbol is a start trigger `T = quest started` and after send will be changed to `F`
Second symbol is a `first` relay condition. If relay is opened symbol is `T` and if closed `F`
...  
Last symbol is a stop trigger `T = quest stopped` and after send will be changed to `F`  
   
## Outgoing communication between App and Arduino board
Every Button element send a special string like `EAAAAA`
Arduino board reading data strings from opened Socket and do some steps (like relay open, pin enable etc.)
First symbol is `E` then open first relay
Second symbol is `E` then open second relay
...

## Usage
1. Flash Arduino with sketch
2. Get compiled artifact from `/out/artifacts/reality-quest-control-by-arduino_jar/reality-quest-control-by-arduino.jar` for test your sketch 
2. Then configure App 
2.1. Change `bg.png` specific for your quest rooms disposition
2.2. Change prop `text` for all `Buttons` as you need
2.3. Change `Rectangle` position in Scene Builder as you need
3. Build artifact 

```java
// in file sample.fxml change text for each Button
 <Button fx:id="EAAAAA" layoutX="332.0" layoutY="24.0" mnemonicParsing="false" onAction="#butClick" prefHeight="25.0" prefWidth="130.0" text="Bed" />
// and position for each Ractangle
<Rectangle fx:id="r9" arcHeight="5.0" arcWidth="5.0" fill="DODGERBLUE" height="24.0" layoutX="22.0" layoutY="258.0" stroke="BLACK" strokeType="INSIDE" width="24.0" />
```

![Alt text](/screen_main.png?raw=true)
![Alt text](/screen_back.png?raw=true)

## Play music
App can play music or sounds when Arduino send a specific signal.
For use this scenario find file Controller.java and change some params:  
Build new artifact

```java
    private String filesPath = "C:\\0\\";
    private String playerParams = "--play-and-exit";
    private String playerPath = "c:\\Program Files\\VideoLAN\\VLC\\vlc.exe";
```


## Uploading Sketch To Arduino
You can upload it as follows:

- Plug your Arduino to the computer
- Launch Arduino IDE
- Select `File -> Open -> /reality-quest-control-by-arduino/src/arduino/quest.ino ` in IDE's menu
- Select your board in `Tools -> Board`
- Select the port in `Tools -> Port` (it is already selected if you have uploaded something to your Arduino)
- Click on `Upload` button


## Contributing
Contributions are welcome. If you discover a bug or would like to propose a new
feature, please, [open a new issue](https://github.com/kotovmak/reality-quest-control-by-arduino/issues/new).

If you have an improvement to share, please, do the following:

1. Fork this repository
2. Clone your own fork to your machine (`git clone https://github.com/<your_username>/reality-quest-control-by-arduino.git`)
3. Create a feature branch (`git checkout -b my-new-feature`)
4. Change the code
5. Commit the changes (`git commit -am 'Adds some feature'`)
6. Push to the branch (`git push origin my-new-feature`)
7. Create new Pull Request

## License
**reality-quest-control-by-arduino** is distributed under the terms of the MIT License. See the
[LICENSE](https://github.com/kotovmak/reality-quest-control-by-arduino/blob/master/LICENSE) file.