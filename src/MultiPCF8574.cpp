#include "MultiPCF8574.h"


MultiPCF8574::MultiPCF8574(
    uint8_t numModules,
    uint8_t firstAddress,
    TwoWire &wire
)
{
    // Limitamos al máximo de 8 direcciones
    if (numModules > MAX_MODULES) {
        numModules = MAX_MODULES;
    }

    _numModules = numModules;
    _firstAddress = firstAddress;
    _wire = &wire;

    for (uint8_t i = 0; i < MAX_MODULES; i++) {
        _found[i] = false;

        // 0xFF significa todos los pines liberados/INPUT
        _state[i] = 0xFF;
    }
}


// --------------------------------------------------
// Inicialización
// --------------------------------------------------

bool MultiPCF8574::begin()
{
    bool allFound = true;

    for (uint8_t i = 0; i < _numModules; i++) {

        _found[i] = detectModule(i);

        if (_found[i]) {

            // Al arrancar dejamos todos los pines en HIGH.
            //
            // En el PCF8574 esto equivale a liberar los pines,
            // que es el comportamiento habitual para INPUT.
            _state[i] = 0xFF;

            writeRaw(i, _state[i]);

        } else {

            allFound = false;
        }
    }

    return allFound;
}


// --------------------------------------------------
// Información
// --------------------------------------------------

uint8_t MultiPCF8574::moduleCount() const
{
    return _numModules;
}


uint16_t MultiPCF8574::pinCount() const
{
    return (uint16_t)_numModules * 8;
}


uint8_t MultiPCF8574::address(uint8_t module) const
{
    if (module >= _numModules) {
        return 0;
    }

    return _firstAddress + module;
}


bool MultiPCF8574::moduleFound(uint8_t module) const
{
    if (module >= _numModules) {
        return false;
    }

    return _found[module];
}


bool MultiPCF8574::validPin(uint16_t virtualPin) const
{
    return virtualPin < pinCount();
}


// --------------------------------------------------
// Conversión de pin virtual
// --------------------------------------------------

uint8_t MultiPCF8574::getModule(uint16_t virtualPin) const
{
    return virtualPin / 8;
}


uint8_t MultiPCF8574::getPhysicalPin(uint16_t virtualPin) const
{
    return virtualPin % 8;
}


// --------------------------------------------------
// Detección
// --------------------------------------------------

bool MultiPCF8574::detectModule(uint8_t module)
{
    if (module >= _numModules) {
        return false;
    }

    uint8_t addr = address(module);

    _wire->beginTransmission(addr);

    uint8_t error = _wire->endTransmission();

    return error == 0;
}


// --------------------------------------------------
// Escritura RAW
// --------------------------------------------------

bool MultiPCF8574::writeRaw(
    uint8_t module,
    uint8_t value
)
{
    if (module >= _numModules) {
        return false;
    }

    if (!_found[module]) {
        return false;
    }

    uint8_t addr = address(module);

    _wire->beginTransmission(addr);

    _wire->write(value);

    uint8_t error = _wire->endTransmission();

    return error == 0;
}


// --------------------------------------------------
// Lectura RAW
// --------------------------------------------------

uint8_t MultiPCF8574::readRaw(uint8_t module)
{
    if (module >= _numModules) {
        return 0;
    }

    if (!_found[module]) {
        return 0;
    }

    uint8_t addr = address(module);

    uint8_t received = _wire->requestFrom(
        (int)addr,
        1
    );

    if (received != 1) {
        return 0;
    }

    return _wire->read();
}


// --------------------------------------------------
// pinMode virtual
// --------------------------------------------------

void MultiPCF8574::pinMode(
    uint16_t virtualPin,
    uint8_t mode
)
{
    if (!validPin(virtualPin)) {
        return;
    }

    uint8_t module = getModule(virtualPin);
    uint8_t pin = getPhysicalPin(virtualPin);

    modulePinMode(module, pin, mode);
}


// --------------------------------------------------
// digitalWrite virtual
// --------------------------------------------------

void MultiPCF8574::digitalWrite(
    uint16_t virtualPin,
    uint8_t value
)
{
    if (!validPin(virtualPin)) {
        return;
    }

    uint8_t module = getModule(virtualPin);
    uint8_t pin = getPhysicalPin(virtualPin);

    moduleDigitalWrite(module, pin, value);
}


// --------------------------------------------------
// digitalRead virtual
// --------------------------------------------------

int MultiPCF8574::digitalRead(
    uint16_t virtualPin
)
{
    if (!validPin(virtualPin)) {
        return LOW;
    }

    uint8_t module = getModule(virtualPin);
    uint8_t pin = getPhysicalPin(virtualPin);

    return moduleDigitalRead(module, pin);
}


// --------------------------------------------------
// pinMode físico
// --------------------------------------------------

void MultiPCF8574::modulePinMode(
    uint8_t module,
    uint8_t pin,
    uint8_t mode
)
{
    if (module >= _numModules) {
        return;
    }

    if (pin > 7) {
        return;
    }

    if (!_found[module]) {
        return;
    }

    uint8_t mask = (1 << pin);

    if (mode == INPUT || mode == INPUT_PULLUP) {

        // PCF8574:
        // HIGH = pin liberado / entrada
        _state[module] |= mask;

    } else {

        // OUTPUT
        //
        // No podemos hacer INPUT_PULLDOWN porque
        // el PCF8574 no dispone de pull-down.
        _state[module] &= ~mask;
    }

    writeRaw(module, _state[module]);
}


// --------------------------------------------------
// digitalWrite físico
// --------------------------------------------------

void MultiPCF8574::moduleDigitalWrite(
    uint8_t module,
    uint8_t pin,
    uint8_t value
)
{
    if (module >= _numModules) {
        return;
    }

    if (pin > 7) {
        return;
    }

    if (!_found[module]) {
        return;
    }

    uint8_t mask = (1 << pin);

    if (value == HIGH) {
        _state[module] |= mask;
    } else {
        _state[module] &= ~mask;
    }

    writeRaw(module, _state[module]);
}


// --------------------------------------------------
// digitalRead físico
// --------------------------------------------------

int MultiPCF8574::moduleDigitalRead(
    uint8_t module,
    uint8_t pin
)
{
    if (module >= _numModules) {
        return LOW;
    }

    if (pin > 7) {
        return LOW;
    }

    if (!_found[module]) {
        return LOW;
    }

    uint8_t value = readRaw(module);

    return (value & (1 << pin)) ? HIGH : LOW;
}


// --------------------------------------------------
// Escribir módulo completo
// --------------------------------------------------

bool MultiPCF8574::writeByte(
    uint8_t module,
    uint8_t value
)
{
    if (module >= _numModules) {
        return false;
    }

    if (!_found[module]) {
        return false;
    }

    _state[module] = value;

    return writeRaw(module, value);
}


// --------------------------------------------------
// Leer módulo completo
// --------------------------------------------------

uint8_t MultiPCF8574::readByte(
    uint8_t module
)
{
    return readRaw(module);
}


// --------------------------------------------------
// Estado de salida
// --------------------------------------------------

uint8_t MultiPCF8574::outputState(
    uint8_t module
) const
{
    if (module >= _numModules) {
        return 0;
    }

    return _state[module];
}
