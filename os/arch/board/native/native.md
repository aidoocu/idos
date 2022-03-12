# Plataforma nativa


## Network
Pese a que ya está deprecated se ha utilizado **tun/tap** para recrear la interface de red. En la comprensión de uso fue revelador el artículo [Tun/Tap interface tutorial](https://piratelearner.com/en/bookmarks/tuntap-interface-tutorial/14/) el cual recomendamos.

Para obtener los valores ya configurados en la interface vamos a utilizar el método descrito en [este artículo](http://www.microhowto.info/howto/get_the_mac_address_of_an_ethernet_interface_in_c_using_siocgifhwaddr.html) usando la función *ioctl()* que guarda los datos en una estructura tipo *ifreq* en el caso de Linux y para Macos ver [aquí](https://stackoverflow.com/questions/3964494/having-a-problem-figuring-out-how-to-get-ethernet-interface-info-on-mac-os-x-usi).
### Instalando en MacOS

Para instalar tun/tap recomendamos brew

``` bash
    $ brew tap caskroom/cask 
    $ brew cask install tuntap
```

