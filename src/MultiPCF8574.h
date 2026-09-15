#ifndef MULTI_PCF8574_H
#define MULTI_PCF8574_H

#include <Arduino.h>
#include <Wire.h>

class MultiPCF8574 {
public:

    // El PCF8574 normal permite 8 direcciones: 0x20 - 0x27
    static const uint8_t MAX_MODULES = 8;

    // Constructor
    MultiPCF8574(
        uint8_t numModules,
        uint8_t firstAddress = 0x20,
        TwoWire &wire = Wire
    );

    // Inicializa y detecta los módulos
    bool begin();

    // Número de módulos configurados
    uint8_t moduleCount() const;

    // Número total de pines virtuales
    uint16_t pinCount() const;

    // Dirección I2C de un módulo
    uint8_t address(uint8_t module) const;

    // ¿Existe físicamente el módulo?
    bool moduleFound(uint8_t module) const;

    // ¿Es válido un pin virtual?
    bool validPin(uint16_t virtualPin) const;

    // Configuración de pines
    void pinMode(uint16_t virtualPin, uint8_t mode);

    // Escritura de un pin
    void digitalWrite(uint16_t virtualPin, uint8_t value);

    // Lectura de un pin
    int digitalRead(uint16_t virtualPin);

    // --------------------------------------------------
    // Acceso directo a módulos completos
    // --------------------------------------------------

    // Escribe los 8 bits de un módulo
    bool writeByte(uint8_t module, uint8_t value);

    // Lee los 8 bits de un módulo
    uint8_t readByte(uint8_t module);

    // --------------------------------------------------
    // Acceso mediante pines físicos
    // --------------------------------------------------

    void modulePinMode(
        uint8_t module,
        uint8_t pin,
        uint8_t mode
    );

    void moduleDigitalWrite(
        uint8_t module,
        uint8_t pin,
        uint8_t value
    );

    int moduleDigitalRead(
        uint8_t module,
        uint8_t pin
    );

    // --------------------------------------------------
    // Estado
    // --------------------------------------------------

    // Devuelve el último byte escrito/conocido
    uint8_t outputState(uint8_t module) const;

private:

    uint8_t _numModules;
    uint8_t _firstAddress;

    TwoWire *_wire;

    bool _found[MAX_MODULES];

    // Estado que mantenemos localmente.
    // Es importante porque el PCF8574 no tiene registros
    // de dirección como un expansor convencional.
    uint8_t _state[MAX_MODULES];

    // Convierte pin virtual -> módulo
    uint8_t getModule(uint16_t virtualPin) const;

    // Convierte pin virtual -> pin físico
    uint8_t getPhysicalPin(uint16_t virtualPin) const;

    // Escribe un byte directamente al módulo
    bool writeRaw(uint8_t module, uint8_t value);

    // Lee un byte directamente del módulo
    uint8_t readRaw(uint8_t module);

    // Comprueba si responde una dirección I2C
    bool detectModule(uint8_t module);
};

#endif
