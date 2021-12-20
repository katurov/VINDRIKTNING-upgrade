# Upgrading a VINDRIKTNING from IKEA

## The base:

1. Original device is 5v, so the first question is about safety to use ESP with it. Accouding to [the documentation it is possible](https://ba0sh1.com/2016/08/03/is-esp8266-io-really-5v-tolerant/) with no damage, if ESP's pin is configured as INPUT.
2. The original pin-out and the meaning of test pads are found [here](https://revspace.nl/VINDRIKTNING) and it is based on original [hacking](https://threadreaderapp.com/thread/1415291684569632768.html). Please note that on original page here is the mess with power legs of the controller: VCC on 4th and Ground on 2nd.
3. And [the most ready project](https://hackaday.com/2021/08/13/hacked-ikea-air-quality-sensor-gets-custom-pcb/) found on hackaday.com

