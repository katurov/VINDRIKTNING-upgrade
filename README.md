# Upgrading a VINDRIKTNING from IKEA

## The base:

1. Original device is 5v, so the first question is about safety to use ESP with it. Accouding to [the documentation it is possible](https://ba0sh1.com/2016/08/03/is-esp8266-io-really-5v-tolerant/) with no damage, if ESP's pin is configured as INPUT.
2. The original pin-out and the meaning of test pads are found [here](https://revspace.nl/VINDRIKTNING) and it is based on the original [hack](https://threadreaderapp.com/thread/1415291684569632768.html). Please note that on original page here is the mess with power legs of the controller: VCC on 4th and Ground on 2nd.
3. And [the most ready project](https://hackaday.io/project/181195-ikea-vindriktning-pcb) found [on hackaday.com](https://hackaday.com/2021/08/13/hacked-ikea-air-quality-sensor-gets-custom-pcb/)
4. Lots of forks from [original project](https://github.com/Hypfer/esp8266-vindriktning-particle-sensor) this will be one of 'em, but I see no meaning to use software serial and planned to use it in my environment.

## The main Idea

1. As soon as everybody uses ESP8266 & 8285 with "usual" USB2Serial which have 5V on Serial lines, it have to be possible to work with VINDRIKTNING Serial without shifting the level. 
2. Serial communication is just a Serial communication. So it can be sniffed
3. No matter how the signature of the sensor changed it is the same sensor (mostly)

## The driver and the signature

The original driver from Adafruit looking for sequence ```16 0d 02``` as first three bytes of [the signature from the documentation](https://en.gassensor.com.cn/Product_files/Specifications/LED%20Particle%20Sensor%20PM1006K%20Specification.pdf), but VINDRIKTNING sends ```16 11 0B``` instead. As we can see in the community [e.g. here](https://issueexplorer.com/issue/arendst/Tasmota/13012) it isn't a big deal, just keep in mind that values of pm1.0 and pm10 are probably dummy.

## The usage

1. Connects to TX sensor's wire in parallel with original schema
2. Fetch 20 bytes and read the signature
3. Get bytes 5 & 6 (counting from 0) witch are DF3 DF4
4. Calculate by formula PM2.5 = DF3 * 256 + DF4
5. Send values: over http (for "arduino" version) or ESPNow protocol (for "espnow" version)
