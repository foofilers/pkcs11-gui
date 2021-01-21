# pkcs11-gui
A smartcard manager (using libp11 library)

# Development
## sync git submodule
```bash
$> git submodule update --init #first time only
$> git submodule update
```
## compile & install libp11
```bash
$> cd libp11
$> ./bootstrap
$> ./configure --prefix=/usr/
$> make
$> sudo make install
```
## compile pkcs11-gui
### QtCreator
Open QtCreator and open the pkcs11-gui/pkcs11-gui.pro project
### Console
```bash
$> cd pkcs11-gui
$> qmake
$> make
```
