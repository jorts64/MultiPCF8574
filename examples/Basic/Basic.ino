#include <Wire.h>
#include <MultiPCF8574.h>


// --------------------------------------------------
// CONFIGURACIÓN
// --------------------------------------------------

// 3 módulos:
//
// 0x20 -> pines virtuales 0-7
// 0x21 -> pines virtuales 8-15
// 0x22 -> pines virtuales 16-23
//
MultiPCF8574 io(3, 0x20);


void setup()
{
    Serial.begin(115200);

    Wire.begin();

    // Detectar módulos
    bool ok = io.begin();

    Serial.println();
    Serial.println("MultiPCF8574");
    Serial.println("----------------");

    Serial.print("Modulos configurados: ");
    Serial.println(io.moduleCount());

    Serial.print("Pines virtuales: ");
    Serial.println(io.pinCount());

    Serial.println();


    // Mostrar qué módulos existen
    for (uint8_t i = 0; i < io.moduleCount(); i++) {

        Serial.print("Modulo ");
        Serial.print(i);

        Serial.print(" - direccion 0x");
        Serial.print(io.address(i), HEX);

        Serial.print(" -> ");

        if (io.moduleFound(i)) {
            Serial.println("OK");
        } else {
            Serial.println("NO ENCONTRADO");
        }
    }


    // ------------------------------------------------
    // Configurar pines
    // ------------------------------------------------

    // Primeros 16 como OUTPUT
    for (uint8_t pin = 0; pin < 16; pin++) {
        io.pinMode(pin, OUTPUT);
    }


    // Últimos 8 como INPUT
    for (uint8_t pin = 16; pin < io.pinCount(); pin++) {
        io.pinMode(pin, INPUT);
    }


    Serial.println();
    Serial.println("Sistema listo.");
}


void loop()
{
    // ------------------------------------------------
    // EFECTO COCHE FANTÁSTICO
    // ------------------------------------------------

    for (uint8_t pin = 0; pin < 16; pin++) {

        io.digitalWrite(pin, HIGH);

        delay(100);

        io.digitalWrite(pin, LOW);
    }


    // ------------------------------------------------
    // Leer botón del pin virtual 16
    // ------------------------------------------------

    int estado = io.digitalRead(16);

    if (estado == LOW) {
        Serial.println("Boton pulsado en pin virtual 16");
    }

    delay(10);
}
